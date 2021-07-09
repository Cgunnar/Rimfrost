#pragma once

#include "Entity.hpp"

namespace Rimfrost
{
	using ComponentTypeID = size_t;




	struct ComponentMetaData
	{
		ComponentMetaData(ComponentTypeID typeID, size_t index) : typeID(typeID), index(index) {}
		ComponentTypeID typeID;
		size_t index;
	};

	struct BaseComponent
	{
		static ComponentTypeID registerComponent(size_t size);
		EntityIndex entityIndex = -1;

	private:
		static std::vector<ComponentTypeID>* s_componentRegister;
	};


	template<typename T>
	struct Component : public BaseComponent
	{
		/*static const componentCreateFunction create;
		static const componentRemoveFunction remove;*/
		static const ComponentTypeID typeID;
		static const size_t size;
	};

	template<typename T>
	const ComponentTypeID Component<T>::typeID = BaseComponent::registerComponent(sizeof(T));

	//init componentSize
	template<typename T>
	const size_t Component<T>::size = sizeof(T);

}
