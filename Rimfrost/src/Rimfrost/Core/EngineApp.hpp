#pragma once

#include "Rimfrost\Graphics\ForwardRenderer.hpp"
#include "Rimfrost\Core\Window.hpp"
#include "Rimfrost\Scene\SceneGraph.hpp"

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

		std::vector<std::shared_ptr<Rimfrost::SceneGraph>> m_scenes;

		Rimfrost::ForwardRenderer m_renderer;

		

		int m_paused;
		bool m_isRunning = false;

	protected:
		std::shared_ptr<Rimfrost::SceneGraph> m_acticeScene;
		virtual void update(double dt) = 0;

	private:
		void setPaused(bool pause);
		bool isPaused() const;

		void handlePauseAndFullscreen();
	};


	//define this function in client app derived from this
	EngineApp* CreateApp();
}
