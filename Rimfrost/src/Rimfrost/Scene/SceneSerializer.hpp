#pragma once
#include "Rimfrost\Scene\IScene.hpp"
namespace Rimfrost
{

	class SceneSerializer
	{
	public:
		static void serialize(const std::string& fileName, IScene& scene);
		static void deSerialize(const std::string& fileName, IScene& scene);

	private:

	};

}