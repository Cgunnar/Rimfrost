#pragma once

#include "Core.h"

#include "Rimfrost\Graphics\ForwardRenderer.hpp"
#include "Rimfrost\Window.hpp"


#include "Rimfrost\Scene\Scene.hpp"
namespace Rimfrost
{


	class EngineApp
	{
	public:
		EngineApp();
		virtual ~EngineApp();

		void Run();
	private:
		Engine1::Window m_window;

		std::vector<std::shared_ptr<Engine1::Scene>> m_scenes;
		std::shared_ptr<Engine1::Scene> m_acticeScene;

		Engine1::ForwardRenderer m_renderer;

		int m_paused;
		bool m_isRunning = false;

	protected:
		virtual void update(double dt) abstract;

	private:
		void setPaused(bool pause);
		bool isPaused() const;

		void handlePauseAndFullscreen();
	};


	//define this function in client app derived from this
	EngineApp* CreateApp();
}
