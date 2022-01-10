#include "Collision.h"
#include "cCollider.h"

#include <Engine/Asserts/Asserts.h>



// Interface
//==========

eae6320::cResult eae6320::Collision::Initialize()
{
	cResult result = Results::Success;

	s_Colliders = new std::list<cCollider*>();

	return result;
}


eae6320::cResult eae6320::Collision::CleanUp()
{
	cResult result = Results::Success;

	delete s_Colliders;

	return result;
}

void eae6320::Collision::Register(cCollider* coll)
{
	s_Colliders->push_back(coll);
}

void eae6320::Collision::Unregister(cCollider* coll)
{
	s_Colliders->remove(coll);
}


void eae6320::Collision::UpdateCollisions(float i_deltaTime)
{
	std::list<sCollision> collidedObjects = std::list<sCollision>();
	GetCollidedObjects(&collidedObjects, i_deltaTime);

	float lastTime = 0.0f;
	for (std::list<sCollision>::iterator it = collidedObjects.begin(); it != collidedObjects.end(); ++it)
	{
		// If we would start the frame collided, don't advance physics, just try to resolve the collision
		if (it->timeSeparation >= 0.0f)
		{
			float timeInterval = it->timeSeparation - lastTime;
			EAE6320_ASSERTF(timeInterval >= 0.0f, "Time interval must was negative when calculating collision.");
			lastTime = it->timeSeparation;
			ResolveSingleCollision((*it));
			AdvancePhysicsByInterval(timeInterval);
		}
		else
		{
			// If objects collided last frame and are still collided, then do our best to resolve it
			ResolveSingleCollision((*it));
		}
		
		it->colliderA->InvokeCollisionCallbacks(*it);
		it->colliderB->InvokeCollisionCallbacks(it->GetInverted());
	}

	EAE6320_ASSERTF(i_deltaTime >= lastTime, "Collisions were calculated over a time interval greater than the requested time interval.");
	AdvancePhysicsByInterval(i_deltaTime - lastTime);
}

void eae6320::Collision::GetCollidedObjects(std::list<sCollision>* o_CollisionList, float timeInterval)
{
	// Iterate through every paired rigidbody interaction
	for (std::list<cCollider*>::iterator itOuter = s_Colliders->begin(); itOuter != std::prev(s_Colliders->end(), 1); ++itOuter)
	{
	//for (int i = 0; i < s_Colliders->size() - 1; i++) {
		for (std::list<cCollider*>::iterator itInner = std::next(itOuter, 1); itInner != s_Colliders->end(); ++itInner)
		{
		//for (int j = i + 1; j < s_Colliders->size(); j++) {
			cCollider* collA = (*itOuter);
			cCollider* collB = (*itInner);
			float outCollisionTime;
			float outPenetrationDistance;
			Math::sVector outCollisionNormal;
			if (collA->rigidbody != collB->rigidbody && cCollider::DoesCollide(*(collA), *(collB), timeInterval, outCollisionTime, outCollisionNormal, outPenetrationDistance))
			{

				// Assemble the collision object
 				sCollision coll = sCollision();
				coll.colliderA = collA;
				coll.colliderB = collB;
				coll.normal = outCollisionNormal;
				coll.penetrationDepth = outPenetrationDistance + 0.0001f;	// Add a small value to prevent colliders from "sticking"
				coll.timeSeparation = outCollisionTime;

				bool hasAdded = false;
				for (std::list<sCollision>::iterator it = o_CollisionList->begin(); it != o_CollisionList->end(); ++it)
				{
					if (coll.timeSeparation < it->timeSeparation)
					{
						o_CollisionList->insert(it, coll);
						hasAdded = true;
						break;
					}
				}
				if (!hasAdded)
				{
					o_CollisionList->push_back(coll);
				}

			}
		}
	}
}

void eae6320::Collision::AdvancePhysicsByInterval(float timeInterval)
{
	for (std::list<cCollider*>::iterator it = s_Colliders->begin(); it != s_Colliders->end(); ++it)
	{
		(*it)->rigidbody->Update(timeInterval);
	}
}

void eae6320::Collision::ResolveSingleCollision(const sCollision& coll)
{
	// If either collider is a trigger, don't resolve the collision
	if (coll.colliderA->isTrigger || coll.colliderB->isTrigger)
	{
		return;
	}

	// Move objects so they're no longer intersecting
	// Only move one of the objects this way, and only if it shouldn't be treated as kinematic
	if (!coll.colliderA->resolveAsKinematic) {
		coll.colliderA->rigidbody->position += -coll.normal * coll.penetrationDepth;
	}
	else if (!coll.colliderB->resolveAsKinematic)
	{
		coll.colliderB->rigidbody->position += coll.normal * coll.penetrationDepth;
	}


	Math::sVector AVel = coll.colliderA->rigidbody->velocity;
	Math::sVector BVel = coll.colliderB->rigidbody->velocity;

	float totalMass = coll.colliderA->mass + coll.colliderB->mass;
	float AMassProp = coll.colliderA->mass / totalMass;
	float BMassProp = coll.colliderB->mass / totalMass;
	// If one of the 2 objects says we should use the max restitution instead, do that. Otherwise average it.
	float restitution;
	if (coll.colliderA->shouldUseMaxRestitution || coll.colliderB->shouldUseMaxRestitution)
	{
		restitution = std::max(coll.colliderA->restitution, coll.colliderB->restitution);
	}
	else
	{
		restitution = (coll.colliderA->restitution + coll.colliderB->restitution) / 2.0f;
	}

	// Perform an in-elastic collision. Use mass unless at least one of the objects is kinematic.
	if (!coll.colliderA->resolveAsKinematic && !coll.colliderB->resolveAsKinematic)
	{

		coll.colliderA->rigidbody->velocity = (AVel * coll.colliderA->mass + BVel * coll.colliderB->mass + coll.colliderB->mass * restitution * (BVel - AVel)) / totalMass;
		coll.colliderB->rigidbody->velocity = (AVel * coll.colliderA->mass + BVel * coll.colliderB->mass + coll.colliderA->mass * restitution * (AVel - BVel)) / totalMass;
	}
	else if (!coll.colliderA->resolveAsKinematic && coll.colliderB->resolveAsKinematic)
	{
		Math::sVector reflectedVel = AVel - (1 + restitution) * (Math::Dot(AVel, -coll.normal)) * -coll.normal;
		coll.colliderA->rigidbody->velocity = BVel + reflectedVel;
	}
	else if (coll.colliderA->resolveAsKinematic && !coll.colliderB->resolveAsKinematic)
	{
		Math::sVector reflectedVel = BVel - (1 + restitution) * (Math::Dot(BVel, coll.normal)) * coll.normal;
		coll.colliderB->rigidbody->velocity = AVel + reflectedVel;
	}
	
}