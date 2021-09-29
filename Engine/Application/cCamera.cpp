#include "cCamera.h"

eae6320::Application::Camera::Camera(Math::cQuaternion i_cameraOrientation, Math::sVector i_cameraPosition)
{
	translation.position = i_cameraPosition;
	translation.orientation = i_cameraOrientation;
}
