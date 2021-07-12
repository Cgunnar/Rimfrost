#pragma once

#include "Rimfrost\Graphics\ForwardRenderer.hpp"
#include "Rimfrost\Core\Window.hpp"
#include "Rimfrost\Scene\SceneGraph.hpp"
#include "Rimfrost\Scene\IScene.hpp"

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


		Rimfrost::ForwardRenderer m_renderer;

		

		int m_paused;
		bool m_isRunning = false;

	protected:
		std::vector<std::shared_ptr<IScene>> m_scenes;
		std::shared_ptr<IScene> m_acticeScene;
		virtual void update(double dt) = 0;
		void setPaused(bool pause);
		bool isPaused() const;

	private:
		

		void handlePauseAndFullscreen();
	};


	//define this function in client app derived from this
	EngineApp* CreateApp();
}
