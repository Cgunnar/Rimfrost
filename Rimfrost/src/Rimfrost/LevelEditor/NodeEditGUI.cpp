#include "rfpch.hpp"

#include "NodeEditGUI.hpp"
#include "imgui.h"


using namespace DirectX;
namespace Rimfrost
{

	NodeEditGUI::NodeEditGUI(NodeHandle handle, std::function<void(RadioButtonRefSystem)> selectTranslate,
		std::function<void(RadioButtonRefSystem)> selectRotate, std::function<void()> selectScale)
	{
		m_node = handle;
		m_selectTranslate = selectTranslate;
		m_selectRotate = selectRotate;
		m_selectScale = selectScale;
	}

	void NodeEditGUI::view()
	{
		if (!m_node.isValid()) return;

		ImGui::Begin("selected model");
		ImGui::Text("id: %u", m_node->getID());

		ImGui::Text("Reference system");
		ImGui::RadioButton("World", (int*)&m_radioRefSys, (int)RadioButtonRefSystem::WORLD); ImGui::SameLine();
		ImGui::RadioButton("Local", (int*)&m_radioRefSys, (int)RadioButtonRefSystem::LOCAL); ImGui::SameLine();
		ImGui::RadioButton("Parent", (int*)&m_radioRefSys, (int)RadioButtonRefSystem::PARENT);
		ImGui::NewLine();
		ImGui::RadioButton("translate", (int*)&m_radioTRS, (int)RadioButtonTRS::TRANSLATE); ImGui::SameLine();
		ImGui::RadioButton("rotate", (int*)&m_radioTRS, (int)RadioButtonTRS::ROTATE); ImGui::SameLine();
		ImGui::RadioButton("scale", (int*)&m_radioTRS, (int)RadioButtonTRS::SCALE);
		auto fl3 = m_node->localMatrix.getTranslation();
		float pos[3] = { fl3.x, fl3.y, fl3.z };
		if (ImGui::SliderFloat3("position", (float*)&pos, -10, 10))
		{
			m_node->localMatrix.setTranslation(pos[0], pos[1], pos[2]);
		}

		if (ImGui::Button("Button"))
		{
			pos[0] = 0;
			pos[1] = 0;
			pos[2] = 0;
			m_node->localMatrix.setTranslation(pos[0], pos[1], pos[2]);
		}
		ImGui::End();


		if (m_radioTRS != m_prevRadioTRS || m_radioRefSys != m_prevRadioRefSys)
		{
			selectedTRS();
		}
		m_prevRadioTRS = m_radioTRS;
		m_prevRadioRefSys = m_radioRefSys;
	}


	void NodeEditGUI::selectedTRS()
	{
		switch (m_radioTRS)
		{
		case Rimfrost::NodeEditGUI::RadioButtonTRS::TRANSLATE:
			m_selectTranslate(m_radioRefSys);
			break;
		case Rimfrost::NodeEditGUI::RadioButtonTRS::ROTATE:
			m_selectRotate(m_radioRefSys);
			break;
		case Rimfrost::NodeEditGUI::RadioButtonTRS::SCALE:
			m_selectScale();
			break;
		default:
			break;
		}
	}
}