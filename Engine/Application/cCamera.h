#ifndef EAE6320_APPLICATION_CCAMERA_H
#define EAE6320_APPLICATION_CCAMERA_H


#include <Engine/Physics/sRigidBodyState.h>



namespace eae6320
{
	namespace Application
	{
		class Camera
		{
		public:
			eae6320::Physics::sRigidBodyState translation;

			Camera(Math::cQuaternion i_cameraOrientation, Math::sVector i_cameraPosition);
			Camera() = default;
		};
	}
}
#endif