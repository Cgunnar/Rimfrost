#include "rfpch.hpp"
#include "SubModel.hpp"

using namespace DirectX;
using namespace std;

namespace Rimfrost
{

	SubModel::SubModel(const EngineMeshSubset& meshInfo, const Model::ModelData& model, SubModelID id)
		: m_textured(false), m_specTextured(false), m_normalMapped(false)
	{
		m_baseVertexLocation = -1;
		m_startIndexLocation = -1;
		m_indexCount = -1;
		m_InstanceID = generateID();
		m_subModelID = id;

		this->initialize(meshInfo, model);
	}

	SubModel::~SubModel()
	{
	}

	void SubModel::initialize(const EngineMeshSubset& meshInfo, const Model::ModelData& model)
	{
		m_model = model;

		m_materialName = meshInfo.name;
		m_baseVertexLocation = meshInfo.vertexStart;
		m_startIndexLocation = meshInfo.indexStart;
		m_indexCount = meshInfo.indexCount;

		m_mat = meshInfo.material;
		m_mat.createConstantBuffer(this->getDevice());


		if ((meshInfo.texTypes & TextureTypes::NORMAL) == TextureTypes::NORMAL)
		{
			std::string normalFilePath = meshInfo.filePath + meshInfo.normalFileName;
			m_normTexture = TextureRepo::addTexture(normalFilePath, false);
			m_normalMapped = true;
		}

		if ((meshInfo.texTypes & TextureTypes::SPECULAR) == TextureTypes::SPECULAR)
		{
			std::string specularFilePath = meshInfo.filePath + meshInfo.specularFileName;
			m_specTexture = TextureRepo::addTexture(specularFilePath, true);
			m_specTextured = true;
		}
		if ((meshInfo.texTypes & TextureTypes::DIFFUSE) == TextureTypes::DIFFUSE)
		{
			std::string diffuseFilePath = meshInfo.filePath + meshInfo.diffuseFileName;
			m_diffTexture = TextureRepo::addTexture(diffuseFilePath, true);
			auto& tex = TextureRepo::getTexture(m_diffTexture)->get();
			if (tex.shouldAlphaBlend()) m_mat.markForAlphaBlending();
			else if (tex.shouldAlphaTest()) m_mat.markForAlphaTesting();

			m_textured = true;
		}

		if ((model.m_settings & ModelSettings::TRANSPARENCY_IS_TWOSIDED) == ModelSettings::TRANSPARENCY_IS_TWOSIDED
			&& ((m_mat.getType() & (MaterialProperties::ALPHA_BLENDING | MaterialProperties::ALPHA_TESTING)) != MaterialProperties::NONE))
		{
			m_mat.addProperty(MaterialProperties::NO_BACKFACE_CULLING);
		}

		if ((model.m_settings & ModelSettings::WIREFRAME) == ModelSettings::WIREFRAME)
		{
			m_mat.addProperty(MaterialProperties::WIREFRAME);
		}

		std::string shaderFolderPath = "../Rimfrost/src/Rimfrost/Shaders/";
		if (meshInfo.texTypes == (TextureTypes::DIFFUSE | TextureTypes::SPECULAR | TextureTypes::NORMAL))
			m_pixelShader = ShaderRepo::addShader(shaderFolderPath + "PhongDiffSpecNormPS.hlsl", ShaderEnum::PIXELSHADER);
		else if (meshInfo.texTypes == (TextureTypes::DIFFUSE | TextureTypes::SPECULAR))
			m_pixelShader = ShaderRepo::addShader(shaderFolderPath + "PhongDiffSpecPS.hlsl", ShaderEnum::PIXELSHADER);
		else if (meshInfo.texTypes == (TextureTypes::DIFFUSE | TextureTypes::NORMAL))
			m_pixelShader = ShaderRepo::addShader(shaderFolderPath + "PhongDiffNormPS.hlsl", ShaderEnum::PIXELSHADER);
		else if (meshInfo.texTypes == TextureTypes::DIFFUSE)
			m_pixelShader = ShaderRepo::addShader(shaderFolderPath + "PhongDiffPS.hlsl", ShaderEnum::PIXELSHADER);
		else if (meshInfo.texTypes == TextureTypes::NONE)
			m_pixelShader = ShaderRepo::addShader(shaderFolderPath + "PhongPS.hlsl", ShaderEnum::PIXELSHADER);
		else
			assert(false); //fix more if statements, but this should never happend

		if ((meshInfo.texTypes & TextureTypes::NORMAL) == TextureTypes::NORMAL)
			m_vertexShader = ShaderRepo::addShader(shaderFolderPath + "normalMapVS.hlsl", ShaderEnum::VERTEXSHADER);
		else
			m_vertexShader = ShaderRepo::addShader(shaderFolderPath + "VertexShader.hlsl", ShaderEnum::VERTEXSHADER);

		m_inputLayout = std::make_shared<InputLayout>();
		m_inputLayout->init(reinterpret_cast<VertexShader&>(ShaderRepo::getShader(m_vertexShader)), model.layout);

	}

	void SubModel::bind() const
	{
		m_model.m_vertexBuffer->bind();
		m_model.m_indexBuffer->bind();

		m_mat.bind(4, this->getContext());

		this->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (m_textured) TextureRepo::getTexture(m_diffTexture)->get().bind();
		if (m_specTextured) TextureRepo::getTexture(m_specTexture)->get().bind(2);
		if (m_normalMapped) TextureRepo::getTexture(m_normTexture)->get().bind(3);

		ShaderRepo::getShader(m_vertexShader).bind();
		ShaderRepo::getShader(m_pixelShader).bind();

		m_inputLayout->bind();
	}

	const std::string& SubModel::getMaterialName() const noexcept
	{
		return m_materialName;
	}

	const Model::ModelData& SubModel::getModelData() const noexcept
	{
		return m_model;
	}

	const uint32_t& SubModel::getIndexCount() const noexcept
	{
		return m_indexCount;
	}

	const uint32_t& SubModel::getStartIndexLocation() const noexcept
	{
		return m_startIndexLocation;
	}

	const int32_t& SubModel::getBaseVertexLocation() const noexcept
	{
		return m_baseVertexLocation;
	}

	const Rimfrost::MaterialProperties& SubModel::getMaterialType() const noexcept
	{
		return m_mat.getType();
	}

	SubModelID SubModel::getSubModelID() const noexcept
	{
		return m_subModelID;
	}

	uint32_t SubModel::generateID()
	{
		static uint32_t id = 0;
		return ++id;
	}

}