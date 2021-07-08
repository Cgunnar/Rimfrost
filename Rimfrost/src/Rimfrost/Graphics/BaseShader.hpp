#pragma once
#include "Engine1.hpp"
#include "DXAccess.hpp"
#include <string>

namespace Rimfrost
{

	class BaseShader : public DXAccess
	{
	public:
		BaseShader(ShaderEnum shaderType/* = Rimfrost::ShaderEnum::NONE*/);
		virtual ~BaseShader();

		virtual void compile(const std::string& filePath) = 0;
		virtual void bind() = 0;
		virtual void unBind() = 0;

	protected:
		ID3DBlob* internalCompile();
		virtual void createShader(ID3DBlob* shaderBlob) = 0;
	protected:
		ShaderEnum m_shaderType;
		std::string m_filePath;
		std::string m_entryPoint;
		std::string m_shaderModel;

	};
}
