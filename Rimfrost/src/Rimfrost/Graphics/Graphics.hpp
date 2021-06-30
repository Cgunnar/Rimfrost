#pragma once

#include <dxgi1_6.h>
#include "FrameBufferRepo.hpp"
#include "BufferRepo.hpp"

namespace Engine1
{

	class Graphics
	{
		friend class DXAccess;
		friend class Window;
	public:
		Graphics(HWND hWnd, UINT width, UINT height);
		~Graphics();

		void beginFrame();
		void endFrame();

		void useImGui(bool onOff) noexcept;

		bool isFullScreen();
		bool setFullScreen(BOOL fullScreen);

		Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice();
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetDeviceContext();
		Microsoft::WRL::ComPtr<IDXGISwapChain>& GetSwapChain();
		Microsoft::WRL::ComPtr<ID3D11Texture2D> getBackBuffer();

		std::tuple<uint32_t, uint32_t> getBackBufferResoltion();
	private:
		void createDevice();
		void createSwapChain(HWND hWnd);
		void setViewPort(float width, float height);

		void setUpBackBuffer();
		void dropBackBuffer();

		void checkMonitorRes();


		void onResize(UINT newWidth, UINT newHeight);
		void resizeTarget(UINT width, UINT height);

	private:
		bool m_useImGui;

		UINT m_width;
		UINT m_height;

		UINT m_nativeWidth;
		UINT m_nativeHeight;

		BOOL m_fullScreen;
		BOOL m_shouldBeFullScreen;

		//ID3D11Debug* m_debug = nullptr;

		Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;

		FrameBufferRepo m_frameBuffers;
		BufferRepo m_buffers;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rendertargetViewNonSRGB;
	};

}