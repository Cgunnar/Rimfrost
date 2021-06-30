#pragma once
#include "VertexShader.hpp"
#include "PixelShader.hpp"
#include "BaseShader.hpp"

namespace Engine1
{
	typedef std::string ShaderID;

	class ShaderRepo
	{
	public:
		ShaderRepo();
		~ShaderRepo();

		static BaseShader& getShader(const ShaderID& id);
		static ShaderID addShader(const ShaderID& filePath, Engine1::ShaderEnum shaderType);

	private:
		static std::unordered_map<ShaderID, std::unique_ptr<BaseShader>> s_shaders;
	};
}
