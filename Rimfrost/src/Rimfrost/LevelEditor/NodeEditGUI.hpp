#pragma once
#include <functional>

#include "Rimfrost\Scene\Node.hpp"

namespace Rimfrost
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
		~NodeEditGUI() = default;
		NodeEditGUI(NodeHandle handle, bool isOwnedByEntity, std::function<void(RadioButtonRefSystem)> selectTranslate,
			std::function<void(RadioButtonRefSystem)> selectRotate, std::function<void()> selectScale, std::function<void()> deSelectNode);

		void view();
		void selectedTRS();

	private:
		NodeHandle m_node;
		bool m_nodeIsOwnedByEntity;
		std::function<void(RadioButtonRefSystem)> m_selectTranslate;
		std::function<void(RadioButtonRefSystem)> m_selectRotate;
		std::function<void()> m_selectScale;
		std::function<void()> m_deSelectNode;

		RadioButtonRefSystem m_radioRefSys = RadioButtonRefSystem::WORLD;
		RadioButtonRefSystem m_prevRadioRefSys = RadioButtonRefSystem::WORLD;
		RadioButtonTRS m_radioTRS = RadioButtonTRS::TRANSLATE;
		RadioButtonTRS m_prevRadioTRS = RadioButtonTRS::TRANSLATE;
	};

}