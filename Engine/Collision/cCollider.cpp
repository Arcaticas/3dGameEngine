
#include "cCollider.h"
#include "Collision.h"

#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>


#include <algorithm>


eae6320::Collision::cCollider::cCollider(Math::sVector center, Math::cQuaternion orientation, Math::sVector extents, bool isTrigger, Physics::sRigidBodyState* rigidbody)
	: center(center), orientation(orientation), extents(extents), isTrigger(isTrigger), rigidbody(rigidbody), resolveAsKinematic(false), mass(1.0f), restitution(0.0f), shouldUseMaxRestitution(false)
{
	collisionCallbacks = new std::vector<std::function<void(eae6320::Collision::sCollision coll)>>();
}

eae6320::Collision::cCollider::~cCollider()
{
	Collision::Unregister(this);
	delete collisionCallbacks;
}

eae6320::cResult eae6320::Collision::cCollider::CreateCollider(Physics::sRigidBodyState* i_rigidbody, Math::sVector i_extents, Math::sVector center, Math::cQuaternion orientation, bool isTrigger, cCollider*& o_collider)
{
	cResult result = Results::Success;
	cCollider* newCollider = nullptr;

	// Scope guard objects will execute their lambda function when they leave scope
	cScopeGuard scopeGuard([&o_collider, &result, &newCollider]
		{
			if (result)
			{
				EAE6320_ASSERT(newCollider != nullptr);
				o_collider = newCollider;
				Collision::Register(o_collider);
			} else
			{
				if (newCollider)
				{
					delete newCollider;
					newCollider = nullptr;
				}
				o_collider = nullptr;
			}
		});

	{
		newCollider = new (std::nothrow) cCollider(center, orientation, i_extents, isTrigger, i_rigidbody);
		if (!newCollider)
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the collider");
			Logging::OutputError("Couldn't allocate memory for the collider");
			return result;
		}
	}

	return result;
}

eae6320::cResult eae6320::Collision::cCollider::CreateCollider(Physics::sRigidBodyState* i_rigidbody, Math::sVector i_extents, bool isTrigger, cCollider*& o_collider)
{
	return cCollider::CreateCollider(i_rigidbody, i_extents, Math::sVector(), Math::cQuaternion(), isTrigger, o_collider);
}

void eae6320::Collision::cCollider::SetPhysics(float mass, float restitution)
{
	this->mass = mass;
	this->restitution = restitution;
}

void eae6320::Collision::cCollider::SetPhysics(float mass, float restitution, bool shouldUseMaxRestitution)
{
	this->mass = mass;
	this->restitution = restitution;
	this->shouldUseMaxRestitution = shouldUseMaxRestitution;
}


void eae6320::Collision::cCollider::SetKinematic(bool newState)
{
	this->resolveAsKinematic = newState;
}

void eae6320::Collision::cCollider::InvokeCollisionCallbacks(sCollision coll)
{
	for (size_t i = 0; i < (*collisionCallbacks).size(); i++) {
		(*collisionCallbacks)[i](coll);
	}
}

void eae6320::Collision::cCollider::ListenToCollision(std::function<void(sCollision coll)> callback)
{
	collisionCallbacks->push_back(callback);
}

// Check if 2 colliders are separated, this is a non-swept collision check
bool eae6320::Collision::cCollider::AreSeparated(const cCollider& A, const cCollider& B)
{
	Math::cMatrix_transformation AToWorld = Math::cMatrix_transformation(A.rigidbody->orientation, A.rigidbody->position);
	Math::cMatrix_transformation worldToB = Math::cMatrix_transformation(B.rigidbody->orientation.GetInverse(), -B.rigidbody->position);
	Math::cMatrix_transformation AToB = Math::cMatrix_transformation::ConcatenateAffine(worldToB, AToWorld);
	return cCollider::AreSeparatedOneWay(A, B, AToB) || cCollider::AreSeparatedOneWay(B, A, AToB);
}

