#pragma once

#include "Rimfrost\EntCom\rfEntity.hpp"

namespace Rimfrost
{
	//learn c++20 modules to get encapsulation, these static classes are not nice
	class ECSSerializer
	{
	public:
		struct JComponentInfoStruct
		{
			std::string path;
			std::string typeName;
			ComponentTypeID componentTypeID;
			size_t componentSize;
			size_t componentCount;
		};


		static void serialize(const std::string& fileName);
		static void deSerialize(const std::string& fileName, std::vector<Entity>& outEntities);

	private:
		static std::optional<std::map<ComponentTypeID, ComponentTypeID>> remapTypeID(std::vector<JComponentInfoStruct>& componentsFromJson);
	};
}
