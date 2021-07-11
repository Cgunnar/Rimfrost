#include "Game.hpp"
#include "Components\TestComponents.hpp"
#include "Rimfrost\EventSystem\MouseEvent.hpp"

Rimfrost::EngineApp* Rimfrost::CreateApp()
{
	return new Game();
}

using namespace Rimfrost;

Game::Game()
{
	Entity player = EC::createEntity();

	SphereCollider sp;
	sp.radius = 2;
	EC::addComponent<SphereCollider>(player, sp);

	m_entities.push_back(std::move(player));
}

Game::~Game()
{
}

void Game::update(double dt)
{
	EC::update(dt);

	//m_acticeScene->

}

void Game::onEvent(const Event& e)
{
	if (e.type() == MouseButtonsEvent::eventType)
	{
		auto& mouse = static_cast<const MouseButtonsEvent&>(e).mouseState;
		if (mouse.RMBClicked)
		{
		}
		else
		{
		}
	}
}
