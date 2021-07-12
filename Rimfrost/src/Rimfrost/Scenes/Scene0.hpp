#pragma once
#include "SceneGraph.hpp"
#include "PointLight.hpp"

namespace Rimfrost
{

	class Scene0 : public SceneGraph
	{
	public:
		Scene0();
		~Scene0();

		void setUpScene() override;

	private:
		PointLight m_whiteLight;
		NodeHandle* m_lightSphere = nullptr;
		NodeHandle* testHandle = nullptr;
		std::vector<PointLight> m_randomLights;
		void derivedSceneUpdate(double dt) override;
	};

}