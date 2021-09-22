#ifndef EAE6320_CRENDERER_H
#define EAE6320_CRENDERER_H

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>
#include "cVertexFormat.h"
#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "sContext.h"

#if defined( EAE6320_PLATFORM_D3D )
#include <Engine/Graphics/Direct3D/Includes.h>
#elif defined( EAE6320_PLATFORM_GL )
#include <Engine/Graphics/OpenGL/Includes.h>
#endif

#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <utility>


namespace eae6320
{
	namespace Graphics
	{
		class cRenderer
		{
		public:

			eae6320::cResult InitializeViews(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight);

			void ClearImageBuffer(eae6320::Graphics::s_colorData);
			void ClearDepthBuffer();
			void DrawFrontBuffer();

			eae6320::cResult Initialize(const sInitializationParameters& i_initializationParameters);
			eae6320::cResult CleanUp();

		};
	}
}

#endif