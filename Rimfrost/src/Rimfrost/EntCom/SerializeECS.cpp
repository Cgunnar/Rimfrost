#include "rfpch.hpp"
#include <json.hpp>

#include "Rimfrost\EntCom\SerializeECS.hpp"
#include "Rimfrost\EntCom\rfComponents.hpp"
#include "Rimfrost\Utilities\FileUtility.hpp"

namespace Rimfrost
{
	std::string removeIllegalChars(const std::string& str);
	void to_json(nlohmann::json& j, const ECSSerializer::JComponentInfoStruct& c);
	void from_json(const nlohmann::json& j, ECSSerializer::JComponentInfoStruct& c);



	//serialize ecs to json
	void ECSSerializer::serialize(std::string path)
	{
		path = path + saveDirector;
		if (!std::filesystem::exists(path)) 
			std::filesystem::create_directories(path);

		//write components
		nlohmann::json j;

		j["numberOfEntitiesAndFreeSlots"] = EntityReg::m_entCompManInstance.m_entitiesComponentHandles.size();

		//save unused slots for entitys
		std::vector<EntityID> freeSlotQueueAsVector;
		while (!EntityReg::m_entCompManInstance.m_freeEntitySlots.empty())
		{
			freeSlotQueueAsVector.push_back(EntityReg::m_entCompManInstance.m_freeEntitySlots.front());
			EntityReg::m_entCompManInstance.m_freeEntitySlots.pop();
		}
		writefileBin(reinterpret_cast<char*>(freeSlotQueueAsVector.data()), freeSlotQueueAsVector.size(), sizeof(EntityID),
			path + freeEntitySlotsFileName);


		size_t cIndex = 0;
		
		std::string componentPath = path + compBin;
		if (!std::filesystem::exists(componentPath))
			std::filesystem::create_directories(componentPath);
		for (auto& c : BaseComponent::s_componentRegister)
		{
			std::string componentPathAndName = componentPath + removeIllegalChars(c.name);
			writefileBin(c.getArrayPointer(), c.componentCount(), c.size, componentPathAndName);

			JComponentInfoStruct cInfo{ componentPathAndName, c.name, cIndex, c.size, c.componentCount() };

			nlohmann::json jc;
			to_json(jc, cInfo);

			j["componentInfo"].push_back(jc);
			cIndex++;
		}

		std::ofstream o(path + compFileName);
		o << std::setw(4) << j << std::endl;
	}

	//deserialize ecs from json
	void ECSSerializer::deSerialize(std::string path)
	{
		path = path + saveDirector;
		if (!EntityReg::getAllEntities().empty())
		{
			throw std::runtime_error("Can only deserialize when ecs is empty.");
		}
		assert(std::filesystem::exists(path + compFileName));
		std::ifstream f(path + compFileName);

		nlohmann::json j = nlohmann::json::parse(f);
		std::vector<JComponentInfoStruct> componentInfoFromFile = j["componentInfo"].get<std::vector<JComponentInfoStruct>>();

		bool remapNeeded = false;
		for (auto& jc : componentInfoFromFile)
		{
			const auto& compFromReg = BaseComponent::s_componentRegister[jc.componentTypeID];
			if (jc.componentTypeID >= BaseComponent::s_componentRegister.size())
			{
				//if more types of components are saved then what this verison supports
				throw std::runtime_error("componentRegister does not contain typeID:" + std::to_string(jc.componentTypeID) +
					" found in file: " + jc.path);
			}
			if (compFromReg.name != jc.typeName)
			{
				remapNeeded = true;
				Logger::getLogger().debugLog("[WARNING!] deSerialize: typeName: " + jc.typeName + " found in file has typeID: " + std::to_string(jc.componentTypeID) +
					"\nwhich is referring to typeName: " + compFromReg.name + " from componentRegistry. \nTrying to remap ID. IF NAMES HAVE BEEN SWAPPED THIS WILL BE A FALSE POSITIVE AND CREATE A BUG!!!\n\n");
			}
		}
		if (remapNeeded)
		{
			auto idRemap = remapTypeID(componentInfoFromFile);
			if (idRemap)
			{
				Logger::getLogger().debugLog("deSerialize: successfully remaped typeID.\n");
			}
			else
			{
				throw std::runtime_error("Components have likely changed name, or does not match the saved components on some other way\n");
			}
		}

		size_t numberOfEntitiesAndFreeSlots = j["numberOfEntitiesAndFreeSlots"];
		EntityReg::m_entCompManInstance.m_entitiesComponentHandles.resize(numberOfEntitiesAndFreeSlots);

		//read componet bin files
		for (const auto& c : componentInfoFromFile)
		{
			size_t byteStride = 0;
#ifdef DEBUG
			//want to know if something is weird, but this should never happen

			readfileBin(nullptr, byteStride, c.path);
			assert(byteStride == c.componentCount * c.componentSize);
#endif // DEBUG

			auto compUtil = BaseComponent::getComponentUtility(c.componentTypeID);
			assert(compUtil.name == c.typeName); //this has already been tested for so i just want to assert the test and remap worked, lol

			compUtil.resizeArrayT(c.componentCount);
			byteStride = c.componentCount * c.componentSize;
			readfileBin(compUtil.getArrayPointer(), byteStride, c.path);


			//create componentData for the entities
			for (size_t i = 0; i < c.componentCount; i++)
			{
				EntityID eID = reinterpret_cast<BaseComponent*>(compUtil.getArrayPointer() + i * c.componentSize)->entityIndex;

				Logger::getLogger().debugLog("entity Index: " + std::to_string(eID) + " has Component: " + c.typeName + "\n");
				EntityReg::m_entCompManInstance.m_entitiesComponentHandles[eID].emplace_back(c.componentTypeID, i, eID);
			}
		}


		//load unused slots for entitys
		std::vector<EntityID> freeSlotQueueAsVector;
		size_t fileSize = 0;
		readfileBin(nullptr, fileSize, path + freeEntitySlotsFileName);
		freeSlotQueueAsVector.resize(fileSize / sizeof(EntityID));
		readfileBin(reinterpret_cast<char*>(freeSlotQueueAsVector.data()), fileSize, path + freeEntitySlotsFileName);
		for (const auto& e : freeSlotQueueAsVector)
		{
			EntityReg::m_entCompManInstance.m_freeEntitySlots.push(e);
		}


		//create the entities and return these

		//reuse freeSlot vector to only create Entities not in freeSlots
		std::ranges::sort(freeSlotQueueAsVector, std::greater<>());
		EntityID index = 0;
		EntityReg::getAllEntities().reserve(EntityReg::m_entCompManInstance.m_entitiesComponentHandles.size() - freeSlotQueueAsVector.size());
		while (index < EntityReg::m_entCompManInstance.m_entitiesComponentHandles.size())
		{
			if (freeSlotQueueAsVector.empty() || index != freeSlotQueueAsVector.back())
			{
				EntityReg::getAllEntities().emplace_back(EntityReg::m_entCompManInstance.createEntityForDeSerialization(index));
			}
			else
			{
				freeSlotQueueAsVector.pop_back();
			}
			index++;
		}
	}

