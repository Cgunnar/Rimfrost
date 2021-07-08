#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <functional>

//#define DirectInput
namespace Rimfrost
{

	struct MouseState
	{
		int x = 0;
		int y = 0;
		int z = 0;

		float deltaX = 0;
		float deltaY = 0;
		float deltaZ = 0;

		bool LMBClicked = false;
		bool LMBReleased = false;
		bool LMBHeld = false;

		bool RMBClicked = false;
		bool RMBReleased = false;
		bool RMBHeld = false;

		uint32_t width = 0, height = 0;
	};
	class Window;
	class Mouse
	{
		friend Window;
	public:
		Mouse(HWND hWnd, HINSTANCE hInst, std::function<std::pair<uint32_t, uint32_t>()> getHWNDCallback,
			std::function<HWND()> getRectCallback);
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;
		~Mouse();

		void update();

		MouseState GetMouseState() const;

		void showCursor(bool yn);
		void confineCursor(bool yn);

#ifdef DirectInput
		void TakeMouseInput(float mouseCOF);
		void Unacquire();
		void Acquire();
#endif

	private:

#ifdef DirectInput
		IDirectInputDevice8* m_pMouseDevice = nullptr;
		DIMOUSESTATE m_mouseLastState = {};
		LPDIRECTINPUT8 m_directInput;
#endif

		MouseState m_mouseState{ 0 };

		bool m_showCursor = false;
		bool m_windowOutOfFocus = false;

		bool m_cursorIsConfined = true;


		std::function<std::pair<uint32_t, uint32_t>()> m_getWindowSize;
		std::function<HWND()> m_getHWND;
	};

}