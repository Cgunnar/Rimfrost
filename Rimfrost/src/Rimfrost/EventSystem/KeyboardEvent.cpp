#include "rfpch.hpp"
#include "KeyboardEvent.hpp"



namespace Rimfrost
{
	KeyboardEvent::KeyboardEvent(std::shared_ptr<KeyboardState> board, KeyAndState keyState, char c) : keyAndState(keyState), character(c), boardState(std::move(board))
	{
	}
	Event::EventType KeyboardEvent::type() const
	{
		return eventType;
	}
}
