#include "../Geometry.h"

#include "Includes.h"


#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <new>
#include <utility>

eae6320::cResult eae6320::Graphics::Geometry::InitializeGeometry(eae6320::Graphics::VertexFormats::sVertex_mesh i_vertexInputs[], uint16_t i_indexArray[], int i_vSize, int i_iSize)
{
	triangleCount = (i_iSize / 3);

	auto result = eae6320::Results::Success;

	// Create a vertex array object and make it active
	{
		constexpr GLsizei arrayCount = 1;
		glGenVertexArrays(arrayCount, &m_vertexArrayId);


		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindVertexArray(m_vertexArrayId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex array ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}


	// Create a vertex buffer object and make it active
	{
		constexpr GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &m_vertexBufferId);


		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}
	// Assign the data to the buffer
	{


		GLsizeiptr bufferSize = sizeof(i_vertexInputs[0]) * i_vSize;
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<GLsizeiptr>::max());
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bufferSize), reinterpret_cast<GLvoid*>(i_vertexInputs),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW);




		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}
	// Initialize vertex format
	{
		// The "stride" defines how large a single vertex is in the stream of data
		// (or, said another way, how far apart each position element is)
		constexpr auto stride = static_cast<GLsizei>(sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh));

		// Position (0)
		// 3 floats == 12 bytes
		// Offset = 0
		{
			constexpr GLuint vertexElementLocation = 0;
			constexpr GLint elementCount = 3;
			constexpr GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320::Graphics::VertexFormats::sVertex_mesh, x)));


			const auto errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
	}

	uint16_t* leftHandArray = new uint16_t[i_iSize];

	// Create a index buffer object and make it active
	{
		for (int i = 0; i < i_iSize; i++)
		{
			leftHandArray[i] = i_indexArray[i];
		}
		
		int temp = 0;
		for (int i = 0; i < i_iSize; i++)
		{
			if (i % 3 == 1)
			{
				int temp = i_indexArray[i];
				leftHandArray[i] = i_indexArray[i+1];
				leftHandArray[i+1] = temp;
				
			}
			
		}

		constexpr GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &m_indexBufferObject);

		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject);

			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}
	// Assign the data to the index buffer
	{

		GLsizeiptr indexBufferSize = sizeof(leftHandArray[0]) * i_iSize;
		EAE6320_ASSERT(indexBufferSize <= std::numeric_limits<GLsizeiptr>::max());

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indexBufferSize), reinterpret_cast<GLvoid*>(leftHandArray), GL_STATIC_DRAW);

		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}

	}

	if (leftHandArray)
	{
		delete[](leftHandArray);
	}




	return result;
}

void eae6320::Graphics::Geometry::Draw()
{
	// Bind a specific vertex buffer to the device as a data source
	{
		EAE6320_ASSERT(m_vertexArrayId != 0);
		glBindVertexArray(m_vertexArrayId);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	// Render triangles from the currently-bound vertex buffer
	{
		// The mode defines how to interpret multiple vertices as a single "primitive";
		// a triangle list is defined
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		constexpr GLenum mode = GL_TRIANGLES;
		// As of this comment only a single triangle is drawn
		// (you will have to update this code in future assignments!)
		constexpr unsigned int indexCountPerTriangle = 3;
		auto indexCountToRender = triangleCount * indexCountPerTriangle;
		// It's possible to start rendering primitives in the middle of the stream
		const GLvoid* const offset = 0;
		glDrawElements(mode, static_cast<GLsizei>(indexCountToRender), GL_UNSIGNED_SHORT, offset);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}

}

eae6320::cResult eae6320::Graphics::Geometry::Initialize(eae6320::Graphics::VertexFormats::sVertex_mesh i_vertexInputs[], uint16_t i_indexArray[], int i_vSize, int i_iSize)
{
	auto result = Results::Success;

	{
		if (!(result = InitializeGeometry(i_vertexInputs, i_indexArray, i_vSize, i_iSize)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
			return result;
		}
	}
	return result;
}

eae6320::cResult eae6320::Graphics::Geometry::CleanUp()
{
	auto result = Results::Success;

	if (m_vertexArrayId != 0)
	{
		// Make sure that the vertex array isn't bound
		{
			// Unbind the vertex array
			glBindVertexArray(0);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				if (result)
				{
					result = Results::Failure;
				}
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				Logging::OutputError("OpenGL failed to unbind all vertex arrays before cleaning up geometry: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
		}
		constexpr GLsizei arrayCount = 1;
		glDeleteVertexArrays(arrayCount, &m_vertexArrayId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			Logging::OutputError("OpenGL failed to delete the vertex array: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_vertexArrayId = 0;
	}
	if (m_vertexBufferId != 0)
	{
		constexpr GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &m_vertexBufferId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_vertexBufferId = 0;
	}
	if (m_indexBufferObject != 0)
	{
		constexpr GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &m_indexBufferObject);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_indexBufferObject = 0;
	}

	return result;
}