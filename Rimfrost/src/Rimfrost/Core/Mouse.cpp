#include "rfpch.hpp"
#include "Mouse.hpp"
#include <assert.h>

#include "Logger.hpp"
#include <EventSystem.hpp>
#include "MouseEvent.hpp"

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")


namespace Engine1
{
	Mouse::Mouse(HWND hWnd, HINSTANCE hInst, std::function<std::pair<uint32_t, uint32_t>()> getSizeCallback, std::function<HWND()> getHWNDCallback)
	{
#ifdef DirectInput
		m_mouseLastState = { 0 };
		HRESULT hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
		assert(SUCCEEDED(hr));

		hr = m_directInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, NULL);
		assert(SUCCEEDED(hr));

		hr = m_pMouseDevice->SetDataFormat(&c_dfDIMouse);
		assert(SUCCEEDED(hr));

		hr = m_pMouseDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
		assert(SUCCEEDED(hr));
#else

#endif
		m_getWindowSize = getSizeCallback;
		m_getHWND = getHWNDCallback;
	}

	Mouse::~Mouse()
	{
		Logger::getLogger().debugLog("~Mouse\n");
#ifdef DirectInput
		this->m_pMouseDevice->Unacquire();
		this->m_pMouseDevice->Release();
		this->m_directInput->Release();
#endif
	}

	void Mouse::update()
	{
		//auto [width, height] = m_getWindowSize();
		//
		//m_mouseState.y = std::clamp(m_mouseState.y, 0, (int)height);
		//m_mouseState.y = std::clamp(m_mouseState.y, 0, (int)height); fix X




		if (m_mouseState.LMBClicked || m_mouseState.LMBHeld || m_mouseState.LMBReleased || m_mouseState.RMBClicked || m_mouseState.RMBHeld || m_mouseState.RMBReleased)
		{
			//Logger::getLogger().debugLog("mouseXY = " + std::to_string(m_mouseState.x) + ", " + std::to_string(m_mouseState.y) + "\n");
			//EventSystem::post(MouseButtonsEvent(m_mouseState.x, m_mouseState.y, m_mouseState.LMBClicked, m_mouseState.LMBReleased, m_mouseState.LMBHeld, m_mouseState.RMBClicked, m_mouseState.RMBReleased, m_mouseState.RMBHeld));
			auto [w, h] = m_getWindowSize();
			m_mouseState.width = w;
			m_mouseState.height = h;
			EventSystem::post(MouseButtonsEvent(m_mouseState));
		}

		//if (!m_showCursor && !m_windowOutOfFocus)
		//{
		//	auto [x, y] = m_getWindowSize();
		//	m_mouseState.x = x/2;
		//	m_mouseState.y = y/2;

		//	SetCursorPos(x/2, y/2);
		//}

		m_mouseState.deltaX = 0;
		m_mouseState.deltaY = 0;

		m_mouseState.LMBReleased = false;
		m_mouseState.LMBClicked = false;

		m_mouseState.RMBReleased = false;
		m_mouseState.RMBClicked = false;
	}

	MouseState Mouse::GetMouseState() const
	{
		return this->m_mouseState;
	}
	void Mouse::showCursor(bool yn)
	{
		m_showCursor = yn;
		ShowCursor(yn);
	}
	void Mouse::confineCursor(bool yn)
	{
		m_cursorIsConfined = yn;
		if (m_cursorIsConfined && !m_windowOutOfFocus)
		{
			RECT r;
			GetClientRect(m_getHWND(), &r);
			MapWindowPoints(m_getHWND(), nullptr, (POINT*)&r, 2);
			ClipCursor(&r);
		}
		else
		{
			ClipCursor(nullptr);
		}
	}
#ifdef DirectInput
	void Mouse::TakeMouseInput(float mouseCOF)
	{

		DIMOUSESTATE mouseCurrentState;
		m_pMouseDevice->Acquire();

		m_mouseState.deltaX = 0;
		m_mouseState.deltaY = 0;

		m_pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrentState);
		if (mouseCurrentState.lX < -40000) return;
		if (mouseCurrentState.lX != m_mouseLastState.lX)
		{
			m_mouseState.x += mouseCurrentState.lX /** mouseCOF*/;
			m_mouseState.deltaX = (float)mouseCurrentState.lX * mouseCOF;
		}
		if (mouseCurrentState.lY != m_mouseLastState.lY)
		{
			m_mouseState.y -= mouseCurrentState.lY /** mouseCOF*/;
			m_mouseState.deltaY = (float)mouseCurrentState.lY * mouseCOF;
		}
		/*if (mouseCurrentState.lZ != mouseLastState.lZ)
		{
			gMouseState.z = mouseCurrentState.lZ;
		}*/

		auto [width, height] = m_getWindowSize();
		m_mouseState.x = std::clamp(m_mouseState.x, 0, (int)width);
		m_mouseState.y = std::clamp(m_mouseState.y, 0, (int)height);

		m_mouseState.LMBClicked = mouseCurrentState.rgbButtons[0] && !m_mouseLastState.rgbButtons[0];
		m_mouseState.LMBHeld = mouseCurrentState.rgbButtons[0];
		m_mouseState.LMBReleased = !mouseCurrentState.rgbButtons[0] && m_mouseLastState.rgbButtons[0];

		m_mouseState.RMBClicked = mouseCurrentState.rgbButtons[1] && !m_mouseLastState.rgbButtons[1];
		m_mouseState.RMBHeld = mouseCurrentState.rgbButtons[1];
		m_mouseState.RMBReleased = !mouseCurrentState.rgbButtons[1] && m_mouseLastState.rgbButtons[1];

		m_mouseLastState = mouseCurrentState;

		if (m_mouseState.LMBClicked || m_mouseState.LMBHeld || m_mouseState.LMBReleased || m_mouseState.RMBClicked || m_mouseState.RMBHeld || m_mouseState.RMBReleased)
		{
			Logger::getLogger().debugLog("mouseXY = " + std::to_string(m_mouseState.x) + ", " + std::to_string(m_mouseState.y) + "\n");
			EventSystem::post(MouseButtonsEvent(m_mouseState.x, m_mouseState.y, m_mouseState.LMBClicked, m_mouseState.LMBReleased, m_mouseState.LMBHeld, m_mouseState.RMBClicked, m_mouseState.RMBReleased, m_mouseState.RMBHeld));
		}


	}

	void Mouse::Unacquire()
	{

		this->m_pMouseDevice->Unacquire();

	}

	void Mouse::Acquire()
	{
		m_pMouseDevice->Acquire();

	}
#endif
}