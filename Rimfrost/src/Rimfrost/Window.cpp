

#include "rfpch.hpp"
#include "Window.hpp"
#include "Logger.hpp"
//#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"



#ifndef UNICODE
#define UNICODE
#endif 

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace Engine1
{
	Window::SingletonWindow Window::SingletonWindow::m_singlWindo;

	//SingletonWindow
	Window::SingletonWindow::SingletonWindow() noexcept : m_hInst(GetModuleHandle(nullptr))
	{
		WNDCLASSEX wc = { };
		wc.cbSize = sizeof(wc);
		wc.lpfnWndProc = HandleMsgSetup;
		wc.hInstance = GetInstance();
		wc.lpszClassName = GetName();

		RegisterClassEx(&wc);
	}

	Window::SingletonWindow::~SingletonWindow()
	{
		UnregisterClass(this->m_wndClassName, this->m_hInst);
		OutputDebugString(L"~SingletonWindow()\n");
	}

	const wchar_t* Window::SingletonWindow::GetName() noexcept
	{
		return m_wndClassName;
	}

	HINSTANCE Window::SingletonWindow::GetInstance() noexcept
	{
		return m_singlWindo.m_hInst;
	}



	//Window
	Window::Window(const wchar_t* windowName, int width, int height)
	{
		this->m_isClosed = false;
		this->m_height = height;
		this->m_width = width;
		RECT wr;
		wr.left = 100;
		wr.right = width + wr.left;
		wr.top = 100;
		wr.bottom = height + wr.top;
		AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

		this->m_hWnd = CreateWindow(SingletonWindow::GetName(), windowName, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr,
			SingletonWindow::GetInstance(), this);

		assert(this->m_hWnd);

		//show window
		ShowWindow(this->m_hWnd, SW_SHOWDEFAULT);

#ifndef DirectInput
		RAWINPUTDEVICE rid;
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = nullptr;
		if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) assert(false); //failed to register device
#endif

		this->m_graphics = new Engine1::Graphics(this->m_hWnd, this->m_width, this->m_height);
		this->m_mouse = std::make_shared<Mouse>(this->m_hWnd, SingletonWindow::GetInstance(),
			[this]()->std::pair<uint32_t, uint32_t> { return this->getClientSize(); },
			[this]()->HWND { return this->getHwnd(); });
		m_mouse->confineCursor(true);
		m_mouse->showCursor(false);
		this->m_pKeyboard = std::make_shared<Keyboard>(this->m_hWnd, SingletonWindow::GetInstance());

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(this->getHwnd());
		ImGui_ImplDX11_Init(this->m_graphics->GetDevice().Get(), this->m_graphics->GetDeviceContext().Get());

	}
	Window::~Window()
	{
		//DestroyWindow(this->m_hWnd);
		//OutputDebugString(L"~Window()\n");
		Logger::getLogger().debugLog("~Window\n");
		//delete m_graphics;
		//m_graphics = nullptr;
		//assert(m_graphics == nullptr);
	}

	void Window::changeWindowText(const char* text)
	{
		std::wstringstream win32text;
		win32text << text;
		std::wstring ws = win32text.str();
		SetWindowText(this->getHwnd(), ws.c_str());
	}

	std::pair<uint32_t, uint32_t> Window::getClientSize() const
	{
		return std::pair<uint32_t, uint32_t>(m_width, m_height);
	}

	void Window::print(const wchar_t* out)
	{
		OutputDebugString(out);
	}

	void Window::print(const char* out)
	{
		std::wstringstream win32text;
		win32text << out;
		std::wstring ws = win32text.str();
		OutputDebugString(ws.c_str());
	}

	void Window::print(const std::string& out)
	{
		print(out.c_str());
	}

	void Window::destroyDirectAPI()
	{
		this->m_pKeyboard.reset();
		this->m_mouse.reset();
		delete this->m_graphics;
		this->m_graphics = nullptr;
	}

	bool Window::isClosed() const
	{
		return this->m_isClosed;
	}

	Engine1::Graphics& Window::getGraphics()
	{
		return *m_graphics;
	}

	const std::shared_ptr<Mouse>& Window::getMouse() const noexcept
	{
		return m_mouse;
	}

	const std::shared_ptr<Keyboard>& Window::getKeyboard() const noexcept
	{
		return m_pKeyboard;
	}

	HWND Window::getHwnd() const
	{
		return this->m_hWnd;
	}


	//extern LRESULT ImGui_ImplWin32


	LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
		if (msg == WM_NCCREATE)
		{
			// extract ptr to window class from creation data
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
			// set WinAPI-managed user data to store ptr to window instance
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			// set message proc to normal (non-setup) handler now that setup is finished
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
			// forward message to window instance handler
			return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
		}
		// if we get a message before the WM_NCCREATE message, handle with default handler
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// retrieve ptr to window instance
		Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		// forward message to window instance handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	
	LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		{
			return true;
		}



		switch (msg)
		{
			wchar_t msg[200];


			//case WM_KEYDOWN:
			//{
			//	swprintf_s(msg, L"WM_KEYDOWN: 0x%x\n", (wchar_t)wParam);
			//	OutputDebugString(msg);
			//	switch (wParam)
			//	{
			//	/*case 0x1b:
			//	{
			//		PostQuitMessage(0);
			//		return 0;
			//	}*/
			//	}
			//	break;
			//}
			//case WM_KEYUP:
			//{
			//	swprintf_s(msg, L"WM_KEYUP: 0x%x\n", (wchar_t)wParam);
			//	OutputDebugString(msg);
			//	break;
			//}


		case WM_MOUSEMOVE:
		{
			POINTS p = MAKEPOINTS(lParam);

			m_mouse->m_mouseState.x = p.x;
			m_mouse->m_mouseState.y = p.y;



			break;
		}
		case WM_MOUSEWHEEL:
		{
			m_mouse->m_mouseState.z += GET_WHEEL_DELTA_WPARAM(wParam);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			m_mouse->m_mouseState.LMBClicked = true;
			m_mouse->m_mouseState.LMBHeld = true;
			//print("MousePos:  \n");
			break;
		}

		case WM_LBUTTONUP:
		{
			m_mouse->m_mouseState.LMBReleased = true;
			m_mouse->m_mouseState.LMBHeld = false;
			break;
		}
		case WM_RBUTTONDOWN:
		{
			m_mouse->m_mouseState.RMBClicked = true;
			m_mouse->m_mouseState.RMBHeld = true;
			//print("MousePos:  \n");
			break;
		}

		case WM_RBUTTONUP:
		{
			m_mouse->m_mouseState.RMBReleased = true;
			m_mouse->m_mouseState.RMBHeld = false;
			break;
		}


		case WM_INPUT:
		{
			UINT bufferSize{};
			UINT errorCode = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &bufferSize, sizeof(RAWINPUTHEADER));
			assert(errorCode != -1);
			if (errorCode == -1) break;

			m_ridData.resize(bufferSize);
			errorCode = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, m_ridData.data(), &bufferSize, sizeof(RAWINPUTHEADER));
			assert(errorCode != -1);
			if (errorCode == -1) break;



			RAWINPUT& rawMouseInput = (RAWINPUT&)(*m_ridData.data());
			if (rawMouseInput.header.dwType == RIM_TYPEMOUSE)
			{
				if (rawMouseInput.data.mouse.lLastX || rawMouseInput.data.mouse.lLastY)
				{
					m_mouse->m_mouseState.deltaX = static_cast<float>(rawMouseInput.data.mouse.lLastX);
					m_mouse->m_mouseState.deltaY = static_cast<float>(rawMouseInput.data.mouse.lLastY);
				}
			}


			break;
		}



		case WM_CLOSE:
		{
			m_isClosed = true;
			swprintf_s(msg, L"WM_CLOSE: 0x%x\n", (wchar_t)wParam);
			OutputDebugString(msg);

			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			destroyDirectAPI();
			DestroyWindow(m_hWnd);
			break;
		}
		case WM_DESTROY:
		{
			swprintf_s(msg, L"WM_DESTROY: 0x%x\n", (wchar_t)wParam);
			OutputDebugString(msg);
			PostQuitMessage(0);
			return 0;
		}
		case WM_ACTIVATEAPP:
		{
			if (this->m_graphics == nullptr) break;
			swprintf_s(msg, L"WM_ACTIVATEAPP: %d\n", (int)wParam);
			OutputDebugString(msg);
			if (wParam)
			{
				//this->GetGraphics().SetFullScreen(this->GetGraphics().ShouldBeFullScreen());


				m_mouse->m_windowOutOfFocus = false; //window is a friend of mouse to fix alt tab
				m_mouse->confineCursor(m_mouse->m_cursorIsConfined);
			}
			else
			{
				m_mouse->m_windowOutOfFocus = true;
			}
			break;

		}
		case WM_WINDOWPOSCHANGED:
		{
			if (this->m_graphics != nullptr)
			{
				RECT winRect;
				GetClientRect(this->getHwnd(), &winRect);
				int rx = winRect.right - winRect.left;
				int ry = winRect.bottom - winRect.top;

				swprintf_s(msg, L"WM_WINDOWPOSCHANGED, res: %d x %d\n", rx, ry);
				OutputDebugString(msg);

				this->getGraphics().onResize(rx, ry);

				GetClientRect(this->getHwnd(), &winRect);
				m_width = winRect.right - winRect.left;
				m_height = winRect.bottom - winRect.top;

				m_mouse->confineCursor(m_mouse->m_cursorIsConfined); //need to resize

#ifdef DEBUG
				auto [gResX, gResY] = this->getGraphics().getBackBufferResoltion();
				auto [wResX, wResY] = this->getClientSize();
				if (gResX != wResX || gResY != wResY)
				{
					print("WARNING BACKBUFFER SIZE DOES NOT MATCH THE CLIENTSIZE\n");
					print("backbuffer: " + std::to_string(gResX) + ", " + std::to_string(gResY) + "\n");
					print("clientsize: " + std::to_string(wResX) + ", " + std::to_string(wResY) + "\n");
				}
#endif // DEBUG


			}
			break;
		}
		default:
			break;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}