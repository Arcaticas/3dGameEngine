#include "cMeshParser.h"
#include <Engine/Asserts/Asserts.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <iostream>


eae6320::cResult eae6320::Graphics::Parser::ReadMeshValues(const char* const i_path, Graphics::s_meshData*& o_meshData)
{
	// The LoadAsset() function does _exactly_ what was shown
	// in the LoadTableFromFile examples.
	// After the table is loaded at the top of the stack, though,
	// LoadTableValues() is called,
	// so that is the function you should pay attention to.

	auto result = eae6320::Results::Success;

	
	if (!(result = LoadAsset(i_path, o_meshData)))
	{
		return result;
	}

	return result;
}



eae6320::cResult eae6320::Graphics::Parser::LoadAsset(const char* const i_path, Graphics::s_meshData*& o_meshData)
{
	auto result = eae6320::Results::Success;

	// Create a new Lua state
	lua_State* luaState = nullptr;
	eae6320::cScopeGuard scopeGuard_onExit([&luaState]
		{
			if (luaState)
			{
				// If I haven't made any mistakes
				// there shouldn't be anything on the stack
				// regardless of any errors
				EAE6320_ASSERT(lua_gettop(luaState) == 0);

				lua_close(luaState);
				luaState = nullptr;
			}
		});
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			std::cerr << "Failed to create a new Lua state" << std::endl;
			return result;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, i_path);
		if (luaResult != LUA_OK)
		{
			result = eae6320::Results::Failure;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "Asset files must return a table (instead of a " <<
						luaL_typename(luaState, -1) << ")" << std::endl;
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "Asset files must return a single table (instead of " <<
					returnedValueCount << " values)" << std::endl;
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	eae6320::cScopeGuard scopeGuard_popAssetTable([luaState]
		{
			lua_pop(luaState, 1);
		});
	result = LoadTableValues(*luaState, o_meshData);
	

	return result;
}

eae6320::cResult eae6320::Graphics::Parser::LoadTableValues(lua_State& io_luaState, Graphics::s_meshData*& o_meshData)
{
	auto result = eae6320::Results::Success;

	VertexFormats::sVertex_mesh* verticesArray = nullptr;
	int vertexCount = 0;
	int* vertexCountptr = &vertexCount;
	uint16_t* indexArray = nullptr;
	int indexCount = 0;
	int* indexCountptr = &indexCount;

	if (!(result = LoadTableValues_vertices(io_luaState, vertexCountptr, verticesArray)))
	{
		return result;
	}
	if (!(result = LoadTableValues_indexes(io_luaState, indexCountptr, indexArray)))
	{
		return result;
	}

	if (!(verticesArray && vertexCount != 0 && indexArray && indexCount != 0))
	{
		result = eae6320::Results::Failure;
		std::cerr << "Failed to assign mesh data struct" << std::endl;
		return result;
	}

	o_meshData->i_vertexInputs = verticesArray;
	o_meshData->i_vSize = vertexCount;
	o_meshData->i_indexArray = indexArray;
	o_meshData->i_iSize = indexCount;

	return result;
}

