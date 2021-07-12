#include "rfpch.hpp"
#include "TestScene.hpp"

namespace Rimfrost
{
	void TestScene::setUpScene()
	{
	}
	void TestScene::update(double dt)
	{
		m_camera.update(static_cast<float>(dt), true);
	}
	Camera& TestScene::camera()
	{
		return m_camera;
	}

	SceneGraph& TestScene::sceneGraph()
	{
		return *m_sceneGraph;
	}

	Lights& TestScene::lights()
	{
		return *m_lights;
	}

}