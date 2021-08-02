#include "rfpch.hpp"

#include <imgui.h>

#include "EntityEditGUI.hpp"
#include "Rimfrost\EntCom\rfComponents.hpp"


namespace Rimfrost
{
	EntityEditGUI::EntityEditGUI()
	{
		m_selectedEntity = std::make_shared<Entity>();
	}
	void EntityEditGUI::view()
	{
		ImGui::Begin("Entity");
		if (!m_selectedEntity || m_selectedEntity->empty())
		{
			ImGui::End();
			return;
		}
		std::string topText = "entityID: " + std::to_string(m_selectedEntity->getID()) + " selected";
		ImGui::Text(topText.c_str());

		if (ImGui::Button("Remove"))
		{
			removeEntity();
		}
		



		ImGui::End();
	}

	bool EntityEditGUI::trySelectEntityFromNode(NodeID id)
	{
		for (auto& n : EntityReg::getComponentArray<NodeComponent>())
		{
			if (n.nodeHandel == id)
			{
				selectEntity(std::make_shared<Entity>(n.getEntity()));
				return true;
			}
		}
		return false;
	}

	void EntityEditGUI::selectEntity(std::shared_ptr<Entity> ent)
	{
		assert(!ent->empty());
		m_selectedEntity = ent;
	}
	void EntityEditGUI::removeEntity()
	{
		Logger::getLogger().debugLog("EntityEditGUI::removeEntity() selectedEntityRefCount: " + std::to_string(m_selectedEntity->getRefCount()) + "\n");
		assert(m_selectedEntity->getRefCount() == 2);
		m_selectedEntity->reset();
		m_selectedEntity.reset();
	}
}


