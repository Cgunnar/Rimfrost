#pragma once

#include "Event.hpp"
#include "Keyboard.hpp"

namespace Rimfrost
{
	class KeyboardEvent : public Event
	{
	public:
		//MouseButtonsEvent(uint32_t x, uint32_t y, bool lmbClicked, bool lmbReleased, bool lmbHeld, bool rmbClicked, bool rmbReleased, bool rmbHeld);
		KeyboardEvent(std::shared_ptr<KeyboardState> board, KeyAndState keyState, char c = 0);
		virtual ~KeyboardEvent() = default;
		static constexpr EventType eventType = "KeyboardEvent";

		KeyAndState keyAndState;
		std::shared_ptr<KeyboardState> boardState;
		char character;

		virtual EventType type() const override;
	};
}