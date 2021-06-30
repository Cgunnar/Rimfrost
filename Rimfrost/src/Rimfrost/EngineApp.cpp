#include "rfpch.hpp"
#include "EngineApp.h"
#include <Rimfrost\Utilities\FrameTimer.hpp>
#include "Rimfrost\Window.hpp"
#include "Rimfrost\Core\Mouse.hpp"
#include "Rimfrost\Core\Keyboard.hpp"

#include "Rimfrost\Scenes\Scene0.hpp"
#include "Rimfrost\Scenes\Scene1.hpp"
#include "Rimfrost\Scenes\LevelEditor.hpp"
#include "Rimfrost\Scene\SceneSerializer.hpp"
#include <Rimfrost\EventSystem\EventSystem.hpp>

namespace Rimfrost
{
	EngineApp::EngineApp()
	{
		m_window = new Engine1::Window(L"MyWindow", 1280, 720);


		m_paused = 0;
		m_window->getGraphics().useImGui(true);

		m_scenes.emplace_back(std::make_shared<Engine1::Scene0>());
		m_scenes.emplace_back(std::make_shared<Engine1::Scene1>());
		m_scenes.emplace_back(std::make_shared<Engine1::LevelEditor>());

		//serializationTesting
		m_scenes[0]->setUpScene();
		Engine1::SceneSerializer::serialize("LEVEL.json", m_scenes[0]);


		Engine1::SceneSerializer::deSerialize("LEVEL.json", m_scenes[2]);

		m_acticeScene = m_scenes[2];
		m_acticeScene->bindInput(m_window->getKeyboard(), m_window->getMouse());
		m_acticeScene->setUpScene();
	}
	EngineApp::~EngineApp()
	{
	}
	void EngineApp::Run()
	{

		FrameTimer timer;
		Timer t = Timer(Duration::MICROSECONDS);

		MSG msg = { };
		while (!m_window->isClosed())
		{
			timer.frameStart();

			m_window->getMouse()->update();
			while (PeekMessageW(&msg, m_window->getHwnd(), 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (m_window->isClosed()) continue;


			//pause
			handlePauseAndFullscreen();
			//m_window->getGraphics().beginFrame();

			//if (!isPaused())
			//{

			//	//physics
			//	deltaTime += timer.dt();
			//	while (timeStep < deltaTime)
			//	{

			//		deltaTime -= timeStep;
			//	}
			//}
			m_acticeScene->update(timer.dt());
			m_renderer.renderScene(m_acticeScene);
			m_window->getGraphics().endFrame();
			timer.frameStop();
		}
	}

	void EngineApp::setPaused(bool pause)
	{
		if (pause)
			this->m_paused++;
		else if (this->m_paused > 0)
			this->m_paused--;
	}

	bool EngineApp::isPaused() const
	{
		return this->m_paused > 0 ? true : false;
	}

	void EngineApp::handlePauseAndFullscreen()
	{
		m_window->getKeyboard()->TakeKeyboardInput();
		if (!isPaused() && m_window->getKeyboard()->IsKeyPressed(DIK_ESCAPE) && !m_window->getKeyboard()->WasKeyPressed(DIK_ESCAPE))
		{
			this->setPaused(true);
			Engine1::EventSystem::post(Engine1::PauseEvent(true));
			m_window->getMouse()->confineCursor(false);
			m_window->getMouse()->showCursor(true);
		}
		else if (isPaused() && m_window->getKeyboard()->IsKeyPressed(DIK_ESCAPE) && !m_window->getKeyboard()->WasKeyPressed(DIK_ESCAPE))
		{
			this->setPaused(false);
			Engine1::EventSystem::post(Engine1::PauseEvent(false));
			m_window->getMouse()->confineCursor(true);
			m_window->getMouse()->showCursor(false);
		}
		//fullscreen
		if (m_window->getKeyboard()->IsKeyPressed(DIK_F11) && !m_window->getKeyboard()->WasKeyPressed(DIK_F11))
		{
			m_window->getGraphics().setFullScreen(!m_window->getGraphics().isFullScreen());
		}
	}
}


