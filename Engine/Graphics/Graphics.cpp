// Includes
//=========

#include "Graphics.h"

#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cRenderState.h"
#include "cShader.h"
#include "sContext.h"
#include "VertexFormats.h"
#include "cRenderer.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Time/Time.h>
#include <Engine/UserOutput/UserOutput.h>
#include <new>
#include <utility>



// Static Data
//============

namespace
{
	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_frame(eae6320::Graphics::ConstantBufferTypes::Frame);

	// Submission Data
	//----------------
	
	
	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		eae6320::Graphics::ConstantBufferFormats::sFrame constantData_frame;
		eae6320::Graphics::s_colorData backgroundColor;
	};
	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be in the process of being populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated and in the process of being rendered from in the render thread
	// (In other words, one is being produced while the other is being consumed)
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	auto* s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	auto* s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;



	eae6320::Graphics::cRenderer RenderData;

	// Geometry Data
	//--------------

	eae6320::Graphics::Geometry GeometryData1;
	eae6320::Graphics::Geometry GeometryData2;

	// Shading Data
	//-------------

	eae6320::Graphics::cEffect ShaderData1;
	eae6320::Graphics::cEffect ShaderData2;
}


// Submission
//-----------

void eae6320::Graphics::SetBackgroundColor(float i_r, float i_g, float i_b, float i_alpha)
{
	s_dataBeingSubmittedByApplicationThread->backgroundColor.r = i_r;
	s_dataBeingSubmittedByApplicationThread->backgroundColor.g = i_g;
	s_dataBeingSubmittedByApplicationThread->backgroundColor.b = i_b;
	s_dataBeingSubmittedByApplicationThread->backgroundColor.alpha = i_alpha;
}

void eae6320::Graphics::SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_frame = s_dataBeingSubmittedByApplicationThread->constantData_frame;
	constantData_frame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_frame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds)
{
	return Concurrency::WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}



// Render
//-------

void eae6320::Graphics::RenderFrame()
{
	// Wait for the application loop to submit data to be rendered
	{
		if (Concurrency::WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread))
		{
			// Switch the render data pointers so that
			// the data that the application just submitted becomes the data that will now be rendered
			std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);
			// Once the pointers have been swapped the application loop can submit new data
			if (!s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal())
			{
				EAE6320_ASSERTF(false, "Couldn't signal that new graphics data can be submitted");
				Logging::OutputError("Failed to signal that new render data can be submitted");
				UserOutput::Print("The renderer failed to signal to the application that new graphics data can be submitted."
					" The application is probably in a bad state and should be exited");
				return;
			}
		}
		else
		{
			EAE6320_ASSERTF(false, "Waiting for the graphics data to be submitted failed");
			Logging::OutputError("Waiting for the application loop to submit data to be rendered failed");
			UserOutput::Print("The renderer failed to wait for the application to submit data to be rendered."
				" The application is probably in a bad state and should be exited");
			return;
		}
	}
	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	{
		RenderData.ClearImageBuffer(s_dataBeingRenderedByRenderThread->backgroundColor);
	}
	// In addition to the color buffer there is also a hidden image called the "depth buffer"
	// which is used to make it less important which order draw calls are made.
	// It must also be "cleared" every frame just like the visible color buffer.
	{
		RenderData.ClearDepthBuffer();
	}

	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);

	// Update the frame constant buffer
	{
		auto& constantData_frame = s_dataBeingRenderedByRenderThread->constantData_frame;
		s_constantBuffer_frame.Update(&constantData_frame);
	}
	// Bind the shading data
	ShaderData1.Draw();
	// Draw the geometry
	GeometryData1.Draw();
	
	ShaderData2.Draw();
	
	GeometryData2.Draw();

	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it the contents of the back buffer must be "presented"
	// (or "swapped" with the "front buffer", which is the image that is actually being displayed)
	{
		RenderData.DrawFrontBuffer();
	}

	// After all of the data that was submitted for this frame has been used
	// you must make sure that it is all cleaned up and cleared out
	// so that the struct can be re-used (i.e. so that data for a new frame can be submitted to it)
	{
		// (At this point in the class there isn't anything that needs to be cleaned up)
		//dataRequiredToRenderFrame	// TODO
	}
}


// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	auto result = Results::Success;

	// Initialize the platform-specific context
	if (!(result = sContext::g_context.Initialize(i_initializationParameters)))
	{
		EAE6320_ASSERTF(false, "Can't initialize Graphics without context");
		return result;
	}
	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_frame.Initialize())
		{
			// There is only a single frame constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_frame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without frame constant buffer");
			return result;
		}
	}

	// Initialize the events
	{
		if (!(result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data has been submitted from the application thread");
			return result;
		}
		if (!(result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
			Concurrency::EventState::Signaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data can be submitted from the application thread");
			return result;
		}
	}

	// Initialize the views
	{
		result = RenderData.Initialize(i_initializationParameters);
	}

	const char* const shaderPath1 = "data/Shaders/Fragment/flasher.shader";
	const char* const shaderPath2 = "data/Shaders/Fragment/flasher2.shader";

	// Initialize the shading data
	{
		result = ShaderData1.Initialize(shaderPath1);
		result = ShaderData2.Initialize(shaderPath2);
	}

	constexpr int stuff1Size = 5;
	eae6320::Graphics::VertexFormats::sVertex_mesh stuff1[stuff1Size];
	{
		stuff1[0].x = 0.0f;
		stuff1[0].y = -1.0f;
		stuff1[0].z = 0.0f;

		stuff1[1].x = .5f;
		stuff1[1].y = 0.0f;
		stuff1[1].z = 0.0f;

		stuff1[2].x = 1.0f;
		stuff1[2].y = -1.0f;
		stuff1[2].z = 0.0f;

		stuff1[3].x = -.5f;
		stuff1[3].y = 0.0f;
		stuff1[3].z = 0.0f;

		stuff1[4].x = -1.0f;
		stuff1[4].y = -1.0f;
		stuff1[4].z = 0.0f;
	}
	const auto stuff2Size = 6;
	uint16_t stuff2[stuff2Size];
	{
		stuff2[0] = 0;
		stuff2[1] = 1;
		stuff2[2] = 2;
		stuff2[3] = 0;
		stuff2[4] = 4;
		stuff2[5] = 3;
	}
	constexpr int stuff3Size = 3;
	eae6320::Graphics::VertexFormats::sVertex_mesh stuff3[stuff3Size];
	{
		stuff3[0].x = 0.0f;
		stuff3[0].y = 1.0f;
		stuff3[0].z = 0.0f;

		stuff3[1].x = .5f;
		stuff3[1].y = 0.0f;
		stuff3[1].z = 0.0f;

		stuff3[2].x = -.5f;
		stuff3[2].y = 0.0f;
		stuff3[2].z = 0.0f;

	}
	const auto stuff4Size = 6;
	uint16_t stuff4[stuff4Size];
	{
		stuff4[0] = 0;
		stuff4[1] = 1;
		stuff4[2] = 2;

	}
	// Initialize the geometry
	{
		result = GeometryData1.Initialize(stuff1, stuff2, stuff1Size, stuff2Size);
		result = GeometryData2.Initialize(stuff3, stuff4, stuff3Size, stuff4Size);
	}
	sizeof(ShaderData1);

	return result;
}


eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	RenderData.CleanUp();

	ShaderData1.CleanUp();

	GeometryData1.CleanUp();

	ShaderData2.CleanUp();

	GeometryData2.CleanUp();

	{
		const auto result_constantBuffer_frame = s_constantBuffer_frame.CleanUp();
		if (!result_constantBuffer_frame)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_frame;
			}
		}
	}

	{
		const auto result_context = sContext::g_context.CleanUp();
		if (!result_context)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_context;
			}
		}
	}

	return result;
}