#pragma once

#include <unordered_map>
#include <any>
#include <queue>
#include <vector>
#include <type_traits>
#include <functional>
#include <assert.h>

#ifdef _DEBUG
#include <windows.h>
#include <debugapi.h>
#include <string>
#include "Logger.hpp"
#endif // _DEBUG


namespace Rimfrost
{

	using ComponentTypeID = size_t;
	using ComponentIndex = size_t;
	using EntityID = size_t;

	class EntityComponentManager;
	class EntityReg;
	class ECSSerializer;

	
	class Entity
	{
		friend EntityComponentManager;
	public:
		~Entity();

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity(Entity&& other) noexcept
		{
			this->m_entityIndex = other.m_entityIndex;
			this->m_entCompManRef = other.m_entCompManRef;
			//invalidate other
			other.m_entCompManRef = nullptr;
			other.m_entityIndex = -1;
		}
		Entity& operator=(Entity&& other) noexcept
		{
			this->m_entityIndex = other.m_entityIndex;
			this->m_entCompManRef = other.m_entCompManRef;
			//invalidate other
			other.m_entCompManRef = nullptr;
			other.m_entityIndex = -1;
			return *this;
		}
		template<typename T>
		T* getComponent();

		template<typename T>
		requires std::is_trivially_copy_assignable_v<T>
			T* addComponent(const T& component) const;

		template<typename T>
		void removeComponent() const;

		EntityID getID() const { return m_entityIndex; }

		operator const EntityID() const { return m_entityIndex; }

	private:
		Entity(EntityID ID, EntityComponentManager* entReg) : m_entityIndex(ID), m_entCompManRef(entReg) {}
		Entity() : m_entityIndex(-1), m_entCompManRef(nullptr) {}
		EntityID m_entityIndex;
		EntityComponentManager* m_entCompManRef;
	};

	struct ComponentMetaData
	{
		ComponentMetaData(ComponentTypeID typeID, ComponentIndex index, EntityID entIndex) : typeID(typeID), index(index), entityIndex(entIndex) {}
		ComponentTypeID typeID;
		ComponentIndex index;
		EntityID entityIndex;
	};


	

	class BaseComponent
	{
		friend EntityComponentManager;
		friend ECSSerializer;
	public:
		EntityID getEntityID() const { return entityIndex; }


	protected:
		static ComponentTypeID registerComponent(size_t size, std::string name, std::function<ComponentIndex(BaseComponent*)> createFunc, 
			std::function<BaseComponent* (ComponentIndex)> fetchFunc, std::function<EntityID(ComponentIndex)> deleteFunc,
			std::function<void(size_t)> resize, std::function<char*()> getArray, std::function<size_t()> count)
		{
			ComponentTypeID compID = s_componentRegister.size();
			ComponentUtility comUtil;
			comUtil.size = size;
			comUtil.name = std::move(name);
			comUtil.createComponent = createFunc;
			comUtil.fetchComponentAsBase = fetchFunc;
			comUtil.deleteComponent = deleteFunc;
			comUtil.getArrayPointer = getArray;
			comUtil.resizeArrayT = resize;
			comUtil.componentCount = count;
			s_componentRegister.push_back(comUtil);
			return compID;
		}

		EntityID entityIndex = -1;
	private:


		struct ComponentUtility
		{
			size_t size;
			std::string name;
			std::function<ComponentIndex(BaseComponent*)> createComponent;
			std::function<BaseComponent* (ComponentIndex)> fetchComponentAsBase;
			std::function<EntityID(ComponentIndex)> deleteComponent;
			std::function<void(size_t)> resizeArrayT;
			std::function<char*()> getArrayPointer;
			std::function<size_t()> componentCount;

		};

		inline static size_t getSize(ComponentTypeID id)
		{
			return s_componentRegister[id].size;
		}
		inline static std::function<ComponentIndex(BaseComponent*)> getCreateComponentFunction(ComponentTypeID id)
		{
			return s_componentRegister[id].createComponent;
		}
		inline static ComponentUtility getComponentUtility(ComponentTypeID id)
		{
			return s_componentRegister[id];
		}

		inline static std::vector<ComponentUtility> s_componentRegister;
	};


	
	

	template<typename T>
	struct Component : public BaseComponent
	{
		friend EntityComponentManager;
		friend EntityReg;
		static const ComponentTypeID typeID;
		static const size_t size;
		static const std::string componentName;

#ifdef DEBUG
		~Component()
		{
			auto& v = componentArray;
			Logger::getLogger().debugLog("~" + componentName + "(), number of components deleted: " + std::to_string(v.size()) + "\n");
		}
#endif // DEBUG

		

