#pragma once


#include "Rimfrost\Scene\SceneGraph.hpp"
#include "Rimfrost\Graphics\Camera.hpp"
#include "Rimfrost\Graphics\PointLightContainer.hpp"

namespace Rimfrost
{
	struct IScene
	{
		IScene() = delete;
		virtual ~IScene() = default;

		virtual std::shared_ptr<SceneGraph> sceneGraph() const = 0;
		virtual std::shared_ptr<Camera> camera() const = 0;
		virtual std::shared_ptr<PointLightContainer> pointLights() const = 0;
	};
}
