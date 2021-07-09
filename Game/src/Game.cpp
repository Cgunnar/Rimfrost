#include "Game.hpp"
#include "Components\TestComponents.hpp"

Rimfrost::EngineApp* Rimfrost::CreateApp()
{
	return new Game();
}

Game::Game()
{
	Rimfrost::Entity player = m_entityReg.createEntity();
	SphereCollider* sp = m_entityReg.addComponent<SphereCollider>(player, SphereCollider());
	m_entities.push_back(player);
}

Game::~Game()
{
}

void Game::update(double dt)
{
	this->m_entityReg.update(dt);
}
