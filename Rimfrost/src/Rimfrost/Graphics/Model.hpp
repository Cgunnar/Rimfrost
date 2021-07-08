#pragma once
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "InputLayout.hpp"
#include "MeshStructures.hpp"
#include <memory>
#include <vector>

namespace Rimfrost
{

	struct Model
	{
		std::string path;
		ModelSettings settings = ModelSettings::NONE;
		SubMeshTree subModelTree;
		std::vector<EngineMeshSubset> subModelData;
		struct ModelData
		{
			ModelID m_modelID = -1;
			std::shared_ptr<VertexBuffer> m_vertexBuffer;
			std::shared_ptr<IndexBuffer> m_indexBuffer;
			InputLayout::Layout layout = InputLayout::Layout::POS_TEX_NOR;
			Rimfrost::ModelSettings m_settings = Rimfrost::ModelSettings::NONE;
			std::string m_filePath;

		} commonData;

	};
}