#pragma once

#include "Event.hpp"
#include "Camera.hpp"

namespace Rimfrost
{
	class CameraEvent : public Event
	{
	public:
		CameraEvent(const Camera* cam);
		virtual ~CameraEvent() = default;
		static constexpr EventType eventType = "CameraEvent";
		const Camera*  camera;

		virtual EventType type() const override;
	};
}
