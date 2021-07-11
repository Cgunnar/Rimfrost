#pragma once

#include <Rimfrost.h>
#include "Rimfrost\EntCom\rfEntity.hpp"

class Game : public Rimfrost::EngineApp
{
public:
	Game();
	~Game();
protected:
	void update(double dt) override;

private:

	std::vector<Rimfrost::Entity> m_entities;

};