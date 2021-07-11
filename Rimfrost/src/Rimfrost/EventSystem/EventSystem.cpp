#include "rfpch.hpp"
#include "EventSystem.hpp"

namespace Rimfrost
{
	EventSystem::EventDispatcher EventSystem::s_evenDisp;
	std::queue<std::unique_ptr<Event>> EventSystem::m_eventQueue;

	void EventSystem::EventDispatcher::dispatchPost(const Event& event)
	{
		Event::EventType type = event.type();

		if (!m_observers.contains(type))
			return; //observer does not exist

		for (auto&& observer : m_observers[type])
		{
			observer(event); // calls the onEvent function in EventObserver
		}
	}

	void EventSystem::postInstantly(const Event& event)
	{
		s_evenDisp.dispatchPost(event);
	}
	
	void EventSystem::dispatchQueued()
	{
		while (!m_eventQueue.empty())
		{
			s_evenDisp.dispatchPost(*m_eventQueue.front());
			m_eventQueue.pop();
		}
	}
}