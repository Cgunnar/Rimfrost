#include "rfpch.hpp"
#include "FrameBuffers.hpp"
#include <EventSystem.hpp>
#include "ResizeEvent.hpp"
#include "Logger.hpp"

namespace Rimfrost
{
	FrameBuffers::FrameBuffers()
	{
		EventSystem::addObserver(*this, PreResizeEvent::eventType);
		EventSystem::addObserver(*this, PostResizeEvent::eventType);
	}

	FrameBuffers::FrameBuffers(const std::string& name) : FrameBuffers()
	{
		this->m_debugName = name;
	}

	FrameBuffers::~FrameBuffers()
	{
	}

	bool FrameBuffers::AttachFrameBuffer(const std::vector<FrameBufferDesc>& descriptors)
	{
		if (descriptors.empty()) return false;
		HRESULT hr;



		m_bufferHandlers.reserve(descriptors.size());

		m_texture2Ds.resize(descriptors.size());
		m_renderTargetViews.resize(descriptors.size() + numRTVs);
		m_shaderResourceViews.resize(descriptors.size() + numSRVs);

		D3D11_SUBRESOURCE_DATA* subresdata = nullptr;

		for (int i = 0; i < descriptors.size(); i++)
		{
			BufferMetaData handle;
			handle.desc = descriptors[i];
			handle.tex2dID = i;

			if ((descriptors[i].settings & FrameBufferSetting::BACKBUFFER_RENDER_TARGET) == FrameBufferSetting::BACKBUFFER_RENDER_TARGET)
			{
				m_texture2Ds[i] = this->getBackBuffer();
				m_texture2Ds[i]->GetDesc(&handle.desc.tex2dDesc);
			}
			else
			{
				hr = this->getDevice()->CreateTexture2D(&descriptors[i].tex2dDesc, subresdata, &m_texture2Ds[i]);
				assert(SUCCEEDED(hr));
			}

			if (handle.desc.tex2dDesc.BindFlags & D3D11_BIND_RENDER_TARGET)
			{
				D3D11_RENDER_TARGET_VIEW_DESC desc = {};
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				desc.Format = descriptors[i].tex2dDesc.Format;
				desc.Texture2D.MipSlice = 0;

				handle.rtvIndex = numRTVs;
				hr = this->getDevice()->CreateRenderTargetView(m_texture2Ds[i].Get(), 0/*&desc*/, &m_renderTargetViews[numRTVs++]);
				assert(SUCCEEDED(hr));
			}
			if (handle.desc.tex2dDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				desc.Format = descriptors[i].tex2dDesc.Format;
				desc.Texture2D.MostDetailedMip = 0;
				desc.Texture2D.MipLevels = 1;

				handle.srvIndex = numSRVs;
				hr = this->getDevice()->CreateShaderResourceView(m_texture2Ds[i].Get(), &desc, &m_shaderResourceViews[numSRVs++]);
				assert(SUCCEEDED(hr));
			}


			if (handle.desc.tex2dDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
				desc.Format = descriptors[i].tex2dDesc.Format;
				desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

				handle.hasDepthStencil = true;
				hr = getDevice()->CreateDepthStencilView(m_texture2Ds[i].Get(), &desc, &m_depthStencilView);
				assert(SUCCEEDED(hr));

				m_hasDepthStencil = true;
			}

			m_bufferHandlers.push_back(handle);
		}
		return true;
	}


	bool FrameBuffers::AttachUAVs(const std::vector<D3D11_BUFFER_DESC>& descriptors)
	{
		if (descriptors.empty()) return false;

		m_buffers.resize(descriptors.size());
		m_unorderdAccessViews.resize(descriptors.size());

		for (int i = 0; i < descriptors.size(); i++)
		{
			HRESULT hr = this->getDevice()->CreateBuffer(&descriptors[i], nullptr, &m_buffers[i]);
			assert(SUCCEEDED(hr));

			if (descriptors[i].BindFlags & D3D11_BIND_UNORDERED_ACCESS)
			{
				D3D11_UNORDERED_ACCESS_VIEW_DESC desc = { };
				desc.Format = DXGI_FORMAT_UNKNOWN;
				desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
				desc.Buffer.Flags = 0;
				desc.Buffer.FirstElement = 0;
				desc.Buffer.NumElements = 1;

				hr = this->getDevice()->CreateUnorderedAccessView(m_buffers[i].Get(), &desc, &m_unorderdAccessViews[numUAVs++]);
				assert(SUCCEEDED(hr));
			}
		}
		return true;
	}