	void ECSSerializer::reCoupleWithSceneGraph(SceneGraph& sceneGraph)
	{
		//scene graph connection
		for (auto& nodeComp : EntityReg::getComponentArray<NodeComponent>())
		{
			nodeComp.nodeHandel.m_sceneRef = std::make_optional(std::reference_wrapper(sceneGraph));
			//coldID is a persisting id that survives when the scenegraph gets serialized
			NodeID coldID = nodeComp.nodeHandel.m_coldNodeID;
			NodeID normalID = nodeComp.nodeHandel.m_nodeID;
			if (auto it = std::ranges::find_if(sceneGraph.getNodes().begin(), sceneGraph.getNodes().end(),
				[coldID, normalID](Node n) { return coldID == n.getColdID(); });
				it != sceneGraph.getNodes().end())
			{
				if (normalID != it->getID())
				{
					nodeComp.nodeHandel.m_nodeID = it->getID();
				}
			}
		}
	}

	std::optional<std::map<ComponentTypeID, ComponentTypeID>> ECSSerializer::remapTypeID(std::vector<JComponentInfoStruct>& componentsFromJson)
	{
		std::map<ComponentTypeID, ComponentTypeID> remap;
		size_t matchingNamesCounter = 0;
		for (size_t i = 0; i < BaseComponent::s_componentRegister.size(); i++)
		{
			const auto& name = BaseComponent::s_componentRegister[i].name;
			if (auto it = std::ranges::find_if(componentsFromJson.begin(), componentsFromJson.end(),
				[&name](JComponentInfoStruct c) { return name == c.typeName; });
				it != componentsFromJson.end())
			{
				it->componentTypeID = i;
				remap.emplace(it->componentTypeID, i);
				matchingNamesCounter++;
			}
		}
		if (matchingNamesCounter == componentsFromJson.size())
		{
			return std::make_optional(remap);
		}
		else
		{
			Logger::getLogger().debugLog("number of names mismatched: " + std::to_string(componentsFromJson.size() - matchingNamesCounter) + "\n");
			return std::nullopt;
		}

	}


	std::string removeIllegalChars(const std::string& str)
	{
		std::string illegalChars = "\\/:?\"<>|";
		std::string legalString = "";
		for (int i = 0; i < str.length(); i++)
		{
			if (illegalChars.find(str[i]) == std::string::npos) {
				legalString += str[i];
			}
		}
		return legalString;
	}


	void to_json(nlohmann::json& j, const ECSSerializer::JComponentInfoStruct& c)
	{
		j = nlohmann::json{ {"path", c.path }, {"typeName", c.typeName}, {"componentTypeID", c.componentTypeID},
					{ "componentSize", c.componentSize }, {"componentCount", c.componentCount} };
	}
	void from_json(const nlohmann::json& j, ECSSerializer::JComponentInfoStruct& c)
	{
		j.at("path").get_to(c.path);
		j.at("typeName").get_to(c.typeName);
		j.at("componentTypeID").get_to(c.componentTypeID);
		j.at("componentSize").get_to(c.componentSize);
		j.at("componentCount").get_to(c.componentCount);
	}

}