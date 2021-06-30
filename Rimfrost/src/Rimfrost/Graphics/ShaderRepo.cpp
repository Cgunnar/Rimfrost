#include "rfpch.hpp"
#include "ShaderRepo.hpp"

using namespace std;

namespace Engine1
{
    unordered_map<ShaderID, unique_ptr<BaseShader>> ShaderRepo::s_shaders;

    ShaderRepo::ShaderRepo()
    {
    }

    ShaderRepo::~ShaderRepo()
    {
    }

    BaseShader& ShaderRepo::getShader(const ShaderID& id)
    {
        assert(s_shaders.contains(id));
        return *s_shaders[id];
    }

    ShaderID ShaderRepo::addShader(const ShaderID& filePath, Engine1::ShaderEnum shaderType)
    {
        if (s_shaders.contains(filePath)) return filePath; //shader exists

        std::unique_ptr<BaseShader> newShader;

        switch (shaderType)
        {
        case Engine1::ShaderEnum::NONE:
            break;
        case Engine1::ShaderEnum::VERTEXSHADER:
            newShader = make_unique<VertexShader>();
            break;
        case Engine1::ShaderEnum::HULLSHADER:
            break;
        case Engine1::ShaderEnum::DOMAINSHADER:
            break;
        case Engine1::ShaderEnum::GEOMETRYSHADER:
            break;
        case Engine1::ShaderEnum::PIXELSHADER:
            newShader = make_unique<PixelShader>();
            break;
        case Engine1::ShaderEnum::COMPUTESHADER:
            break;
        default:
            break;
        }
        newShader->compile(filePath);
        s_shaders.insert_or_assign(filePath, move(newShader));
        return filePath;
    }
}