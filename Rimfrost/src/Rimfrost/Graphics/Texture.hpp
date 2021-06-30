#pragma once

#include "DXAccess.hpp"
#include "Engine1.hpp"


namespace Engine1
{
	class Texture : public DXAccess
	{
	public:

		//Texture();
		Texture(bool srgb, const void* data, int width, int height, bool alphaBlending = false, bool alphaTesting = false);
		//Texture(const Texture&) = delete;
		//Texture& operator =(const Texture&) = delete;
		~Texture();

		void bind(std::uint32_t texBindSlot = 0, Engine1::ShaderEnum shader = Engine1::ShaderEnum::PIXELSHADER);
		void unBind();


		int getHeight() const;
		int getWidth() const;
		bool shouldAlphaBlend() const noexcept;
		bool shouldAlphaTest() const noexcept;

		/*ID3D11ShaderResourceView* GetSRV();
		ID3D11SamplerState* GetSampleState();*/


	private:
		int m_stride = 0;
		bool m_alphaBlending;
		bool m_alphaTesting;

		std::uint32_t m_texBindSlot = 0;

		Engine1::ShaderEnum m_eShader;

		D3D11_TEXTURE2D_DESC m_texture2dDesc;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture2d;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;

		void createTexture(bool srgb, const void* textureData, int height, int width);
		void setSubResData(const void* dataPtr, D3D11_SUBRESOURCE_DATA*& subResMipArray, int mipNumber);
		void createSRV();
	};

}