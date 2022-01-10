// Includes
//=========

#include "cPhysicsBuilder.h"

#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <iostream>
#include <fstream>

std::ofstream s_physicsBinaryFile;

// Build
//------

eae6320::cResult eae6320::Assets::cPhysicsBuilder::Build(const std::vector<std::string>& i_arguments)
{
	auto result = eae6320::Results::Success;
	std::string errorMessage;
	constexpr auto noErrorIfTargetAlreadyExists = false;
	constexpr auto updateTheTargetFileTime = true;
	//result = eae6320::Platform::CopyFile(m_path_source, m_path_target, noErrorIfTargetAlreadyExists, updateTheTargetFileTime, &errorMessage);
	s_physicsBinaryFile.open(m_path_target, std::ios_base::out | std::ios_base::binary);
	result = LoadPhysics(m_path_source, &errorMessage);
	s_physicsBinaryFile.close();
	if (result == Results::Failure)
	{
		eae6320::Assets::OutputErrorMessageWithFileInfo(m_path_source, errorMessage.c_str());
		return Results::Failure;
	}
	return result;
}

eae6320::cResult eae6320::Assets::cPhysicsBuilder::LoadPhysics(const char* const i_path, std::string* const o_errorMessage)
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
				//EAE6320_ASSERT(lua_gettop(luaState) == 0);

				lua_close(luaState);
				luaState = nullptr;
			}
		});
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			//std::cerr << "Failed to create a new Lua state" << std::endl;
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
			//std::cerr << lua_tostring(luaState, -1) << std::endl;
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
					//std::cerr << "Asset files must return a table (instead of a " <<
						//luaL_typename(luaState, -1) << ")" << std::endl;
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				//std::cerr << "Asset files must return a single table (instead of " <<
					//returnedValueCount << " values)" << std::endl;
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			//std::cerr << lua_tostring(luaState, -1) << std::endl;
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
	result = LoadPhysicsValues(*luaState);

	return result;
}

eae6320::cResult eae6320::Assets::cPhysicsBuilder::LoadPhysicsValues(lua_State& io_luaState)
{
	auto result = eae6320::Results::Success;

	if (!(result = LoadPhysicsValues_dimensions(io_luaState)))
	{
		return result;
	}
	if (!(result = LoadPhysicsValues_controls(io_luaState)))
	{
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cPhysicsBuilder::LoadPhysicsValues_dimensions(lua_State& io_luaState)
{
	auto result = eae6320::Results::Success;
	{
		constexpr auto* const key = "width";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popWidth([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		const auto value = lua_tonumber(&io_luaState, -1);
		s_physicsBinaryFile.write((char*)&value, sizeof(double));
	}
	{
		constexpr auto* const key = "height";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popHeight([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		const auto value = lua_tonumber(&io_luaState, -1);
		s_physicsBinaryFile.write((char*)&value, sizeof(double));
	}
	{
		constexpr auto* const key = "depth";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popDepth([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		const auto value = lua_tonumber(&io_luaState, -1);
		s_physicsBinaryFile.write((char*)&value, sizeof(double));
	}
	return result;
}

eae6320::cResult eae6320::Assets::cPhysicsBuilder::LoadPhysicsValues_controls(lua_State& io_luaState)
{
	auto result = eae6320::Results::Success;
	{
		constexpr auto* const key = "left";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popLeft([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		const auto value = lua_tostring(&io_luaState, -1);
		const std::string myValue = value;
		size_t sizeOfValue = myValue.length();
		s_physicsBinaryFile.write((char*)&sizeOfValue, sizeof(size_t));
		s_physicsBinaryFile.write(myValue.c_str(), myValue.length());
	}
	{
		constexpr auto* const key = "up";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popUp([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		const auto value = lua_tostring(&io_luaState, -1);
		const std::string myValue = value;
		size_t sizeOfValue = myValue.length();
		s_physicsBinaryFile.write((char*)&sizeOfValue, sizeof(size_t));
		s_physicsBinaryFile.write(myValue.c_str(), myValue.length());
	}
	{
		constexpr auto* const key = "right";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popRight([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		const auto value = lua_tostring(&io_luaState, -1);
		const std::string myValue = value;
		size_t sizeOfValue = myValue.length();
		s_physicsBinaryFile.write((char*)&sizeOfValue, sizeof(size_t));
		s_physicsBinaryFile.write(myValue.c_str(), myValue.length());
	}
	{
		constexpr auto* const key = "down";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popDown([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		const auto value = lua_tostring(&io_luaState, -1);
		const std::string myValue = value;
		size_t sizeOfValue = myValue.length();
		s_physicsBinaryFile.write((char*)&sizeOfValue, sizeof(size_t));
		s_physicsBinaryFile.write(myValue.c_str(), myValue.length());
	}
	{
		constexpr auto* const key = "rotateLeft";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popRotLeft([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		const auto value = lua_tostring(&io_luaState, -1);
		const std::string myValue = value;
		size_t sizeOfValue = myValue.length();
		s_physicsBinaryFile.write((char*)&sizeOfValue, sizeof(size_t));
		s_physicsBinaryFile.write(myValue.c_str(), myValue.length());
	}
	{
		constexpr auto* const key = "rotateRight";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popRotRight([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		const auto value = lua_tostring(&io_luaState, -1);
		const std::string myValue = value;
		size_t sizeOfValue = myValue.length();
		s_physicsBinaryFile.write((char*)&sizeOfValue, sizeof(size_t));
		s_physicsBinaryFile.write(myValue.c_str(), myValue.length());
	}
	{
		constexpr auto* const key = "forward";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popForward([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		const auto value = lua_tostring(&io_luaState, -1);
		const std::string myValue = value;
		size_t sizeOfValue = myValue.length();
		s_physicsBinaryFile.write((char*)&sizeOfValue, sizeof(size_t));
		s_physicsBinaryFile.write(myValue.c_str(), myValue.length());
	}
	{
		constexpr auto* const key = "backward";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popBackward([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		const auto value = lua_tostring(&io_luaState, -1);
		const std::string myValue = value;
		size_t sizeOfValue = myValue.length();
		s_physicsBinaryFile.write((char*)&sizeOfValue, sizeof(size_t));
		s_physicsBinaryFile.write(myValue.c_str(), myValue.length());
	}
	return result;
}