#pragma once

#include <unordered_map>
#include <any>
#include <queue>
#include <vector>

#ifdef _DEBUG
#include <windows.h>
#include <debugapi.h>
#include <string>
#endif // _DEBUG


namespace Rimfrost
{

	using ComponentTypeID = size_t;
	using ComponentIndex = size_t;
	using EntityIndex = size_t;


	class EntityRegistry;
	class Entity
	{
		friend EntityRegistry;
	public:
		~Entity();

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity(Entity&& other) noexcept
		{
			this->entityIndex = other.entityIndex;
			this->m_entRegRef = other.m_entRegRef;
			//invalidate other
			other.m_entRegRef = nullptr;
			other.entityIndex = -1;
		}
		Entity& operator=(Entity&& other) noexcept
		{
			this->entityIndex = other.entityIndex;
			this->m_entRegRef = other.m_entRegRef;
			//invalidate other
			other.m_entRegRef = nullptr;
			other.entityIndex = -1;
			return *this;
		}
		template<typename T>
		const T* getComponent() const;

		template<typename T>
		const T* addComponent(const T& component) const;

		template<typename T>
		void removeComponent() const;

	private:
		Entity(EntityIndex ID, EntityRegistry* entReg) : entityIndex(ID), m_entRegRef(entReg) {}
		EntityIndex entityIndex;
		EntityRegistry* m_entRegRef;
	};

	struct ComponentMetaData
	{
		ComponentMetaData(ComponentTypeID typeID, ComponentIndex index) : typeID(typeID), index(index) {}
		ComponentTypeID typeID;
		ComponentIndex index;
	};

	class EntityRegistry;
	struct BaseComponent
	{
		friend EntityRegistry;
		static ComponentTypeID registerComponent(size_t size)
		{
			ComponentTypeID compID = s_componentRegister.size();
			s_componentRegister.push_back(size);
			return compID;
		}
	private:

		EntityIndex entityIndex = -1;

		inline static size_t getSize(ComponentTypeID id)
		{
			return s_componentRegister[id];
		}
		inline static std::vector<ComponentTypeID> s_componentRegister;
	};

	template<typename T>
	struct Component : public BaseComponent
	{
		friend EntityRegistry;
		static const ComponentTypeID typeID;
		static const size_t size;

	private:
		static std::vector<T> componentArray;
	};

	template<typename T>
	std::vector<T> Component<T>::componentArray;

	template<typename T>
	const ComponentTypeID Component<T>::typeID = BaseComponent::registerComponent(sizeof(T));

	//init componentSize
	template<typename T>
	const size_t Component<T>::size = sizeof(T);

	class EC;
	class EntityRegistry
	{
		friend EC;
		EntityRegistry() = default;
		EntityRegistry(const EntityRegistry&) = delete;
		EntityRegistry& operator=(const EntityRegistry&) = delete;
		~EntityRegistry() = default;
	public:

		Entity createEntity()
		{
			EntityIndex index;
			if (!m_freeEntitySlots.empty())
			{
				index = m_freeEntitySlots.front();
				m_freeEntitySlots.pop();
				m_entitiesComponentHandles[index].clear();
			}
			else
			{
				index = m_entitiesComponentHandles.size();
				m_entitiesComponentHandles.emplace_back(std::vector<ComponentMetaData>());
			}

			return Entity(index, this);
		}
		void removeEntity(Entity& entity)
		{
			auto& components = m_entitiesComponentHandles[entity.entityIndex];
			for (auto& c : components)
			{
				removeComponent(c);
			}
			m_freeEntitySlots.push(entity.entityIndex);
			m_entitiesComponentHandles[entity.entityIndex].clear();
			entity.m_entRegRef = nullptr;
		}

		void update(double dt)
		{

		}

		template<typename T>
		T* addComponent(const Entity& entity, const T& component);

		template<typename T>
		void removeComponent(const Entity& entity);

		template<typename T>
		T* getComponent(const Entity& entity);

