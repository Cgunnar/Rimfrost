#include "rfpch.hpp"
#include "IndexBuffer.hpp"

namespace Engine1
{

	void IndexBuffer::initialize(uint32_t* indices, uint32_t count)
	{
		//create indexbuffer
		D3D11_BUFFER_DESC indexDesc;
		indexDesc.ByteWidth = count * sizeof(uint32_t);
		indexDesc.MiscFlags = 0;
		indexDesc.StructureByteStride = 0;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA subres;

		subres.pSysMem = indices;

		HRESULT hr = this->getDevice()->CreateBuffer(&indexDesc, &subres, &m_indexBuffer);
		assert(SUCCEEDED(hr));
	}

	void IndexBuffer::bind()
	{
		this->getContext()->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	void IndexBuffer::unBind()
	{
		this->getContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	}
}