bool eae6320::Collision::cCollider::AreSeparatedOneWay(const cCollider& A, const cCollider& B, const Math::cMatrix_transformation& AToB)
{
	Math::sVector ACenterInB = AToB * A.center;
	Math::cMatrix_transformation AToBNoTranslation = Math::cMatrix_transformation(AToB);
	// Strip out the translation component
	AToBNoTranslation.m_03 = 0.0f;
	AToBNoTranslation.m_13 = 0.0f;
	AToBNoTranslation.m_23 = 0.0f;
	AToBNoTranslation.m_33 = 1.0f;
	Math::sVector AExtentXInB = AToBNoTranslation * (A.extents.x * Math::sVector(1.0f, 0.0f, 0.0f));
	Math::sVector AExtentYInB = AToBNoTranslation * (A.extents.y * Math::sVector(0.0f, 1.0f, 0.0f));
	Math::sVector AExtentZInB = AToBNoTranslation * (A.extents.z * Math::sVector(0.0f, 0.0f, 1.0f));

	float AProjectionOntoBx = abs(AExtentXInB.x) + abs(AExtentYInB.x) + abs(AExtentZInB.x);
	float AProjectionOntoBy = abs(AExtentXInB.y) + abs(AExtentYInB.y) + abs(AExtentZInB.y);
	float AProjectionOntoBz = abs(AExtentXInB.z) + abs(AExtentYInB.z) + abs(AExtentZInB.z);

	bool xSeparationB = abs(ACenterInB.x - B.center.x) > (AProjectionOntoBx + B.extents.x);
	bool ySeparationB = abs(ACenterInB.y - B.center.y) > (AProjectionOntoBy + B.extents.y);
	bool zSeparationB = abs(ACenterInB.z - B.center.z) > (AProjectionOntoBz + B.extents.z);

	return xSeparationB || ySeparationB || zSeparationB;
}


// Perform a Swept Axis Separation Collision Check
// outCollisionTime and outBCollisionAxisNormal are only valid if this is true
bool eae6320::Collision::cCollider::DoesCollide(const cCollider& A, const cCollider& B, const float deltaTime, float& o_collisionTime, Math::sVector& o_BCollisionAxisNormal, float& o_penetrationDistance)
{
	float collisionTimeAB, collisionTimeBA;
	float penentrationDistanceAB, penetrationDistanceBA;
	Math::sVector collisionVectorAB, collisionVectorBA;

	Math::cMatrix_transformation AToWorld = Math::cMatrix_transformation(A.rigidbody->orientation, A.rigidbody->position);
	Math::cMatrix_transformation BToWorld = Math::cMatrix_transformation(B.rigidbody->orientation, B.rigidbody->position);
	Math::cMatrix_transformation worldToA = Math::cMatrix_transformation(A.rigidbody->orientation.GetInverse(), -A.rigidbody->position);
	Math::cMatrix_transformation worldToB = Math::cMatrix_transformation(B.rigidbody->orientation.GetInverse(), -B.rigidbody->position);
	Math::cMatrix_transformation AToB = Math::cMatrix_transformation::ConcatenateAffine(worldToB, AToWorld);
	Math::cMatrix_transformation BToA = Math::cMatrix_transformation::ConcatenateAffine(worldToA, BToWorld);

	bool doesCollideAB = cCollider::DoesCollideOneWay(A, A.rigidbody->velocity, B, B.rigidbody->velocity, AToB, worldToB, deltaTime, collisionTimeAB, collisionVectorAB, penentrationDistanceAB);
	bool doesCollideBA = cCollider::DoesCollideOneWay(B, B.rigidbody->velocity, A, A.rigidbody->velocity, BToA, worldToA, deltaTime, collisionTimeBA, collisionVectorBA, penetrationDistanceBA);

	// Collision only exists if there is no axis of separation in the axes of either bounding box
	// There can be an axis of separation 
	if (doesCollideAB && doesCollideBA) {
		// Take the longer of the two collision times, as the time is only correct when both are collided (which is when the longer one would collide)
		if (collisionTimeAB > collisionTimeBA) {
			o_collisionTime = collisionTimeAB;
			o_BCollisionAxisNormal = collisionVectorAB;
		} else {
			o_collisionTime = collisionTimeBA;
			o_BCollisionAxisNormal = -collisionVectorBA;
		}

		o_penetrationDistance = std::min(penentrationDistanceAB, penetrationDistanceBA);
		return true;
	}
	return false;
}

