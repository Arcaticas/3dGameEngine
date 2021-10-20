#include "cGameObject.h"
#include "iApplication.h"
#include "Engine/Graphics/cMeshParser.h"

eae6320::cResult eae6320::Application::GameObject::CreateGameObject(const char* const i_meshPath, const char* const i_shaderPath, Math::sVector i_transform, GameObject*& o_gameObject)
{
	auto result = Results::Success;

	//Parses mesh data from file
	eae6320::Graphics::s_meshData* meshData = new eae6320::Graphics::s_meshData;
	if (!(result = eae6320::Graphics::Parser::ReadMeshValues(i_meshPath, meshData)))
	{
		EAE6320_ASSERTF(false, "Mesh Parser Failed");
	}


	o_gameObject = new GameObject();

	if (!(result = eae6320::Graphics::cEffect::Load(i_shaderPath, o_gameObject->effect)))
	{
		EAE6320_ASSERTF(false, "no");
	}

	if (!(result = eae6320::Graphics::Geometry::Load(meshData->i_vertexInputs,
		meshData->i_indexArray,
		meshData->i_vSize,
		meshData->i_iSize,
		o_gameObject->mesh)))
	{
		EAE6320_ASSERTF(false, "no");
	}

	o_gameObject->translation.position = i_transform;

	return result;
}

eae6320::cResult eae6320::Application::GameObject::UpdateMesh(eae6320::Graphics::s_meshData i_meshData)
{
	auto result = Results::Success;

	mesh->DecrementReferenceCount();

	if (!(result = eae6320::Graphics::Geometry::Load(i_meshData.i_vertexInputs,
		i_meshData.i_indexArray,
		i_meshData.i_vSize,
		i_meshData.i_iSize,
		mesh)))
	{
		EAE6320_ASSERTF(false, "no");
	}

	return result;
}

eae6320::Application::GameObject::~GameObject()
{
	effect->DecrementReferenceCount();
	mesh->DecrementReferenceCount();
}

