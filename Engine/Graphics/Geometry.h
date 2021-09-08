
#ifndef EAE6320_GEOMETRY_H
#define EAE6320_GEOMETRY_H

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
#include <Engine/Windows/Includes.h>
#endif


namespace eae6320
{
	namespace Graphics
	{
		class Geometry
		{
		
			// Geometry Data
			//--------------

			eae6320::Graphics::cVertexFormat* m_vertexFormat = nullptr;

			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* m_vertexBuffer = nullptr;

		public:
			eae6320::cResult InitializeGeometry();
			
			void Draw();
			eae6320::cResult Initialize();
			eae6320::cResult CleanUp();

			operator bool();
		};
	}
}


#endif	// EAE6320_GEOMETRY_H