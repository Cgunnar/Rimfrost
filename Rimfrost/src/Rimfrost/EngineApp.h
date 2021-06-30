#pragma once

#include "Core.h"

#include "Rimfrost\TestClass.hpp"
#include "Rimfrost\Math\Vector3.hpp"
#include "Rimfrost\Window.hpp"
namespace Rimfrost
{


	class /*RIMFROST_API*/ EngineApp
	{
	public:
		EngineApp();
		virtual ~EngineApp();

		void Run();

		TestClass m_testClass = TestClass(4);

		Vector3 m_vector;
	private:
		Window* m_window;
	};


	//define this function in client app derived from this
	EngineApp* CreateApp();
}
