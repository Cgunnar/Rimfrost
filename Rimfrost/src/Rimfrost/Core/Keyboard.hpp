#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>



namespace Rimfrost
{
	class Keyboard
	{
	public:
		Keyboard(HWND hWnd, HINSTANCE hInst);
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator= (const Keyboard&) = delete;
		~Keyboard();



		void TakeKeyboardInput();
		bool IsKeyPressed(int key);
		bool WasKeyPressed(int key);



	private:
		IDirectInputDevice8* m_pKeyboardDevice = nullptr;
		LPDIRECTINPUT8 m_directInput;

		BYTE m_keyBoardState[256] = { 0 };
		BYTE m_keyBoardPrevState[256] = { 0 };

	};
}
