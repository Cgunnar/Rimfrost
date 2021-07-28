#pragma once


#include "Rimfrost\Graphics\Engine1.hpp"
#include "Rimfrost\EntCom\rfEntity.hpp"
namespace Rimfrost
{
	class EntityEditGUI
	{


	public:
		void view();
		[[nodiscard]] bool trySelectEntityFromNode(NodeID id);
		void selectEntity(Entity ent);

	private:
		Entity m_selectedEntity;
	};
}

