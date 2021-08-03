#include "rfpch.hpp"
#include "PauseEvent.hpp"
namespace Rimfrost
{
    PauseEvent::PauseEvent(bool state) : m_isPaused(state)
    {
    }

    Event::EventType PauseEvent::type() const
    {
        return this->eventType;
    }
}