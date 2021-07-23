#include "Game.hpp"
#include "Components\TestComponents.hpp"
#include "Rimfrost\EntCom\rfComponents.hpp"

#include "Rimfrost\EventSystem\EventSystem.hpp"
#include "Rimfrost\EventSystem\MouseEvent.hpp"
#include "Rimfrost\EventSystem\KeyboardEvent.hpp"
#include "Rimfrost\EventSystem\PauseEvent.hpp"
#include "Rimfrost\Scene\LevelEditor.hpp"
#include "Rimfrost\Scene\SceneSerializer.hpp"
#include "Rimfrost\EntCom\SerializeECS.hpp"
#include "SandboxMap.hpp"


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


	m_scenes.emplace_back(std::make_shared<LevelEditor>("Saves/TestSave/", "Saves/TestSave/"));
	m_scenes.emplace_back(std::make_shared<SandboxMap>());

	m_acticeScene = m_scenes[0];
	m_acticeScene->setUpScene();

	testAddStuffToECS();
}

Game::~Game()
{
	m_acticeScene->save("Saves/TestSave/");
	EntityReg::clear();
}

void Game::update(double dt)
{
	m_acticeScene->update(dt);

	assert(!EntityReg::getAllEntities().empty());
}

void Game::testAddStuffToECS()
{
	//warning
	auto n = m_acticeScene->sceneGraph().addModel("Models/green_glass.obj", ModelSettings::TRANSPARENCY_IS_TWOSIDED);
	n->localMatrix.setTranslation(-7, 1, 1);
	Entity e = EntityReg::createEntity();
	e.addComponent(NodeComponent())->nodeHandel = n;
	PointLightComponent p;
	p.color = { 1,0,0 };
	p.strength = 300;
	e.addComponent(p);
}

void Game::testLoadStuffToECS()
{
	//add lights from components
	

}

void Game::onEvent(const Event& e)
{
	if (e.type() == PauseEvent::eventType)
	{
		this->m_acticeScene->camera().lockTranslation(this->isPaused());
		this->m_acticeScene->camera().lockRotation(this->isPaused());
	}

	/*if (e.type() == MouseButtonsEvent::eventType)
	{
		auto& mouse = static_cast<const MouseButtonsEvent&>(e).mouseState;
		if (mouse.RMBClicked || mouse.RMBHeld)
		{
			EventSystem::postTOQueue(MousePickingRequestEvent(mouse.x, mouse.y));
		}
	}*/
}
