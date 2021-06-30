#pragma once
#include "DXAccess.hpp"
#include "BaseShader.hpp"
namespace Engine1
{
	class PixelShader : public BaseShader
	{
	public:
		//PixelShader(const wchar_t* shaderFileName);

		PixelShader(std::string entryPoint = "main", std::string shaderModel = "ps_5_0");

		void init(const std::string& shaderFileName);
		void bind() override;
		void unBind() override;

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shader;


	private:
		void compile(const std::string& filePath) override;
		void createShader(ID3DBlob* shaderBlob) override;
	};

}