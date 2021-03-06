#pragma once


#include "Rimfrost\Scene\SceneGraph.hpp"
#include "Rimfrost\Graphics\Camera.hpp"
#include "Rimfrost\Graphics\Lights.hpp"

namespace Rimfrost
{
	class IScene
	{
	public:
		virtual ~IScene() = default;

		virtual void setUpScene() = 0;
		virtual void update(double dt) = 0;
		virtual Camera& camera() = 0;
		virtual Lights& lights() = 0;
		virtual SceneGraph& sceneGraph() = 0;
		virtual void load(std::string path = "") = 0;
		virtual void save(std::string path = "") = 0;
	};
}
