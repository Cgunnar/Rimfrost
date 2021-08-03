#pragma once
#include "Rimfrost\EventSystem\Event.hpp"

namespace Rimfrost
{
	class EventObserver
	{
	public:
		EventObserver() = default;
		virtual ~EventObserver() = default;

		virtual void onEvent(const Event& e) = 0;
	};
}
