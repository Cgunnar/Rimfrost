#pragma once
namespace Rimfrost
{
	class SerializeECS
	{
	public:
		static void serialize(const std::string& fileName);
		static void deSerialize(const std::string& fileName);
	};
}
