#include "cMeshBuilder.h"
#include "Engine/Platform/Platform.h"
#include <Tools/AssetBuildLibrary/Functions.h>

eae6320::cResult eae6320::Assets::cMeshBuilder::Build(const std::vector<std::string>& i_arguments)
{
	
	return eae6320::Platform::CopyFileW(this->m_path_source, this->m_path_target);
	
}
