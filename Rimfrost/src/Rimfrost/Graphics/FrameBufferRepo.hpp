#pragma once

#include "DXAccess.hpp"
#include "Engine1.hpp"

#include <unordered_map>
#include <string>
namespace Rimfrost
{
	class Graphics;
	
	


	struct FrameBufferDescriptor
	{
		FrameBufferDescriptor() = default;
		FrameBufferDescriptor(D3D11_TEXTURE2D_DESC tex2dDesc, const std::string& name = "", bool fixedResolution = false)
			: tex2dDesc(tex2dDesc), name(name), fixedResolution(fixedResolution) {}
		D3D11_TEXTURE2D_DESC tex2dDesc = {};
		std::string name = "";
		bool fixedResolution = false;
	};
	struct FrameBufferView
	{
		FrameBufferView() = default;
		~FrameBufferView() = default;

		FrameBufferView(const FrameBufferView&) = delete;
		FrameBufferView& operator=(const FrameBufferView&) = delete;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> buffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
		FrameBufferDescriptor desc;

	};

	
	class FrameBufferRepo : DXAccess
	{
		friend Graphics;
	private:

	public:
		static constexpr BufferID backBuffer = 0;
		FrameBufferRepo();
		~FrameBufferRepo() = default;

		FrameBufferRepo(const FrameBufferRepo&) = delete;
		FrameBufferRepo& operator=(const FrameBufferRepo&) = delete;


		BufferID createBuffer(const FrameBufferDescriptor& descriptor);
		std::shared_ptr<FrameBufferView> getBuffer(BufferID id);
		std::shared_ptr<FrameBufferView> getBuffer(const std::string& name);

	private:
		void internalCreateBuffer(std::shared_ptr<FrameBufferView>& bufferView);

		std::vector<std::shared_ptr<FrameBufferView>> m_frameBuffers;
		std::unordered_map<std::string, BufferID> m_nameToIDMap;
		std::vector<BufferID> m_IDs;

	private:
		void preResize();
		void postResize(const UINT width, const UINT height);


		void addBackBuffer();
	};

}