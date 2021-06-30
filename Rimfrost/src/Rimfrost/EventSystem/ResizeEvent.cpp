#include "pch.hpp"
#include "ResizeEvent.hpp"
namespace Engine1
{
    PreResizeEvent::PreResizeEvent()
    {
    }

    Event::EventType PreResizeEvent::type() const
    {
        return this->eventType;
    }

    PostResizeEvent::PostResizeEvent(uint32_t width, uint32_t height) : width(width), height(height)
    {
    }

    Event::EventType PostResizeEvent::type() const
    {
        return this->eventType;
    }
}