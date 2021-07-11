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
			EventSystem::postTOQueue(KeyboardEvent({ Key::ESC, KeyState::KEY_DOWN }));
		}
		else if(IsKeyPressed(DIK_ESCAPE) && !WasKeyPressed(DIK_ESCAPE))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::ESC, KeyState::KEY_CLICKED }));
			EventSystem::postTOQueue(KeyboardEvent({ Key::ESC, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_ESCAPE) && WasKeyPressed(DIK_ESCAPE))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::ESC, KeyState::KEY_RELEASED }));
		}

		//SPACE
		if (IsKeyPressed(DIK_SPACE) && WasKeyPressed(DIK_SPACE))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::SPACE, KeyState::KEY_DOWN }, ' '));
		}
		else if (IsKeyPressed(DIK_SPACE) && !WasKeyPressed(DIK_SPACE))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::SPACE, KeyState::KEY_CLICKED }, ' '));
			EventSystem::postTOQueue(KeyboardEvent({ Key::SPACE, KeyState::KEY_DOWN }, ' '));
		}
		else if (!IsKeyPressed(DIK_SPACE) && WasKeyPressed(DIK_SPACE))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::SPACE, KeyState::KEY_RELEASED }));
		}

		//TAB
		if (IsKeyPressed(DIK_TAB) && WasKeyPressed(DIK_TAB))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::TAB, KeyState::KEY_DOWN }, '\t'));
		}
		else if (IsKeyPressed(DIK_TAB) && !WasKeyPressed(DIK_TAB))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::TAB, KeyState::KEY_CLICKED }, '\t'));
			EventSystem::postTOQueue(KeyboardEvent({ Key::TAB, KeyState::KEY_DOWN }, '\t'));
		}
		else if (!IsKeyPressed(DIK_TAB) && WasKeyPressed(DIK_TAB))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::TAB, KeyState::KEY_RELEASED }));
		}

		//LCTRL
		if (IsKeyPressed(DIK_LCONTROL) && WasKeyPressed(DIK_LCONTROL))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::LCTRL, KeyState::KEY_DOWN }));
		}
		else if (IsKeyPressed(DIK_LCONTROL) && !WasKeyPressed(DIK_LCONTROL))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::LCTRL, KeyState::KEY_CLICKED }));
			EventSystem::postTOQueue(KeyboardEvent({ Key::LCTRL, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_LCONTROL) && WasKeyPressed(DIK_LCONTROL))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::LCTRL, KeyState::KEY_RELEASED }));
		}

		//LSHIFT
		char shift = 0;
		if (IsKeyPressed(DIK_LSHIFT) && WasKeyPressed(DIK_LSHIFT))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::LSHIFT, KeyState::KEY_DOWN }));
			shift = 32;
		}
		else if (IsKeyPressed(DIK_LSHIFT) && !WasKeyPressed(DIK_LSHIFT))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::LSHIFT, KeyState::KEY_CLICKED }));
			EventSystem::postTOQueue(KeyboardEvent({ Key::LSHIFT, KeyState::KEY_DOWN }));
			shift = 32;
		}
		else if (!IsKeyPressed(DIK_LSHIFT) && WasKeyPressed(DIK_LSHIFT))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::LSHIFT, KeyState::KEY_RELEASED }));
		}

		//ENTER
		if (IsKeyPressed(DIK_RETURN) && WasKeyPressed(DIK_RETURN))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::ENTER, KeyState::KEY_DOWN }, '\n'));
		}
		else if (IsKeyPressed(DIK_RETURN) && !WasKeyPressed(DIK_RETURN))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::ENTER, KeyState::KEY_CLICKED }, '\n'));
			EventSystem::postTOQueue(KeyboardEvent({ Key::ENTER, KeyState::KEY_DOWN }, '\n'));
		}
		else if (!IsKeyPressed(DIK_RETURN) && WasKeyPressed(DIK_RETURN))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::ENTER, KeyState::KEY_RELEASED }));
		}

		//BACK_SPACE
		if (IsKeyPressed(DIK_BACK) && WasKeyPressed(DIK_BACK))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::BACK_SPACE, KeyState::KEY_DOWN }));
		}
		else if (IsKeyPressed(DIK_BACK) && !WasKeyPressed(DIK_BACK))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::BACK_SPACE, KeyState::KEY_CLICKED }));
			EventSystem::postTOQueue(KeyboardEvent({ Key::BACK_SPACE, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_BACK) && WasKeyPressed(DIK_BACK))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::BACK_SPACE, KeyState::KEY_RELEASED }));
		}

		//ARROW_DOWN
		if (IsKeyPressed(DIK_DOWNARROW) && WasKeyPressed(DIK_DOWNARROW))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::DOWN, KeyState::KEY_DOWN }));
		}
		else if (IsKeyPressed(DIK_DOWNARROW) && !WasKeyPressed(DIK_DOWNARROW))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::DOWN, KeyState::KEY_CLICKED }));
			EventSystem::postTOQueue(KeyboardEvent({ Key::DOWN, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_DOWNARROW) && WasKeyPressed(DIK_DOWNARROW))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::DOWN, KeyState::KEY_RELEASED }));
		}

		//ARROW_UP
		if (IsKeyPressed(DIK_UPARROW) && WasKeyPressed(DIK_UPARROW))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::UP, KeyState::KEY_DOWN }));
		}
		else if (IsKeyPressed(DIK_UPARROW) && !WasKeyPressed(DIK_UPARROW))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::UP, KeyState::KEY_CLICKED }));
			EventSystem::postTOQueue(KeyboardEvent({ Key::UP, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_UPARROW) && WasKeyPressed(DIK_UPARROW))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::UP, KeyState::KEY_RELEASED }));
		}

		//ARROW_RIGHT
		if (IsKeyPressed(DIK_RIGHTARROW) && WasKeyPressed(DIK_RIGHTARROW))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::RIGHT, KeyState::KEY_DOWN }));
		}
		else if (IsKeyPressed(DIK_RIGHTARROW) && !WasKeyPressed(DIK_RIGHTARROW))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::RIGHT, KeyState::KEY_CLICKED }));
			EventSystem::postTOQueue(KeyboardEvent({ Key::RIGHT, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_RIGHTARROW) && WasKeyPressed(DIK_RIGHTARROW))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::RIGHT, KeyState::KEY_RELEASED }));
		}
		
		//ARROW_LEFT
		if (IsKeyPressed(DIK_LEFTARROW) && WasKeyPressed(DIK_LEFTARROW))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::LEFT, KeyState::KEY_DOWN }));
		}
		else if (IsKeyPressed(DIK_LEFTARROW) && !WasKeyPressed(DIK_LEFTARROW))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::LEFT, KeyState::KEY_CLICKED }));
			EventSystem::postTOQueue(KeyboardEvent({ Key::LEFT, KeyState::KEY_DOWN }));
		}
		else if (!IsKeyPressed(DIK_LEFTARROW) && WasKeyPressed(DIK_LEFTARROW))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::LEFT, KeyState::KEY_RELEASED }));
		}

		//numbers
		//1
		if (IsKeyPressed(DIK_1) && WasKeyPressed(DIK_1))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_1, KeyState::KEY_DOWN }, '1'));
		}
		else if (IsKeyPressed(DIK_1) && !WasKeyPressed(DIK_1))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_1, KeyState::KEY_CLICKED }, '1'));
			EventSystem::postTOQueue(KeyboardEvent({ Key::_1, KeyState::KEY_DOWN }, '1'));
		}
		else if (!IsKeyPressed(DIK_1) && WasKeyPressed(DIK_1))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_1, KeyState::KEY_RELEASED }));
		}

		//2
		if (IsKeyPressed(DIK_2) && WasKeyPressed(DIK_2))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_2, KeyState::KEY_DOWN }, '2'));
		}
		else if (IsKeyPressed(DIK_2) && !WasKeyPressed(DIK_2))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_2, KeyState::KEY_CLICKED }, '2'));
			EventSystem::postTOQueue(KeyboardEvent({ Key::_2, KeyState::KEY_DOWN }, '2'));
		}
		else if (!IsKeyPressed(DIK_2) && WasKeyPressed(DIK_2))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_2, KeyState::KEY_RELEASED }));
		}

		//3
		if (IsKeyPressed(DIK_3) && WasKeyPressed(DIK_3))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_3, KeyState::KEY_DOWN }, '3'));
		}
		else if (IsKeyPressed(DIK_3) && !WasKeyPressed(DIK_3))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_3, KeyState::KEY_CLICKED }, '3'));
			EventSystem::postTOQueue(KeyboardEvent({ Key::_3, KeyState::KEY_DOWN }, '3'));
		}
		else if (!IsKeyPressed(DIK_3) && WasKeyPressed(DIK_3))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_3, KeyState::KEY_RELEASED }));
		}

		//4
		if (IsKeyPressed(DIK_4) && WasKeyPressed(DIK_4))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_4, KeyState::KEY_DOWN }, '4'));
		}
		else if (IsKeyPressed(DIK_4) && !WasKeyPressed(DIK_4))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_4, KeyState::KEY_CLICKED }, '4'));
			EventSystem::postTOQueue(KeyboardEvent({ Key::_4, KeyState::KEY_DOWN }, '4'));
		}
		else if (!IsKeyPressed(DIK_4) && WasKeyPressed(DIK_4))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_4, KeyState::KEY_RELEASED }));
		}

		//5
		if (IsKeyPressed(DIK_5) && WasKeyPressed(DIK_5))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_5, KeyState::KEY_DOWN }, '5'));
		}
		else if (IsKeyPressed(DIK_5) && !WasKeyPressed(DIK_5))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_5, KeyState::KEY_CLICKED }, '5'));
			EventSystem::postTOQueue(KeyboardEvent({ Key::_5, KeyState::KEY_DOWN }, '5'));
		}
		else if (!IsKeyPressed(DIK_5) && WasKeyPressed(DIK_5))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_5, KeyState::KEY_RELEASED }));
		}

		//6
		if (IsKeyPressed(DIK_6) && WasKeyPressed(DIK_6))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_6, KeyState::KEY_DOWN }, '6'));
		}
		else if (IsKeyPressed(DIK_6) && !WasKeyPressed(DIK_6))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_6, KeyState::KEY_CLICKED }, '6'));
			EventSystem::postTOQueue(KeyboardEvent({ Key::_6, KeyState::KEY_DOWN }, '6'));
		}
		else if (!IsKeyPressed(DIK_6) && WasKeyPressed(DIK_6))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_6, KeyState::KEY_RELEASED }));
		}

		//7
		if (IsKeyPressed(DIK_7) && WasKeyPressed(DIK_7))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_7, KeyState::KEY_DOWN }, '7'));
		}
		else if (IsKeyPressed(DIK_7) && !WasKeyPressed(DIK_7))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_7, KeyState::KEY_CLICKED }, '7'));
			EventSystem::postTOQueue(KeyboardEvent({ Key::_7, KeyState::KEY_DOWN }, '7'));
		}
		else if (!IsKeyPressed(DIK_7) && WasKeyPressed(DIK_7))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_7, KeyState::KEY_RELEASED }));
		}

		//8
		if (IsKeyPressed(DIK_8) && WasKeyPressed(DIK_8))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_8, KeyState::KEY_DOWN }, '8'));
		}
		else if (IsKeyPressed(DIK_8) && !WasKeyPressed(DIK_8))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_8, KeyState::KEY_CLICKED }, '8'));
			EventSystem::postTOQueue(KeyboardEvent({ Key::_8, KeyState::KEY_DOWN }, '8'));
		}
		else if (!IsKeyPressed(DIK_8) && WasKeyPressed(DIK_8))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_8, KeyState::KEY_RELEASED }));
		}

		//9
		if (IsKeyPressed(DIK_9) && WasKeyPressed(DIK_9))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_9, KeyState::KEY_DOWN }, '9'));
		}
		else if (IsKeyPressed(DIK_9) && !WasKeyPressed(DIK_9))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_9, KeyState::KEY_CLICKED }, '9'));
			EventSystem::postTOQueue(KeyboardEvent({ Key::_9, KeyState::KEY_DOWN }, '9'));
		}
		else if (!IsKeyPressed(DIK_9) && WasKeyPressed(DIK_9))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_9, KeyState::KEY_RELEASED }));
		}

		//0
		if (IsKeyPressed(DIK_0) && WasKeyPressed(DIK_0))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_0, KeyState::KEY_DOWN }, '0'));
		}
		else if (IsKeyPressed(DIK_0) && !WasKeyPressed(DIK_0))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_0, KeyState::KEY_CLICKED }, '0'));
			EventSystem::postTOQueue(KeyboardEvent({ Key::_0, KeyState::KEY_DOWN }, '0'));
		}
		else if (!IsKeyPressed(DIK_0) && WasKeyPressed(DIK_0))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::_0, KeyState::KEY_RELEASED }));
		}

		// characters----------------------------
		
		//A
		if (IsKeyPressed(DIK_A) && WasKeyPressed(DIK_A))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::A, KeyState::KEY_DOWN }, 'a' - shift));
		}
		else if (IsKeyPressed(DIK_A) && !WasKeyPressed(DIK_A))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::A, KeyState::KEY_CLICKED }, 'a' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::A, KeyState::KEY_DOWN }, 'a' - shift));
		}
		else if (!IsKeyPressed(DIK_A) && WasKeyPressed(DIK_A))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::A, KeyState::KEY_RELEASED }));
		}

		//B
		if (IsKeyPressed(DIK_B) && WasKeyPressed(DIK_B))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::B, KeyState::KEY_DOWN }, 'b' - shift));
		}
		else if (IsKeyPressed(DIK_B) && !WasKeyPressed(DIK_B))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::B, KeyState::KEY_CLICKED }, 'b' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::B, KeyState::KEY_DOWN }, 'b' - shift));
		}
		else if (!IsKeyPressed(DIK_B) && WasKeyPressed(DIK_B))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::B, KeyState::KEY_RELEASED }));
		}

		//C
		if (IsKeyPressed(DIK_C) && WasKeyPressed(DIK_C))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::C, KeyState::KEY_DOWN }, 'C' - shift));
		}
		else if (IsKeyPressed(DIK_C) && !WasKeyPressed(DIK_C))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::C, KeyState::KEY_CLICKED }, 'c' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::C, KeyState::KEY_DOWN }, 'c' - shift));
		}
		else if (!IsKeyPressed(DIK_C) && WasKeyPressed(DIK_C))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::C, KeyState::KEY_RELEASED }));
		}

		//D
		if (IsKeyPressed(DIK_D) && WasKeyPressed(DIK_D))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::D, KeyState::KEY_DOWN }, 'd' - shift));
		}
		else if (IsKeyPressed(DIK_D) && !WasKeyPressed(DIK_D))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::D, KeyState::KEY_CLICKED }, 'd' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::D, KeyState::KEY_DOWN }, 'd' - shift));
		}
		else if (!IsKeyPressed(DIK_D) && WasKeyPressed(DIK_D))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::D, KeyState::KEY_RELEASED }));
		}

		//E
		if (IsKeyPressed(DIK_E) && WasKeyPressed(DIK_E))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::E, KeyState::KEY_DOWN }, 'e' - shift));
		}
		else if (IsKeyPressed(DIK_E) && !WasKeyPressed(DIK_E))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::E, KeyState::KEY_CLICKED }, 'e' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::E, KeyState::KEY_DOWN }, 'e' - shift));
		}
		else if (!IsKeyPressed(DIK_E) && WasKeyPressed(DIK_E))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::E, KeyState::KEY_RELEASED }));
		}

		//F
		if (IsKeyPressed(DIK_F) && WasKeyPressed(DIK_F))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::F, KeyState::KEY_DOWN }, 'f' - shift));
		}
		else if (IsKeyPressed(DIK_F) && !WasKeyPressed(DIK_F))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::F, KeyState::KEY_CLICKED }, 'f' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::F, KeyState::KEY_DOWN }, 'f' - shift));
		}
		else if (!IsKeyPressed(DIK_F) && WasKeyPressed(DIK_F))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::F, KeyState::KEY_RELEASED }));
		}

		//G
		if (IsKeyPressed(DIK_G) && WasKeyPressed(DIK_G))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::G, KeyState::KEY_DOWN }, 'g' - shift));
		}
		else if (IsKeyPressed(DIK_G) && !WasKeyPressed(DIK_G))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::G, KeyState::KEY_CLICKED }, 'g' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::G, KeyState::KEY_DOWN }, 'g' - shift));
		}
		else if (!IsKeyPressed(DIK_G) && WasKeyPressed(DIK_G))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::G, KeyState::KEY_RELEASED }));
		}

		//H
		if (IsKeyPressed(DIK_H) && WasKeyPressed(DIK_H))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::H, KeyState::KEY_DOWN }, 'h' - shift));
		}
		else if (IsKeyPressed(DIK_H) && !WasKeyPressed(DIK_H))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::H, KeyState::KEY_CLICKED }, 'h' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::H, KeyState::KEY_DOWN }, 'h' - shift));
		}
		else if (!IsKeyPressed(DIK_H) && WasKeyPressed(DIK_H))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::H, KeyState::KEY_RELEASED }));
		}

		//I
		if (IsKeyPressed(DIK_I) && WasKeyPressed(DIK_I))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::I, KeyState::KEY_DOWN }, 'i' - shift));
		}
		else if (IsKeyPressed(DIK_I) && !WasKeyPressed(DIK_I))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::I, KeyState::KEY_CLICKED }, 'i' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::I, KeyState::KEY_DOWN }, 'i' - shift));
		}
		else if (!IsKeyPressed(DIK_I) && WasKeyPressed(DIK_I))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::I, KeyState::KEY_RELEASED }));
		}

		//J
		if (IsKeyPressed(DIK_J) && WasKeyPressed(DIK_J))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::J, KeyState::KEY_DOWN }, 'j' - shift));
		}
		else if (IsKeyPressed(DIK_J) && !WasKeyPressed(DIK_J))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::J, KeyState::KEY_CLICKED }, 'j' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::J, KeyState::KEY_DOWN }, 'j' - shift));
		}
		else if (!IsKeyPressed(DIK_J) && WasKeyPressed(DIK_J))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::J, KeyState::KEY_RELEASED }));
		}

		//K
		if (IsKeyPressed(DIK_K) && WasKeyPressed(DIK_K))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::K, KeyState::KEY_DOWN }, 'k' - shift));
		}
		else if (IsKeyPressed(DIK_K) && !WasKeyPressed(DIK_K))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::K, KeyState::KEY_CLICKED }, 'k' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::K, KeyState::KEY_DOWN }, 'k' - shift));
		}
		else if (!IsKeyPressed(DIK_K) && WasKeyPressed(DIK_K))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::K, KeyState::KEY_RELEASED }));
		}

		//L
		if (IsKeyPressed(DIK_L) && WasKeyPressed(DIK_L))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::L, KeyState::KEY_DOWN }, 'l' - shift));
		}
		else if (IsKeyPressed(DIK_L) && !WasKeyPressed(DIK_L))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::L, KeyState::KEY_CLICKED }, 'l' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::L, KeyState::KEY_DOWN }, 'l' - shift));
		}
		else if (!IsKeyPressed(DIK_L) && WasKeyPressed(DIK_L))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::L, KeyState::KEY_RELEASED }));
		}

		//M
		if (IsKeyPressed(DIK_M) && WasKeyPressed(DIK_M))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::M, KeyState::KEY_DOWN }, 'm' - shift));
		}
		else if (IsKeyPressed(DIK_M) && !WasKeyPressed(DIK_M))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::M, KeyState::KEY_CLICKED }, 'm' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::M, KeyState::KEY_DOWN }, 'm' - shift));
		}
		else if (!IsKeyPressed(DIK_M) && WasKeyPressed(DIK_M))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::M, KeyState::KEY_RELEASED }));
		}

		//N
		if (IsKeyPressed(DIK_N) && WasKeyPressed(DIK_N))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::N, KeyState::KEY_DOWN }, 'n' - shift));
		}
		else if (IsKeyPressed(DIK_N) && !WasKeyPressed(DIK_N))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::N, KeyState::KEY_CLICKED }, 'n' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::N, KeyState::KEY_DOWN }, 'n' - shift));
		}
		else if (!IsKeyPressed(DIK_N) && WasKeyPressed(DIK_N))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::N, KeyState::KEY_RELEASED }));
		}

		//O
		if (IsKeyPressed(DIK_O) && WasKeyPressed(DIK_O))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::O, KeyState::KEY_DOWN }, 'o' - shift));
		}
		else if (IsKeyPressed(DIK_O) && !WasKeyPressed(DIK_O))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::O, KeyState::KEY_CLICKED }, 'o' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::O, KeyState::KEY_DOWN }, 'o' - shift));
		}
		else if (!IsKeyPressed(DIK_O) && WasKeyPressed(DIK_O))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::O, KeyState::KEY_RELEASED }));
		}

		//P
		if (IsKeyPressed(DIK_P) && WasKeyPressed(DIK_P))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::P, KeyState::KEY_DOWN }, 'p' - shift));
		}
		else if (IsKeyPressed(DIK_P) && !WasKeyPressed(DIK_P))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::P, KeyState::KEY_CLICKED }, 'p' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::P, KeyState::KEY_DOWN }, 'p' - shift));
		}
		else if (!IsKeyPressed(DIK_P) && WasKeyPressed(DIK_P))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::P, KeyState::KEY_RELEASED }));
		}

		//Q
		if (IsKeyPressed(DIK_Q) && WasKeyPressed(DIK_Q))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::Q, KeyState::KEY_DOWN }, 'q' - shift));
		}
		else if (IsKeyPressed(DIK_Q) && !WasKeyPressed(DIK_Q))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::Q, KeyState::KEY_CLICKED }, 'q' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::Q, KeyState::KEY_DOWN }, 'q' - shift));
		}
		else if (!IsKeyPressed(DIK_Q) && WasKeyPressed(DIK_Q))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::Q, KeyState::KEY_RELEASED }));
		}

		//R
		if (IsKeyPressed(DIK_R) && WasKeyPressed(DIK_R))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::R, KeyState::KEY_DOWN }, 'r' - shift));
		}
		else if (IsKeyPressed(DIK_R) && !WasKeyPressed(DIK_R))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::R, KeyState::KEY_CLICKED }, 'r' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::R, KeyState::KEY_DOWN }, 'r' - shift));
		}
		else if (!IsKeyPressed(DIK_R) && WasKeyPressed(DIK_R))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::R, KeyState::KEY_RELEASED }));
		}

		//S
		if (IsKeyPressed(DIK_S) && WasKeyPressed(DIK_S))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::S, KeyState::KEY_DOWN }, 's' - shift));
		}
		else if (IsKeyPressed(DIK_S) && !WasKeyPressed(DIK_S))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::S, KeyState::KEY_CLICKED }, 's' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::S, KeyState::KEY_DOWN }, 's' - shift));
		}
		else if (!IsKeyPressed(DIK_S) && WasKeyPressed(DIK_S))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::S, KeyState::KEY_RELEASED }));
		}

		//T
		if (IsKeyPressed(DIK_T) && WasKeyPressed(DIK_T))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::T, KeyState::KEY_DOWN }, 't' - shift));
		}
		else if (IsKeyPressed(DIK_T) && !WasKeyPressed(DIK_T))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::T, KeyState::KEY_CLICKED }, 't' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::T, KeyState::KEY_DOWN }, 't' - shift));
		}
		else if (!IsKeyPressed(DIK_T) && WasKeyPressed(DIK_T))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::T, KeyState::KEY_RELEASED }));
		}

		//U
		if (IsKeyPressed(DIK_U) && WasKeyPressed(DIK_U))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::U, KeyState::KEY_DOWN }, 'u' - shift));
		}
		else if (IsKeyPressed(DIK_U) && !WasKeyPressed(DIK_U))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::U, KeyState::KEY_CLICKED }, 'u' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::U, KeyState::KEY_DOWN }, 'u' - shift));
		}
		else if (!IsKeyPressed(DIK_U) && WasKeyPressed(DIK_U))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::U, KeyState::KEY_RELEASED }));
		}

		//V
		if (IsKeyPressed(DIK_V) && WasKeyPressed(DIK_V))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::V, KeyState::KEY_DOWN }, 'v' - shift));
		}
		else if (IsKeyPressed(DIK_V) && !WasKeyPressed(DIK_V))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::V, KeyState::KEY_CLICKED }, 'v' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::V, KeyState::KEY_DOWN }, 'v' - shift));
		}
		else if (!IsKeyPressed(DIK_V) && WasKeyPressed(DIK_V))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::V, KeyState::KEY_RELEASED }));
		}

		//W
		if (IsKeyPressed(DIK_W) && WasKeyPressed(DIK_W))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::W, KeyState::KEY_DOWN }, 'w' - shift));
		}
		else if (IsKeyPressed(DIK_W) && !WasKeyPressed(DIK_W))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::W, KeyState::KEY_CLICKED }, 'w' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::W, KeyState::KEY_DOWN }, 'w' - shift));
		}
		else if (!IsKeyPressed(DIK_W) && WasKeyPressed(DIK_W))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::W, KeyState::KEY_RELEASED }));
		}

		//X
		if (IsKeyPressed(DIK_X) && WasKeyPressed(DIK_X))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::X, KeyState::KEY_DOWN }, 'x' - shift));
		}
		else if (IsKeyPressed(DIK_X) && !WasKeyPressed(DIK_X))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::X, KeyState::KEY_CLICKED }, 'x' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::X, KeyState::KEY_DOWN }, 'x' - shift));
		}
		else if (!IsKeyPressed(DIK_X) && WasKeyPressed(DIK_X))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::X, KeyState::KEY_RELEASED }));
		}

		//Y
		if (IsKeyPressed(DIK_Y) && WasKeyPressed(DIK_Y))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::Y, KeyState::KEY_DOWN }, 'y' - shift));
		}
		else if (IsKeyPressed(DIK_Y) && !WasKeyPressed(DIK_Y))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::Y, KeyState::KEY_CLICKED }, 'y' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::Y, KeyState::KEY_DOWN }, 'y' - shift));
		}
		else if (!IsKeyPressed(DIK_Y) && WasKeyPressed(DIK_Y))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::Y, KeyState::KEY_RELEASED }));
		}

		//Z
		if (IsKeyPressed(DIK_Z) && WasKeyPressed(DIK_Z))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::Y, KeyState::KEY_DOWN }, 'z' - shift));
		}
		else if (IsKeyPressed(DIK_Z) && !WasKeyPressed(DIK_Z))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::Y, KeyState::KEY_CLICKED }, 'z' - shift));
			EventSystem::postTOQueue(KeyboardEvent({ Key::Y, KeyState::KEY_DOWN }, 'z' - shift));
		}
		else if (!IsKeyPressed(DIK_Z) && WasKeyPressed(DIK_Z))
		{
			EventSystem::postTOQueue(KeyboardEvent({ Key::Z, KeyState::KEY_RELEASED }));
		}

	}
}