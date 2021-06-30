#pragma once
#include "Event.hpp"
#include "Engine1.hpp"
#include "Mouse.hpp"

namespace Engine1
{
	class MouseButtonsEvent : public Event
	{
	public:
		//MouseButtonsEvent(uint32_t x, uint32_t y, bool lmbClicked, bool lmbReleased, bool lmbHeld, bool rmbClicked, bool rmbReleased, bool rmbHeld);
		MouseButtonsEvent(const MouseState& ms);
		virtual ~MouseButtonsEvent() = default;
		static constexpr EventType eventType = "MouseClickEvent";

		virtual EventType type() const override;

		MouseState mouseState;

		//uint32_t x, y;
		//bool lmbClicked, lmbReleased, lmbHeld, rmbClicked, rmbReleased, rmbHeld;
	};

	class MousePickingEvent : public Event
	{
	public:
		MousePickingEvent(NodeID id, DirectX::XMFLOAT3 pos);
		virtual ~MousePickingEvent() = default;
		static constexpr EventType eventType = "MousePickingEvent";

		virtual EventType type() const override;

		NodeID nodeID;
		DirectX::XMFLOAT3 position;
	};
}