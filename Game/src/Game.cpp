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


	m_scenes.emplace_back(std::make_shared<LevelEditor>("Maps/LevelEditorOutPut.json", "Maps/LevelEditorOutPut.json"));
	//m_scenes.emplace_back(std::make_shared<LevelEditor>("Maps/SandboxMap.json", "Maps/LevelEditorOutPut.json"));
	m_scenes.emplace_back(std::make_shared<SandboxMap>());

	m_acticeScene = m_scenes[0];
	m_acticeScene->setUpScene();

	

	bool testDeSerialize = true;
	if (testDeSerialize)
	{
		ECSSerializer::deSerialize("Saves/TestSave/", m_entities);
		ECSSerializer::reCoupleWithSceneGraph(m_acticeScene->sceneGraph(), m_entities);
		testLoadStuffToECS();
	}
	else
	{
		testAddStuffToECS();
		ECSSerializer::serialize("Saves/TestSave/");
	}

	//SceneSerializer::serialize("Maps/SandboxMap.json", *m_acticeScene);
}

Game::~Game()
{
}

void Game::update(double dt)
{
	EC::update(dt);

	m_acticeScene->update(dt);



	assert(!m_entities.empty());

	for (auto& pcComp : EC::getComponentArray<PointLightComponent>())
	{
		if (auto nodeComp = EC::getComponent<NodeComponent>(pcComp.getEntityID()); nodeComp)
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
	Entity removeTest0 = EC::createEntity();
	
	Entity testE = EC::createEntity();
	Entity emptyE = EC::createEntity();
	m_entities.push_back(std::move(emptyE));
	NodeComponent nc;

	testE.addComponent(TestComponent2());
	testE.addComponent(TestComponent());
	testE.addComponent(TransformComponent());
	testE.addComponent(PointMass());
	m_entities.push_back(std::move(testE));

	Entity removeThisTest = EC::createEntity();
	

	Entity testE2 = EC::createEntity();
	testE2.addComponent(TestComponent());
	testE2.addComponent(PointMass());
	testE2.addComponent(SphereCollider());
	m_entities.push_back(std::move(testE2));




	

	Entity emptyE2 = EC::createEntity();
	m_entities.push_back(std::move(emptyE2));
	Entity emptyE3 = EC::createEntity();
	m_entities.push_back(std::move(emptyE3));

	EC::removeEntity(removeThisTest);
	EC::removeEntity(removeTest0);

	Entity redCone = EC::createEntity();
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
	m_entities.push_back(std::move(redCone));
}

void Game::testLoadStuffToECS()
{
	//add lights from components
	for (const auto& compL : EC::getComponentArray<PointLightComponent>())
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
