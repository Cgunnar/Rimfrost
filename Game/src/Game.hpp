#pragma once

#include <Rimfrost.hpp>
#include "Rimfrost\EntCom\rfEntity.hpp"

class Game : public Rimfrost::EngineApp, public Rimfrost::EventObserver
{
public:
	Game();
	~Game();


	void onEvent(const Rimfrost::Event& e) override;
protected:
	void update(double dt) override;

private:
	void testAddStuffToECS();
	void testLoadStuffToECS();

private:
	std::vector<Rimfrost::Entity> m_entities;
	std::unordered_map<int64_t, Rimfrost::PointLight> m_poinLightMap;

};