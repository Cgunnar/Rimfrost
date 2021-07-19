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
#endif // _DEBUG


namespace Rimfrost
{

	using ComponentTypeID = size_t;
	using ComponentIndex = size_t;
	using EntityIndex = size_t;

	class EntityRegistry;
	class EC;
	class ECSSerializer;

	
	class Entity
	{
		friend EntityRegistry;
	public:
		~Entity();

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity(Entity&& other) noexcept
		{
			this->m_entityIndex = other.m_entityIndex;
			this->m_entRegRef = other.m_entRegRef;
			//invalidate other
			other.m_entRegRef = nullptr;
			other.m_entityIndex = -1;
		}
		Entity& operator=(Entity&& other) noexcept
		{
			this->m_entityIndex = other.m_entityIndex;
			this->m_entRegRef = other.m_entRegRef;
			//invalidate other
			other.m_entRegRef = nullptr;
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

		EntityIndex getID() const { return m_entityIndex; }

		operator const EntityIndex() const { return m_entityIndex; }

	private:
		Entity(EntityIndex ID, EntityRegistry* entReg) : m_entityIndex(ID), m_entRegRef(entReg) {}
		Entity() : m_entityIndex(-1), m_entRegRef(nullptr) {}
		EntityIndex m_entityIndex;
		EntityRegistry* m_entRegRef;
	};

	struct ComponentMetaData
	{
		ComponentMetaData(ComponentTypeID typeID, ComponentIndex index, EntityIndex entIndex) : typeID(typeID), index(index), entityIndex(entIndex) {}
		ComponentTypeID typeID;
		ComponentIndex index;
		EntityIndex entityIndex;
	};


	

	class BaseComponent
	{
		friend EntityRegistry;
		friend ECSSerializer;
	public:
		EntityIndex getEntityID() const { return entityIndex; }


	protected:
		static ComponentTypeID registerComponent(size_t size, std::string name, std::function<ComponentIndex(BaseComponent*)> createFunc, 
			std::function<BaseComponent* (ComponentIndex)> fetchFunc, std::function<EntityIndex(ComponentIndex)> deleteFunc,
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

		EntityIndex entityIndex = -1;
	private:


		struct ComponentUtility
		{
			size_t size;
			std::string name;
			std::function<ComponentIndex(BaseComponent*)> createComponent;
			std::function<BaseComponent* (ComponentIndex)> fetchComponentAsBase;
			std::function<EntityIndex(ComponentIndex)> deleteComponent;
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
		friend EntityRegistry;
		friend EC;
		static const ComponentTypeID typeID;
		static const size_t size;
		static const std::string componentName;
		

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
		static EntityIndex deleteComponent(ComponentIndex index)
		{
			EntityIndex entityOwningBackComponent = -1;
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

	class EntityRegistry
	{
		friend EC;
		friend ECSSerializer;
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
			entity.m_entRegRef = nullptr;
		}

		void update(double dt)
		{

		}

		template<typename T>
		requires std::is_trivially_copy_assignable_v<T>
		T* addComponent(EntityIndex entityID, const T& component);

		void addComponent(EntityIndex entityID, ComponentTypeID typeID, BaseComponent* component)
		{
			auto createFunc = BaseComponent::getCreateComponentFunction(typeID);
			ComponentIndex index = createFunc(component);
			m_entitiesComponentHandles[entityID].emplace_back(typeID, index, entityID);
		}

		template<typename T>
		void removeComponent(EntityIndex entityID);

		template<typename T>
		T* getComponent(EntityIndex entityID);

	private:
		void removeComponent(ComponentTypeID type, EntityIndex entityID)
		{
			auto& entityComponentHandles = m_entitiesComponentHandles[entityID];
			if (auto iti = std::ranges::find_if(entityComponentHandles.begin(), entityComponentHandles.end(),
				[type](ComponentMetaData c) { return type == c.typeID; });
				iti != entityComponentHandles.end())
			{
				auto componentUtilityFunctions = BaseComponent::getComponentUtility(iti->typeID);
				if (EntityIndex movedCompEntity = componentUtilityFunctions.deleteComponent(iti->index); movedCompEntity != -1)
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

		Entity createEntityForDeSerialization(EntityIndex id)
		{

			return Entity(id, this);
		}
		

	private:
		std::vector<std::vector<ComponentMetaData>> m_entitiesComponentHandles;
		std::queue<EntityIndex> m_freeEntitySlots;
	};
	inline Entity::~Entity()
	{
#ifdef _DEBUG
		OutputDebugString(L"~Entity\tindex: ");
		OutputDebugString(std::to_wstring(m_entityIndex).c_str());
		OutputDebugString(L"\n");
#endif // _DEBUG

		if (m_entRegRef)
		{
			m_entRegRef->removeEntity(*this);
		}
	}

	class EC
	{
		friend ECSSerializer;
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

		static void addComponent(EntityIndex entityID, ComponentTypeID typeID, BaseComponent* component)
		{
			m_entRegInstance.addComponent(entityID, typeID, component);
		}

		template<typename T>
		requires std::is_copy_assignable_v<T>&& std::is_copy_constructible_v<T>
			static T* addComponent(EntityIndex entityID, const T& component)
		{
			return m_entRegInstance.addComponent<T>(entityID, component);
		}

		template<typename T>
		static void removeComponent(EntityIndex entityID)
		{
			m_entRegInstance.removeComponent<T>(entityID);
		}

		template<typename T>
		static T* getComponent(EntityIndex ID)
		{
			return m_entRegInstance.getComponent<T>(ID);
		}

		template<typename T>
		static std::vector<T>& getComponentArray()
		{
			return T::componentArray;
		}

	private:
		inline static EntityRegistry m_entRegInstance;
	};



	template<typename T>
	inline T* Entity::getComponent()
	{
		return m_entRegRef->getComponent<T>(this->m_entityIndex);
	}

	template<typename T>
	requires std::is_trivially_copy_assignable_v<T>
		inline T* Entity::addComponent(const T& component) const
	{
		return m_entRegRef->addComponent<T>(this->m_entityIndex, component);
	}

	template<typename T>
	inline void Entity::removeComponent() const
	{
		m_entRegRef->removeComponent<T>(this->m_entityIndex);
	}

	template<typename T>
	requires std::is_trivially_copy_assignable_v<T>
	inline T* EntityRegistry::addComponent(EntityIndex entityID, const T& comp)
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
	inline void EntityRegistry::removeComponent(EntityIndex entityID)
	{
		removeComponent(T::typeID, entityID);
	}

	template<typename T>
	inline T* EntityRegistry::getComponent(EntityIndex entityID)
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
