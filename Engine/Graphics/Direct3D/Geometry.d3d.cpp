#include "../Geometry.h"

#include "Includes.h"

#include "../sContext.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>



eae6320::cResult eae6320::Graphics::Geometry::InitializeGeometry(eae6320::Graphics::VertexFormats::sVertex_mesh i_vertexInputs[], uint16_t i_indexArray[], int i_vSize, int i_iSize)
{
	EAE6320_ASSERTF(i_iSize % 3 == 0, "Index Array not divisible by 3");
	triangleCount = (i_iSize / 3);

	auto result = Results::Success;

	auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);

	// Vertex Format
	{
		if (!(result = eae6320::Graphics::cVertexFormat::Load(eae6320::Graphics::eVertexType::Mesh, m_vertexFormat,
			"data/Shaders/Vertex/vertexInputLayout_mesh.shader")))
		{
			EAE6320_ASSERTF(false, "Can't initialize geometry without vertex format");
			return result;
		}
	}
	// Vertex Buffer
	{

		auto bufferSize = sizeof(i_vertexInputs[0]) * i_vSize;
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());
		auto bufferDescription = [bufferSize]
		{
			D3D11_BUFFER_DESC bufferDescription{};

			bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
			bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;	// Not used

			return bufferDescription;
		}();

		const auto initialData = [i_vertexInputs]
		{
			D3D11_SUBRESOURCE_DATA initialData{};

			initialData.pSysMem = i_vertexInputs;
			// (The other data members are ignored for non-texture buffers)

			return initialData;
		}();

		const auto result_create = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &m_vertexBuffer);
		if (FAILED(result_create))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "3D object vertex buffer creation failed (HRESULT %#010x)", result_create);
			eae6320::Logging::OutputError("Direct3D failed to create a 3D object vertex buffer (HRESULT %#010x)", result_create);
			return result;
		}
	}

	//Index Buffer
	{

		auto bufferSize = sizeof(i_indexArray[0]) * i_iSize;
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());
		auto bufferDescription = [bufferSize]
		{
			D3D11_BUFFER_DESC bufferDescription{};

			bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
			bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;	// Not used

			return bufferDescription;
		}();

		const auto initialData = [i_indexArray]
		{
			D3D11_SUBRESOURCE_DATA initialData{};

			initialData.pSysMem = i_indexArray;
			// (The other data members are ignored for non-texture buffers)

			return initialData;
		}();

		const auto result_create = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &m_indexBuffer);
		if (FAILED(result_create))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "3D object vertex buffer creation failed (HRESULT %#010x)", result_create);
			eae6320::Logging::OutputError("Direct3D failed to create a 3D object vertex buffer (HRESULT %#010x)", result_create);
			return result;
		}
	}

	return result;
}

void eae6320::Graphics::Geometry::Draw()
{
	


	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	{
		// Bind a specific vertex buffer to the device as a data source
		{
			
			EAE6320_ASSERT(m_vertexBuffer != nullptr);
			constexpr unsigned int startingSlot = 0;
			constexpr unsigned int vertexBufferCount = 1;
			// The "stride" defines how large a single vertex is in the stream of data
			constexpr unsigned int bufferStride = sizeof(VertexFormats::sVertex_mesh);
			// It's possible to start streaming data in the middle of a vertex buffer
			constexpr unsigned int bufferOffset = 0;
			direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &m_vertexBuffer, &bufferStride, &bufferOffset);
		}
		// Bind index buffer 
		{
			EAE6320_ASSERT(m_indexBuffer);
			constexpr DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
			// The indices start at the beginning of the buffer
			constexpr unsigned int offset = 0;
			direct3dImmediateContext->IASetIndexBuffer(m_indexBuffer, indexFormat, offset);
		}

		// Specify what kind of data the vertex buffer holds
		{
			// Bind the vertex format (which defines how to interpret a single vertex)
			{
				EAE6320_ASSERT(m_vertexFormat != nullptr);
				m_vertexFormat->Bind();
			}
			// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
			// the vertex buffer was defined as a triangle list
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}




		// Render triangles from the currently-bound vertex buffer
		{
			// As of this comment only a single triangle is drawn
			// (you will have to update this code in future assignments!)
			constexpr unsigned int indexCountPerTriangle = 3;
			auto indexCountToRender = triangleCount * indexCountPerTriangle;
			// It's possible to start rendering primitives in the middle of the stream
			constexpr unsigned int indexOfFirstIndexToUse = 0;
			constexpr unsigned int offsetToAddToEachIndex = 0;
			direct3dImmediateContext->DrawIndexed(static_cast<unsigned int>(indexCountToRender), indexOfFirstIndexToUse, offsetToAddToEachIndex);
		}
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

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
	if (m_vertexFormat)
	{
		m_vertexFormat->DecrementReferenceCount();
		m_vertexFormat = nullptr;
	}
	if (m_indexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	return result;
}

eae6320::Graphics::Geometry::operator bool()
{
	return m_vertexBuffer && m_vertexFormat;
}



