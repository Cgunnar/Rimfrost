#include "rfpch.hpp"
#include "Scene0.hpp"
#include "RandGen.hpp"
#include "Logger.hpp"
#include "imgui.h"

using namespace DirectX;

namespace Rimfrost
{

	Scene0::Scene0() : Rimfrost::Scene(), m_whiteLight({ 0, 0, 0 }, { 1.0f, 1.0f, 1.0f }, 100, "whiteLight")
	{
	}

	Scene0::~Scene0()
	{
		if (m_lightSphere) delete m_lightSphere;
		if (testHandle) delete testHandle;
	}

	void Scene0::setUpScene()
	{
		m_camera->SetPosition({ 0, 0, -6 });
		m_camera->SetPosition({ 0, 4, 10 });
		m_camera->AddYaw(XM_PI);
		m_pointLightContainer = std::make_shared<PointLightContainer>();
		m_pointLightContainer->addPointLight(m_whiteLight);

		//randomLights
		const int numLights = 128;
		for (int i = 0; i < numLights; i++)
		{
			float x = RandGen::GenRandomFloat(-60, 60, 0 + numLights + i);
			float y = RandGen::GenRandomFloat(1, 60, 1 + numLights + i);
			float z = RandGen::GenRandomFloat(-60, 60, 2 + numLights + i);

			float r = RandGen::GenRandomFloat(0.3f, 1, 3 + numLights + i);
			float g = RandGen::GenRandomFloat(0.3f, 1, 4 + numLights + i);
			float b = RandGen::GenRandomFloat(0.3f, 1, 5 + numLights + i);

			m_randomLights.emplace_back(PointLight(Vector3(x, y, z), Vector3(r, g, b), 5));
			m_pointLightContainer->addPointLight(m_randomLights.back());
		}

		auto sponza = addModel("Models/Sponza2/sponza.obj", Rimfrost::ModelSettings::TRANSPARENCY_IS_TWOSIDED);
		sponza->localMatrix.setTranslation(4.0f, 0, -2.0f);
		sponza->localMatrix.scale(0.07f);

		testHandle = new NodeHandle(addModel("Models/Arrows/DXRefSys.obj"));
		(*testHandle)->localMatrix.setTranslation(0, 1, 2);
		//auto nanoSuit = addModel("Models/brick_wall/brick_wall.obj", *testHandle);
		//auto nanoSuit = addModel("Models/nanosuit/nanosuit.obj", *testHandle);
		auto glassCube = addModel("Models/orangeGlassCube.obj", *testHandle);
		glassCube->localMatrix.setTranslation(-3, 0, 0);

		auto BasisVectors = addModel("Models/Arrows/DXRefSys.obj");

		m_lightSphere = new NodeHandle(addModel("Models/smallInvNormSphere.obj"));

		NodeHandle sun = addModel("Models/Sun/Sun.obj");
		sun->localMatrix.setTranslation(0, 2.0f, -10);

		/*auto sponza = addModel("Models/Sponza2/sponza.obj");
		sponza->localMatrix.setPosition(4.0f, 0, -2.0f);
		sponza->localMatrix.scale(0.07f);*/

		auto nanoSuit1 = addModel("Models/nanosuit_broken_normals/nanosuit.obj");
		nanoSuit1->localMatrix.setTranslation(25, 0, -7);
		nanoSuit1->localMatrix.setRotationDeg(0, 45, 0);
		nanoSuit1->localMatrix.setScale(0.7f);

		auto nanoSuit2 = addModel("Models/nanosuit/nanosuit.obj");
		nanoSuit2->localMatrix.setTranslation(25, 0, 7);
		nanoSuit2->localMatrix.setRotationDeg(0, 90, 0);
		nanoSuit2->localMatrix.setScale(0.7f);

		auto nanoSuit3 = addModel("Models/nanosuit/nanosuit.obj");
		nanoSuit3->localMatrix.setTranslation(8, 0, -7);
		nanoSuit3->localMatrix.setRotationDeg(0, 90, 0);
		nanoSuit3->localMatrix.setScale(0.7f);

		auto nanoSuit4 = addModel("Models/nanosuit/nanosuit.obj");
		nanoSuit4->localMatrix.setTranslation(8, 0, 7);
		nanoSuit4->localMatrix.setRotationDeg(0, 90, 0);
		nanoSuit4->localMatrix.setScale(0.7f);



		auto nanoSuit5 = addModel("Models/nanosuit/nanosuit.obj", Rimfrost::ModelSettings::WIREFRAME);
		nanoSuit5->localMatrix.setTranslation(-8, 0, -7);
		nanoSuit5->localMatrix.setRotationDeg(0, -90, 0);
		nanoSuit5->localMatrix.setScale(0.7f);

		auto nanoSuit6 = addModel("Models/nanosuit/nanosuit.obj");
		nanoSuit6->localMatrix.setTranslation(-8, 0, 7);
		nanoSuit6->localMatrix.setRotationDeg(0, -90, 0);
		nanoSuit6->localMatrix.setScale(0.7f);

		auto nanoSuit7 = addModel("Models/nanosuit/nanosuit.obj");
		nanoSuit7->localMatrix.setTranslation(-25, 0, -7);
		nanoSuit7->localMatrix.setRotationDeg(0, -90, 0);
		nanoSuit7->localMatrix.setScale(0.7f);

		auto nanoSuit8 = addModel("Models/nanosuit/nanosuit.obj");
		nanoSuit8->localMatrix.setTranslation(-25, 0, 7);
		nanoSuit8->localMatrix.setRotationDeg(0, -90, 0);
		nanoSuit8->localMatrix.setScale(0.7f);
	}

