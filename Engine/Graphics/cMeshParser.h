#ifndef EAE6320_CMESHPARSER_H
#define EAE6320_CMESHPARSER_H

#include <External/Lua/Includes.h>
#include <Engine/Results/Results.h>
#include "VertexFormats.h"
#include "Graphics.h"

namespace eae6320
{
	namespace Graphics
	{
		namespace Parser
		{
		
			eae6320::cResult ReadMeshValues(const char* const i_path, Graphics::s_meshData*& o_meshData);

			eae6320::cResult LoadTableValues(lua_State& io_luaState, s_meshData*& o_meshData);

			eae6320::cResult LoadTableValues_vertices(lua_State& io_luaState, int*& o_vertexCount, VertexFormats::sVertex_mesh*& o_vertices);
			eae6320::cResult LoadTableValues_parseVertices(lua_State& io_luaState, int*& o_vertexCount, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertices);

			eae6320::cResult LoadTableValues_indexes(lua_State& io_luaState, int*& o_indexCount, uint16_t*& o_indexArray);
			eae6320::cResult LoadTableValues_parseIndexes(lua_State& io_luaState, int*& o_indexCount, uint16_t*& o_indexArray);

			eae6320::cResult LoadAsset(const char* const i_path, Graphics::s_meshData*& o_meshData);
		}
	}
}

#endif