	private:
		template<typename T>
		requires std::is_trivially_copy_assignable_v<T>
		static ComponentIndex createComponent(BaseComponent* comp)
		{	

			componentArray.push_back(*static_cast<T*>(comp));
			return componentArray.size() - 1;
		}
		template<typename T>
		static BaseComponent* fetchComponent(ComponentIndex index)
		{
			return static_cast<BaseComponent*>(&componentArray[index]);
		}

		template<typename T>
		static EntityID deleteComponent(ComponentIndex index)
		{
			EntityID entityOwningBackComponent = -1;
			if (index + 1 != componentArray.size()) // last component
			{
				componentArray[index] = componentArray.back();
				//componentArray[index].
				entityOwningBackComponent = componentArray[index].entityIndex;
			}
			componentArray.pop_back();
			return entityOwningBackComponent;
		}

		static void resizeComponentArray(size_t elements)
		{
			componentArray.resize(elements);
		}

		static char* getArrayStartPointer()
		{
			return reinterpret_cast<char*>(componentArray.data());
		}

		static size_t getCount()
		{
			return componentArray.size();
		}

		static std::vector<T> componentArray;
	};

	template<typename T>
	std::vector<T> Component<T>::componentArray;

	template<typename T>
	const ComponentTypeID Component<T>::typeID = BaseComponent::registerComponent(sizeof(T), typeid(T).name(), Component<T>::createComponent<T>,
		Component<T>::fetchComponent<T>, Component<T>::deleteComponent<T>, Component<T>::resizeComponentArray, Component<T>::getArrayStartPointer,
		Component<T>::getCount);

	//init componentSize
	template<typename T>
	const size_t Component<T>::size = sizeof(T);

	template<typename T>
	const std::string Component<T>::componentName = typeid(T).name();

	class EntityComponentManager
	{
		friend EntityReg;
		friend ECSSerializer;
		EntityComponentManager() = default;
		EntityComponentManager(const EntityComponentManager&) = delete;
		EntityComponentManager& operator=(const EntityComponentManager&) = delete;
		~EntityComponentManager()
		{
			m_entityRegistry.clear(); // this will delete alla entities and components, before componentesArray gets destroyed
		}
	public:

		Entity& createEntity()
		{
			EntityID index;
			if (!m_freeEntitySlots.empty())
			{
				index = m_freeEntitySlots.front();
				m_freeEntitySlots.pop();
				m_entitiesComponentHandles[index].clear();
				m_entityRegistry[index] = std::move(Entity(index, this));
			}
			else
			{
				index = m_entitiesComponentHandles.size();
				m_entitiesComponentHandles.emplace_back(std::vector<ComponentMetaData>());
				m_entityRegistry.emplace_back(Entity(index, this));
				//assert(m_entityRegistry.back().getID() == m_entityRegistry[index].getID());
			}
			return m_entityRegistry[index];
		}
		void removeEntity(Entity& entity)
		{
			auto& components = m_entitiesComponentHandles[entity.m_entityIndex];
			for (auto& c : components)
			{
				removeComponent(c.typeID, entity.m_entityIndex);
			}
			//this is the entity in the back of the vector
			if (entity.m_entityIndex + 1 == m_entitiesComponentHandles.size())
			{
				m_entitiesComponentHandles.pop_back();
			}
			else
			{
				m_entitiesComponentHandles[entity.m_entityIndex].clear();
				m_freeEntitySlots.push(entity.m_entityIndex);
			}
			entity.m_entCompManRef = nullptr;
		}

		void update(double dt)
		{

		}

		template<typename T>
		requires std::is_trivially_copy_assignable_v<T>
		T* addComponent(EntityID entityID, const T& component);

		void addComponent(EntityID entityID, ComponentTypeID typeID, BaseComponent* component)
		{
			auto createFunc = BaseComponent::getCreateComponentFunction(typeID);
			ComponentIndex index = createFunc(component);
			m_entitiesComponentHandles[entityID].emplace_back(typeID, index, entityID);
		}

		template<typename T>
		void removeComponent(EntityID entityID);

		template<typename T>
		T* getComponent(EntityID entityID);

