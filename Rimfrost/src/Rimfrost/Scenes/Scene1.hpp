#pragma once
#include "SceneGraph.hpp"
#include "PointLight.hpp"

namespace Rimfrost
{

	class Scene1 : public Rimfrost::SceneGraph
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