eae6320::cResult eae6320::Graphics::Parser::LoadTableValues_vertices(lua_State& io_luaState, int*& o_vertexCount, VertexFormats::sVertex_mesh*& o_vertices)
{
	auto result = eae6320::Results::Success;

	// Right now the asset table is at -1.
	// After the following table operation it will be at -2
	// and the "vertices" table will be at -1:
	constexpr auto* const key = "vertices";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	// It can be hard to remember where the stack is at
	// and how many values to pop.
	// There are two ways that I suggest making it easier to keep track of this:
	//	1) Use scope guards to pop things automatically
	//	2) Call a different function when you are at a new level
	// Right now we know that we have an original table at -2,
	// and a new one at -1,
	// and so we _know_ that we always have to pop at least _one_
	// value before leaving this function
	// (to make the original table be back to index -1).
	// We can create a scope guard immediately as soon as the new table has been pushed
	// to guarantee that it will be popped when we are done with it:
	eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});
	// Additionally, I try not to do any further stack manipulation in this function
	// and call other functions that assume the "textures" table is at -1
	// but don't know or care about the rest of the stack
	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = LoadTableValues_parseVertices(io_luaState, o_vertexCount, o_vertices)))
		{
			return result;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::Parser::LoadTableValues_parseVertices(lua_State& io_luaState, int*& o_vertexCount, VertexFormats::sVertex_mesh*& o_vertices)
{
	// Right now the asset table is at -2
	// and the textures table is at -1.
	// NOTE, however, that it doesn't matter to me in this function
	// that the asset table is at -2.
	// Because I've carefully called a new function for every "stack level"
	// The only thing I care about is that the textures table that I care about
	// is at the top of the stack.
	// As long as I make sure that when I leave this function it is _still_
	// at -1 then it doesn't matter to me at all what is on the stack below it.

	auto result = eae6320::Results::Success;

	std::cout << "Iterating through every vertex triplet:" << std::endl;
	const auto vertexCount = luaL_len(&io_luaState, -1);
	*o_vertexCount = (int)vertexCount;
	VertexFormats::sVertex_mesh* returnMeshPtr = new VertexFormats::sVertex_mesh[vertexCount];

	for (int i = 1; i <= vertexCount; ++i)
	{
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popVertex([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});

		const auto cordCount = 3;
		for (int j = 1; j <= cordCount; ++j)
		{
			lua_pushinteger(&io_luaState, j);
			lua_gettable(&io_luaState, -2);
			
			eae6320::cScopeGuard scopeGuard_popVertexCord([&io_luaState]
				{
					lua_pop(&io_luaState, 1);
				});


			if (j == 1)
			{
				returnMeshPtr[i - 1].x = (float)lua_tonumber(&io_luaState, -1);
			}
			if (j == 2)
			{
				returnMeshPtr[i - 1].y = (float)lua_tonumber(&io_luaState, -1);
			}
			if (j == 3)
			{
				returnMeshPtr[i - 1].z = (float)lua_tonumber(&io_luaState, -1);
			}
			float debug = (float)lua_tonumber(&io_luaState, -1);
		}
		
	}
	o_vertices = returnMeshPtr;
	return result;
}

eae6320::cResult eae6320::Graphics::Parser::LoadTableValues_indexes(lua_State& io_luaState, int*& o_indexCount, uint16_t*& o_indexArray)
{
	auto result = eae6320::Results::Success;

	// Right now the asset table is at -1.
	// After the following table operation it will be at -2
	// and the "indexes" table will be at -1:
	constexpr auto* const key = "indexes";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	eae6320::cScopeGuard scopeGuard_popParameters([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});
	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = LoadTableValues_parseIndexes(io_luaState, o_indexCount, o_indexArray)))
		{
			return result;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::Parser::LoadTableValues_parseIndexes(lua_State& io_luaState, int*& o_indexCount, uint16_t*& o_indexArray)
{
	// Right now the asset table is at -2
	// and the index table is at -1.
	// NOTE, however, that it doesn't matter to me in this function
	// that the asset table is at -2.
	// Because I've carefully called a new function for every "stack level"
	// The only thing I care about is that the textures table that I care about
	// is at the top of the stack.
	// As long as I make sure that when I leave this function it is _still_
	// at -1 then it doesn't matter to me at all what is on the stack below it.

	auto result = eae6320::Results::Success;

	std::cout << "Iterating through every index:" << std::endl;
	const auto indexCount = luaL_len(&io_luaState, -1);
	*o_indexCount = (int)indexCount;
	uint16_t* returnIndexArray = new uint16_t[indexCount];

	for (int i = 1; i <= indexCount; ++i)
	{
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popTexturePath([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});

		returnIndexArray[i-1] = (uint16_t)lua_tointeger(&io_luaState, -1);
		uint16_t debug = (uint16_t)lua_tointeger(&io_luaState, -1);
	}
	o_indexArray = returnIndexArray;

	return result;
}
