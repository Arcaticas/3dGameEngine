/*
	This file should be #included by all shaders
*/

// Version Information
//====================

#if defined( EAE6320_PLATFORM_GL )

	// GLSL shaders require the version to be #defined before anything else in the shader
	#version 420

#endif

// Constant Buffers
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )
	#define FLOAT4X4 float4x4
	#define FLOAT4 float4
	#define FLOAT2 float2
	

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name
	#define FLOAT4X4 mat4
	#define FLOAT4 vec4
	#define FLOAT2 vec2
#endif

DeclareConstantBuffer(g_constantBuffer_frame, 0)
{
	FLOAT4X4 g_transform_worldToCamera;
	FLOAT4X4 g_transform_cameraToProjected;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For float4 alignment
	FLOAT2 g_padding;
};