bool eae6320::Collision::cCollider::DoesCollideOneWay(const cCollider& A, const Math::sVector& AVelocity, const cCollider& B, const Math::sVector& BVelocity, const Math::cMatrix_transformation& AToB, const Math::cMatrix_transformation& WorldToB, const float deltaTime, float& o_collisionTime, Math::sVector& o_BCollisionAxisNormal, float& o_penetrationDistance)
{
	Math::sVector AVelocityRelativeToB = AVelocity - BVelocity;
	Math::sVector AVelocityInB = VecMulNoTranslation(WorldToB, AVelocityRelativeToB);

	Math::sVector ACenterInB = AToB * A.center;
	Math::cMatrix_transformation AToBNoTranslation = Math::cMatrix_transformation(AToB);
	// Strip out the translation component
	AToBNoTranslation.m_03 = 0.0f;
	AToBNoTranslation.m_13 = 0.0f;
	AToBNoTranslation.m_23 = 0.0f;
	AToBNoTranslation.m_33 = 1.0f;
	Math::sVector AExtentXInB = AToBNoTranslation * (A.extents.x * Math::sVector(1.0f, 0.0f, 0.0f));
	Math::sVector AExtentYInB = AToBNoTranslation * (A.extents.y * Math::sVector(0.0f, 1.0f, 0.0f));
	Math::sVector AExtentZInB = AToBNoTranslation * (A.extents.z * Math::sVector(0.0f, 0.0f, 1.0f));

	Math::sVector AExtentProjectionOntoB = Math::sVector(abs(AExtentXInB.x) + abs(AExtentYInB.x) + abs(AExtentZInB.x), abs(AExtentXInB.y) + abs(AExtentYInB.y) + abs(AExtentZInB.y), abs(AExtentXInB.z) + abs(AExtentYInB.z) + abs(AExtentZInB.z));

	Math::sVector AMinimalEdges = ACenterInB - AExtentProjectionOntoB;
	Math::sVector AMaximalEdges = ACenterInB + AExtentProjectionOntoB;
	Math::sVector BMinimalEdges = B.center - B.extents;
	Math::sVector BMaximalEdges = B.center + B.extents;

	// If we're not moving in this axis, just do a simple overlap check. If we don't overlap, we're never going to overlap on this axis this frame.
	bool XSeparation = AMinimalEdges.x > BMaximalEdges.x || AMaximalEdges.x < BMinimalEdges.x;
	bool YSeparation = AMinimalEdges.y > BMaximalEdges.y || AMaximalEdges.y < BMinimalEdges.y;
	bool ZSeparation = AMinimalEdges.z > BMaximalEdges.z || AMaximalEdges.z < BMinimalEdges.z;

	if (AVelocityInB == Math::sVector(0.0f, 0.0f, 0.0f)) {
		// If we overlap on all axis
		o_collisionTime = 0.0f;
		o_BCollisionAxisNormal = Math::sVector(0.0f, 0.0f, 0.0f);	// Special value for started the frame in collision
		o_penetrationDistance = 0.0f;
		if (!XSeparation && !YSeparation && !ZSeparation) {
			return true;
		}
	}

	// Check if there's separation, in which case we can bail early
	if (AVelocityInB.x == 0.0f) {
		if (XSeparation) {
			return false;
		}
	}
	if (AVelocityInB.y == 0.0f) {
		if (YSeparation) {
			return false;
		}
	}
	if (AVelocityInB.z == 0.0f) {
		if (ZSeparation) {
			return false;
		}
	}

	Math::sVector closeDistance = (BMinimalEdges - AMaximalEdges);
	Math::sVector tClose = Math::sVector(closeDistance.x / AVelocityInB.x, closeDistance.y / AVelocityInB.y, closeDistance.z / AVelocityInB.z);

	Math::sVector openDistance = (BMaximalEdges - AMinimalEdges);
	Math::sVector tOpen = Math::sVector(openDistance.x / AVelocityInB.x, openDistance.y / AVelocityInB.y, openDistance.z / AVelocityInB.z);

	Math::sVector collisionAxisNormalsDir = Math::sVector(1.0f, 1.0f, 1.0f);

	if (tOpen.x < tClose.x) {
		float tmp = tOpen.x;
		tOpen.x = tClose.x;
		tClose.x = tmp;
		collisionAxisNormalsDir.x = -1.0f;
	}
	if (tOpen.y < tClose.y) {
		float tmp = tOpen.y;
		tOpen.y = tClose.y;
		tClose.y = tmp;
		collisionAxisNormalsDir.y = -1.0f;
	}
	if (tOpen.z < tClose.z) {
		float tmp = tOpen.z;
		tOpen.z = tClose.z;
		tClose.z = tmp;
		collisionAxisNormalsDir.z = -1.0f;
	}

	// If any tClose would be greater than the time passing this frame, then that means we're separated on at least one axis and don't collide until after this frame is over
	if (tClose.x > deltaTime || tClose.y > deltaTime || tClose.z > deltaTime) {
		return false;
	}

	// If any tOpen is less than 0, then that means any collision that would happen on that axis would have happened in the past, and we're separated on at least one axis
	if (tOpen.x < 0.0f || tOpen.y < 0.0f || tOpen.z < 0.0f) {
		return false;
	}

	bool xClosedBeforeAnyOpened = tClose.x < tOpen.y && tClose.x < tOpen.z;
	bool yClosedBeforeAnyOpened = tClose.y < tOpen.x && tClose.y < tOpen.z;
	bool zClosedBeforeAnyOpened = tClose.z < tOpen.x && tClose.z < tOpen.y;

	if (xClosedBeforeAnyOpened && yClosedBeforeAnyOpened && zClosedBeforeAnyOpened) {
		if (tClose.x >= tClose.y && tClose.x >= tClose.z) {
			o_collisionTime = tClose.x;
			// Transform normalized directions by multiplying by the transpose of the inverse of M instead of by M. Also don't include translation in the transformation.
			// We want to go from B -> World, which would be WorldToB.Inverse(). However we're transforming normals,
			//		so we do WorldToB.Transpose() instead since that's equivalent to WorldToB.Inverse().Inverse().Transpose() for normal transformation
			o_BCollisionAxisNormal = VecMulNoTranslation(GetTransposedMatrix(GetInvertedAffineMatrix(WorldToB)), Math::sVector(collisionAxisNormalsDir.x, 0.0f, 0.0f));
			o_BCollisionAxisNormal.Normalize();
			o_penetrationDistance = std::min((BMaximalEdges.x - AMinimalEdges.x), (AMaximalEdges.x - BMinimalEdges.x));
		} else if (tClose.y >= tClose.x && tClose.y >= tClose.z) {
			o_collisionTime = tClose.y;
			o_BCollisionAxisNormal = VecMulNoTranslation(GetTransposedMatrix(GetInvertedAffineMatrix(WorldToB)), Math::sVector(0.0f, collisionAxisNormalsDir.y, 0.0f));
			o_BCollisionAxisNormal.Normalize();
			o_penetrationDistance = std::min((BMaximalEdges.y - AMinimalEdges.y), (AMaximalEdges.y - BMinimalEdges.y));
		} else if (tClose.z >= tClose.x && tClose.z >= tClose.y) {
			o_collisionTime = tClose.z;
			o_BCollisionAxisNormal = VecMulNoTranslation(GetTransposedMatrix(GetInvertedAffineMatrix(WorldToB)), Math::sVector(0.0f, 0.0f, collisionAxisNormalsDir.z));
			o_BCollisionAxisNormal.Normalize();
			o_penetrationDistance = std::min((BMaximalEdges.z - AMinimalEdges.z), (AMaximalEdges.z - BMinimalEdges.z));
		}

		return true;
	} else {
		return false;
	}
}