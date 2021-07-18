#include "rfpch.hpp"
#include <json.hpp>

#include "SerializeECS.hpp"
#include "Rimfrost\EntCom\rfEntity.hpp"
#include "Rimfrost\Utilities\FileUtility.hpp"

namespace Rimfrost
{
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

	struct JComponentInfoStruct
	{
		std::string path;
		std::string typeName;
		ComponentTypeID componentTypeID;
		size_t componentSize;
		size_t componentCount;
	};
	void to_json(nlohmann::json& j, const JComponentInfoStruct& c)
	{
		j = nlohmann::json{ {"path", c.path }, {"typeName", c.typeName}, {"componentTypeID", c.componentTypeID},
					{ "componentSize", c.componentSize }, {"componentCount", c.componentCount} };
	}
	void from_json(const nlohmann::json& j, JComponentInfoStruct& c)
	{
		j.at("path").get_to(c.path);
		j.at("typeName").get_to(c.typeName);
		j.at("componentTypeID").get_to(c.componentTypeID);
		j.at("componentSize").get_to(c.componentSize);
		j.at("componentCount").get_to(c.componentCount);
	}

	//serialize ecs to json
	void SerializeECS::serialize(const std::string& fileName)
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
	void SerializeECS::deSerialize(const std::string& fileName)
	{
		std::ifstream f(fileName);

		nlohmann::json j = nlohmann::json::parse(f);
		std::vector<JComponentInfoStruct> nodes = j["componentInfo"].get<std::vector<JComponentInfoStruct>>();
	}
}