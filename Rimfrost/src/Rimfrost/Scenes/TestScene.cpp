#include "rfpch.hpp"
#include "TestScene.hpp"

namespace Rimfrost
{
	std::shared_ptr<Camera> TestScene::camera() const
	{
		return std::shared_ptr<Camera>();
	}

	std::shared_ptr<SceneGraph> TestScene::sceneGraph() const
	{
		return std::shared_ptr<SceneGraph>();
	}

	std::shared_ptr<PointLightContainer> TestScene::pointLights() const
	{
		return std::shared_ptr<PointLightContainer>();
	}

}