#pragma once

#include <Rimfrost.h>
#include "Rimfrost\EntCom\rfEntity.hpp"

class Game : public Rimfrost::EngineApp, public Rimfrost::EventObserver
{
public:
	Game();
	~Game();
protected:
	void update(double dt) override;

private:
	void onEvent(const Rimfrost::Event& e) override;
private:
	std::vector<Rimfrost::Entity> m_entities;

};