#pragma once

#include "Rimfrost\Scene\IScene.hpp"
namespace Rimfrost
{
	class TestScene : public IScene
	{
	public:
		TestScene() = default;
		~TestScene() = default;

		void setUpScene() override;
		void update(double dt) override;
		Camera& camera() override;
		SceneGraph& sceneGraph() override;
		Lights& lights() override;

	private:
		Camera m_camera;
		std::unique_ptr<SceneGraph> m_sceneGraph;
		std::unique_ptr<Lights> m_lights;
	};

}
