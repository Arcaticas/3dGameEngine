#ifndef EAE6320_APPLICATION_CGAMEOBJECT_H
#define EAE6320_APPLICATION_CGAMEOBJECT_H

#include <Engine/Graphics/cEffect.h>
#include <Engine/Graphics/Geometry.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Physics/sRigidBodyState.h>
namespace eae6320
{
	namespace Application
	{

		class GameObject 
		{
			

		public:
			eae6320::Graphics::cEffect* effect = nullptr;
			eae6320::Graphics::Geometry* mesh = nullptr;
			eae6320::Physics::sRigidBodyState translation;

			static eae6320::cResult CreateGameObject(eae6320::Graphics::s_meshData i_meshData, const char* const i_shaderPath,Math::sVector i_transform, GameObject*& o_gameObject);

			eae6320::cResult UpdateMesh(eae6320::Graphics::s_meshData i_meshData);

			GameObject() = default;
			~GameObject();

		private:

			
		};
	}
}
#endif