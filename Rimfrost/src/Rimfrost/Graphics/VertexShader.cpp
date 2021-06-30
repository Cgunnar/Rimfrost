#include "pch.hpp"
#include "VertexShader.hpp"

//VertexShader::VertexShader(const wchar_t* shaderFileName)
//{
//	this->compileShader(shaderFileName);
//}
namespace Engine1
{
	VertexShader::VertexShader(std::string entryPoint, std::string shaderModel) : BaseShader(Engine1::ShaderEnum::VERTEXSHADER)
	{
		this->m_entryPoint = entryPoint;
		this->m_shaderModel = shaderModel;
	}

	VertexShader::~VertexShader()
	{
		if (this->m_shaderBlob) this->m_shaderBlob->Release();
	}

	void VertexShader::init(const std::string& shaderFileName)
	{
		this->compile(shaderFileName);
	}

	void VertexShader::bind()
	{
		this->getContext()->VSSetShader(this->m_shader.Get(), nullptr, 0);
	}

	void VertexShader::unBind()
	{
		this->getContext()->PSSetShader(nullptr, nullptr, 0);
	}

	void VertexShader::compile(const std::string& filePath)
	{
		m_filePath = filePath;
		m_shaderBlob = this->internalCompile();
		this->createShader(m_shaderBlob);
	}

	void VertexShader::createShader(ID3DBlob* shaderBlob)
	{
		HRESULT hr = this->getDevice()->CreateVertexShader
		(
			this->m_shaderBlob->GetBufferPointer(),
			this->m_shaderBlob->GetBufferSize(),
			nullptr,
			&m_shader
		);
		assert(SUCCEEDED(hr));
	}
}