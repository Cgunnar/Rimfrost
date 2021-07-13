#include "rfpch.hpp"
#include <json.hpp>
#include "SceneSerializer.hpp"
#include "RimfrostMath.hpp"
#include "Rimfrost\Scene\IScene.hpp"
#include "AssetManager.hpp"
#include "Logger.hpp"



using namespace std;

namespace Rimfrost
{

	void to_json(nlohmann::json& j, const Matrix& t)
	{
		j = nlohmann::json{ {"00", t[0][0]}, {"01", t[0][1]}, {"02", t[0][2]}, {"03", t[0][3]},
							{"10", t[1][0]}, {"11", t[1][1]}, {"12", t[1][2]}, {"13", t[1][3]},
							{"20", t[2][0]}, {"21", t[2][1]}, {"22", t[2][2]}, {"23", t[2][3]},
							{"30", t[3][0]}, {"31", t[3][1]}, {"32", t[3][2]}, {"33", t[3][3]} };
	}
	void from_json(const nlohmann::json& j, Matrix& t)
	{
		j.at("00").get_to(t[0][0]);
		j.at("01").get_to(t[0][1]);
		j.at("02").get_to(t[0][2]);
		j.at("03").get_to(t[0][3]);

		j.at("10").get_to(t[1][0]);
		j.at("11").get_to(t[1][1]);
		j.at("12").get_to(t[1][2]);
		j.at("13").get_to(t[1][3]);

		j.at("20").get_to(t[2][0]);
		j.at("21").get_to(t[2][1]);
		j.at("22").get_to(t[2][2]);
		j.at("23").get_to(t[2][3]);

		j.at("30").get_to(t[3][0]);
		j.at("31").get_to(t[3][1]);
		j.at("32").get_to(t[3][2]);
		j.at("33").get_to(t[3][3]);
	}

	struct JnodeStruct
	{
		NodeID ID;
		NodeID parenID;
		NodeID coldID;
		bool modelParent;
		SubModelID subModelID;
		Transform transform;
		std::string modelPath;
		ModelSettings settings;
		std::string materialName;
	};

	void to_json(nlohmann::json& j, const JnodeStruct& n)
	{
		j = nlohmann::json{ {"ID", n.ID }, {"parentID", n.parenID}, {"coldID", n.coldID}, { "modelParent", n.modelParent }, {"subModelID", n.subModelID},
					{ "transform", n.transform }, {"modelPath", n.modelPath}, {"settings", n.settings},
					{ "materialName", n.materialName} };
	}
	void from_json(const nlohmann::json& j, JnodeStruct& n)
	{
		j.at("ID").get_to(n.ID);
		j.at("parentID").get_to(n.parenID);
		j.at("coldID").get_to(n.coldID);
		j.at("modelParent").get_to(n.modelParent);
		j.at("subModelID").get_to(n.subModelID);
		j.at("transform").get_to(n.transform);
		j.at("modelPath").get_to(n.modelPath);
		j.at("settings").get_to(n.settings);
		j.at("materialName").get_to(n.materialName);

	}

	
	void SceneSerializer::serialize(const std::string& fileName, IScene& scene)
	{
		const SceneGraph& sceneGraph = scene.sceneGraph();
		const Camera& camera = scene.camera();

		nlohmann::json j;

		j["camera"] = Matrix(camera.GetWorldMatrix());

		Logger::getLogger().debugLog("serialize: numberOfNodes = " + std::to_string(sceneGraph.getNodes().size()) + "\n");

		for (auto& node : sceneGraph.getNodes())
		{
			nlohmann::json jsonNode;

			jsonNode["ID"] = node.m_ID;
			jsonNode["parentID"] = node.m_parentID;
			jsonNode["coldID"] = node.m_coldID;
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

	void SceneSerializer::deSerialize(const string& fileName, IScene& scene)
	{
		SceneGraph& sceneGraph = scene.sceneGraph();
		Camera& camera = scene.camera();


		std::ifstream f(fileName);

		auto j = nlohmann::json::parse(f);

		Rimfrost::Matrix cameraWoldMatrix = j["camera"];
		camera.SetWorldMatrix(cameraWoldMatrix);


		sceneGraph.getNodes().clear();

		std::vector<JnodeStruct> nodes = j["nodeArray"].get<std::vector<JnodeStruct>>();

		for (auto& n : nodes)
		{
			Node node = Node(n.ID, n.parenID, n.coldID, n.modelParent);
			node.localMatrix = n.transform;
			string modelPath = n.modelPath;
			if (!modelPath.empty())
			{
				ModelID id = AssetManager::addModel(modelPath, n.settings);
				const Model& model = AssetManager::getModel(id);

				node.m_subModel = std::make_optional<SubModel>(model.subModelData[n.subModelID], model.commonData, n.subModelID);
			}

			
			
			sceneGraph.getNodes().push_back(node);

			
		}
		//link childs
		for (auto& n : sceneGraph.getNodes())
		{
			if (n.m_parentID != rootNode)
			{
				sceneGraph.getNodes()[n.m_parentID].m_childIDs.push_back(n.m_ID);
			}
		}
		Logger::getLogger().debugLog("deSerialize: numberOfNodes = " + std::to_string(sceneGraph.getNodes().size()) + "\n");
	}
}