	private:
		void removeComponent(ComponentTypeID type, EntityID entityID)
		{
			auto& entityComponentHandles = m_entitiesComponentHandles[entityID];
			if (auto iti = std::ranges::find_if(entityComponentHandles.begin(), entityComponentHandles.end(),
				[type](ComponentMetaData c) { return type == c.typeID; });
				iti != entityComponentHandles.end())
			{
				auto componentUtilityFunctions = BaseComponent::getComponentUtility(iti->typeID);
				if (EntityID movedCompEntity = componentUtilityFunctions.deleteComponent(iti->index); movedCompEntity != -1)
				{
					auto& movedCompEntityHandles = m_entitiesComponentHandles[movedCompEntity];
					if (auto itj = std::ranges::find_if(movedCompEntityHandles.begin(), movedCompEntityHandles.end(),
						[type](ComponentMetaData c) { return type == c.typeID; });
						itj != movedCompEntityHandles.end())
					{
						itj->index = iti->index;
					}
					else
					{
						assert(false); // find_if above should not fail
					}
				}
				*iti = entityComponentHandles.back();
				entityComponentHandles.pop_back();
			}
		}

		Entity createEntityForDeSerialization(EntityID id)
		{

			return Entity(id, this);
		}
		

	private:
		std::vector<std::vector<ComponentMetaData>> m_entitiesComponentHandles;
		std::queue<EntityID> m_freeEntitySlots;
		std::vector<Entity> m_entityRegistry;
	};
	inline Entity::~Entity()
	{
#ifdef _DEBUG
		OutputDebugString(L"~Entity\tindex: ");
		OutputDebugString(std::to_wstring(m_entityIndex).c_str());
		OutputDebugString(L"\n");
#endif // _DEBUG

		if (m_entCompManRef)
		{
			m_entCompManRef->removeEntity(*this);
		}
	}

	class EntityReg
	{
		friend ECSSerializer;
		EntityReg() = delete;
	public:
		static Entity& createEntity()
		{
			return m_entCompManInstance.createEntity();
		}
		static void removeEntity(Entity& entity)
		{
			m_entCompManInstance.removeEntity(entity);
		}

		static void update(double dt)
		{
			m_entCompManInstance.update(dt);
		}

		static void addComponent(EntityID entityID, ComponentTypeID typeID, BaseComponent* component)
		{
			m_entCompManInstance.addComponent(entityID, typeID, component);
		}

		template<typename T>
		requires std::is_copy_assignable_v<T>&& std::is_copy_constructible_v<T>
			static T* addComponent(EntityID entityID, const T& component)
		{
			return m_entCompManInstance.addComponent<T>(entityID, component);
		}

		template<typename T>
		static void removeComponent(EntityID entityID)
		{
			m_entCompManInstance.removeComponent<T>(entityID);
		}

		template<typename T>
		static T* getComponent(EntityID ID)
		{
			return m_entCompManInstance.getComponent<T>(ID);
		}

		template<typename T>
		static std::vector<T>& getComponentArray()
		{
			return T::componentArray;
		}

		static std::vector<Entity>& getAllEntities()
		{
			return m_entCompManInstance.m_entityRegistry;
		}

	private:
		inline static EntityComponentManager m_entCompManInstance;
		
	};



	template<typename T>
	inline T* Entity::getComponent()
	{
		return m_entCompManRef->getComponent<T>(this->m_entityIndex);
	}

	template<typename T>
	requires std::is_trivially_copy_assignable_v<T>
		inline T* Entity::addComponent(const T& component) const
	{
		return m_entCompManRef->addComponent<T>(this->m_entityIndex, component);
	}

	template<typename T>
	inline void Entity::removeComponent() const
	{
		m_entCompManRef->removeComponent<T>(this->m_entityIndex);
	}

	template<typename T>
	requires std::is_trivially_copy_assignable_v<T>
	inline T* EntityComponentManager::addComponent(EntityID entityID, const T& comp)
	{
		ComponentTypeID typeID = T::typeID;
		ComponentIndex index;
		index = comp.componentArray.size();
		T::componentArray.emplace_back(comp);

		T* compPtr = &T::componentArray[index];
		compPtr->entityIndex = entityID;
		m_entitiesComponentHandles[entityID].emplace_back(typeID, index, entityID);
		return compPtr;
	}

	template<typename T>
	inline void EntityComponentManager::removeComponent(EntityID entityID)
	{
		removeComponent(T::typeID, entityID);
	}

	template<typename T>
	inline T* EntityComponentManager::getComponent(EntityID entityID)
	{
		auto& entityComponents = m_entitiesComponentHandles[entityID];
		if (auto it = std::ranges::find_if(entityComponents.begin(), entityComponents.end(),
			[](ComponentMetaData c) { return T::typeID == c.typeID; });
			it != entityComponents.end())
		{
			auto r = &T::componentArray[it->index];
			return r;
		}
		return nullptr;
	}
}
