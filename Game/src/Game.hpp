#pragma once

#include <Rimfrost.h>

class Game : public Rimfrost::EngineApp
{
public:
	Game();
	~Game();
protected:
	void update(double dt) override;


};