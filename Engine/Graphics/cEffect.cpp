#include "cEffect.h"


#include <Engine/Logging/Logging.h>

void eae6320::Graphics::cEffect::Draw()
{
	this->BindData();

	// Render state
	{
		m_renderState.Bind();
	}

}

eae6320::cResult eae6320::Graphics::cEffect::Initialize()
{
	auto result = Results::Success;

	if (!(result = InitializeShadingData()))
	{
		EAE6320_ASSERTF(false, "Can't initialize Graphics without the shading data");
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
	auto result = Results::Success;

#if defined(EAE6320_PLATFORM_GL)
	if (m_programId != 0)
	{
		glDeleteProgram(m_programId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_programId = 0;
	}
#endif
	

	if (m_vertexShader)
	{
		m_vertexShader->DecrementReferenceCount();
		m_vertexShader = nullptr;
	}

	if (m_fragmentShader)
	{
		m_fragmentShader->DecrementReferenceCount();
		m_fragmentShader = nullptr;
	}

	return result;
}

eae6320::Graphics::cEffect::operator bool()
{
#if defined(EAE6320_PLATFORM_GL)
	return m_vertexShader && m_fragmentShader && m_programId != 0;
#endif
	return m_vertexShader && m_fragmentShader;
}

