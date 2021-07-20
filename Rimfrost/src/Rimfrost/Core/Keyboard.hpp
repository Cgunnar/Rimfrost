#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>



namespace Rimfrost
{
	enum class Key
	{
		Q, W, E, R, T, Y, U, I, O, P,
		A, S, D, F, G, H, J, K, L,
		Z, X, C, V, B, N, M,

		_1, _2, _3, _4, _5, _6, _7, _8, _9, _0,

		SPACE, LCTRL, LSHIFT, ESC, ENTER, BACK_SPACE,
		TAB, RIGHT, LEFT, UP, DOWN,
	};
	constexpr int numberOfkeys = 47;
	

	enum class KeyState
	{
		KEY_DOWN = 0,
		KEY_UP,
		KEY_CLICKED,
		KEY_RELEASED,
	};

	

	struct KeyAndState
	{
		Key key;
		KeyState state;
	};

	class Keyboard;
	class KeyboardState
	{
		friend Keyboard;
		std::array<KeyState, numberOfkeys> m_keyArray;
	public:
		KeyState& operator[](Key k) { return m_keyArray[static_cast<size_t>(k)]; }
	};

	class Keyboard
	{
	public:
		Keyboard(HWND hWnd, HINSTANCE hInst);
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator= (const Keyboard&) = delete;
		~Keyboard();



		void pollKeyboard();
		bool IsKeyPressed(int key);
		bool WasKeyPressed(int key);

		



	private:
		IDirectInputDevice8* m_pKeyboardDevice = nullptr;
		LPDIRECTINPUT8 m_directInput;

		BYTE m_keyBoardState[256] = { 0 };
		BYTE m_keyBoardPrevState[256] = { 0 };

		
		std::shared_ptr<KeyboardState> m_boardState;
	};

	

}
