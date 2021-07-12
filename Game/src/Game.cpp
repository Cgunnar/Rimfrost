#include "Game.hpp"
#include "Components\TestComponents.hpp"


#include "Rimfrost\EventSystem\EventSystem.hpp"
#include "Rimfrost\EventSystem\MouseEvent.hpp"
#include "Rimfrost\EventSystem\KeyboardEvent.hpp"
#include "Rimfrost\EventSystem\PauseEvent.hpp"

Rimfrost::EngineApp* Rimfrost::CreateApp()
{
	return new Game();
}

using namespace Rimfrost;

Game::Game()
{
	EventSystem::addObserver(*this, PauseEvent::eventType);
	EventSystem::addObserver(*this, MouseButtonsEvent::eventType);
	EventSystem::addObserver(*this, MouseMoveEvent::eventType);
	EventSystem::addObserver(*this, KeyboardEvent::eventType);


	Entity player = EC::createEntity();

	SphereCollider sp;
	sp.radius = 2;
	EC::addComponent<SphereCollider>(player, sp);

	m_entities.push_back(std::move(player));

	m_acticeScene->setUpScene();
}

Game::~Game()
{
}

void Game::update(double dt)
{
	EC::update(dt);

	m_acticeScene->update(dt);

}

void Game::onEvent(const Event& e)
{
	if (e.type() == PauseEvent::eventType)
	{
		this->m_acticeScene->camera().lockTranslation(this->isPaused());
		this->m_acticeScene->camera().lockRotation(this->isPaused());
	}

	auto& mouse = static_cast<const MouseButtonsEvent&>(e).mouseState;
	if (mouse.RMBClicked)
	{

	}
	else
	{
	}
}
