#pragma once

#include "DXAccess.hpp"
#include "FrameBufferRepo.hpp"
#include "BufferRepo.hpp"
#include "EventObserver.hpp"
#include "Sampler.hpp"
#include "Engine1.hpp"

namespace Rimfrost
{
	

	class RenderPass : DXAccess, public EventObserver
	{
	public:
		

		RenderPass();
		~RenderPass() = default;

		void AttachFrameBufferViews(std::vector<View>&& views);
		void AttachBufferViews(std::vector<View>&& views);
		void addSamplers(const std::vector<D3D11_SAMPLER_DESC>& samplers, int bindSlot, ShaderEnum shader);
		void addBlendState(const D3D11_BLEND_DESC & blendDesc);
		void bind();
		void unBind();
		void clearRenderTargets();
		void clearDepthStencil();
		void clearUnorderdAccessViews();


		void acquireResources();
		void releaseResources();
		void onEvent(const Event& e) override;

	private:
		std::vector<std::tuple<Sampler, int, ShaderEnum>> m_samplers;

		std::vector<View> m_frameBufferViews;
		std::vector<View> m_bufferViews;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView = nullptr;
		std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_renderTargetViews;
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_shaderResourceViews;
		std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>> m_unorderdAccessViews;


		Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;

	private:
		
	};

}