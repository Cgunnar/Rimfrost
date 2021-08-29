#pragma once


#include "MeshStructures.hpp"
#include "Model.hpp"
#include "Engine1.hpp"
namespace Rimfrost
{
	

	

	class AssetManager
	{
	public:
		AssetManager() = default;
		~AssetManager() = default;
		//const std::vector<EngineMeshSubset>&
		static ModelID addModel(const std::string& modelFilePath, ModelSettings modelSettings = ModelSettings::NONE);
		static const Model& getModel(ModelID id);
		static SubMeshTree getSubModelTree(ModelID id);

	private:
		static std::unordered_map<ModelID, Model> s_modelMap;
		//static std::unordered_map<ModelID, SubMeshTree> s_subMeshTreeMap;
		static std::unordered_map<std::string, ModelID> s_filePathToIDMap;
	};

}

