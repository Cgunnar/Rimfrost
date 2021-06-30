#include "pch.hpp"
#include "MouseEvent.hpp"

namespace Engine1
{
    MouseButtonsEvent::MouseButtonsEvent(const MouseState& ms)
    {
        this->mouseState = ms;
    }

    Event::EventType MouseButtonsEvent::type() const
    {
        return eventType;
    }

    MousePickingEvent::MousePickingEvent(Engine1::NodeID id, DirectX::XMFLOAT3 pos)
    {
        this->nodeID = id;
        this->position = pos;
    }

    Event::EventType MousePickingEvent::type() const
    {
        return eventType;
    }
}