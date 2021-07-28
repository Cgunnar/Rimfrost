#include "rfpch.hpp"

#include <imgui.h>

#include "EntityEditGUI.hpp"
#include "Rimfrost\EntCom\rfComponents.hpp"


namespace Rimfrost
{
	void EntityEditGUI::view()
	{
		std::string topWindowText = "Entities: ";
		if (m_selectedEntity.empty())
			topWindowText += "no entity selected";
		else
			topWindowText += "entity " + std::to_string(m_selectedEntity.getID()) + " selected";

		ImGui::Begin(topWindowText.c_str());

		ImGui::End();
	}

	bool EntityEditGUI::trySelectEntityFromNode(NodeID id)
	{
		for (auto& n : EntityReg::getComponentArray<NodeComponent>())
		{
			if (n.nodeHandel == id)
			{
				selectEntity(n.getEntity());
				return true;
			}
		}
		return false;
	}

	void EntityEditGUI::selectEntity(Entity ent)
	{
		assert(!ent.empty());
		m_selectedEntity = ent;
	}
}
