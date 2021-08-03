#include "Game.hpp"
#include "Components\TestComponents.hpp"
#include "Rimfrost\EntCom\rfComponents.hpp"


#include "Rimfrost\LevelEditor\LevelEditor.hpp"
#include "Rimfrost\Scene\SceneSerializer.hpp"
#include "Rimfrost\EntCom\SerializeECS.hpp"
#include "Rimfrost\EntCom\rfEntity.hpp"
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

	/*auto n = m_acticeScene->sceneGraph().addNode();
	n->localMatrix.setTranslation(-7, 1, 1);
	Entity e = EntityReg::createEntity();
	e.addComponent(NodeComponent())->nodeHandel = n;
	PointLightComponent p;
	p.color = { 1,1,1 };
	p.strength = 300;
	p.position = { -7, 1, 1 };
	e.addComponent(p);

	m_entities.emplace_back(std::move(e));*/

	//testAddStuffToECS();
}

Game::~Game()
{
	m_acticeScene->save("Saves/TestSave/");
	m_entities.clear();
	m_acticeScene.reset();
	m_scenes.clear();
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
	p.position = { -7, 1, 1 };
	e.addComponent(p);
	
	
	//EntityReg::removeEntity(e);
}

void Game::testLoadStuffToECS()
{
	//add lights from components
	

}

void Game::onEvent(const Event& e)
{
	if (e.type() == PauseEvent::eventType)
	{
		bool lock = this->isPaused();
		this->m_acticeScene->camera().lockCamera(lock, lock);
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
