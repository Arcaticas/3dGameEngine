#ifndef EAE6320_GEOMETRY_H
#define EAE6320_GEOMETRY_H

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>
#include "cVertexFormat.h"
#include "cConstantBuffer.h"

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
#if defined( EAE6320_PLATFORM_D3D )
			eae6320::Graphics::cVertexFormat*  m_vertexFormat = nullptr;
			ID3D11Buffer* m_vertexBuffer = nullptr;
#elif defined( EAE6320_PLATFORM_GL )
			// A vertex buffer holds the data for each vertex
			GLuint m_vertexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint m_vertexArrayId = 0;
			//A index buffer hold references to the index of a particular vertex
			GLuint m_indexBufferObject = 0;
#endif

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