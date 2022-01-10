#ifndef EAE6320_SCOLLISION_H
#define EAE6320_SCOLLISION_H

#include <Engine/Math/sVector.h>

namespace eae6320
{
	namespace Collision
	{
		// Forward delcare
		class cCollider;

		struct sCollision {
		public:
			cCollider* colliderA = nullptr;
			cCollider* colliderB = nullptr;

			Math::sVector normal;
			float penetrationDepth = 0.0f;
			float timeSeparation = 0.0f;

			inline sCollision GetInverted() {
				sCollision inverted = sCollision();
				inverted.colliderA = this->colliderB;
				inverted.colliderB = this->colliderA;
				inverted.normal = -this->normal;
				inverted.penetrationDepth = this->penetrationDepth;
				inverted.timeSeparation = this->timeSeparation;
				return inverted;
			}
		};
	}
}

#endif	// EAE6320_SCOLLISION_H
