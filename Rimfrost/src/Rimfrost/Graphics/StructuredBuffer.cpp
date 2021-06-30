#include "rfpch.hpp"
#include "StructuredBuffer.hpp"
#include <assert.h>

namespace Engine1
{
	StructuredBuffer::StructuredBuffer(std::uint32_t structSize, std::uint32_t structCount)
	{
		this->m_eShader = ShaderEnum::PIXELSHADER;
		this->m_bindSlot = 0;
		this->m_structCount = structCount;
		this->m_structSize = structSize;
		this->createBuffer(structSize, structCount);
	}

	void StructuredBuffer::updateBuffer(const void* data)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubRes;
		this->getContext()->Map(m_pBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubRes);
		memcpy(mappedSubRes.pData, data, m_bufferDesc.ByteWidth);
		this->getContext()->Unmap(m_pBuffer.Get(), 0);
	}

	void StructuredBuffer::bind(std::uint32_t bindSlot, ShaderEnum shader)
	{
		assert(shader == ShaderEnum::PIXELSHADER); //only one supported, fix
		this->m_eShader = shader;
		this->m_bindSlot = bindSlot;
		this->getContext()->PSSetShaderResources(this->m_bindSlot, 1, this->m_structerdBufferSRV.GetAddressOf());
	}

	void StructuredBuffer::unBind()
	{
		ID3D11ShaderResourceView* SRV_NULL = nullptr;
		this->getContext()->PSSetShaderResources(this->m_bindSlot, 1, &SRV_NULL);
		//fixa
	}

	void StructuredBuffer::createBuffer(std::uint32_t structSize, std::uint32_t structCount, const void* data)
	{
		this->m_bufferDesc.ByteWidth = structCount * structSize;
		this->m_bufferDesc.StructureByteStride = structSize;
		this->m_bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		this->m_bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		this->m_bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		this->m_bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		HRESULT hr;
		if (data)
		{
			this->m_subRes.pSysMem = data;
			hr = this->getDevice()->CreateBuffer(&this->m_bufferDesc, &this->m_subRes, &this->m_pBuffer);
			assert(SUCCEEDED(hr));
		}
		else
		{
			hr = this->getDevice()->CreateBuffer(&this->m_bufferDesc, nullptr, &this->m_pBuffer);
			assert(SUCCEEDED(hr));
		}

		//deskriptorn till den SRV som buffern binds till
		this->m_SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		this->m_SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		this->m_SRVDesc.Buffer.FirstElement = 0;
		this->m_SRVDesc.Buffer.ElementWidth = structSize;
		this->m_SRVDesc.Buffer.NumElements = structCount;

		hr = this->getDevice()->CreateShaderResourceView(this->m_pBuffer.Get(), &this->m_SRVDesc, &this->m_structerdBufferSRV);
		assert(SUCCEEDED(hr));
	}
}