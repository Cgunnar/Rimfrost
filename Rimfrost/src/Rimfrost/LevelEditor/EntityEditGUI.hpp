#pragma once


#include "Rimfrost\Graphics\Engine1.hpp"
#include "Rimfrost\EntCom\rfEntity.hpp"
namespace Rimfrost
{
	class EntityEditGUI
	{


	public:
		EntityEditGUI();
		void view();
		[[nodiscard]] bool trySelectEntityFromNode(NodeID id);
		void selectEntity(std::shared_ptr<Entity> ent);
		void deSelectEntity();

	private:
		std::shared_ptr<Entity> m_selectedEntity;

		void removeEntity();

	};
}

