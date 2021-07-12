#pragma once
#include "SceneGraph.hpp"
#include "PointLight.hpp"
#include "Rimfrost\Scene\IScene.hpp"

namespace Rimfrost
{

	class Scene1 : public IScene
	{
	public:
		Scene1();
		~Scene1();

		void setUpScene() override;
		Camera& camera() override;
		SceneGraph& sceneGraph() override;
		Lights& lights() override;

	private:
		Lights m_lights;
		Camera m_camera;
		std::unique_ptr<SceneGraph> m_sceneGraph;
		PointLight m_whiteLight;
		NodeHandle* m_lightSphere = nullptr;
		NodeHandle* testHandle = nullptr;
		void update(double dt) override;
	};

}