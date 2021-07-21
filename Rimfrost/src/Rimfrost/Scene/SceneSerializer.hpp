#pragma once
#include "Rimfrost\Scene\IScene.hpp"
namespace Rimfrost
{

	class SceneSerializer
	{
	public:
		static void serialize(const std::string& path, IScene& scene);
		static void deSerialize(const std::string& path, IScene& scene);

	private:
		static constexpr char sceneFileName[] = "scene_hierarchy.json";
	};

}