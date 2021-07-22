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

	bool testDeSerialize = true;
	if (testDeSerialize)
	{
		/*ECSSerializer::deSerialize("Saves/TestSave/");
		ECSSerializer::reCoupleWithSceneGraph(m_acticeScene->sceneGraph());
		testLoadStuffToECS();*/
	}
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

	for (auto& pcComp : EntityReg::getComponentArray<PointLightComponent>())
	{
		if (auto nodeComp = EntityReg::getComponent<NodeComponent>(pcComp.getEntityID()); nodeComp)
		{	
			assert(m_poinLightMap.contains(pcComp.getKey()));
			auto& pointLight = m_poinLightMap[pcComp.getKey()];

			pcComp.position = nodeComp->nodeHandel->worldMatrix.getTranslation();
			pointLight.setPosition(pcComp.position);
		}
	}
}

void Game::testAddStuffToECS()
{
	Entity removeTest0 = EntityReg::createEntity();
	
	Entity testE = EntityReg::createEntity();
	Entity emptyE = EntityReg::createEntity();
	NodeComponent nc;

	testE.addComponent(TestComponent2());
	testE.addComponent(TestComponent());
	testE.addComponent(TransformComponent());
	testE.addComponent(PointMass());

	Entity removeThisTest = EntityReg::createEntity();

	Entity testE2 = EntityReg::createEntity();
	testE2.addComponent(TestComponent());
	testE2.addComponent(PointMass());
	testE2.addComponent(SphereCollider());
	

	Entity emptyE2 = EntityReg::createEntity();
	Entity emptyE3 = EntityReg::createEntity();

	EntityReg::removeEntity(removeThisTest);
	removeTest0.addComponent(SphereCollider());
	EntityReg::removeEntity(removeTest0);

	Entity redCone = EntityReg::createEntity();
	nc.nodeHandel = m_acticeScene->sceneGraph().addModel("Models/red_cone.obj");

	redCone.addComponent<NodeComponent>(nc);
	PointMass pm;
	pm.mass = 42;
	redCone.addComponent<PointMass>(pm);

	auto pl = redCone.addComponent<PointLightComponent>(PointLightComponent());
	pl->color = { 0, 0, 1 };
	pl->position = { 0, 2, 0 };
	pl->strength = 100;

	m_poinLightMap.insert_or_assign(pl->getKey(), PointLight(pl->position, pl->color, pl->strength));
	m_acticeScene->lights().pointLights->addPointLight(m_poinLightMap[pl->getKey()]);
}

void Game::testLoadStuffToECS()
{
	//add lights from components
	for (const auto& compL : EntityReg::getComponentArray<PointLightComponent>())
	{
		assert(!m_poinLightMap.contains(compL.getKey()));
		m_poinLightMap.insert_or_assign(compL.getKey(), PointLight(compL.position, compL.color, compL.strength));
		m_acticeScene->lights().pointLights->addPointLight(m_poinLightMap[compL.getKey()]);
	}

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
