#pragma once
#include "Scene.hpp"
#include "PointLight.hpp"

namespace Engine1
{

	class Scene0 : public Scene
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