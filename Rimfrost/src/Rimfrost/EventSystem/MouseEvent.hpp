#pragma once
#include "Event.hpp"
#include "Engine1.hpp"
#include "Mouse.hpp"

namespace Rimfrost
{
	struct MouseButtonsEvent : public Event
	{
		MouseButtonsEvent(const MouseState& ms);
		virtual ~MouseButtonsEvent() = default;
		static constexpr EventType eventType = "MouseButtonsEvent";

		virtual EventType type() const override;

		MouseState mouseState;
	};

	struct MouseMoveEvent : public Event
	{
		MouseMoveEvent(const MouseState& ms) : mouseState(ms) {}
		virtual ~MouseMoveEvent() = default;
		static constexpr EventType eventType = "MouseMoveEvent";

		virtual EventType type() const override;

		MouseState mouseState;
	};

	struct MousePickingEvent : public Event
	{
		MousePickingEvent(NodeID id, Vector3 pos);
		virtual ~MousePickingEvent() = default;
		static constexpr EventType eventType = "MousePickingEvent";

		virtual EventType type() const override;

		NodeID nodeID;
		Vector3 position;
	};
}