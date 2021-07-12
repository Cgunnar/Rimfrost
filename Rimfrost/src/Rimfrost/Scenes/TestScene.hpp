#pragma once

#include "Rimfrost\Scene\IScene.hpp"
namespace Rimfrost
{
	class TestScene : public IScene
	{
	public:
		TestScene() = default;
		~TestScene() = default;

		std::shared_ptr<Camera> camera() const override;
		std::shared_ptr<SceneGraph> sceneGraph() const override;
		std::shared_ptr<PointLightContainer> pointLights() const override;
	};

}
