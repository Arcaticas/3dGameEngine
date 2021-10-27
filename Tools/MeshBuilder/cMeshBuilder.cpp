#include "cMeshBuilder.h"
#include "Engine/Platform/Platform.h"
#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Graphics/cMeshParser.h>

eae6320::cResult eae6320::Assets::cMeshBuilder::Build(const std::vector<std::string>& i_arguments)
{
	eae6320::Graphics::s_meshData* data = new eae6320::Graphics::s_meshData;
	eae6320::Graphics::Parser::ReadMeshValues(this->m_path_source, data);

	return eae6320::Platform::CopyFileW(this->m_path_source, this->m_path_target);
	
}