	private:
		void removeComponent(ComponentTypeID type, EntityIndex entIndex)
		{
			auto& entityComponents = m_entitiesComponentHandles[entIndex];
			if (auto it = std::ranges::find_if(entityComponents.begin(), entityComponents.end(),
				[type](ComponentMetaData c) { return type == c.typeID; });
				it != entityComponents.end())
			{
				m_mapTofreeComponentSlots[it->typeID].push(it->index);
				it->typeID = -1;
			}
		}
		void removeComponent(ComponentMetaData& componentInfo)
		{
			m_mapTofreeComponentSlots[componentInfo.typeID].push(componentInfo.index);
			componentInfo.typeID = -1;
		}

	private:
		std::unordered_map<ComponentTypeID, std::any> m_mapToComponentArrays;
		std::vector<std::vector<ComponentMetaData>> m_entitiesComponentHandles;
		std::queue<EntityIndex> m_freeEntitySlots;
		std::unordered_map<ComponentTypeID, std::queue<ComponentIndex>> m_mapTofreeComponentSlots;
	};
	Entity::~Entity()
	{
#ifdef _DEBUG
		OutputDebugString(L"~Entity\tindex: ");
		OutputDebugString(std::to_wstring(entityIndex).c_str());
		OutputDebugString(L"\n");
#endif // _DEBUG

		if (m_entRegRef)
		{
			m_entRegRef->removeEntity(*this);
		}
	}

	class EC
	{
		EC() = delete;
	public:
		static Entity createEntity()
		{
			return m_entRegInstance.createEntity();
		}
		static void removeEntity(Entity& entity)
		{
			m_entRegInstance.removeEntity(entity);
		}

		static void update(double dt)
		{
			m_entRegInstance.update(dt);
		}

		template<typename T>
		static T* addComponent(const Entity& entity, const T& component)
		{
			return m_entRegInstance.addComponent<T>(entity, component);
		}

		template<typename T>
		static void removeComponent(const Entity& entity)
		{
			m_entRegInstance.removeComponent<T>(entity);
		}

		template<typename T>
		static T* getComponent(const Entity& entity)
		{
			return m_entRegInstance.getComponent<T>(entity);
		}

	private:
		inline static EntityRegistry m_entRegInstance;
	};



	template<typename T>
	inline const T* Entity::getComponent() const
	{
		return m_entRegRef->getComponent<T>(*this);
	}

	template<typename T>
	inline const T* Entity::addComponent(const T& component) const
	{
		return m_entRegRef->addComponent<T>(*this, component);
	}

	template<typename T>
	inline void Entity::removeComponent() const
	{
		m_entRegRef->removeComponent<T>(*this);
	}

	template<typename T>
	inline T* EntityRegistry::addComponent(const Entity& entity, const T& comp)
	{
		ComponentTypeID typeID = T::typeID;
		if (!m_mapToComponentArrays.contains(typeID))
		{
			m_mapToComponentArrays[typeID] = std::any_cast<std::vector<T>*>(&comp.componentArray);
			m_mapTofreeComponentSlots[typeID] = std::queue<ComponentIndex>();
		}
		ComponentIndex index;

		auto& freeComponentIndex = m_mapTofreeComponentSlots[typeID];
		if (!freeComponentIndex.empty())
		{
			index = freeComponentIndex.front();
			freeComponentIndex.pop();
			comp.componentArray[index] = comp;
		}
		else
		{
			index = comp.componentArray.size();
			comp.componentArray.push_back(comp);
		}

		T* compPtr = &comp.componentArray[index];
		compPtr->entityIndex = entity.entityIndex;
		m_entitiesComponentHandles[entity.entityIndex].emplace_back(typeID, index);
		return compPtr;
	}

	template<typename T>
	inline void EntityRegistry::removeComponent(const Entity& entity)
	{
		removeComponent(T::typeID, entity.entityIndex);
	}

	template<typename T>
	inline T* EntityRegistry::getComponent(const Entity& entity)
	{
		auto& entityComponents = m_entitiesComponentHandles[entity.entityIndex];
		if (auto it = std::ranges::find_if(entityComponents.begin(), entityComponents.end(),
			[](ComponentMetaData c) { return T::typeID == c.typeID; });
			it != entityComponents.end())
		{
			auto cArr = std::any_cast<std::vector<T>*>(m_mapToComponentArrays[it->typeID]);
			return &(*cArr)[it->index];
		}
		return nullptr;
	}
}
