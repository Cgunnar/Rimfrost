#include "rfpch.hpp"
#include "AssetManager.hpp"
#include "AssimpLoader.hpp"
#include <SubModel.hpp>

namespace Rimfrost
{
	using namespace std;

	unordered_map<ModelID, Model> AssetManager::s_modelMap;
	unordered_map<ModelID, SubMeshTree> AssetManager::s_subMeshTreeMap;
	unordered_map<string, ModelID> AssetManager::s_filePathToIDMap;

	

	ModelID AssetManager::addModel(const std::string& modelFilePath, ModelSettings modelSettings)
	{
		static ModelID newID = 0;

		string key = modelFilePath + std::to_string((int)modelSettings);
		if (s_filePathToIDMap.contains(key))
		{
			return s_filePathToIDMap[key]; // model exists
		}

		//save pathName
		s_filePathToIDMap.insert_or_assign(key, newID);


		AssimpLoader loader;
		EngineMeshData meshData = loader.loadStaticModel(modelFilePath);

		Model m;
		m.path = modelFilePath;
		m.settings = modelSettings;
		m.subModelData = meshData.subMeshesVector;
		m.subModelTree = meshData.subsetsInfo;

		m.commonData.m_modelID = newID;
		m.commonData.m_filePath = modelFilePath;
		m.commonData.m_settings = modelSettings;
		m.commonData.m_vertexBuffer = make_shared<VertexBuffer>();
		m.commonData.m_indexBuffer = make_shared<IndexBuffer>();

		m.commonData.m_vertexBuffer->createBuffer(meshData.getVertextBuffer(), meshData.getVertexCount() * meshData.getVertexSize(), static_cast<uint32_t>(meshData.getVertexSize()));
		m.commonData.m_indexBuffer->initialize((uint32_t*)meshData.getIndicesData(), static_cast<uint32_t>(meshData.getIndicesCount()));

		if (meshData.hasNormalMaps())
			m.commonData.layout = InputLayout::Layout::POS_TEX_NOR_TAN_BITAN;
		else
			m.commonData.layout = InputLayout::Layout::POS_TEX_NOR;

		
		


		s_modelMap.insert_or_assign(newID, m);
		
		return newID++;
	}

	const Model& AssetManager::getModel(ModelID id)
	{
		if (s_modelMap.contains(id)) return s_modelMap[id];
		
		assert(false); //no model exists
		return s_modelMap[0];
	}

	SubMeshTree AssetManager::getSubModelTree(ModelID id)
	{
		assert(false); //fix
		return SubMeshTree();
	}

}