#pragma once

#include "BaseComponent.hpp"
#include "Entity.hpp"

namespace Rimfrost
{
	
	class EntityRegistry
	{
	public:
		EntityRegistry() = default;
		EntityRegistry(const EntityRegistry&) = delete;
		EntityRegistry& operator=(const EntityRegistry&) = delete;
		~EntityRegistry();

		Entity createEntity();

		void update(double dt);

		template<typename T>
		T* addComponent(Entity handle, T&& component);


	private:
		std::unordered_map<ComponentTypeID, std::vector<char>> m_mapToComponentMemory;
		std::vector<std::vector<ComponentMetaData>> m_entitiesComponentHandles;


	private:
		/*template<typename T>
		size_t internalAddComponent<T>()*/
	};



	template<typename T>
	inline T* EntityRegistry::addComponent(Entity entity, T&& comp)
	{
		ComponentTypeID typeID = T::typeID;
		std::vector<char>& componentMemory = m_mapToComponentMemory[typeID];
		size_t index = componentMemory.size();;
		m_entitiesComponentHandles[entity.entityIndex].emplace_back(typeID, index);

		componentMemory.resize(index + T::size);
		T* component = new(&componentMemory[index]) T(std::forward<T>(comp));

		component->entityIndex = entity.entityIndex;
		return component;
	}
}
