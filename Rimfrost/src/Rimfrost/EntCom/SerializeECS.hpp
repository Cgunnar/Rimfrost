#pragma once
namespace Rimfrost
{
	//learn c++20 modules to get encapsulation, these static classes are not nice
	class ECSSerializer
	{
	public:
		struct JComponentInfoStruct
		{
			std::string path;
			std::string typeName;
			size_t componentTypeID;
			size_t componentSize;
			size_t componentCount;
		};


		static void serialize(const std::string& fileName);
		static void deSerialize(const std::string& fileName);

	private:
		static bool remapTypeID(std::vector<JComponentInfoStruct>& componentsFromJson);
	};
}
