/*
	This class builds physics
*/

#ifndef EAE6320_CPHYSICSBUILDER_H
#define EAE6320_CPHYSICSBUILDER_H

// Includes
//=========

#include <Tools/AssetBuildLibrary/iBuilder.h>
#include <External/Lua/Includes.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cPhysicsBuilder final : public iBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			cResult Build(const std::vector<std::string>& i_arguments) final;
			static cResult LoadPhysics(const char* const i_path, std::string* const o_errorMessage);
			static cResult LoadPhysicsValues(lua_State& io_luaState);
			static cResult LoadPhysicsValues_dimensions(lua_State& io_luaState);
			static cResult LoadPhysicsValues_controls(lua_State& io_luaState);

		};
	}
}
#endif	// EAE6320_CPHYSICSBUILDER_H

