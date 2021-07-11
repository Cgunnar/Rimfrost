#include "Game.hpp"
#include "Components\TestComponents.hpp"

Rimfrost::EngineApp* Rimfrost::CreateApp()
{
	return new Game();
}

using namespace Rimfrost;

Game::Game()
{
	Entity player = EC::createEntity();
	SphereCollider hej;
	hej.radius = 4245;
	SphereCollider* sp = EC::addComponent<SphereCollider>(player, hej);
	PointMass* y = EC::addComponent<PointMass>(player, PointMass());
	m_entities.push_back(std::move(player));

	auto p = EC::getComponent<PointMass>(m_entities.back());
	float mass = p->mass;
}

Game::~Game()
{
}

void Game::update(double dt)
{
	EC::update(dt);
}
