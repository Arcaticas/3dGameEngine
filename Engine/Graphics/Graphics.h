/*
	This file declares the external interface for the graphics system
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Includes
//=========

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>
#include <Engine/Physics/sRigidBodyState.h>


#include "Geometry.h"
#include "cEffect.h"

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include <Engine/Windows/Includes.h>
#endif

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{

		//Color data struct
		struct s_colorData
		{
			float r = 0;
			float g = 0;
			float b = 0;
			float alpha = 0;
		};
		//Mesh struct
		struct s_meshData
		{
			eae6320::Graphics::VertexFormats::sVertex_mesh* i_vertexInputs;
			uint16_t* i_indexArray;
			uint16_t i_vSize;
			uint16_t i_iSize;
		};

		struct s_meshEffectPair
		{
			eae6320::Graphics::cEffect* effect = nullptr;
			eae6320::Graphics::Geometry* mesh = nullptr;
		};
		
		// Submission
		//-----------

		// These functions should be called from the application (on the application loop thread)

		void SetBackgroundColor(float, float, float, float);

		eae6320::cResult SubmitGameObject(eae6320::Graphics::Geometry* i_mesh, eae6320::Graphics::cEffect* i_effect, eae6320::Math::cMatrix_transformation i_trans);
		eae6320::cResult SubmitGameCamera(eae6320::Physics::sRigidBodyState i_trans);
		// As the class progresses you will add your own functions for submitting data,
		// but the following is an example (that gets called automatically)
		// of how the application submits the total elapsed times
		// for the frame currently being submitted
		void SubmitElapsedTime( const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime );

		// When the application is ready to submit data for a new frame
		// it should call this before submitting anything
		// (or, said another way, it is not safe to submit data for a new frame
		// until this function returns successfully)
		cResult WaitUntilDataForANewFrameCanBeSubmitted( const unsigned int i_timeToWait_inMilliseconds );
		// When the application has finished submitting data for a frame
		// it must call this function
		cResult SignalThatAllDataForAFrameHasBeenSubmitted();

		// Render
		//-------

		// This is called (automatically) from the main/render thread.
		// It will render a submitted frame as soon as it is ready
		// (i.e. as soon as SignalThatAllDataForAFrameHasBeenSubmitted() has been called)
		void RenderFrame();






		// Initialize / Clean Up
		//----------------------

		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow = NULL;
	#if defined( EAE6320_PLATFORM_D3D )
			uint16_t resolutionWidth = 0, resolutionHeight = 0;
	#elif defined( EAE6320_PLATFORM_GL )
			HINSTANCE thisInstanceOfTheApplication = NULL;
	#endif
#endif
		};

		cResult Initialize( const sInitializationParameters& i_initializationParameters );
		cResult CleanUp();
	}
}

#endif	// EAE6320_GRAPHICS_H
