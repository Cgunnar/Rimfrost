#include "rfpch.hpp"
#include "SerializeECS.hpp"
#include "Rimfrost\EntCom\rfEntity.hpp"
#include "Rimfrost\Utilities\FileUtility.hpp"

namespace Rimfrost
{
	//serialize ecs to json
	void SerializeECS::serialize(const std::string& fileName)
	{
		for (auto& c : BaseComponent::s_componentRegister)
		{
			writefileBin(c.getArrayPointer(), c.componentCount(), c.size, "Saves/Components/" + (std::string)c.name);
		}
	}

	//deserialize ecs from json
	void SerializeECS::deSerialize(const std::string& fileName)
	{

	}
}