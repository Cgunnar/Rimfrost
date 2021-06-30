#pragma once

#include "DXAccess.hpp"
#include "BaseShader.hpp"

namespace Engine1
{
	class VertexShader : public BaseShader
	{
		friend class InputLayout;
	public:
		VertexShader(std::string entryPoint = "main", std::string shaderModel = "vs_5_0");
		~VertexShader();
		void init(const std::string& shaderFileName);
		void bind() override;
		void unBind() override;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shader;
		ID3DBlob* m_shaderBlob = nullptr;

	private:
		void compile(const std::string& filePath) override;
		void createShader(ID3DBlob* shaderBlob) override;
	};
}
