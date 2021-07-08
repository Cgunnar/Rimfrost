#include "rfpch.hpp"
#include "SceneSerializer.hpp"
#include "json.hpp"
#include "RimfrostMath.hpp"
#include "Scene.hpp"
#include "AssetManager.hpp"
#include "Logger.hpp"



using namespace std;

namespace Rimfrost
{

	void to_json(nlohmann::json& j, const Transform& t)
	{
		j = nlohmann::json{ {"11", t.m_matrix._11}, {"12", t.m_matrix._12}, {"13", t.m_matrix._13}, {"14", t.m_matrix._14},
							{"21", t.m_matrix._21}, {"22", t.m_matrix._22}, {"23", t.m_matrix._23}, {"24", t.m_matrix._24},
							{"31", t.m_matrix._31}, {"32", t.m_matrix._32}, {"33", t.m_matrix._33}, {"34", t.m_matrix._34},
							{"41", t.m_matrix._41}, {"42", t.m_matrix._42}, {"43", t.m_matrix._43}, {"44", t.m_matrix._44} };
	}
	void from_json(const nlohmann::json& j, Transform& t)
	{
		j.at("11").get_to(t.m_matrix._11);
		j.at("12").get_to(t.m_matrix._12);
		j.at("13").get_to(t.m_matrix._13);
		j.at("14").get_to(t.m_matrix._14);

		j.at("21").get_to(t.m_matrix._21);
		j.at("22").get_to(t.m_matrix._22);
		j.at("23").get_to(t.m_matrix._23);
		j.at("24").get_to(t.m_matrix._24);

		j.at("31").get_to(t.m_matrix._31);
		j.at("32").get_to(t.m_matrix._32);
		j.at("33").get_to(t.m_matrix._33);
		j.at("34").get_to(t.m_matrix._34);

		j.at("41").get_to(t.m_matrix._41);
		j.at("42").get_to(t.m_matrix._42);
		j.at("43").get_to(t.m_matrix._43);
		j.at("44").get_to(t.m_matrix._44);
	}

	struct JnodeStruct
	{
		NodeID ID;
		NodeID parenID;
		bool modelParent;
		SubModelID subModelID;
		Transform transform;
		std::string modelPath;
		ModelSettings settings;
		std::string materialName;
	};

	void to_json(nlohmann::json& j, const JnodeStruct& n)
	{
		j = nlohmann::json{ {"ID", n.ID }, {"parentID", n.parenID}, {"modelParent", n.modelParent}, { "subModelID", n.subModelID },
					{ "transform", n.transform }, {"modelPath", n.modelPath}, {"settings", n.settings},
					{ "materialName", n.materialName} };
	}
	void from_json(const nlohmann::json& j, JnodeStruct& n)
	{
		j.at("ID").get_to(n.ID);
		j.at("parentID").get_to(n.parenID);
		j.at("modelParent").get_to(n.modelParent);
		j.at("subModelID").get_to(n.subModelID);
		j.at("transform").get_to(n.transform);
		j.at("modelPath").get_to(n.modelPath);
		j.at("settings").get_to(n.settings);
		j.at("materialName").get_to(n.materialName);

	}




	void SceneSerializer::serialize(const string& fileName, const shared_ptr<Rimfrost::Scene>& scene)
	{
		serialize(fileName, *scene);
	}
	
	void SceneSerializer::serialize(const std::string& fileName, const Scene& scene)
	{
		nlohmann::json j;

		j["camera"] = Rimfrost::Transform(scene.m_camera.GetWorldMatrix());

		Logger::getLogger().debugLog("serialize: numberOfNodes = " + std::to_string(scene.m_nodes.size()) + "\n");

		for (auto& node : scene.m_nodes)
		{
			nlohmann::json jsonNode;

			jsonNode["ID"] = node.m_ID;
			jsonNode["parentID"] = node.m_parentID;
			jsonNode["modelParent"] = node.m_isModelParent;
			jsonNode["transform"] = node.localMatrix;
			

			if (node.m_subModel)
			{
				jsonNode["modelPath"] = node.m_subModel->getModelData().m_filePath;
				jsonNode["subModelID"] = node.m_subModel->getSubModelID();
				jsonNode["settings"] = node.m_subModel->getModelData().m_settings;
				jsonNode["materialName"] = node.m_subModel->getMaterialName();
			}
			else
			{
				jsonNode["modelPath"] = "";
				jsonNode["subModelID"] = SubModelID(-1);
				jsonNode["settings"] = ModelSettings::NONE;
				jsonNode["materialName"] = "";
			}



			j["nodeArray"].push_back(jsonNode);
		}


		std::ofstream o(fileName);
		o << std::setw(4) << j << std::endl;
	}

	void SceneSerializer::deSerialize(const string& fileName, shared_ptr<Rimfrost::Scene>& scene)
	{
		std::ifstream f(fileName);

		auto j = nlohmann::json::parse(f);

		Rimfrost::Transform cameraWoldMatrix = j["camera"];
		scene->m_camera.SetWorldMatrix(*cameraWoldMatrix);


		scene->m_nodes.clear();

		std::vector<JnodeStruct> nodes = j["nodeArray"].get<std::vector<JnodeStruct>>();

		for (auto& n : nodes)
		{
			Node node = Node(n.ID, n.parenID, n.modelParent);
			node.localMatrix = n.transform;
			string modelPath = n.modelPath;
			if (!modelPath.empty())
			{
				ModelID id = AssetManager::addModel(modelPath, n.settings);
				const Model& model = AssetManager::getModel(id);

				node.m_subModel = std::make_optional<SubModel>(model.subModelData[n.subModelID], model.commonData, n.subModelID);
			}

			
			
			scene->m_nodes.push_back(node);

			
		}
		//link childs
		for (auto& n : scene->m_nodes)
		{
			if (n.m_parentID != rootNode)
			{
				scene->m_nodes[n.m_parentID].m_childIDs.push_back(n.m_ID);
			}
		}
		Logger::getLogger().debugLog("deSerialize: numberOfNodes = " + std::to_string(scene->m_nodes.size()) + "\n");

		//serialize("deSerializationTest.json", scene);
	}
}