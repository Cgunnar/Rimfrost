#include "rfpch.hpp"
#include <json.hpp>

#include "SerializeECS.hpp"
#include "Rimfrost\EntCom\rfEntity.hpp"
#include "Rimfrost\Utilities\FileUtility.hpp"

namespace Rimfrost
{
	void removeIllegalChars(std::string& str)
	{
		std::string illegalChars = "\\/:?\"<>|";
		std::string legalString = "";
		for (int i = 0; i < str.length(); i++)
		{
			if (illegalChars.find(str[i]) == std::string::npos) {
				legalString += str[i];
			}
		}
		str = legalString;
	}

	//serialize ecs to json
	void SerializeECS::serialize(const std::string& fileName)
	{
		nlohmann::json j;
		int cIndex = 0;
		for (auto& c : BaseComponent::s_componentRegister)
		{
			std::string componentPath(c.name);
			removeIllegalChars(componentPath);
			componentPath = "Saves/Components/" + componentPath;
			writefileBin(c.getArrayPointer(), c.componentCount(), c.size, componentPath);
			
			nlohmann::json jcomponentInfo;
			jcomponentInfo["path"] = componentPath;
			jcomponentInfo["componentTypeID"] = cIndex;
			jcomponentInfo["componentSize"] = c.size;
			jcomponentInfo["componentCount"] = c.componentCount();
			
			j["componentInfo"].push_back(jcomponentInfo);
			cIndex++;
		}

		std::ofstream o(fileName);
		o << std::setw(4) << j << std::endl;
	}

	//deserialize ecs from json
	void SerializeECS::deSerialize(const std::string& fileName)
	{

	}
}