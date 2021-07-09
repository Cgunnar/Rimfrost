#include "rfpch.hpp"
#include "EntityRegistry.hpp"

namespace Rimfrost
{
	EntityRegistry::~EntityRegistry()
	{

	}
	Entity EntityRegistry::createEntity()
	{
		EntityIndex index = m_entitiesComponentHandles.size();;
		m_entitiesComponentHandles.push_back(std::vector<ComponentMetaData>());
		return Entity(index);
	}
	void EntityRegistry::update(double dt)
	{

	}
}