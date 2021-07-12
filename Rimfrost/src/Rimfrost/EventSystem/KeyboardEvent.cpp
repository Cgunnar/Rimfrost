#include "rfpch.hpp"
#include "KeyboardEvent.hpp"



namespace Rimfrost
{
	KeyboardEvent::KeyboardEvent(std::pair<Key, KeyState> key, char c) : keyAndState(key), character(c)
	{
	}
	Event::EventType KeyboardEvent::type() const
	{
		return eventType;
	}
}
