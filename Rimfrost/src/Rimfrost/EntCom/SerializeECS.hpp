#pragma once

#include "Rimfrost\EntCom\rfEntity.hpp"
#include "Rimfrost\Scene\SceneGraph.hpp"

namespace Rimfrost
{

	constexpr char saveDirector[] = "Saves/";

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


		static void serialize(std::string fileName);
		static void deSerialize(std::string fileName, std::vector<Entity>& outEntities);
		static void reCoupleWithSceneGraph(SceneGraph& sceneGraph, std::vector<Entity>& allEntities);

	private:
		static std::optional<std::map<ComponentTypeID, ComponentTypeID>> remapTypeID(std::vector<JComponentInfoStruct>& componentsFromJson);
		
	};
}
