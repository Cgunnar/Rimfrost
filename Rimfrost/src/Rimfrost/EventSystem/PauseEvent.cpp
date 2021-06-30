#include "rfpch.hpp"
#include "PauseEvent.hpp"
namespace Engine1
{
    PauseEvent::PauseEvent(bool state) : m_isPaused(state)
    {
    }

    Event::EventType PauseEvent::type() const
    {
        return this->eventType;
    }
}