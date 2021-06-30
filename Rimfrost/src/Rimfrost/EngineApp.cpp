#include "rfpch.hpp"
#include "EngineApp.h"

namespace Rimfrost
{
	EngineApp::EngineApp()
	{
		m_window = new Window(L"MyWindow", 1280, 720);
	}
	EngineApp::~EngineApp()
	{
	}
	void EngineApp::Run()
	{
		MSG msg = { };
		while (!m_window->isClosed())
		{
			//timer.frameStart();

			//timeA += timer.dt();
			//m_window->getMouse()->update();
			while (PeekMessageW(&msg, m_window->getHwnd(), 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (m_window->isClosed()) continue;
		}
	}
}
