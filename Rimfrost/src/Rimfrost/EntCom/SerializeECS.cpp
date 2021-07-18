#include "rfpch.hpp"

#include <stdexcept>
#include <json.hpp>

#include "SerializeECS.hpp"
#include "Rimfrost\EntCom\rfEntity.hpp"
#include "Rimfrost\Utilities\FileUtility.hpp"

namespace Rimfrost
{
	std::string removeIllegalChars(const std::string& str);
	void to_json(nlohmann::json& j, const ECSSerializer::JComponentInfoStruct& c);
	void from_json(const nlohmann::json& j, ECSSerializer::JComponentInfoStruct& c);



	//serialize ecs to json
	void ECSSerializer::serialize(const std::string& fileName)
	{
		nlohmann::json j;
		size_t cIndex = 0;
		for (auto& c : BaseComponent::s_componentRegister)
		{
			std::string componentPath = "Saves/Components/" + removeIllegalChars(c.name);
			writefileBin(c.getArrayPointer(), c.componentCount(), c.size, componentPath);

			JComponentInfoStruct cInfo{ componentPath, c.name, cIndex, c.size, c.componentCount() };

			nlohmann::json jc;
			to_json(jc, cInfo);

			j["componentInfo"].push_back(jc);
			cIndex++;
		}

		std::ofstream o(fileName);
		o << std::setw(4) << j << std::endl;
	}

	//deserialize ecs from json
	void ECSSerializer::deSerialize(const std::string& fileName)
	{
		std::ifstream f(fileName);

		nlohmann::json j = nlohmann::json::parse(f);
		std::vector<JComponentInfoStruct> componentInfo = j["componentInfo"].get<std::vector<JComponentInfoStruct>>();

		bool remapNeeded = false;
		for (auto& jc : componentInfo)
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
			}
		}
		if (remapNeeded)
		{
			Logger::getLogger().debugLog("deSerialize: typeName does not match typID, trying to remap ID. IF NAMES HAVE BEEN SWAPPED THIS WILL BE A FALSE POSITIVE AND CREATE A BUG!!!\n");
			if (remapTypeID(componentInfo))
			{
				Logger::getLogger().debugLog("deSerialize: successfully remaped typeID.\n");
			}
			else
			{
				throw std::runtime_error("Components have likely changed name, or does not match the saved components on some other way\n");
			}
		}
	}

	bool ECSSerializer::remapTypeID(std::vector<JComponentInfoStruct>& componentsFromJson)
	{
		size_t matchingNamesCounter = 0;
		for (size_t i = 0; i < BaseComponent::s_componentRegister.size(); i++)
		{
			const auto& name = BaseComponent::s_componentRegister[i].name;
			if (auto it = std::ranges::find_if(componentsFromJson.begin(), componentsFromJson.end(),
				[&name](JComponentInfoStruct c) { return name == c.typeName; });
				it != componentsFromJson.end())
			{
				it->componentTypeID = i;
				matchingNamesCounter++;
			}
		}
		if (matchingNamesCounter == componentsFromJson.size())
		{
			return true;
		}
		else
		{
			Logger::getLogger().debugLog("number of names mismatched: " + std::to_string(componentsFromJson.size() - matchingNamesCounter) + "\n");
			return false;
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