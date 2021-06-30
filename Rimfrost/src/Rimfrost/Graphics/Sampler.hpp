#pragma once

#include "Engine1.hpp"
#include "DXAccess.hpp"

namespace Engine1
{	

	namespace standardSamplers
	{
		extern const D3D11_SAMPLER_DESC linear_wrap;
	}

	class Sampler : DXAccess
	{
	public:
		Sampler(D3D11_SAMPLER_DESC desc);
		~Sampler() = default;
	
		void bind(int bindSlot, ShaderEnum shader);
	
	private:
		//int m_id = -1;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampleState;
	};


	
}
