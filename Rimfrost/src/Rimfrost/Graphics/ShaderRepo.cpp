#include "rfpch.hpp"
#include "ShaderRepo.hpp"

using namespace std;

namespace Rimfrost
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

    ShaderID ShaderRepo::addShader(const ShaderID& filePath, Rimfrost::ShaderEnum shaderType)
    {
        if (s_shaders.contains(filePath)) return filePath; //shader exists

        std::unique_ptr<BaseShader> newShader;

        switch (shaderType)
        {
        case Rimfrost::ShaderEnum::NONE:
            break;
        case Rimfrost::ShaderEnum::VERTEXSHADER:
            newShader = make_unique<VertexShader>();
            break;
        case Rimfrost::ShaderEnum::HULLSHADER:
            break;
        case Rimfrost::ShaderEnum::DOMAINSHADER:
            break;
        case Rimfrost::ShaderEnum::GEOMETRYSHADER:
            break;
        case Rimfrost::ShaderEnum::PIXELSHADER:
            newShader = make_unique<PixelShader>();
            break;
        case Rimfrost::ShaderEnum::COMPUTESHADER:
            break;
        default:
            break;
        }
        newShader->compile(filePath);
        s_shaders.insert_or_assign(filePath, move(newShader));
        return filePath;
    }
}