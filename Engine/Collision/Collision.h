#ifndef EAE6320_COLLISION_H
#define EAE6320_COLLISION_H

#include "cCollider.h"
#include "sCollision.h"

#include <Engine/Results/Results.h>
#include <Engine/Physics/sRigidBodyState.h>
#include <Engine/Math/sVector.h>

#include <list>

// Interface
//==========

namespace eae6320
{
	namespace Collision
	{


		// Update
		//-------

		void UpdateCollisions(float i_deltaTime);

		void GetCollidedObjects(std::list<sCollision>* o_CollisionList, float timeInterval);
		void AdvancePhysicsByInterval(float timeInterval);
		void ResolveSingleCollision(const sCollision& coll);

		// Initialize / Clean Up
		//----------------------

		cResult Initialize();
		cResult CleanUp();


		// Registration
		//----------------------
		
		void Register(cCollider* coll);
		void Unregister(cCollider* coll);
		
		static std::list<cCollider*>* s_Colliders = nullptr;
	}
}

#endif	// EAE6320_COLLISION_H
