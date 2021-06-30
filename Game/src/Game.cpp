
#include <Rimfrost.h>

class Game : public Rimfrost::EngineApp
{
public:
	Game()
	{
		m_testClass.print();
		m_vector.lengt();
	}
	~Game()
	{

	}


};

Rimfrost::EngineApp* Rimfrost::CreateApp()
{
	return new Game();
}