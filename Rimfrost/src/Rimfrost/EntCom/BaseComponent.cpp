#include "rfpch.hpp"
#include "BaseComponent.hpp"

namespace Rimfrost
{
	std::vector<ComponentTypeID>* BaseComponent::s_componentRegister;

	ComponentTypeID BaseComponent::registerComponent(size_t size)
	{
		if (s_componentRegister == nullptr)
		{
			s_componentRegister = new std::vector<size_t>();
		}
		ComponentTypeID compID = s_componentRegister->size();
		s_componentRegister->push_back(size);
		return compID;
	}
}