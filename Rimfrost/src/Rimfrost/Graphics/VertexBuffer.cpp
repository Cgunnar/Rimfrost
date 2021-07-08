#include "rfpch.hpp"
#include "VertexBuffer.hpp"
#include "Rimfrost\Utilities\Logger.hpp"
//#include "Logger.hpp"

using namespace Microsoft::WRL;

namespace Rimfrost
{
	VertexBuffer::VertexBuffer()
	{

	}

	VertexBuffer::~VertexBuffer()
	{
		//Logger::getLogger().debugLog("~VertexBuffer\n");
		if (this->m_vertexPtr)
			delete[] this->m_vertexPtr;
	}

	void VertexBuffer::bind()
	{
		assert(this->m_vertexPtr);
		this->getContext()->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &this->m_vertexStride, &this->m_vertexOffset);
	}

	void VertexBuffer::unBind()
	{
		//this->GetDeviceContextPtr(graphics)->IASetVertexBuffers(0, 1, nullptr, 0,0);
	}

	void VertexBuffer::createBuffer(const float* vertexArray, size_t arraySize, uint32_t vertexStride, uint32_t vertexOffset)
	{
		assert(this->m_vertexPtr == nullptr);

		this->m_vertexStride = vertexStride;
		this->m_vertexOffset = vertexOffset;
		this->m_vertexCount = static_cast<uint32_t>(arraySize) / vertexStride;

		this->m_vertexBufferDesc.ByteWidth = static_cast<uint32_t>(arraySize);
		this->m_vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		this->m_vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		this->m_vertexPtr = new float[arraySize];
		memcpy(this->m_vertexPtr, vertexArray, arraySize);

		D3D11_SUBRESOURCE_DATA srData = { 0 };
		srData.pSysMem = this->m_vertexPtr;

		HRESULT hr = this->getDevice()->CreateBuffer(&this->m_vertexBufferDesc, &srData, &this->m_vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	std::uint32_t VertexBuffer::getVertexCount() const
	{
		return this->m_vertexCount;
	}

	ComPtr<ID3D11Buffer>& VertexBuffer::getBuffer()
	{
		return this->m_vertexBuffer;
	}
}