	void FrameBuffers::bindRTVsAndDSV() const
	{
		if (numUAVs == 0)
		{
			if (m_hasDepthStencil)
			{
				this->getContext()->OMSetRenderTargets(numRTVs, m_renderTargetViews.data()->GetAddressOf(), m_depthStencilView.Get());
			}
			else
			{
				this->getContext()->OMSetRenderTargets(numRTVs, m_renderTargetViews.data()->GetAddressOf(), nullptr);
			}
		}
		else
		{
			//do i need to unbind the uav or is that automatic?

			UINT uavInitCond = -1;
			if (m_hasDepthStencil)
			{
				this->getContext()->OMSetRenderTargetsAndUnorderedAccessViews(numRTVs, m_renderTargetViews.data()->GetAddressOf(),
					m_depthStencilView.Get(), numRTVs, numUAVs, m_unorderdAccessViews.data()->GetAddressOf(), &uavInitCond);
			}
			else
			{
				this->getContext()->OMSetRenderTargetsAndUnorderedAccessViews(numRTVs, m_renderTargetViews.data()->GetAddressOf(),
					nullptr, numRTVs, numUAVs, m_unorderdAccessViews.data()->GetAddressOf(), &uavInitCond);
			}
		}
	}

	// only binds to pixelshader
	void FrameBuffers::bindSRVs() const
	{
		assert(numSRVs > 0);
		if (SRVsIsBound)
		{
			assert(false);
		}
		this->getContext()->PSSetShaderResources(0, numSRVs, m_shaderResourceViews.data()->GetAddressOf());
		SRVsIsBound = true;
	}

	//all cant be bound at the same time but whatever
	void FrameBuffers::unBindAll() const
	{
		unBindInput();
		unBindOutPut();
	}

	void FrameBuffers::unBindOutPut() const
	{
		ID3D11UnorderedAccessView* emptyUAV[8] = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
		ID3D11RenderTargetView* emptyRTV[8] = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
		this->getContext()->OMSetRenderTargetsAndUnorderedAccessViews(numRTVs, emptyRTV, nullptr, numRTVs, 1, emptyUAV, nullptr);
	}

	void FrameBuffers::unBindInput() const
	{
		ID3D11ShaderResourceView* emptySRV[8] = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
		if (!SRVsIsBound)
		{
			Logger::getLogger().debugLog("WARNING SRV is not bound, but unBindInput is called. " + this->m_debugName + "\n");
		}
		else if (numSRVs > 0)
		{
			this->getContext()->PSSetShaderResources(0, numSRVs, emptySRV);
			SRVsIsBound = false;
		}
	}

	void FrameBuffers::clearRenderTargets()
	{
		for (auto& b : m_bufferHandlers)
		{
			if (b.rtvIndex != -1)
			{
				//hoppas inte att någon av dem är en dsv, shadow mapping
				this->getContext()->ClearRenderTargetView(m_renderTargetViews[b.rtvIndex].Get(), b.clearColor);
			}
		}
	}

