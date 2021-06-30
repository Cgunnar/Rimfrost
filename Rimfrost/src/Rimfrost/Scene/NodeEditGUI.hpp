#pragma once
#include "Node.hpp"

namespace Engine1
{

	class NodeEditGUI
	{
	public:
		enum class RadioButtonTRS
		{
			TRANSLATE = 0,
			ROTATE,
			SCALE,
		};
		enum class RadioButtonRefSystem
		{
			WORLD = 0,
			LOCAL,
			PARENT,
		};

		NodeEditGUI() = default;
		NodeEditGUI(NodeHandle handle, std::function<void(RadioButtonRefSystem)> selectTranslate,
			std::function<void(RadioButtonRefSystem)> selectRotate, std::function<void()> selectScale);
		NodeEditGUI& operator=(const NodeEditGUI& other);
		~NodeEditGUI();

		void view();

		

		void selectedTRS();

	private:
		NodeHandle* m_node = nullptr;
		std::function<void(RadioButtonRefSystem)> m_selectTranslate;
		std::function<void(RadioButtonRefSystem)> m_selectRotate;
		std::function<void()> m_selectScale;

		RadioButtonRefSystem m_radioRefSys = RadioButtonRefSystem::WORLD;
		RadioButtonRefSystem m_prevRadioRefSys = RadioButtonRefSystem::WORLD;


		RadioButtonTRS m_radioTRS = RadioButtonTRS::TRANSLATE;
		RadioButtonTRS m_prevRadioTRS = RadioButtonTRS::TRANSLATE;
	};

}