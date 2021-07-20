#include "rfpch.hpp"
#include "SandboxMap.hpp"
#include "RandGen.hpp"
#include "Logger.hpp"
#include "imgui.h"

using namespace DirectX;
namespace Rimfrost
{

	SandboxMap::SandboxMap() : m_whiteLight({ 4, 6, 8 }, { 1.0f, 1.0f, 1.0f }, 100, "whiteLight"), 
		m_redLight({ 1, 4, 3 }, { 1.0f, 0, 0 }, 80, "redLight")
	{
		m_sceneGraph = std::make_unique<SceneGraph>();
	}

	SandboxMap::~SandboxMap()
	{
		if (testHandle) delete testHandle;
	}

	void SandboxMap::setUpScene()
	{
		m_camera.SetPosition({ 0, 0, -4 });
		m_lights.pointLights = std::make_shared<PointLightContainer>();
		m_lights.pointLights->addPointLight(m_whiteLight);
		m_lights.pointLights->addPointLight(m_redLight);

		auto brickWall = m_sceneGraph->addModel("Models/brick_wall/brick_wall.obj");
		brickWall->localMatrix.setTranslation(0, -1, 0);
		brickWall->localMatrix.setRotationDeg(90, 0, 0);
		brickWall->localMatrix.setScale(4);

		testHandle = new NodeHandle(m_sceneGraph->addModel("Models/ring/red_ring.obj", Rimfrost::ModelSettings::WIREFRAME));
		(*testHandle)->localMatrix.setTranslation(0, 0, 4);
		//(*testHandle)->localMatrix.setRotationDeg(40, 0 );

		//testHandle = new NodeHandle(addModel("Models/Arrows/DXRefSys.obj"));
		//(*testHandle)->localMatrix.setScale(0.3f);

		/*(*testHandle)->localMatrix.setPosition(0, 1, 2);

		auto brickWall = addModel("Models/brick_wall/brick_wall.obj", *testHandle);
		auto nanoSuit = addModel("Models/nanosuit/nanosuit.obj", *testHandle);
		nanoSuit->localMatrix.setScale(0.4f);
		nanoSuit->localMatrix.setPosition(-2, -1.4f, 1);*/

		auto BasisVectors = m_sceneGraph->addModel("Models/Arrows/DXRefSys.obj");
		auto cube = m_sceneGraph->addModel("Models/orangeGlassCube.obj", *testHandle);

		auto nanoSuit2 = m_sceneGraph->addModel("Models/nanosuit/nanosuit.obj");
		nanoSuit2->localMatrix.setTranslation(25, 0, 7);
		nanoSuit2->localMatrix.setRotationDeg(0, 0, 0);
		nanoSuit2->localMatrix.setScale(0.7f);

		auto cube2 = m_sceneGraph->addModel("Models/orangeGlassCube.obj");

		auto vectors = m_sceneGraph->addModel("Models/Arrows/DXRefSys.obj");
		vectors->localMatrix.setTranslation(7, 4, 0);
		vectors->localMatrix.setScale(4.5f);

	}

	void SandboxMap::loadFromFile(std::string path)
	{

	}

	void SandboxMap::saveToFile(std::string path)
	{

	}

	Camera& SandboxMap::camera()
	{
		return m_camera;
	}

	SceneGraph& SandboxMap::sceneGraph()
	{
		return *m_sceneGraph;
	}

	Lights& SandboxMap::lights()
	{
		return m_lights;
	}

	void SandboxMap::update(double dt)
	{
		m_camera.update(static_cast<float>(dt));

		//lightcontrol
		//ImGui::Begin("light");

		//static float lightPos[3] = { 0,0,0 };
		//if (ImGui::SliderFloat3("position", (float*)&lightPos, -10, 10))// Edit 3 floats representing a color
		//{
		//	m_whiteLight.setPosition(lightPos[0], lightPos[1], lightPos[2]);
		//	(*m_lightSphere)->localMatrix.setPosition(lightPos[0], lightPos[1], lightPos[2]);
		//}

		//if (ImGui::Button("Button"))
		//{
		//	lightPos[0] = 0;
		//	lightPos[1] = 1;
		//	lightPos[2] = 0;
		//	m_whiteLight.setPosition(lightPos[0], lightPos[1], lightPos[2]);
		//	(*m_lightSphere)->localMatrix.setPosition(lightPos[0], lightPos[1], lightPos[2]);
		//}
		//ImGui::End();



		static float scale = 1;
		static float vec3[3] = { 0,0,0 };

		auto& m = (*testHandle)->localMatrix;
		ImGui::Begin("broken rotation");
		//if (ImGui::SliderFloat("scale", &scale, 0.1f, 2.0f))// Edit 1 float using a slider from 0.0f to 1.0f
		//{
		//	(*testHandle)->localMatrix.setScale(scale);
		//}

		if (ImGui::SliderFloat3("rotation", (float*)&vec3, -90, 90))// Edit 3 floats representing a color
		{
			(*testHandle)->localMatrix.setRotationDeg(vec3[0], vec3[1], vec3[2]);
			//(*testHandle)->localMatrix.setRotationDeg(vec3[1], vec3[0], vec3[2]);
		}
		/*auto r = (*m.getRotationMatrix());
		ImGui::Text("Rotation matrix in column major");
		ImGui::Text("%.2f %.2f %.2f", r._11, r._21, r._31);
		ImGui::Text("%.2f %.2f %.2f", r._12, r._22, r._32);
		ImGui::Text("%.2f %.2f %.2f", r._13, r._23, r._33);*/
		//ImGui::Text();

		if (ImGui::Button("Button"))
		{
			//scale = 1;
			vec3[0] = 0;
			vec3[1] = 0;
			vec3[2] = 0;
			//(*testHandle)->localMatrix.scale(scale);
			(*testHandle)->localMatrix.setRotationDeg(vec3[0], vec3[1], vec3[2]);
		}

		////ImGui::ColorEdit3("clear color", (float*)&vec3); // Edit 3 floats representing a color
		ImGui::End();

	}
}