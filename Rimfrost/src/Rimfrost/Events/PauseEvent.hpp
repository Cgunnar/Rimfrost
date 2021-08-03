#pragma once
#include "Event.hpp"
namespace Rimfrost
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
