#pragma once

#include "DXAccess.hpp"
#include "EventObserver.hpp"

namespace Engine1
{
	enum class FrameBufferSetting
	{
		NONE = 0,
		BACKBUFFER_RENDER_TARGET = 1 << 0,
		CHANGE_RES_ON_RESIZE = 1 << 1,
		FIXED_RESOLUTION = 1 << 2,
	};
	inline FrameBufferSetting operator &(FrameBufferSetting l, FrameBufferSetting r)
	{
		return (FrameBufferSetting)((int)l & (int)r);
	}
	inline FrameBufferSetting operator |(FrameBufferSetting l, FrameBufferSetting r)
	{
		return (FrameBufferSetting)((int)l | (int)r);
	}

	struct FrameBufferDesc
	{
		D3D11_TEXTURE2D_DESC tex2dDesc;
		FrameBufferSetting settings = FrameBufferSetting::CHANGE_RES_ON_RESIZE;
	};

	class FrameBuffers : DXAccess, public EventObserver
	{
	public:
		FrameBuffers();
		FrameBuffers(const std::string& name);
		~FrameBuffers();

		std::string m_debugName;

		bool AttachFrameBuffer(const std::vector<FrameBufferDesc>& descriptor);
		bool AttachUAVs(const std::vector<D3D11_BUFFER_DESC>& descriptor);
		void bindRTVsAndDSV() const;
		void bindSRVs() const;
		void unBindAll() const;
		void unBindOutPut() const;
		void unBindInput() const;
		void clearRenderTargets();
		void clearDepthStencil();
		void clearUAVs();

		void onEvent(const Event& e) override;
		std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_buffers;
	private:
		
	private:


		struct BufferMetaData
		{
			int srvIndex = -1;
			int rtvIndex = -1;
			int tex2dID = -1;
			bool hasDepthStencil = false;
			float clearColor[4] = { 0.1f, 0.0f, 0.2f, 1.0f };
			FrameBufferDesc desc;
		};

		uint32_t numRTVs = 0;
		uint32_t numSRVs = 0;
		uint32_t numUAVs = 0;

		mutable bool SRVsIsBound = false;

		bool m_hasDepthStencil = false;

		std::vector<BufferMetaData> m_bufferHandlers;

		std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> m_texture2Ds;
		//Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilTexture;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
		
		std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_renderTargetViews;
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_shaderResourceViews;
		std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>> m_unorderdAccessViews;
	};

}