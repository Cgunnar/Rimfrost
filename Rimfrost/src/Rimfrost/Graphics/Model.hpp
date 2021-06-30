#pragma once
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "InputLayout.hpp"
#include "MeshStructures.hpp"

namespace Engine1
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
			Engine1::ModelSettings m_settings = Engine1::ModelSettings::NONE;
			std::string m_filePath;

		} commonData;

	};
}