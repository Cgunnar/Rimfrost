#pragma once

#include "Core.h"

#include "Rimfrost\Graphics\ForwardRenderer.hpp"
#include "Rimfrost\Window.hpp"


#include "Rimfrost\Scene\Scene.hpp"
#include "Rimfrost\EntCom\EntityRegistry.hpp"

namespace Rimfrost
{


	class EngineApp
	{
	public:
		EngineApp();
		virtual ~EngineApp();

		void Run();
	private:
		Rimfrost::Window m_window;

		std::vector<std::shared_ptr<Rimfrost::Scene>> m_scenes;
		std::shared_ptr<Rimfrost::Scene> m_acticeScene;

		Rimfrost::ForwardRenderer m_renderer;

		

		int m_paused;
		bool m_isRunning = false;

	protected:
		EntityRegistry m_entityReg;
		virtual void update(double dt) abstract;

	private:
		void setPaused(bool pause);
		bool isPaused() const;

		void handlePauseAndFullscreen();
	};


	//define this function in client app derived from this
	EngineApp* CreateApp();
}
