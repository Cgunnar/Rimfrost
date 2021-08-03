#include "rfpch.hpp"
#include "RenderPass.hpp"
#include "Logger.hpp"
#include "EventSystem.hpp"
#include "ResizeEvent.hpp"


using namespace Microsoft::WRL;
using namespace std;

namespace Rimfrost
{
	RenderPass::RenderPass()
	{
		EventSystem::addObserver(*this, PreResizeEvent::eventType);
		EventSystem::addObserver(*this, PostResizeEvent::eventType);
	}
	
	void RenderPass::AttachFrameBufferViews(std::vector<View>&& views)
	{
		m_frameBufferViews = std::move(views);
		views.clear();
	}
	void RenderPass::AttachBufferViews(std::vector<View>&& views)
	{
		m_bufferViews = std::move(views);
		views.clear();
	}
	void RenderPass::addSamplers(const std::vector<D3D11_SAMPLER_DESC>& samplers, int bindSlot, ShaderEnum shader)
	{
		for (auto& s : samplers)
		{
			m_samplers.emplace_back(make_tuple(s, bindSlot, shader));
		}
	}
	void RenderPass::addBlendState(const D3D11_BLEND_DESC& blendDesc)
	{
		HRESULT hr = this->getDevice()->CreateBlendState(&blendDesc, &m_blendState);
		assert(SUCCEEDED(hr));
	}
	void RenderPass::bind()
	{
		auto& context = getContext();


		for (auto& s : m_samplers)
		{
			auto [sampler, bindslot, shader] = s;
			sampler.bind(bindslot, shader);
		}

		context->OMSetBlendState(m_blendState.Get(), nullptr, 0xFFFFFFFFu);
		


		UINT uavInitCond = -1;
		UINT numRTVs = static_cast<UINT>(m_renderTargetViews.size());
		UINT numUAVs = static_cast<UINT>(m_unorderdAccessViews.size());
		UINT numSRVs = static_cast<UINT>(m_shaderResourceViews.size());

		context->PSSetShaderResources(0, numSRVs, m_shaderResourceViews.data()->GetAddressOf());


		context->OMSetRenderTargetsAndUnorderedAccessViews(
			numRTVs, m_renderTargetViews.data()->GetAddressOf(),
			m_depthStencilView.Get(),
			numRTVs, numUAVs, m_unorderdAccessViews.data()->GetAddressOf(), &uavInitCond);
	}
	void RenderPass::unBind()
	{
		ID3D11ShaderResourceView* emptySRV[8] = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
		this->getContext()->PSSetShaderResources(0, static_cast<UINT>(m_shaderResourceViews.size()), emptySRV);

		UINT numRTVs = static_cast<UINT>(m_renderTargetViews.size());
		UINT numUAVs = static_cast<UINT>(m_unorderdAccessViews.size());
		ID3D11UnorderedAccessView* emptyUAV[8] = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
		ID3D11RenderTargetView* emptyRTV[8] = { nullptr, nullptr, nullptr, nullptr,nullptr, nullptr, nullptr, nullptr };
		this->getContext()->OMSetRenderTargetsAndUnorderedAccessViews(numRTVs, emptyRTV, nullptr, numRTVs, 1, emptyUAV, nullptr);
	}
	void RenderPass::clearRenderTargets()
	{
		float clearColor[4] = { pow(0.1f, 2.2f), pow(0.0f, 2.2f), pow(0.2f, 2.2f), 1.0f };
		for (auto& rtv : m_renderTargetViews)
		{
			getContext()->ClearRenderTargetView(rtv.Get(), clearColor);
		}
	}
	void RenderPass::clearDepthStencil()
	{	
		if (m_depthStencilView)
			getContext()->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
	void RenderPass::clearUnorderdAccessViews()
	{
		constexpr UINT clearValue{};
		for (auto& uav : m_unorderdAccessViews)
		{
			getContext()->ClearUnorderedAccessViewUint(uav.Get(), &clearValue);
		}
	}
	void RenderPass::acquireResources()
	{

		//get frameBuffers from repo
		shared_ptr<FrameBufferView> frameBufferRef;
		
		for (auto& v : m_frameBufferViews)
		{
			if (v.name().empty())
			{
				frameBufferRef = getFrameBufferRepo().getBuffer(v.ID());
			}
			else
			{
				frameBufferRef = getFrameBufferRepo().getBuffer(v.name());
			}

			switch (v.type())
			{
			case ViewType::RTV:
				assert(frameBufferRef->rtv);
				m_renderTargetViews.push_back(frameBufferRef->rtv);
				break;
			case ViewType::SRV:
				assert(frameBufferRef->srv);
				m_shaderResourceViews.push_back(frameBufferRef->srv);
				break;
			case ViewType::UAV:
				assert(frameBufferRef->uav);
				m_unorderdAccessViews.push_back(frameBufferRef->uav);
				break;
			case ViewType::DSV:
				assert(frameBufferRef->dsv);
				m_depthStencilView = frameBufferRef->dsv;
				break;
			default:
				assert(false);
				break;
			}
		}

		//get buffers from repo
		shared_ptr<BufferView> bufferRef;

		for (auto& v : m_bufferViews)
		{
			if (v.name().empty())
			{
				bufferRef = getBufferRepo().getBuffer(v.ID());
			}
			else
			{
				bufferRef = getBufferRepo().getBuffer(v.name());
			}

			switch (v.type())
			{
			case ViewType::SRV:
				assert(bufferRef->srv);
				m_shaderResourceViews.push_back(bufferRef->srv);
				break;
			case ViewType::UAV:
				assert(bufferRef->uav);
				m_unorderdAccessViews.push_back(bufferRef->uav);
				break;
			default:
				assert(false);
				break;
			}
		}

	}
	void RenderPass::releaseResources()
	{
		m_depthStencilView.Reset();
		m_renderTargetViews.clear();
		m_shaderResourceViews.clear();
		m_unorderdAccessViews.clear();
	}
	void RenderPass::onEvent(const Event& e)
	{
		if (e.type() == PreResizeEvent::eventType)
		{

			Logger::getLogger().debugLog("RenderPass releaseResources()\n");

			releaseResources();
		}
		if (e.type() == PostResizeEvent::eventType)
		{
			Logger::getLogger().debugLog("RenderPass acquireResources()\n");
			acquireResources();
		}
	}
}