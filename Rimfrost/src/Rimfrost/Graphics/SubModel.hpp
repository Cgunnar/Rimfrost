#pragma once
#include "Texture.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "MeshStructures.hpp"
#include "VertexShader.hpp"
#include "PixelShader.hpp"
#include "InputLayout.hpp"
#include "DXAccess.hpp"
#include "TextureRepo.hpp"
#include "ShaderRepo.hpp"
#include "Material.hpp"
#include "Model.hpp"

namespace Rimfrost
{

	class SubModel : DXAccess
	{
	public:


		SubModel(const EngineMeshSubset& meshInfo, const Model::ModelData& model, SubModelID id);
		~SubModel();



		void bind() const;

		const std::string& getMaterialName() const noexcept;
		const Model::ModelData& getModelData() const noexcept;
		const uint32_t& getIndexCount() const noexcept;
		const uint32_t& getStartIndexLocation() const noexcept;
		const int32_t& getBaseVertexLocation() const noexcept;
		const Rimfrost::MaterialProperties& getMaterialType() const noexcept;
		SubModelID getSubModelID() const noexcept;

	private:
		bool m_textured;
		bool m_specTextured;
		bool m_normalMapped;
		uint32_t m_InstanceID;
		uint32_t m_indexCount;
		uint32_t m_startIndexLocation;
		int32_t m_baseVertexLocation;

		std::string m_materialName;

		SubModelID m_subModelID;
		Model::ModelData m_model;
		std::shared_ptr<InputLayout> m_inputLayout;

		Rimfrost::Material m_mat;

		TextureID m_diffTexture;
		TextureID m_specTexture;
		TextureID m_normTexture;

		ShaderID  m_vertexShader;
		ShaderID  m_pixelShader;
		//std::shared_ptr<VertexShader> m_vertexShader;
		//std::shared_ptr<PixelShader> m_pixelShader;



	private:
		void initialize(const EngineMeshSubset& meshInfo, const Model::ModelData& model);
		uint32_t generateID();

	};

}