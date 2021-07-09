#pragma once

namespace Rimfrost
{
	using EntityIndex = size_t;

	class EntityRegistry;
	class Entity
	{
		friend EntityRegistry;
	public:
		~Entity() = default;

	private:
		Entity(EntityIndex ID);
		EntityIndex entityIndex;

		
	};

}