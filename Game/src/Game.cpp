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


	m_scenes.emplace_back(std::make_shared<LevelEditor>("Maps/SandboxMap.json", "Maps/LevelEditorOutPut.json"));
	m_scenes.emplace_back(std::make_shared<SandboxMap>());
	
	m_acticeScene = m_scenes[0];
	m_acticeScene->setUpScene();

	testAddStuffToECS();

	SerializeECS::serialize("Saves/ecsTestSave.json");

	//SceneSerializer::serialize("Maps/SandboxMap.json", *m_acticeScene);
}

Game::~Game()
{
}

void Game::update(double dt)
{
	EC::update(dt);

	m_acticeScene->update(dt);

	
	auto& e = m_entities.back(); //test stuf with entity

	auto pc = e.getComponent<PointLightComponent>();
	pc->position = e.getComponent<NodeComponent>()->nodeHandel->worldMatrix.getTranslation();
	auto& pointLight = m_poinLightMap[pc->getID()];

	pointLight.setPosition(pc->position);
}

void Game::testAddStuffToECS()
{
	Entity redCone = EC::createEntity();
	Entity testE = EC::createEntity();
	NodeComponent nc;

	testE.addComponent(PointMass());
	testE.addComponent(TransformComponent());
	m_entities.push_back(std::move(testE));
	nc.nodeHandel = m_acticeScene->sceneGraph().addModel("Models/red_cone.obj");

	redCone.addComponent<NodeComponent>(nc);
	PointMass pm;
	pm.mass = 42;
	redCone.addComponent<PointMass>(pm);
	
	auto pl = redCone.addComponent<PointLightComponent>(PointLightComponent());
	pl->color = { 0, 0, 1 };
	pl->position = { 0, 2, 0};
	pl->strength = 100;
	
	m_poinLightMap.insert_or_assign(pl->getID(), PointLight(pl->position, pl->color, pl->strength));
	m_acticeScene->lights().pointLights->addPointLight(m_poinLightMap[pl->getID()]);
	
	m_entities.push_back(std::move(redCone));
}

void Game::testLoadStuffToECS()
{

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
