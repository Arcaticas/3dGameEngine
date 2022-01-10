#ifndef EAE6320_CSOUNDBUILDER_H
#define EAE6320_CSOUNDBUILDER_H

#include <Tools/AssetBuildLibrary/iBuilder.h>

namespace eae6320
{
	namespace Assets
	{
		class cSoundBuilder final : public iBuilder
		{
		public:
			

		private:

			cResult Build(const std::vector<std::string>& i_arguments) final;

		};	
	}
}

#endif