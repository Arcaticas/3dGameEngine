#include "../cRenderer.h"


eae6320::cResult eae6320::Graphics::cRenderer::InitializeViews(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight)
{
	return Results::Success;
}



void eae6320::Graphics::cRenderer::ClearImageBuffer(eae6320::Graphics::s_colorData i_backgroundColor)
{
	{
		glClearColor(i_backgroundColor.r, i_backgroundColor.g, i_backgroundColor.b, i_backgroundColor.alpha);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	{
		constexpr GLbitfield clearColor = GL_COLOR_BUFFER_BIT;
		glClear(clearColor);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
}

void eae6320::Graphics::cRenderer::ClearDepthBuffer()
{
	{
		glDepthMask(GL_TRUE);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		constexpr GLclampd clearToFarDepth = 1.0;
		glClearDepth(clearToFarDepth);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	{
		constexpr GLbitfield clearDepth = GL_DEPTH_BUFFER_BIT;
		glClear(clearDepth);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
}

void eae6320::Graphics::cRenderer::DrawFrontBuffer()
{
	const auto deviceContext = sContext::g_context.deviceContext;
	EAE6320_ASSERT(deviceContext != NULL);
	const auto glResult = SwapBuffers(deviceContext);
	EAE6320_ASSERT(glResult != FALSE);
}


eae6320::cResult eae6320::Graphics::cRenderer::Initialize(const sInitializationParameters& i_initializationParameters)
{
	return Results::Success;
}

eae6320::cResult eae6320::Graphics::cRenderer::CleanUp()
{
	return Results::Success;
}