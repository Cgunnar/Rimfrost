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
		T* addComponent(Entity entity, T&& component);
		template<typename T>
		T* addComponent(Entity entity, const T& component);


		template<typename T>
		T* getComponent(Entity entity);
		


	private:
		std::unordered_map<ComponentTypeID, std::vector<char>> m_mapToComponentMemory;
		std::vector<std::vector<ComponentMetaData>> m_entitiesComponentHandles;


	private:
	};



	template<typename T>
	inline T* EntityRegistry::addComponent(Entity entity, T&& comp)
	{
		ComponentTypeID typeID = T::typeID;
		std::vector<char>& componentMemory = m_mapToComponentMemory[typeID];
		size_t index = componentMemory.size();
		m_entitiesComponentHandles[entity.entityIndex].emplace_back(typeID, index);

		componentMemory.resize(index + T::size);
		T* component = new(&componentMemory[index]) T(std::forward<T>(comp));

		component->entityIndex = entity.entityIndex;
		return component;
	}
	template<typename T>
	inline T* EntityRegistry::addComponent(Entity entity, const T& comp)
	{
		ComponentTypeID typeID = T::typeID;
		std::vector<char>& componentMemory = m_mapToComponentMemory[typeID];
		size_t index = componentMemory.size();
		m_entitiesComponentHandles[entity.entityIndex].emplace_back(typeID, index);

		componentMemory.resize(index + T::size);
		T* component = new(&componentMemory[index]) T(comp);

		component->entityIndex = entity.entityIndex;
		return component;
	}
	template<typename T>
	inline T* EntityRegistry::getComponent(Entity entity)
	{
		auto& entityComponents = m_entitiesComponentHandles[entity.entityIndex];

		if (auto it = std::ranges::find_if(entityComponents.begin(), entityComponents.end(),
			[](ComponentMetaData c) { return T::typeID == c.typeID; });
			it != entityComponents.end())
		{
			return reinterpret_cast<T*>(&m_mapToComponentMemory[it->typeID][it->index]);
		}
		return nullptr;
	}
}
