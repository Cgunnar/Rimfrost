#include "rfpch.hpp"
#include "Keyboard.hpp"
#include <assert.h>

#include "Logger.hpp"

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

	void Keyboard::TakeKeyboardInput()
	{
		memcpy(m_keyBoardPrevState, m_keyBoardState, sizeof(m_keyBoardState));
		m_pKeyboardDevice->Acquire();
		m_pKeyboardDevice->GetDeviceState(sizeof(m_keyBoardState), (LPVOID)&m_keyBoardState);

	}
}