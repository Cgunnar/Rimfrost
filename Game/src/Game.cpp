
#include <Rimfrost.h>

class Game : public Rimfrost::EngineApp
{
public:
	Game()
	{
	}
	~Game()
	{

	}


};

Rimfrost::EngineApp* Rimfrost::CreateApp()
{
	return new Game();
}