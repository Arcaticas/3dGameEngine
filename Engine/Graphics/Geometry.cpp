#include "Geometry.h"


#include <Engine/Asserts/Asserts.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Logging/Logging.h>
#include <new>
#include <utility>



eae6320::cResult eae6320::Graphics::Geometry::Load(eae6320::Graphics::VertexFormats::sVertex_mesh i_vertexInputs[], uint16_t i_indexArray[], int i_vSize, int i_iSize, Geometry*& o_mesh)
{
	auto result = Results::Success;
	Geometry* newMesh = nullptr;
	cScopeGuard scopeGuard([&o_mesh, &result, &newMesh, &i_vertexInputs]
		{
			if (result)
			{
				EAE6320_ASSERT(newMesh != nullptr);
				o_mesh = newMesh;
			}
			else
			{
				if (newMesh)
				{
					newMesh->DecrementReferenceCount();
					newMesh = nullptr;
				}
				o_mesh = nullptr;
			}
		});


	if (!(i_iSize % 3 == 0))
	{
		EAE6320_ASSERTF(false, "Index Array not divisible by 3");
		return result = Results::Failure;
	}

	newMesh = new (std::nothrow) Geometry();
	if (!newMesh)
	{
		result = Results::OutOfMemory;
		EAE6320_ASSERTF(false, "Couldn't allocate memory for the shader %s");
		Logging::OutputError("Failed to allocate memory for the shader %s");
		return result;
	}

	result = newMesh->Initialize(i_vertexInputs, i_indexArray, i_vSize, i_iSize);

	return result;
}

eae6320::Graphics::Geometry::~Geometry()
{
	EAE6320_ASSERT(m_referenceCount == 0);
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}