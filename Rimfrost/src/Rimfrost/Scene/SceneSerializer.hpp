#pragma once
#include "Scene.hpp"
namespace Rimfrost
{

	class SceneSerializer
	{
	public:
		static void serialize(const std::string& fileName, const Scene& scene);
		static void serialize(const std::string& fileName, const std::shared_ptr<Scene>& scene);
		static void deSerialize(const std::string& fileName, std::shared_ptr<Scene>& scene);

	private:

	};

}