#include "rfpch.hpp"
#include "MouseEvent.hpp"

namespace Rimfrost
{
    MouseButtonsEvent::MouseButtonsEvent(const MouseState& ms)
    {
        this->mouseState = ms;
    }

    Event::EventType MouseButtonsEvent::type() const
    {
        return eventType;
    }

    MousePickingEvent::MousePickingEvent(Rimfrost::NodeID id, Vector3 pos)
    {
        this->nodeID = id;
        this->position = pos;
    }

    Event::EventType MousePickingEvent::type() const
    {
        return eventType;
    }
    Event::EventType MouseMoveEvent::type() const
    {
        return eventType;
    }
}