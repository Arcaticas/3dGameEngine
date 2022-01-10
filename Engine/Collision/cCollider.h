#ifndef EAE6320_CCOLLIDER_H
#define EAE6320_CCOLLIDER_H

// Includes
//=========

#include "sCollision.h"

#include <Engine/Math/cQuaternion.h>
#include <Engine/Math/sVector.h>
#include <Engine/Math/cMatrix_transformation.h>
#include <Engine/Physics/sRigidBodyState.h>
#include <Engine/Results/Results.h>


#include <functional>


// Struct Declaration
//===================

namespace eae6320
{
	namespace Collision
	{
		class cCollider
		{
		public:

			~cCollider();

			// Data
			//=====

			Math::sVector center;
			Math::cQuaternion orientation;
			Math::sVector extents;
			// If this is true, collisions will be detected but not automatically resolved
			bool isTrigger;
			// If this is true, then this object will be treated as kinematic (infinite mass) during collisions. 
			bool resolveAsKinematic;
			// The mass of the collider
			float mass;
			// The "bounciness" of the collider. 1 is perfectly elastic, 0 is perfectly inelastic. >1 means more than elastic (physically impossible). <0 will break things.
			float restitution;
			// If this is true on one of the two colliding objects, the maximum restitution of the two objects will be used instead of the average.
			bool shouldUseMaxRestitution;
			Physics::sRigidBodyState* rigidbody;	// It is the responsability of the application to ensure this memory remains valid as long as the collider is being updated and used

			// Interface
			//==========

			static cResult CreateCollider(Physics::sRigidBodyState* i_rigidbody, Math::sVector i_extents, bool isTrigger, cCollider*& o_collider);
			static cResult CreateCollider(Physics::sRigidBodyState* i_rigidbody, Math::sVector i_extents, Math::sVector center, Math::cQuaternion orientation, bool isTrigger, cCollider*& o_collider);

			void SetPhysics(float mass, float restitution);
			void SetPhysics(float mass, float restitution, bool shouldUseMaxRestitution);
			void SetKinematic(bool newState);

			// Check if 2 colliders are separated, this is a non-swept collision check
			static bool AreSeparated(const cCollider& A, const cCollider& B);
			// Check if 2 colliders collide over a time period, this a swept collision check
			static bool DoesCollide(const cCollider& A, const cCollider& B, const float deltaTime, float& o_collisionTime, Math::sVector& o_BCollisionAxisNormal, float& o_penetrationDistance);

			void InvokeCollisionCallbacks(sCollision coll);
			void ListenToCollision(std::function<void(sCollision coll)> callback);

		private:

			// Data
			//=====
			
			std::vector<std::function<void(sCollision coll)>>* collisionCallbacks;
			
			// Interface
			//==========
			
			// Check if 1 collider is separated from the other, this must be done both ways to verify that the colliders are actually separated
			static bool AreSeparatedOneWay(const cCollider& A, const cCollider& B, const Math::cMatrix_transformation& AToB);
			// Check if 1 collider collides with another over a time period, this must be done both ways to verify that the colliders didn't collide
			static bool DoesCollideOneWay(const cCollider& A, const Math::sVector& AVelocity, const cCollider& B, const Math::sVector& BVelocity, const Math::cMatrix_transformation& AToB, const Math::cMatrix_transformation& WorldToB, const float deltaTime, float& o_collisionTime, Math::sVector& o_BCollisionAxisNormal, float& o_penetrationDistance);

			static Math::sVector VecMulNoTranslation(const Math::cMatrix_transformation& i_lhs, const Math::sVector& i_rhs)
			{
				return Math::sVector(
					(i_lhs.m_00 * i_rhs.x) + (i_lhs.m_01 * i_rhs.y) + (i_lhs.m_02 * i_rhs.z),
					(i_lhs.m_10 * i_rhs.x) + (i_lhs.m_11 * i_rhs.y) + (i_lhs.m_12 * i_rhs.z),
					(i_lhs.m_20 * i_rhs.x) + (i_lhs.m_21 * i_rhs.y) + (i_lhs.m_22 * i_rhs.z)
				);
			}

			static Math::cMatrix_transformation GetTransposedMatrix(const Math::cMatrix_transformation& mat) {
				Math::cMatrix_transformation newMat = Math::cMatrix_transformation();
				newMat.m_00 = mat.m_00; newMat.m_01 = mat.m_10; newMat.m_02 = mat.m_20; newMat.m_03 = mat.m_30;
				newMat.m_10 = mat.m_01; newMat.m_11 = mat.m_11; newMat.m_12 = mat.m_21; newMat.m_13 = mat.m_31;
				newMat.m_20 = mat.m_02; newMat.m_21 = mat.m_12; newMat.m_22 = mat.m_22; newMat.m_23 = mat.m_32;
				newMat.m_30 = mat.m_03; newMat.m_31 = mat.m_13; newMat.m_32 = mat.m_23; newMat.m_33 = mat.m_33;
				return newMat;
			}

			static Math::cMatrix_transformation GetInvertedAffineMatrix(const Math::cMatrix_transformation& mat) {
				Math::sVector translationVector = mat.GetTranslation();

				Math::cMatrix_transformation inv = GetTransposedMatrix(mat);
				Math::sVector invertedTranslation = inv * -translationVector;
				inv.m_03 = translationVector.x;
				inv.m_13 = translationVector.y;
				inv.m_23 = translationVector.z;
				inv.m_33 = 1.0f;
				return inv;
			}

			cCollider(Math::sVector center, Math::cQuaternion orientation, Math::sVector extents, bool isTrigger, Physics::sRigidBodyState* rigidbody);

			cCollider(const cCollider&) = delete;
			cCollider(cCollider&&) = delete;
			cCollider& operator =(const cCollider&) = delete;
			cCollider& operator =(cCollider&&) = delete;
		};
	}
}

#endif	// EAE6320_CCOLLIDER_H