#include "rfpch.hpp"
#include "Keyboard.hpp"
#include <assert.h>

#include "Logger.hpp"
#include "EventSystem.hpp"
#include "KeyboardEvents.hpp"

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

namespace Rimfrost
{

	Keyboard::Keyboard(HWND hWnd, HINSTANCE hInst)
	{
		HRESULT hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
		assert(SUCCEEDED(hr));

		hr = m_directInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, NULL);
		assert(SUCCEEDED(hr));

		hr = m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
		assert(SUCCEEDED(hr));

		hr = m_pKeyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		assert(SUCCEEDED(hr));
	}

	Keyboard::~Keyboard()
	{
		Logger::getLogger().debugLog("~KeyBoard\n");
		m_pKeyboardDevice->Unacquire();
		m_pKeyboardDevice->Release();
		m_directInput->Release();
	}

	bool Keyboard::IsKeyPressed(int key)
	{
		return m_keyBoardState[key] & 0x80;
	}

	bool Keyboard::WasKeyPressed(int key)
	{
		return m_keyBoardPrevState[key] & 0x80;
	}

	void Keyboard::pollKeyboard()
	{
		memcpy(m_keyBoardPrevState, m_keyBoardState, sizeof(m_keyBoardState));
		m_pKeyboardDevice->Acquire();
		m_pKeyboardDevice->GetDeviceState(sizeof(m_keyBoardState), (LPVOID)&m_keyBoardState);


		
		// ESCAPE
		if (IsKeyPressed(DIK_ESCAPE) && WasKeyPressed(DIK_ESCAPE))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::ESC, KeyState::KEY_DOWN }));
		}
		else if(IsKeyPressed(DIK_ESCAPE) && !WasKeyPressed(DIK_ESCAPE))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::ESC, KeyState::KEY_CLICKED }));
			EventSystem::postInstantly(KeyboardEvent({ Key::ESC, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_ESCAPE) && WasKeyPressed(DIK_ESCAPE))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::ESC, KeyState::KEY_RELEASED }));
		}

		//SPACE
		if (IsKeyPressed(DIK_SPACE) && WasKeyPressed(DIK_SPACE))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::SPACE, KeyState::KEY_DOWN }, ' '));
		}
		else if (IsKeyPressed(DIK_SPACE) && !WasKeyPressed(DIK_SPACE))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::SPACE, KeyState::KEY_CLICKED }, ' '));
			EventSystem::postInstantly(KeyboardEvent({ Key::SPACE, KeyState::KEY_DOWN }, ' '));
		}
		else if (!IsKeyPressed(DIK_SPACE) && WasKeyPressed(DIK_SPACE))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::SPACE, KeyState::KEY_RELEASED }));
		}

		//TAB
		if (IsKeyPressed(DIK_TAB) && WasKeyPressed(DIK_TAB))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::TAB, KeyState::KEY_DOWN }, '\t'));
		}
		else if (IsKeyPressed(DIK_TAB) && !WasKeyPressed(DIK_TAB))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::TAB, KeyState::KEY_CLICKED }, '\t'));
			EventSystem::postInstantly(KeyboardEvent({ Key::TAB, KeyState::KEY_DOWN }, '\t'));
		}
		else if (!IsKeyPressed(DIK_TAB) && WasKeyPressed(DIK_TAB))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::TAB, KeyState::KEY_RELEASED }));
		}

		//LCTRL
		if (IsKeyPressed(DIK_LCONTROL) && WasKeyPressed(DIK_LCONTROL))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::LCTRL, KeyState::KEY_DOWN }));
		}
		else if (IsKeyPressed(DIK_LCONTROL) && !WasKeyPressed(DIK_LCONTROL))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::LCTRL, KeyState::KEY_CLICKED }));
			EventSystem::postInstantly(KeyboardEvent({ Key::LCTRL, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_LCONTROL) && WasKeyPressed(DIK_LCONTROL))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::LCTRL, KeyState::KEY_RELEASED }));
		}

		//LSHIFT
		char shift = 0;
		if (IsKeyPressed(DIK_LSHIFT) && WasKeyPressed(DIK_LSHIFT))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::LSHIFT, KeyState::KEY_DOWN }));
			shift = 32;
		}
		else if (IsKeyPressed(DIK_LSHIFT) && !WasKeyPressed(DIK_LSHIFT))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::LSHIFT, KeyState::KEY_CLICKED }));
			EventSystem::postInstantly(KeyboardEvent({ Key::LSHIFT, KeyState::KEY_DOWN }));
			shift = 32;
		}
		else if (!IsKeyPressed(DIK_LSHIFT) && WasKeyPressed(DIK_LSHIFT))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::LSHIFT, KeyState::KEY_RELEASED }));
		}

		//ENTER
		if (IsKeyPressed(DIK_RETURN) && WasKeyPressed(DIK_RETURN))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::ENTER, KeyState::KEY_DOWN }, '\n'));
		}
		else if (IsKeyPressed(DIK_RETURN) && !WasKeyPressed(DIK_RETURN))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::ENTER, KeyState::KEY_CLICKED }, '\n'));
			EventSystem::postInstantly(KeyboardEvent({ Key::ENTER, KeyState::KEY_DOWN }, '\n'));
		}
		else if (!IsKeyPressed(DIK_RETURN) && WasKeyPressed(DIK_RETURN))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::ENTER, KeyState::KEY_RELEASED }));
		}

		//BACK_SPACE
		if (IsKeyPressed(DIK_BACK) && WasKeyPressed(DIK_BACK))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::BACK_SPACE, KeyState::KEY_DOWN }));
		}
		else if (IsKeyPressed(DIK_BACK) && !WasKeyPressed(DIK_BACK))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::BACK_SPACE, KeyState::KEY_CLICKED }));
			EventSystem::postInstantly(KeyboardEvent({ Key::BACK_SPACE, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_BACK) && WasKeyPressed(DIK_BACK))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::BACK_SPACE, KeyState::KEY_RELEASED }));
		}

		//ARROW_DOWN
		if (IsKeyPressed(DIK_DOWNARROW) && WasKeyPressed(DIK_DOWNARROW))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::DOWN, KeyState::KEY_DOWN }));
		}
		else if (IsKeyPressed(DIK_DOWNARROW) && !WasKeyPressed(DIK_DOWNARROW))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::DOWN, KeyState::KEY_CLICKED }));
			EventSystem::postInstantly(KeyboardEvent({ Key::DOWN, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_DOWNARROW) && WasKeyPressed(DIK_DOWNARROW))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::DOWN, KeyState::KEY_RELEASED }));
		}

		//ARROW_UP
		if (IsKeyPressed(DIK_UPARROW) && WasKeyPressed(DIK_UPARROW))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::UP, KeyState::KEY_DOWN }));
		}
		else if (IsKeyPressed(DIK_UPARROW) && !WasKeyPressed(DIK_UPARROW))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::UP, KeyState::KEY_CLICKED }));
			EventSystem::postInstantly(KeyboardEvent({ Key::UP, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_UPARROW) && WasKeyPressed(DIK_UPARROW))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::UP, KeyState::KEY_RELEASED }));
		}

		//ARROW_RIGHT
		if (IsKeyPressed(DIK_RIGHTARROW) && WasKeyPressed(DIK_RIGHTARROW))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::RIGHT, KeyState::KEY_DOWN }));
		}
		else if (IsKeyPressed(DIK_RIGHTARROW) && !WasKeyPressed(DIK_RIGHTARROW))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::RIGHT, KeyState::KEY_CLICKED }));
			EventSystem::postInstantly(KeyboardEvent({ Key::RIGHT, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_RIGHTARROW) && WasKeyPressed(DIK_RIGHTARROW))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::RIGHT, KeyState::KEY_RELEASED }));
		}
		
		//ARROW_LEFT
		if (IsKeyPressed(DIK_LEFTARROW) && WasKeyPressed(DIK_LEFTARROW))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::LEFT, KeyState::KEY_DOWN }));
		}
		else if (IsKeyPressed(DIK_LEFTARROW) && !WasKeyPressed(DIK_LEFTARROW))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::LEFT, KeyState::KEY_CLICKED }));
			EventSystem::postInstantly(KeyboardEvent({ Key::LEFT, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_LEFTARROW) && WasKeyPressed(DIK_LEFTARROW))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::LEFT, KeyState::KEY_RELEASED }));
		}

		// characters----------------------------
		
		//A
		if (IsKeyPressed(DIK_A) && WasKeyPressed(DIK_A))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::A, KeyState::KEY_DOWN }, 'a' - shift));
		}
		else if (IsKeyPressed(DIK_A) && !WasKeyPressed(DIK_A))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::A, KeyState::KEY_CLICKED }, 'a' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::A, KeyState::KEY_DOWN }, 'a' - shift));
		}
		else if (!IsKeyPressed(DIK_A) && WasKeyPressed(DIK_A))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::A, KeyState::KEY_RELEASED }));
		}

		//B
		if (IsKeyPressed(DIK_B) && WasKeyPressed(DIK_B))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::B, KeyState::KEY_DOWN }, 'b' - shift));
		}
		else if (IsKeyPressed(DIK_B) && !WasKeyPressed(DIK_B))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::B, KeyState::KEY_CLICKED }, 'b' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::B, KeyState::KEY_DOWN }, 'b' - shift));
		}
		else if (!IsKeyPressed(DIK_B) && WasKeyPressed(DIK_B))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::B, KeyState::KEY_RELEASED }));
		}

		//C
		if (IsKeyPressed(DIK_C) && WasKeyPressed(DIK_C))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::C, KeyState::KEY_DOWN }, 'C' - shift));
		}
		else if (IsKeyPressed(DIK_C) && !WasKeyPressed(DIK_C))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::C, KeyState::KEY_CLICKED }, 'c' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::C, KeyState::KEY_DOWN }, 'c' - shift));
		}
		else if (!IsKeyPressed(DIK_C) && WasKeyPressed(DIK_C))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::C, KeyState::KEY_RELEASED }));
		}

		//D
		if (IsKeyPressed(DIK_D) && WasKeyPressed(DIK_D))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::D, KeyState::KEY_DOWN }, 'd' - shift));
		}
		else if (IsKeyPressed(DIK_D) && !WasKeyPressed(DIK_D))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::D, KeyState::KEY_CLICKED }, 'd' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::D, KeyState::KEY_DOWN }, 'd' - shift));
		}
		else if (!IsKeyPressed(DIK_D) && WasKeyPressed(DIK_D))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::D, KeyState::KEY_RELEASED }));
		}

		//E
		if (IsKeyPressed(DIK_E) && WasKeyPressed(DIK_E))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::E, KeyState::KEY_DOWN }, 'e' - shift));
		}
		else if (IsKeyPressed(DIK_E) && !WasKeyPressed(DIK_E))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::E, KeyState::KEY_CLICKED }, 'e' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::E, KeyState::KEY_DOWN }, 'e' - shift));
		}
		else if (!IsKeyPressed(DIK_E) && WasKeyPressed(DIK_E))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::E, KeyState::KEY_RELEASED }));
		}

		//F
		if (IsKeyPressed(DIK_F) && WasKeyPressed(DIK_F))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::F, KeyState::KEY_DOWN }, 'f' - shift));
		}
		else if (IsKeyPressed(DIK_F) && !WasKeyPressed(DIK_F))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::F, KeyState::KEY_CLICKED }, 'f' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::F, KeyState::KEY_DOWN }, 'f' - shift));
		}
		else if (!IsKeyPressed(DIK_F) && WasKeyPressed(DIK_F))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::F, KeyState::KEY_RELEASED }));
		}

		//G
		if (IsKeyPressed(DIK_G) && WasKeyPressed(DIK_G))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::G, KeyState::KEY_DOWN }, 'g' - shift));
		}
		else if (IsKeyPressed(DIK_G) && !WasKeyPressed(DIK_G))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::G, KeyState::KEY_CLICKED }, 'g' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::G, KeyState::KEY_DOWN }, 'g' - shift));
		}
		else if (!IsKeyPressed(DIK_G) && WasKeyPressed(DIK_G))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::G, KeyState::KEY_RELEASED }));
		}

		//fix rest later this is boring to write
		// h, i, j, k, l m, m, o, p

		//Q
		if (IsKeyPressed(DIK_Q) && WasKeyPressed(DIK_Q))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::Q, KeyState::KEY_DOWN }, 'q' - shift));
		}
		else if (IsKeyPressed(DIK_Q) && !WasKeyPressed(DIK_Q))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::Q, KeyState::KEY_CLICKED }, 'q' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::Q, KeyState::KEY_DOWN }, 'q' - shift));
		}
		else if (!IsKeyPressed(DIK_Q) && WasKeyPressed(DIK_Q))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::Q, KeyState::KEY_RELEASED }));
		}

		//R
		if (IsKeyPressed(DIK_R) && WasKeyPressed(DIK_R))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::R, KeyState::KEY_DOWN }, 'r' - shift));
		}
		else if (IsKeyPressed(DIK_R) && !WasKeyPressed(DIK_R))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::R, KeyState::KEY_CLICKED }, 'r' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::R, KeyState::KEY_DOWN }, 'r' - shift));
		}
		else if (!IsKeyPressed(DIK_R) && WasKeyPressed(DIK_R))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::R, KeyState::KEY_RELEASED }));
		}

		//S
		if (IsKeyPressed(DIK_S) && WasKeyPressed(DIK_S))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::S, KeyState::KEY_DOWN }, 's' - shift));
		}
		else if (IsKeyPressed(DIK_S) && !WasKeyPressed(DIK_S))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::S, KeyState::KEY_CLICKED }, 's' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::S, KeyState::KEY_DOWN }, 's' - shift));
		}
		else if (!IsKeyPressed(DIK_S) && WasKeyPressed(DIK_S))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::S, KeyState::KEY_RELEASED }));
		}

		//T
		if (IsKeyPressed(DIK_T) && WasKeyPressed(DIK_T))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::T, KeyState::KEY_DOWN }, 't' - shift));
		}
		else if (IsKeyPressed(DIK_T) && !WasKeyPressed(DIK_T))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::T, KeyState::KEY_CLICKED }, 't' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::T, KeyState::KEY_DOWN }, 't' - shift));
		}
		else if (!IsKeyPressed(DIK_T) && WasKeyPressed(DIK_T))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::T, KeyState::KEY_RELEASED }));
		}

		//U
		if (IsKeyPressed(DIK_U) && WasKeyPressed(DIK_U))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::U, KeyState::KEY_DOWN }, 'u' - shift));
		}
		else if (IsKeyPressed(DIK_U) && !WasKeyPressed(DIK_U))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::U, KeyState::KEY_CLICKED }, 'u' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::U, KeyState::KEY_DOWN }, 'u' - shift));
		}
		else if (!IsKeyPressed(DIK_U) && WasKeyPressed(DIK_U))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::U, KeyState::KEY_RELEASED }));
		}

		//V
		if (IsKeyPressed(DIK_V) && WasKeyPressed(DIK_V))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::V, KeyState::KEY_DOWN }, 'v' - shift));
		}
		else if (IsKeyPressed(DIK_V) && !WasKeyPressed(DIK_V))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::V, KeyState::KEY_CLICKED }, 'v' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::V, KeyState::KEY_DOWN }, 'v' - shift));
		}
		else if (!IsKeyPressed(DIK_V) && WasKeyPressed(DIK_V))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::V, KeyState::KEY_RELEASED }));
		}

		//W
		if (IsKeyPressed(DIK_W) && WasKeyPressed(DIK_W))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::W, KeyState::KEY_DOWN }, 'w' - shift));
		}
		else if (IsKeyPressed(DIK_W) && !WasKeyPressed(DIK_W))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::W, KeyState::KEY_CLICKED }, 'w' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::W, KeyState::KEY_DOWN }, 'w' - shift));
		}
		else if (!IsKeyPressed(DIK_W) && WasKeyPressed(DIK_W))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::W, KeyState::KEY_RELEASED }));
		}

		//X
		if (IsKeyPressed(DIK_X) && WasKeyPressed(DIK_X))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::X, KeyState::KEY_DOWN }, 'x' - shift));
		}
		else if (IsKeyPressed(DIK_X) && !WasKeyPressed(DIK_X))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::X, KeyState::KEY_CLICKED }, 'x' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::X, KeyState::KEY_DOWN }, 'x' - shift));
		}
		else if (!IsKeyPressed(DIK_X) && WasKeyPressed(DIK_X))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::X, KeyState::KEY_RELEASED }));
		}

		//Y
		if (IsKeyPressed(DIK_Y) && WasKeyPressed(DIK_Y))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::Y, KeyState::KEY_DOWN }, 'y' - shift));
		}
		else if (IsKeyPressed(DIK_Y) && !WasKeyPressed(DIK_Y))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::Y, KeyState::KEY_CLICKED }, 'y' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::Y, KeyState::KEY_DOWN }, 'y' - shift));
		}
		else if (!IsKeyPressed(DIK_Y) && WasKeyPressed(DIK_Y))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::Y, KeyState::KEY_RELEASED }));
		}

		//Z
		if (IsKeyPressed(DIK_Z) && WasKeyPressed(DIK_Z))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::Y, KeyState::KEY_DOWN }, 'z' - shift));
		}
		else if (IsKeyPressed(DIK_Z) && !WasKeyPressed(DIK_Z))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::Y, KeyState::KEY_CLICKED }, 'z' - shift));
			EventSystem::postInstantly(KeyboardEvent({ Key::Y, KeyState::KEY_DOWN }, 'z' - shift));
		}
		else if (!IsKeyPressed(DIK_Z) && WasKeyPressed(DIK_Z))
		{
			EventSystem::postInstantly(KeyboardEvent({ Key::Z, KeyState::KEY_RELEASED }));
		}

	}
}