	void FrameBuffers::clearDepthStencil()
	{
		assert(m_hasDepthStencil);
		getContext()->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void FrameBuffers::clearUAVs()
	{
		auto& context = getContext();
		const UINT clearValue{};
		for (auto& uav : m_unorderdAccessViews)
		{
			context->ClearUnorderedAccessViewUint(uav.Get(), &clearValue);
		}
	}

	void FrameBuffers::onEvent(const Event& e)
	{
		if (e.type() == PreResizeEvent::eventType)
		{
			Logger::getLogger().debugLog("PreResizeEvent\n");

			for (auto& b : m_bufferHandlers)
			{
				if ((b.desc.settings & FrameBufferSetting::BACKBUFFER_RENDER_TARGET) == FrameBufferSetting::BACKBUFFER_RENDER_TARGET)
				{
					if (b.rtvIndex != -1) m_renderTargetViews[b.rtvIndex].Reset();
					if (b.srvIndex != -1) m_shaderResourceViews[b.srvIndex].Reset();
					if (b.hasDepthStencil) m_depthStencilView.Reset();
					m_texture2Ds[b.tex2dID].Reset();

				}
			}
		}

		if (e.type() == PostResizeEvent::eventType)
		{
#ifdef _DEBUG
			Logger::getLogger().debugLog("PostResizeEvent\n");
			Logger::getLogger().debugLog((std::string)static_cast<const PostResizeEvent&>(e).type() + "\n");
			Logger::getLogger().debugLog((std::to_string(static_cast<const PostResizeEvent&>(e).width) + "\n"));
			Logger::getLogger().debugLog((std::to_string(static_cast<const PostResizeEvent&>(e).height) + "\n"));
#endif // _DEBUG
			for (auto& b : m_bufferHandlers)
			{
				if ((b.desc.settings & FrameBufferSetting::BACKBUFFER_RENDER_TARGET) == FrameBufferSetting::BACKBUFFER_RENDER_TARGET)
				{
					m_texture2Ds[b.tex2dID] = getBackBuffer();
					HRESULT hr = this->getDevice()->CreateRenderTargetView(m_texture2Ds[b.tex2dID].Get(), 0, &m_renderTargetViews[b.tex2dID]);
					assert(SUCCEEDED(hr));
				}
				else if ((b.desc.settings & FrameBufferSetting::CHANGE_RES_ON_RESIZE) == FrameBufferSetting::CHANGE_RES_ON_RESIZE)
				{
					b.desc.tex2dDesc.Height = static_cast<const PostResizeEvent&>(e).height;
					b.desc.tex2dDesc.Width = static_cast<const PostResizeEvent&>(e).width;
					if (b.rtvIndex != -1) m_renderTargetViews[b.rtvIndex].Reset();
					if (b.srvIndex != -1) m_shaderResourceViews[b.srvIndex].Reset();
					if (b.hasDepthStencil) m_depthStencilView.Reset();
					m_texture2Ds[b.tex2dID].Reset();

					D3D11_SUBRESOURCE_DATA* subresdata = nullptr;
					HRESULT hr = this->getDevice()->CreateTexture2D(&b.desc.tex2dDesc, subresdata, &m_texture2Ds[b.tex2dID]);
					assert(SUCCEEDED(hr));

					if (b.rtvIndex != -1)
					{
						D3D11_RENDER_TARGET_VIEW_DESC desc = {};
						desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
						desc.Format = b.desc.tex2dDesc.Format;
						desc.Texture2D.MipSlice = 0;

						hr = this->getDevice()->CreateRenderTargetView(m_texture2Ds[b.tex2dID].Get(), 0/*&desc*/, &m_renderTargetViews[b.rtvIndex]);
						assert(SUCCEEDED(hr));
					}
					if (b.srvIndex != -1)
					{
						D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
						desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
						desc.Format = b.desc.tex2dDesc.Format;
						desc.Texture2D.MostDetailedMip = 0;
						desc.Texture2D.MipLevels = 1;
						hr = this->getDevice()->CreateShaderResourceView(m_texture2Ds[b.tex2dID].Get(), &desc, &m_shaderResourceViews[b.srvIndex]);
						assert(SUCCEEDED(hr));
					}
					if (b.hasDepthStencil)
					{
						D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
						desc.Format = b.desc.tex2dDesc.Format;
						desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
						hr = getDevice()->CreateDepthStencilView(m_texture2Ds[b.tex2dID].Get(), &desc, &m_depthStencilView);
						assert(SUCCEEDED(hr));
					}
				}
			}
		}
	}
}