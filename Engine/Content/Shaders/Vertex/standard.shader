/*
	This is the standard vertex shader

	A vertex shader is responsible for two things:
		* Telling the GPU where the vertex (one of the three in a triangle) should be drawn on screen in a given window
			* The GPU will use this to decide which fragments (i.e. pixels) need to be shaded for a given triangle
		* Providing any data that a corresponding fragment shader will need
			* This data will be interpolated across the triangle and thus vary for each fragment of a triangle that gets shaded
*/

#include <Shaders/shaders.inc>



// Entry Point
//============

#if defined(EAE6320_PLATFORM_GL)

layout( location = 0 ) in vec3 i_vertexPosition_local;

layout(std140, binding = 2) uniform g_constantBuffer_drawCall
{
  mat4 g_transform_localToWorld;
};
#elif defined(EAE6320_PLATFORM_D3D)

cbuffer g_constantBuffer_drawCall : register(b2)
{
	FLOAT4X4 g_transform_localToWorld;
};

#endif



void main(

	// Input
	//======

	// The "semantics" (the keywords in all caps after the colon) are arbitrary,
	// but must match the C call to CreateInputLayout()


#if defined( EAE6320_PLATFORM_D3D )
	// These values come from one of the VertexFormats::sVertex_mesh that the vertex buffer was filled with in C code
	in const float3 i_vertexPosition_local : POSITION,

	// An SV_POSITION value must always be output from every vertex shader
	// so that the GPU can figure out which fragments need to be shaded
	out FLOAT4 o_vertexPosition_projected : SV_POSITION
#endif
)
{
	
	

#if defined( EAE6320_PLATFORM_D3D )
	// Transform the local vertex into world space
	FLOAT4 vertexPosition_world;
	{
		// This will be done in a future assignment.
		// For now, however, local space is treated as if it is the same as world space.
		FLOAT4 vertexPosition_local = FLOAT4(i_vertexPosition_local, 1.0);
		//vertexPosition_world = vertexPosition_local;
		vertexPosition_world = mul(g_transform_localToWorld, vertexPosition_local);
	}

	// Calculate the position of this vertex projected onto the display
	{
		// Transform the vertex from world space into camera space
		FLOAT4 vertexPosition_camera = mul( g_transform_worldToCamera, vertexPosition_world );
		// Project the vertex from camera space into projected space
		o_vertexPosition_projected = mul( g_transform_cameraToProjected, vertexPosition_camera );
	}
#elif defined( EAE6320_PLATFORM_GL )
	// Transform the local vertex into world space
	FLOAT4 vertexPosition_world;
	{
		// This will be done in a future assignment.
		// For now, however, local space is treated as if it is the same as world space.
		FLOAT4 vertexPosition_local = FLOAT4(i_vertexPosition_local, 1.0);
		vertexPosition_world = g_transform_localToWorld * vertexPosition_local;
	}

	{
		// Transform the vertex from world space into camera space
		FLOAT4 vertexPosition_camera = g_transform_worldToCamera * vertexPosition_world;
		// Project the vertex from camera space into projected space
		gl_Position = g_transform_cameraToProjected * vertexPosition_camera;
	}
#endif

}

