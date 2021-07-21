#pragma once

#include "Rimfrost\EntCom\rfEntity.hpp"
#include "Rimfrost\Scene\SceneGraph.hpp"

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


		static void serialize(std::string fileName);
		static void deSerialize(std::string fileName);
		static void reCoupleWithSceneGraph(SceneGraph& sceneGraph);

	private:
		static constexpr char saveDirector[] = "ecs/";
		static constexpr char compBin[] = "comp_bin/";
		static constexpr char compFileName[] = "comp.json";
		static constexpr char freeEntitySlotsFileName[] = "free_entity_slots.bin";
		static std::optional<std::map<ComponentTypeID, ComponentTypeID>> remapTypeID(std::vector<JComponentInfoStruct>& componentsFromJson);
		
	};
}
