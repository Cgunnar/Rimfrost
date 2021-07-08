#include "rfpch.hpp"
#include "PixelShader.hpp"

namespace Rimfrost
{
	PixelShader::PixelShader(std::string entryPoint, std::string shaderModel) : BaseShader(Rimfrost::ShaderEnum::PIXELSHADER)
	{
		this->m_entryPoint = entryPoint;
		this->m_shaderModel = shaderModel;
	}

	void PixelShader::init(const std::string& shaderFileName)
	{
		this->compile(shaderFileName);
	}

	void PixelShader::bind()
	{
		this->getContext()->PSSetShader(this->m_shader.Get(), nullptr, 0);
	}

	void PixelShader::unBind()
	{
		this->getContext()->PSSetShader(nullptr, nullptr, 0);
	}

	void PixelShader::compile(const std::string& filePath)
	{
		m_filePath = filePath;
		ID3DBlob* shaderBlob = this->internalCompile();
		this->createShader(shaderBlob);
		shaderBlob->Release();
	}

	void PixelShader::createShader(ID3DBlob* shaderBlob)
	{
		HRESULT hr = this->getDevice()->CreatePixelShader
		(
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize(),
			nullptr,
			&m_shader
		);
		assert(SUCCEEDED(hr));
	}
}