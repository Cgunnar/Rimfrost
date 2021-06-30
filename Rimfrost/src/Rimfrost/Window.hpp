#pragma once
#include <memory>
#include <Windows.h>
#include <string>
#include <vector>
//#include "Graphics.hpp"
//#include "Mouse.hpp"
//#include "Keyboard.hpp"

class Window
{
private:
	class SingletonWindow
	{
	public:
		static const wchar_t* GetName();
		static HINSTANCE GetInstance();
	private:
		SingletonWindow() noexcept;
		~SingletonWindow();
		SingletonWindow(const SingletonWindow&) = delete;
		SingletonWindow& operator=(const SingletonWindow&) = delete;
		static constexpr const wchar_t m_wndClassName[] = L"ClassName";
		static SingletonWindow m_singlWindo;
		HINSTANCE m_hInst;
	};

public:
	Window(const wchar_t* windowName, int width, int height);
	~Window();

	void changeWindowText(const char* text);
	std::pair<uint32_t, uint32_t> getClientSize() const;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void print(const wchar_t* out);
	void print(const char* out);
	void print(const std::string& out);
	void destroyDirectAPI();

	bool isClosed() const;

	//Engine1::Graphics& getGraphics();
	//const std::shared_ptr<Mouse>& getMouse() const noexcept;
	//const std::shared_ptr<Keyboard>& getKeyboard() const noexcept;

	HWND getHwnd() const;

private:
	int m_width;
	int m_height;
	HWND m_hWnd;
	bool m_isClosed;

	//Engine1::Graphics* m_graphics;
	//std::shared_ptr<Mouse> m_mouse;
	//std::shared_ptr<Keyboard> m_pKeyboard;

	std::vector<unsigned char> m_ridData;
	//App* m_pApp;

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
};
