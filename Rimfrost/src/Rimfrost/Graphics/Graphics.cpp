#include "rfpch.hpp"

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "dxgi.lib" ) 
#pragma comment( lib, "user32" )

#include "Logger.hpp"
#include "Graphics.hpp"
#include "DXAccess.hpp"
#include <EventSystem.hpp>
#include "ResizeEvent.hpp"



extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;


namespace Rimfrost
{
	Graphics::Graphics(HWND hWnd, UINT width, UINT height) : m_width(width), m_height(height), m_nativeWidth(0), m_nativeHeight(0)
	{
		m_useImGui = true;
		m_shouldBeFullScreen = false;

		DXAccess::s_graphics = this;
		createDevice();
		createSwapChain(hWnd);
		setUpBackBuffer();

		checkMonitorRes();

		m_frameBuffers.addBackBuffer();
	}

	Graphics::~Graphics()
	{
		Logger::getLogger().debugLog("~Graphics\n");
		DXAccess::s_graphics = nullptr;
	}

	void Graphics::beginFrame()
	{
		if (m_useImGui)
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}
	}

	void Graphics::endFrame()
	{
		if (m_useImGui)
		{
			m_context->OMSetRenderTargets(1, m_rendertargetViewNonSRGB.GetAddressOf(), nullptr);

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		m_swapChain->Present(0, 0);
	}

	void Graphics::useImGui(bool onOff) noexcept
	{
		m_useImGui = onOff;
	}

	ComPtr<ID3D11Device>& Graphics::GetDevice()
	{
		return m_device;
	}

	ComPtr<ID3D11DeviceContext>& Graphics::GetDeviceContext()
	{
		return m_context;
	}

	ComPtr<IDXGISwapChain>& Graphics::GetSwapChain()
	{
		return m_swapChain;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Graphics::getBackBuffer()
	{
		ComPtr<ID3D11Texture2D> backBuffer;
		HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
		assert(SUCCEEDED(hr));
		return backBuffer;
	}

	std::tuple<uint32_t, uint32_t> Graphics::getBackBufferResoltion()
	{
		HRESULT hr;

		ComPtr<ID3D11Texture2D> backBuffer;
		D3D11_TEXTURE2D_DESC backBufferDesc;
		hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
		backBuffer->GetDesc(&backBufferDesc);
		assert(SUCCEEDED(hr));

		return std::tuple<uint32_t, uint32_t>((uint32_t)backBufferDesc.Width, (uint32_t)backBufferDesc.Height);
	}

	void Graphics::createDevice()
	{
		HRESULT hr;

		D3D_FEATURE_LEVEL feature_level;
		UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(DEBUG) || defined (_DEBUG) 
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 

		hr = D3D11CreateDevice
		(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			flags,
			NULL,
			0,
			D3D11_SDK_VERSION,
			&m_device,
			&feature_level,
			&m_context
		);
	}

	void Graphics::createSwapChain(HWND hWnd)
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Width = this->m_width;
		swapChainDesc.BufferDesc.Width = this->m_height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr;

		IDXGIDevice* deviceIDXGI = nullptr;
		IDXGIAdapter* adapterIDXGI = nullptr;
		IDXGIFactory* factoryIDXGI = nullptr;



		/*hr = this->m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&m_debug);
		assert(SUCCEEDED(hr));*/



		hr = this->m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&deviceIDXGI);
		assert(SUCCEEDED(hr));

		hr = deviceIDXGI->GetParent(__uuidof(IDXGIAdapter), (void**)&adapterIDXGI);
		assert(SUCCEEDED(hr));

		hr = adapterIDXGI->GetParent(__uuidof(IDXGIFactory), (void**)&factoryIDXGI);
		assert(SUCCEEDED(hr));

		hr = factoryIDXGI->CreateSwapChain(this->m_device.Get(), &swapChainDesc, &this->m_swapChain);
		assert(SUCCEEDED(hr));

		factoryIDXGI->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

		factoryIDXGI->Release();
		adapterIDXGI->Release();
		deviceIDXGI->Release();
	}


	void Graphics::setViewPort(float width, float height)
	{
		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		m_context->RSSetViewports(1, &vp);
	}



	void Graphics::setUpBackBuffer()
	{

		//viewPort
		auto [width, height] = getBackBufferResoltion();
		setViewPort(static_cast<float>(width), static_cast<float>(height));

		

		m_frameBuffers.postResize(width, height);

		HRESULT hr = m_device->CreateRenderTargetView(m_frameBuffers.getBuffer(FrameBufferRepo::backBuffer)->buffer.Get(),
			nullptr, &m_rendertargetViewNonSRGB);
		assert(SUCCEEDED(hr));

		EventSystem::postInstantly(PostResizeEvent(width, height));
	}

	void Graphics::dropBackBuffer()
	{
		EventSystem::postInstantly(PreResizeEvent());
		m_frameBuffers.preResize();
		m_rendertargetViewNonSRGB.Reset();

		this->m_context->ClearState();
		this->m_context->Flush();
	}


	void Graphics::checkMonitorRes()
	{
		m_nativeWidth = 0;
		m_nativeHeight = 0;

		IDXGIOutput* outPut = nullptr;
		HRESULT hr = this->m_swapChain->GetContainingOutput(&outPut);
		assert(SUCCEEDED(hr));


		DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;

		UINT numModes = 0;
		hr = outPut->GetDisplayModeList(format, 0, &numModes, 0);
		assert(SUCCEEDED(hr));
		std::vector<DXGI_MODE_DESC> modeVec;
		modeVec.resize(numModes);
		//DXGI_MODE_DESC* modeList = new DXGI_MODE_DESC[numModes];
		//hr = outPut->GetDisplayModeList(format, 0, &numModes, modeList);
		hr = outPut->GetDisplayModeList(format, 0, &numModes, modeVec.data());
		assert(SUCCEEDED(hr));

		/*Logger log = Logger::getLogger();
		log.setFile("modeList.txt");*/
		for (UINT i = 0; i < numModes; i++)
		{
			//if (modeList[i].Width >= m_nativeWidth) m_nativeWidth = modeList[i].Width;
			//if (modeList[i].Height >= m_nativeHeight) m_nativeHeight = modeList[i].Height;

			if (modeVec[i].Width >= m_nativeWidth) m_nativeWidth = modeVec[i].Width;
			if (modeVec[i].Height >= m_nativeHeight) m_nativeHeight = modeVec[i].Height;

			/*log.addLog("width:\t\t" + std::to_string(modeList[i].Width) + "\n");
			log.addLog("height:\t\t" + std::to_string(modeList[i].Height) + "\n");
			log.addLog("RefreshRate:\t" + std::to_string((float)modeList[i].RefreshRate.Numerator / (float)modeList->RefreshRate.Denominator) + "\n");
			log.addLog("Scaling:\t\t" + std::to_string(modeList[i].Scaling) + "\n");
			log.addLog("ScanlineOrdering:\t" + std::to_string(modeList[i].ScanlineOrdering) + "\n");
			log.addLog("Format:\t\t" + std::to_string(modeList[i].Format) + "\n\n");*/
		}

		//log.dumpLogs();
		//delete[] modeList;
		outPut->Release();

		Logger::getLogger().debugLog("Monitor resolution: " + std::to_string(m_nativeWidth) + "x" + std::to_string(m_nativeHeight) + "\n");
	}

	void Graphics::onResize(UINT newWidth, UINT newHeight)
	{
		bool changed = setFullScreen(m_shouldBeFullScreen); //should not matter if argument is true or false, setFullScreen() will figure it out
		if (changed && m_shouldBeFullScreen)
		{
			newWidth = m_nativeWidth;
			newHeight = m_nativeHeight;
		}
		this->dropBackBuffer();



		HRESULT hr;
		hr = this->m_swapChain->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		if (!SUCCEEDED(hr))
		{
			//m_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}
		assert(SUCCEEDED(hr));


		this->setUpBackBuffer();
	}


	bool Graphics::isFullScreen()
	{
		HRESULT hr = this->m_swapChain.Get()->GetFullscreenState(&this->m_fullScreen, nullptr);
		assert(SUCCEEDED(hr));
		return (bool)this->m_fullScreen;
	}

	bool Graphics::setFullScreen(BOOL fullScreen)
	{

		HRESULT hr = m_swapChain.Get()->GetFullscreenState(&m_fullScreen, nullptr);
		assert(SUCCEEDED(hr));
		if (m_fullScreen != m_shouldBeFullScreen) //check if fullscreen has changed, ignore input argument
		{
			fullScreen = m_fullScreen;
		}
		else if (fullScreen == m_fullScreen) //early exit if fullscreen is already at a desired state 
		{
			return false;
		}

		Logger::getLogger().debugLog("setFullScreen to ");
		Logger::getLogger().debugLog(fullScreen ? "true\n" : "false\n");
		if (fullScreen)
		{
			resizeTarget(m_nativeWidth, m_nativeHeight);
		}
		else
		{
			hr = m_swapChain->SetFullscreenState(false, nullptr); //force it to accept resize
			assert(SUCCEEDED(hr));
			resizeTarget(1280, 720);
		}

		hr = m_swapChain->SetFullscreenState(fullScreen, nullptr);
		assert(SUCCEEDED(hr));
		m_shouldBeFullScreen = fullScreen;
		m_fullScreen = fullScreen;

		//resize is needed, if DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL next present will fail otherwise
		dropBackBuffer();
		hr = this->m_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		assert(SUCCEEDED(hr));
		setUpBackBuffer();
		return true;
	}

	void Graphics::resizeTarget(UINT width, UINT height)
	{
		DXGI_MODE_DESC modeDesc = {};

		DXGI_MODE_DESC prefModeDesc = {};

		prefModeDesc.Width = width;
		prefModeDesc.Height = height;
		prefModeDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		prefModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;

		IDXGIOutput* outPut = nullptr;
		HRESULT hr = m_swapChain->GetContainingOutput(&outPut);
		assert(SUCCEEDED(hr));

		hr = outPut->FindClosestMatchingMode(&prefModeDesc, &modeDesc, m_device.Get());
		assert(SUCCEEDED(hr));
		Logger::getLogger().debugLog("FindClosestMatchingMode:\tResolution: " + std::to_string(modeDesc.Width) + "x" + std::to_string(modeDesc.Height) +
			"\tRefreshRate: " + std::to_string((float)modeDesc.RefreshRate.Numerator / (float)modeDesc.RefreshRate.Denominator) + "\n");


		hr = m_swapChain->ResizeTarget(&modeDesc);
		assert(SUCCEEDED(hr));
		outPut->Release();
	}
}