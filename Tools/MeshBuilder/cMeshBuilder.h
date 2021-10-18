#ifndef EAE6320_CMESHBUILDER_H
#define EAE6320_CMESHBUILDER_H

#include <Tools/AssetBuildLibrary/iBuilder.h>

namespace eae6320
{
	namespace Assets
	{
		class cMeshBuilder final : public iBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			cResult Build(const std::vector<std::string>& i_arguments) final;


		};
	}
}

#endif