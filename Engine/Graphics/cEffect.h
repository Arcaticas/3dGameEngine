#ifndef EAE6320_CEFFECT_H
#define EAE6320_CEFFECT_H

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>
#include "cShader.h"
#include "cRenderState.h"

#if defined( EAE6320_PLATFORM_WINDOWS )
#include <Engine/Windows/Includes.h>
#endif

namespace eae6320
{
	namespace Graphics
	{
		class cEffect
		{
			// Shading Data
			//-------------
			eae6320::Graphics::cShader* m_vertexShader = nullptr;
			eae6320::Graphics::cShader* m_fragmentShader = nullptr;

			eae6320::Graphics::cRenderState m_renderState;
#if defined( EAE6320_PLATFORM_D3D )
			
#elif defined( EAE6320_PLATFORM_GL )
			GLuint m_programId = 0;
#endif
		public:
			eae6320::cResult InitializeShadingData(const char* const shaderPath);

			void BindData();
			void Draw();
			eae6320::cResult Initialize(const char* const shaderPath);
			eae6320::cResult CleanUp();

			operator bool();
		};

		
	}
}


#endif	// EAE6320_CEFFECT_H