	void Scene0::derivedSceneUpdate(double dt)
	{
		//lightcontrol
		ImGui::Begin("light");

		static float lightPos[3] = { 0,0,0 };
		if (ImGui::SliderFloat3("position", (float*)&lightPos, -20, 20))// Edit 3 floats representing a color
		{
			m_whiteLight.setPosition(lightPos[0], lightPos[1], lightPos[2]);
			(*m_lightSphere)->localMatrix.setTranslation(lightPos[0], lightPos[1], lightPos[2]);
		}

		if (ImGui::Button("Button"))
		{
			lightPos[0] = 0;
			lightPos[1] = 1;
			lightPos[2] = 0;
			m_whiteLight.setPosition(lightPos[0], lightPos[1], lightPos[2]);
			(*m_lightSphere)->localMatrix.setTranslation(lightPos[0], lightPos[1], lightPos[2]);
		}

		////ImGui::ColorEdit3("clear color", (float*)&vec3); // Edit 3 floats representing a color
		ImGui::End();



		static float scale = 1;
		static float vec3[3] = { 0,0,0 };




		ImGui::Begin("hej");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		/*ImGui::Text("Camera position");
		ImGui::Text("%.2f %.2f %.2f", m_camera.GetX(), m_camera.GetY(), m_camera.GetZ());*/
		if (ImGui::SliderFloat("scale", &scale, 0.1f, 2.0f))// Edit 1 float using a slider from 0.0f to 1.0f
		{
			(*testHandle)->localMatrix.setScale(scale);
		}


		if (ImGui::SliderFloat3("rotation", (float*)&vec3, -180, 180))// Edit 3 floats representing a color
		{
			(*testHandle)->localMatrix.setRotationDeg(vec3[0], vec3[1], vec3[2]);
		}

		if (ImGui::Button("Button"))
		{
			(*testHandle)->localMatrix.scale(2);
		}

		////ImGui::ColorEdit3("clear color", (float*)&vec3); // Edit 3 floats representing a color
		ImGui::End();


		/*auto rot = XMMatrixRotationRollPitchYaw(0, 0.8f * (float)dt, 0);

		XMFLOAT3 newPos;
		for (auto& l : m_randomLights)
		{

			auto r = XMVector3Transform(l.getPosition(), rot);
			XMStoreFloat3(&newPos, r);
			l.setPosition(newPos);
		}*/

	}
}