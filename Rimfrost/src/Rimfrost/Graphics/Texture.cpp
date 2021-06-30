#include "rfpch.hpp"
#include "Texture.hpp"
#include <math.h>
#include <assert.h>

using namespace Microsoft::WRL;
using namespace Engine1;
namespace Engine1
{
	Texture::Texture(bool srgb, const void* data, int width, int height, bool alphaBlending, bool alphaTesting) : m_alphaBlending(alphaBlending), m_alphaTesting(alphaTesting)
	{
		assert(alphaBlending * alphaTesting == 0); // both can not be true
		this->createTexture(srgb, data, height, width);
	}

	Texture::~Texture()
	{


	}

	void Texture::bind(std::uint32_t texBindSlot, ShaderEnum shader)
	{
		this->m_texBindSlot = texBindSlot;
		this->m_eShader = shader;

		switch (this->m_eShader)
		{
		case ShaderEnum::VERTEXSHADER:
		{
			this->getContext()->VSSetShaderResources(m_texBindSlot, 1, this->m_pSRV.GetAddressOf());
			break;
		}
		case ShaderEnum::HULLSHADER:
		{
			this->getContext()->HSSetShaderResources(m_texBindSlot, 1, this->m_pSRV.GetAddressOf());
			break;
		}
		case ShaderEnum::DOMAINSHADER:
		{
			this->getContext()->DSSetShaderResources(m_texBindSlot, 1, this->m_pSRV.GetAddressOf());
			break;
		}
		case ShaderEnum::GEOMETRYSHADER:
		{
			this->getContext()->GSSetShaderResources(m_texBindSlot, 1, this->m_pSRV.GetAddressOf());
			break;
		}
		case ShaderEnum::PIXELSHADER:
		{
			this->getContext()->PSSetShaderResources(m_texBindSlot, 1, this->m_pSRV.GetAddressOf());
			break;
		}
		default:
		{
			assert(false);
			break;
		}
		}
	}

	void Texture::unBind()
	{
		ID3D11ShaderResourceView* SRV_NULL = nullptr;
		ID3D11SamplerState* SAMPLESTATE_NULL = nullptr;
		switch (this->m_eShader)
		{
		case ShaderEnum::VERTEXSHADER:
		{
			this->getContext()->VSSetShaderResources(m_texBindSlot, 1, &SRV_NULL);
			break;
		}
		case ShaderEnum::HULLSHADER:
		{
			this->getContext()->HSSetShaderResources(m_texBindSlot, 1, &SRV_NULL);
			break;
		}
		case ShaderEnum::DOMAINSHADER:
		{
			this->getContext()->DSSetShaderResources(m_texBindSlot, 1, &SRV_NULL);
			break;
		}
		case ShaderEnum::GEOMETRYSHADER:
		{
			this->getContext()->GSSetShaderResources(m_texBindSlot, 1, &SRV_NULL);
			break;
		}
		case ShaderEnum::PIXELSHADER:
		{
			this->getContext()->PSSetShaderResources(m_texBindSlot, 1, &SRV_NULL);
			break;
		}
		default:
		{
			assert(false);
			break;
		}
		}
	}


	int getNumMipLevels(int width, int height)
	{
		return 1 + static_cast<int>(floor(log2(std::max(width, height))));
	}

	void Texture::createTexture(bool srgb, const void* textureData, int height, int width)
	{
		assert(height > 0 && width > 0 && textureData);

		int mipNumber = getNumMipLevels(height, width);

		this->m_stride = width * 4; //hard coded...

		this->m_texture2dDesc.Width = width;
		this->m_texture2dDesc.Height = height;
		this->m_texture2dDesc.MipLevels = mipNumber;
		this->m_texture2dDesc.ArraySize = 1;
		this->m_texture2dDesc.Format = srgb ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
		this->m_texture2dDesc.SampleDesc.Count = 1;
		this->m_texture2dDesc.SampleDesc.Quality = 0;
		//this->m_texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		this->m_texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		//this->m_texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		this->m_texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		this->m_texture2dDesc.CPUAccessFlags = 0;
		this->m_texture2dDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;



		D3D11_SUBRESOURCE_DATA* subResMipArray = nullptr;
		this->setSubResData(textureData, subResMipArray, mipNumber);

		HRESULT hr = this->getDevice()->CreateTexture2D(&this->m_texture2dDesc, subResMipArray, &this->m_pTexture2d);
		assert(SUCCEEDED(hr));

		delete[] subResMipArray;
		this->createSRV();
	}

	int Texture::getHeight() const
	{
		return this->m_texture2dDesc.Height;
	}

	int Texture::getWidth() const
	{
		return this->m_texture2dDesc.Width;
	}

	bool Texture::shouldAlphaBlend() const noexcept
	{
		return m_alphaBlending;
	}

	bool Texture::shouldAlphaTest() const noexcept
	{
		return m_alphaTesting;
	}



	void Texture::setSubResData(const void* dataPtr, D3D11_SUBRESOURCE_DATA*& subResMipArray, int mipNumber)
	{
		assert(dataPtr);

		subResMipArray = new D3D11_SUBRESOURCE_DATA[mipNumber];
		int SysMemPitch = m_stride;

		for (int i = 0; i < mipNumber; i++)
		{
			subResMipArray[i].pSysMem = dataPtr;
			subResMipArray[i].SysMemPitch = SysMemPitch;
			subResMipArray[i].SysMemSlicePitch = 0;
			SysMemPitch >>= 1;
		}
	}

	void Texture::createSRV()
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = this->m_texture2dDesc.Format;
		srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		HRESULT hr = this->getDevice()->CreateShaderResourceView(this->m_pTexture2d.Get(), &srvDesc, &this->m_pSRV);

		this->getContext()->GenerateMips(this->m_pSRV.Get());
		assert(SUCCEEDED(hr));
	}
}