#pragma once
#include "Event.hpp"
namespace Engine1
{
	class PauseEvent : public Event
	{
	public:
		PauseEvent(bool state);
		virtual ~PauseEvent() = default;
		bool m_isPaused;
		static constexpr EventType eventType = "pauseEvent";

		virtual EventType type() const override;

	};
}
