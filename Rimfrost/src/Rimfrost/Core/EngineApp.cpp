#include "rfpch.hpp"
#include "EngineApp.hpp"
#include "Rimfrost\Utilities\FrameTimer.hpp"
#include "Rimfrost\Core\Window.hpp"
#include "Rimfrost\Core\Mouse.hpp"
#include "Rimfrost\Core\Keyboard.hpp"

#include "Rimfrost\EventSystem\EventSystem.hpp"

using namespace Rimfrost;
namespace Rimfrost
{
	EngineApp::EngineApp() : m_window(L"MyWindow", 1280, 720)
	{
		m_paused = 0;
		m_window.getGraphics().useImGui(true);
		
		

		
	}
	EngineApp::~EngineApp()
	{
	}
	void EngineApp::Run()
	{
		assert(m_isRunning == false); //can only be called once
		m_isRunning = true;
		FrameTimer timer;
		Timer t = Timer(Duration::MICROSECONDS);

		MSG msg = { };
		while (!m_window.isClosed())
		{
			timer.frameStart();

			m_window.getMouse()->update();
			while (PeekMessageW(&msg, m_window.getHwnd(), 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (m_window.isClosed()) continue;

			//pause
			handlePauseAndFullscreen();
			EventSystem::dispatchQueued();




			m_window.getGraphics().beginFrame();
			this->update(timer.dt());
			m_acticeScene->sceneGraph().update(timer.dt());
			m_renderer.renderScene(*m_acticeScene);
			m_window.getGraphics().endFrame();
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
		m_window.getKeyboard()->pollKeyboard();
		if (!isPaused() && m_window.getKeyboard()->IsKeyPressed(DIK_ESCAPE) && !m_window.getKeyboard()->WasKeyPressed(DIK_ESCAPE))
		{
			this->setPaused(true);
			Rimfrost::EventSystem::postInstantly(Rimfrost::PauseEvent(true));
			m_window.getMouse()->confineCursor(false);
			m_window.getMouse()->showCursor(true);
		}
		else if (isPaused() && m_window.getKeyboard()->IsKeyPressed(DIK_ESCAPE) && !m_window.getKeyboard()->WasKeyPressed(DIK_ESCAPE))
		{
			this->setPaused(false);
			Rimfrost::EventSystem::postInstantly(Rimfrost::PauseEvent(false));
			m_window.getMouse()->confineCursor(true);
			m_window.getMouse()->showCursor(false);
		}
		//fullscreen
		if (m_window.getKeyboard()->IsKeyPressed(DIK_F11) && !m_window.getKeyboard()->WasKeyPressed(DIK_F11))
		{
			m_window.getGraphics().setFullScreen(!m_window.getGraphics().isFullScreen());
		}
	}
}


