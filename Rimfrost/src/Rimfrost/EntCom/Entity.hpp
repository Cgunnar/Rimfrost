#pragma once

namespace Rimfrost
{
	class EntityRegistry;
	class Entity
	{
		friend EntityRegistry;
	public:

	private:
		Entity();
		~Entity() = default;
	};

}