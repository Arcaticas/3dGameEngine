#include "cEffect.h"


#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>


eae6320::cResult eae6320::Graphics::cEffect::Load(const char* const i_shaderPath, cEffect*& o_effect)
{

	auto result = Results::Success;
	cEffect* newEffect = nullptr;
	cScopeGuard scopeGuard([&o_effect, &result, &newEffect]
		{
			if (result)
			{
				EAE6320_ASSERT(newEffect != nullptr);
				o_effect = newEffect;
			}
			else
			{
				if (newEffect)
				{
					newEffect->DecrementReferenceCount();
					newEffect = nullptr;
				}
				o_effect = nullptr;
			}
		});


	if (!i_shaderPath)
	{
		EAE6320_ASSERTF(false, "Invalid shader file path");
		return result = Results::Failure;
	}

	newEffect = new (std::nothrow) cEffect();
	if (!newEffect)
	{
		result = Results::OutOfMemory;
		EAE6320_ASSERTF(false, "Couldn't allocate memory for the shader %s");
		Logging::OutputError("Failed to allocate memory for the shader %s");
		return result;
	}

	result = newEffect->Initialize(i_shaderPath);

	return result;
}

void eae6320::Graphics::cEffect::Draw()
{
	this->BindData();

	// Render state
	{
		m_renderState.Bind();
	}

}

eae6320::cResult eae6320::Graphics::cEffect::Initialize(const char* const i_shaderPath)
{

	auto result = Results::Success;
	
	if (!(result = InitializeShadingData(i_shaderPath)))
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

eae6320::Graphics::cEffect::~cEffect()
{

	EAE6320_ASSERT(m_referenceCount == 0);
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}

