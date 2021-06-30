#include "rfpch.hpp"
#include "FrameBufferRepo.hpp"
#include "Logger.hpp"

using namespace std;
using namespace Microsoft::WRL;


namespace Engine1
{

	FrameBufferRepo::FrameBufferRepo()
	{
		FrameBufferDescriptor desc;
		desc.name = "backBuffer";

		shared_ptr<FrameBufferView> bufferView = make_shared<FrameBufferView>();

		bufferView->desc = desc;
		m_nameToIDMap[desc.name] = 0;

		m_frameBuffers.push_back(bufferView);
	}

	BufferID FrameBufferRepo::createBuffer(const FrameBufferDescriptor& descriptor)
	{
		BufferID bufferID = m_frameBuffers.size();
		if (!descriptor.name.empty() && !m_nameToIDMap.contains(descriptor.name))
		{
			m_nameToIDMap[descriptor.name] = bufferID;
		}

		shared_ptr<FrameBufferView> bufferView = make_shared<FrameBufferView>();
		bufferView->desc = descriptor;

		internalCreateBuffer(bufferView);

		m_frameBuffers.push_back(bufferView);
		return bufferID;
	}



	void FrameBufferRepo::internalCreateBuffer(std::shared_ptr<FrameBufferView>& bufferView)
	{

		HRESULT hr;
		hr = this->getDevice()->CreateTexture2D(&bufferView->desc.tex2dDesc, nullptr, &bufferView->buffer);
		assert(SUCCEEDED(hr));

		if (bufferView->desc.tex2dDesc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc = {};
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			desc.Format = bufferView->desc.tex2dDesc.Format;
			desc.Texture2D.MipSlice = 0;

			hr = this->getDevice()->CreateRenderTargetView(bufferView->buffer.Get(), &desc, &bufferView->rtv);
			assert(SUCCEEDED(hr));
		}
		if (bufferView->desc.tex2dDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Format = bufferView->desc.tex2dDesc.Format;
			desc.Texture2D.MostDetailedMip = 0;
			desc.Texture2D.MipLevels = 1;

			hr = this->getDevice()->CreateShaderResourceView(bufferView->buffer.Get(), &desc, &bufferView->srv);
			assert(SUCCEEDED(hr));
		}
		if (bufferView->desc.tex2dDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC desc = { };
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			desc.Buffer.Flags = 0;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.NumElements = 1;

			hr = this->getDevice()->CreateUnorderedAccessView(bufferView->buffer.Get(), &desc, &bufferView->uav);
			assert(SUCCEEDED(hr));
		}
		if (bufferView->desc.tex2dDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
			desc.Format = bufferView->desc.tex2dDesc.Format;
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

			hr = getDevice()->CreateDepthStencilView(bufferView->buffer.Get(), &desc, &bufferView->dsv);
			assert(SUCCEEDED(hr));
		}
	}


	std::shared_ptr<FrameBufferView> FrameBufferRepo::getBuffer(BufferID id)
	{
		return m_frameBuffers[id];
	}

	std::shared_ptr<FrameBufferView> FrameBufferRepo::getBuffer(const std::string& name)
	{
		auto iter = m_nameToIDMap.find(name);
		assert(iter != m_nameToIDMap.end());
		return getBuffer(iter->second);
	}

	void FrameBufferRepo::preResize()
	{
		for (int i = 0; i < m_frameBuffers.size(); i++)
		{
			if (!m_frameBuffers[i]->desc.fixedResolution)
			{
				m_frameBuffers[i]->uav.Reset();
				m_frameBuffers[i]->dsv.Reset();
				m_frameBuffers[i]->rtv.Reset();
				m_frameBuffers[i]->srv.Reset();
				m_frameBuffers[i]->buffer.Reset();
			}
		}
	}

	void FrameBufferRepo::postResize(const UINT width, const UINT height)
	{
		Logger::getLogger().debugLog("FrameBufferRepo::postResize\n");
		addBackBuffer();


		for (int i = 1; i < m_frameBuffers.size(); i++)
		{
			if (!m_frameBuffers[i]->desc.fixedResolution)
			{
				m_frameBuffers[i]->desc.tex2dDesc.Height = height;
				m_frameBuffers[i]->desc.tex2dDesc.Width = width;
				internalCreateBuffer(m_frameBuffers[i]);
			}
		}
	}

	void FrameBufferRepo::addBackBuffer()
	{
		assert(m_frameBuffers.size() > 0);
		m_frameBuffers[0]->buffer = getBackBuffer();
		m_frameBuffers[0]->buffer->GetDesc(&m_frameBuffers[0]->desc.tex2dDesc);

		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		//desc.Format = m_frameBuffers[0]->desc.tex2dDesc.Format;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = this->getDevice()->CreateRenderTargetView(m_frameBuffers[0]->buffer.Get(), &desc, &m_frameBuffers[0]->rtv);
		assert(SUCCEEDED(hr));
	}


}