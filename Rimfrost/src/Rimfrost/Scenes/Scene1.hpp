#pragma once
#include "Scene.hpp"
#include "PointLight.hpp"

namespace Engine1
{

	class Scene1 : public Engine1::Scene
	{
	public:
		Scene1();
		~Scene1();

		void setUpScene() override;

	private:
		PointLight m_whiteLight;
		NodeHandle* m_lightSphere = nullptr;
		NodeHandle* testHandle = nullptr;
		void derivedSceneUpdate(double dt) override;
	};

}