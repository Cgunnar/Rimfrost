#pragma once

#include "Event.hpp"
#include "Keyboard.hpp"

namespace Rimfrost
{
	class KeyboardEvent : public Event
	{
	public:
		//MouseButtonsEvent(uint32_t x, uint32_t y, bool lmbClicked, bool lmbReleased, bool lmbHeld, bool rmbClicked, bool rmbReleased, bool rmbHeld);
		KeyboardEvent(std::pair<Key, KeyState>, char c = 0);
		virtual ~KeyboardEvent() = default;
		static constexpr EventType eventType = "KeyboardEvent";

		std::pair<Key, KeyState> keyAndState;
		char character;

		virtual EventType type() const override;
	};
}