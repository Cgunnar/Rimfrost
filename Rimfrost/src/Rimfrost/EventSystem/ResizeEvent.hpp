#pragma once
#include "Event.hpp"

namespace Rimfrost
{
	class PreResizeEvent : public Event
	{
	public:
		PreResizeEvent();
		virtual ~PreResizeEvent() = default;
		static constexpr EventType eventType = "PreResizeEvent";

		virtual EventType type() const override;
	};

	class PostResizeEvent : public Event
	{
	public:
		PostResizeEvent(uint32_t width, uint32_t height);
		virtual ~PostResizeEvent() = default;
		static constexpr EventType eventType = "PostResizeEvent";

		virtual EventType type() const override;

		uint32_t width;
		uint32_t height;
	};
}