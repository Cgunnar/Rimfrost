#pragma once


#include "Graphics.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include "PointLightContainer.hpp"
#include "Scene.hpp"
#include "ForwardRenderer.hpp"

#include "EventObserver.hpp"
#include "EventSystem.hpp"


class App
{
public:
	App();
	~App();
	void run();


private:
	Engine1::Window m_window;

	std::vector<std::shared_ptr<Engine1::Scene>> m_scenes;
	std::shared_ptr<Engine1::Scene> m_acticeScene;

	Engine1::ForwardRenderer m_renderer;

	int m_paused;

private:
	void setPaused(bool pause);
	bool isPaused() const;

	void handlePauseAndFullscreen();
};

