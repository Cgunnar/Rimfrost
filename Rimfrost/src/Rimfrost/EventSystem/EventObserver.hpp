#pragma once
#include "Event.hpp"
#include "PauseEvent.hpp"
#include "Logger.hpp"

namespace Engine1
{
	class EventObserver
	{
	public:
		EventObserver() = default;
		virtual ~EventObserver() = default;

		virtual void onEvent(const Event& e) {}
	};
}
