#pragma once
#include "DXAccess.hpp"
#include "Engine1.hpp"

namespace Engine1
{
	class StructuredBuffer : public DXAccess
	{
	public:


		StructuredBuffer() = default;
		StructuredBuffer(std::uint32_t structSize, std::uint32_t structCount);
		~StructuredBuffer() = default;

		void updateBuffer(const void* data);

		void bind(std::uint32_t bindSlot, ShaderEnum shader = ShaderEnum::PIXELSHADER);
		void unBind();


	private:
		void createBuffer(std::uint32_t structSize, std::uint32_t structCount, const void* data = nullptr);

		std::uint32_t m_structSize = 0;
		std::uint32_t m_structCount = 0;
		std::uint32_t m_bindSlot = 0;
		ShaderEnum m_eShader;
		D3D11_SUBRESOURCE_DATA m_subRes = { 0 };
		D3D11_BUFFER_DESC m_bufferDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC m_SRVDesc;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_structerdBufferSRV;
	};
}
