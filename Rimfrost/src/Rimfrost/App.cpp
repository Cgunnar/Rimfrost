#include "pch.hpp"
#include "App.hpp"
#include "Window.hpp"
#include "FrameTimer.hpp"
#include "Logger.hpp"

#include "RandGen.hpp"
#include "PointLight.hpp"
#include "Scene0.hpp"
#include "Scene1.hpp"
#include "LevelEditor.hpp"
#include "SceneSerializer.hpp"

#include "imgui.h"

#include "PauseEvent.hpp"

#ifndef UNICODE
#define UNICODE
#endif

using namespace DirectX;
using namespace Engine1;


App::App() : m_window(L"MyWindow", 1280, 720)
{
	m_paused = 0;
	m_window.getGraphics().useImGui(true);

	m_scenes.emplace_back(std::make_shared<Scene0>());
	m_scenes.emplace_back(std::make_shared<Scene1>());
	m_scenes.emplace_back(std::make_shared<LevelEditor>());

	//serializationTesting
	m_scenes[0]->setUpScene();
	Engine1::SceneSerializer::serialize("LEVEL.json", m_scenes[0]);
	

	Engine1::SceneSerializer::deSerialize("LEVEL.json", m_scenes[2]);

	m_acticeScene = m_scenes[2];
	m_acticeScene->bindInput(m_window.getKeyboard(), m_window.getMouse());
	m_acticeScene->setUpScene();
	
}

App::~App()
{

}


void App::run()
{
	double deltaTime = 0.0;			//dt need to be constant in physics update
	constexpr double timeStep = 1.0 / 60.0;	//dt need to be constant in physics update

	double timeA = 0;

	FrameTimer timer;
	Timer t = Timer(Duration::MICROSECONDS);
	MSG msg = { };
	while (!m_window.isClosed())
	{
		timer.frameStart();

		timeA += timer.dt();
		m_window.getMouse()->update();
		while (PeekMessageW(&msg, m_window.getHwnd(), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (m_window.isClosed()) continue;

		//pause
		handlePauseAndFullscreen();
		m_window.getGraphics().beginFrame();

		if (!isPaused())
		{

			//physics
			deltaTime += timer.dt();
			while (timeStep < deltaTime)
			{

				deltaTime -= timeStep;
			}
		}
		m_acticeScene->update(timer.dt());
		m_renderer.renderScene(m_acticeScene);
		m_window.getGraphics().endFrame();
		timer.frameStop();
	}
}

void App::setPaused(bool pause)
{
	if (pause)
		this->m_paused++;
	else if (this->m_paused > 0)
		this->m_paused--;
}

bool App::isPaused() const
{
	return this->m_paused > 0 ? true : false;
}

void App::handlePauseAndFullscreen()
{
	m_window.getKeyboard()->TakeKeyboardInput();
	if (!isPaused() && m_window.getKeyboard()->IsKeyPressed(DIK_ESCAPE) && !m_window.getKeyboard()->WasKeyPressed(DIK_ESCAPE))
	{
		this->setPaused(true);
		Engine1::EventSystem::post(PauseEvent(true));
		m_window.getMouse()->confineCursor(false);
		m_window.getMouse()->showCursor(true);
#ifdef DirectInput
		m_window.getMouse()->Unacquire();
#endif
	}
	else if (isPaused() && m_window.getKeyboard()->IsKeyPressed(DIK_ESCAPE) && !m_window.getKeyboard()->WasKeyPressed(DIK_ESCAPE))
	{
		this->setPaused(false);
		EventSystem::post(PauseEvent(false));
		m_window.getMouse()->confineCursor(true);
		m_window.getMouse()->showCursor(false);
	}
	//fullscreen
	if (m_window.getKeyboard()->IsKeyPressed(DIK_F11) && !m_window.getKeyboard()->WasKeyPressed(DIK_F11))
	{
		m_window.getGraphics().setFullScreen(!m_window.getGraphics().isFullScreen());
	}
}

