#include "cMeshBuilder.h"
#include "Engine/Platform/Platform.h"
#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Graphics/cMeshParser.h>


#include <iostream>
#include <fstream>
#include <Engine/Logging/Logging.h>

eae6320::cResult eae6320::Assets::cMeshBuilder::Build(const std::vector<std::string>& i_arguments)
{
	eae6320::cResult result = eae6320::Results::Success;

	eae6320::Graphics::s_meshData* data = new eae6320::Graphics::s_meshData;


	eae6320::Graphics::Parser::ReadMeshValues(this->m_path_source, data);

	


	std::ofstream binaryFile (this->m_path_target, std::ofstream::binary);
	if (!binaryFile.is_open())
	{
		result = eae6320::Results::Failure;
	}
	else
	{
		
		binaryFile.write(reinterpret_cast<char*>(&data->i_vSize), sizeof(data->i_vSize));
		binaryFile.write(reinterpret_cast<char*>(&data->i_iSize), sizeof(data->i_iSize));
		binaryFile.write(reinterpret_cast<char*>(data->i_vertexInputs), sizeof(data->i_vertexInputs[0]) * data->i_vSize);
		binaryFile.write(reinterpret_cast<char*>(data->i_indexArray), sizeof(data->i_indexArray[0]) * data->i_iSize);

		
	}

	binaryFile.close();

	

	return result;
	
}
