#pragma once
#include "SceneGraph.hpp"
#include "PointLight.hpp"
#include "Rimfrost\Scene\IScene.hpp"

namespace Rimfrost
{

	class SandboxMap : public IScene
	{
	public:
		SandboxMap();
		~SandboxMap();

		void setUpScene() override;
		Camera& camera() override;
		SceneGraph& sceneGraph() override;
		Lights& lights() override;

	private:
		Lights m_lights;
		Camera m_camera;
		std::unique_ptr<SceneGraph> m_sceneGraph;
		PointLight m_whiteLight;
		PointLight m_redLight;
		NodeHandle* testHandle = nullptr;
		void update(double dt) override;
	};

}