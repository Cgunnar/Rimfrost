#include "rfpch.hpp"
#include "Sampler.hpp"

namespace Rimfrost
{


	Sampler::Sampler(D3D11_SAMPLER_DESC desc)
	{
		HRESULT hr = getDevice()->CreateSamplerState(&desc, &m_sampleState);
		assert(SUCCEEDED(hr));
	}

	void Sampler::bind(int bindSlot, ShaderEnum shader)
	{
		if ((shader & ShaderEnum::VERTEXSHADER) == ShaderEnum::VERTEXSHADER)
		{
			getContext()->VSSetSamplers(bindSlot, 1, m_sampleState.GetAddressOf());
		}
		if ((shader & ShaderEnum::GEOMETRYSHADER) == ShaderEnum::GEOMETRYSHADER)
		{
			getContext()->GSSetSamplers(bindSlot, 1, m_sampleState.GetAddressOf());
		}
		if ((shader & ShaderEnum::HULLSHADER) == ShaderEnum::HULLSHADER)
		{
			getContext()->HSSetSamplers(bindSlot, 1, m_sampleState.GetAddressOf());
		}
		if ((shader & ShaderEnum::DOMAINSHADER) == ShaderEnum::DOMAINSHADER)
		{
			getContext()->DSSetSamplers(bindSlot, 1, m_sampleState.GetAddressOf());
		}
		if ((shader & ShaderEnum::PIXELSHADER) == ShaderEnum::PIXELSHADER)
		{
			getContext()->PSSetSamplers(bindSlot, 1, m_sampleState.GetAddressOf());
		}
		if ((shader & ShaderEnum::COMPUTESHADER) == ShaderEnum::COMPUTESHADER)
		{
			getContext()->CSSetSamplers(bindSlot, 1, m_sampleState.GetAddressOf());
		}
	}

	namespace standardSamplers
	{
		const D3D11_SAMPLER_DESC linear_wrap = {
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP,
			0.0f,
			1,
			D3D11_COMPARISON_ALWAYS,
			{0,0,0,0},
			0,
			D3D11_FLOAT32_MAX
		};
	}
}