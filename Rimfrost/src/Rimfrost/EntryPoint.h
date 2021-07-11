#pragma once

#include <Windows.h>

extern Rimfrost::EngineApp* Rimfrost::CreateApp();


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
	Rimfrost::EngineApp* gameApp = Rimfrost::CreateApp();
	gameApp->Run();
	delete gameApp;
	return 0;
}

//int main(int argc, char** argv)
//{
//	Rimfrost::EngineApp* gameApp = Rimfrost::CreateApp();
//	gameApp->Run();
//	delete gameApp;
//}