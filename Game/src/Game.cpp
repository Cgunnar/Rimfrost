#include "Game.hpp"
#include "Components\TestComponents.hpp"

Rimfrost::EngineApp* Rimfrost::CreateApp()
{
	return new Game();
}

Game::Game()
{
	Rimfrost::Entity player = m_entityReg.createEntity();
	SphereCollider hej;
	hej.radius = 4245;
	SphereCollider* sp = m_entityReg.addComponent<SphereCollider>(player, hej);
	PointMass* y = m_entityReg.addComponent<PointMass>(player, PointMass());
	m_entities.push_back(player);

	auto p = m_entityReg.getComponent<PointMass>(player);
	float mass = p->mass;

	
}

Game::~Game()
{
}

void Game::update(double dt)
{
	this->m_entityReg.update(dt);
}
