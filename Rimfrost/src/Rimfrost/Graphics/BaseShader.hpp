#pragma once
#include "Engine1.hpp"
#include "DXAccess.hpp"

namespace Engine1
{

	class BaseShader : public DXAccess
	{
	public:
		BaseShader(ShaderEnum shaderType/* = Engine1::ShaderEnum::NONE*/);
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
