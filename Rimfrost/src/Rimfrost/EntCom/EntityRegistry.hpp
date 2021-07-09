#pragma once

#include "BaseComponent.hpp"

namespace Rimfrost
{
	
	class EntityRegistry
	{
	public:
		EntityRegistry() = default;
		EntityRegistry(const EntityRegistry&) = delete;
		EntityRegistry& operator=(const EntityRegistry&) = delete;
		~EntityRegistry();

	private:
		std::unordered_map<ComponentTypeID, std::vector<char>> m_componentMap;

	};
}
