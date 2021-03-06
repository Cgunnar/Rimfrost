#pragma once


namespace Rimfrost
{
	class Event
	{
	public:
		typedef std::string_view EventType;
		Event() = default;
		virtual ~Event() = default;



		virtual EventType type() const = 0;

	};
}
