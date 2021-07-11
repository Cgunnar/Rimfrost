#pragma once

#include "Event.hpp"
#include "EventObserver.hpp"

namespace Rimfrost
{
	class EventSystem
	{
	public:
		EventSystem() = default;
		static void postInstantly(const Event& event);
		template<typename T>
		static void postTOQueue(const Event& event);
		static void dispatchQueued();
		template<typename T>
		static void addObserver(T& observer, Event::EventType type);

	private:
		class EventDispatcher
		{
			friend EventSystem;
		private:
			EventDispatcher() = default;
			~EventDispatcher() = default;
			template<typename T>
			void dispatchAddObserver(T& observer, Event::EventType type);
			void dispatchPost(const Event& event);
			std::map<Event::EventType, std::vector<std::function<void(const Event&)>>> m_observers;
		};
		static std::queue<std::unique_ptr<Event>> m_eventQueue;
		static EventDispatcher s_evenDisp;
	};


	template<typename T>
	inline void EventSystem::EventDispatcher::dispatchAddObserver(T& observer, Event::EventType type)
	{
		m_observers[type].emplace_back(std::bind(&T::onEvent, &observer, std::placeholders::_1));
	}

	template<typename T>
	inline void EventSystem::postTOQueue(const Event& event)
	{
		m_eventQueue.emplace(std::make_unique<T>(event));
	}

	template<typename T>
	inline void EventSystem::addObserver(T& observer, Event::EventType type)
	{
		s_evenDisp.dispatchAddObserver(observer, type);
	}
}