#include "rfpch.hpp"
#include "MiscEvents.hpp"

namespace Rimfrost
{

	CameraEvent::CameraEvent(const Camera* cam)
	{
		assert(cam);
		camera = cam;
	}

	Event::EventType Rimfrost::CameraEvent::type() const
	{
		return this->eventType;
	}

}