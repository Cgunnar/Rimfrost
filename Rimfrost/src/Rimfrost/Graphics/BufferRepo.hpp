#pragma once
#include "DXAccess.hpp"
#include "Engine1.hpp"
#include <d3d11.h>
#include <wrl.h>
#include <unordered_map>
namespace Engine1
{

	struct BufferDescriptor
	{
		BufferDescriptor() = default;
		BufferDescriptor(D3D11_BUFFER_DESC bufferDesc, UINT bindSlot, const std::string& name = "")
			: bufferDesc(bufferDesc), name(name), bindSlot(bindSlot) {}
		D3D11_BUFFER_DESC bufferDesc = {};
		std::string name = "";
		UINT bindSlot = 0;
	};
	struct BufferView : DXAccess
	{
		BufferView() = default;
		~BufferView() = default;

		BufferView(const BufferView&) = delete;
		BufferView& operator=(const BufferView&) = delete;

		void updateBuffer(const void* data);

		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;
		BufferDescriptor desc;


	};

	class BufferRepo : DXAccess
	{
	public:
		BufferRepo() = default;
		~BufferRepo() = default;

		BufferID IDFromName(const std::string& name);

		BufferRepo(const BufferRepo&) = delete;
		BufferRepo& operator=(const BufferRepo&) = delete;


		BufferID createBuffer(const BufferDescriptor& descriptor, D3D11_SUBRESOURCE_DATA* data = nullptr);
		void reCreateBuffer(BufferID ID, D3D11_BUFFER_DESC bufferDesc, D3D11_SUBRESOURCE_DATA* data = nullptr);
		std::shared_ptr<BufferView> getBuffer(BufferID id);
		std::shared_ptr<BufferView> getBuffer(const std::string& name);

	private:
		void internalCreateBuffer(std::shared_ptr<BufferView>& bufferView, D3D11_SUBRESOURCE_DATA* data);

		std::vector<std::shared_ptr<BufferView>> m_buffers;
		std::unordered_map<std::string, BufferID> m_nameToIDMap;
		std::vector<BufferID> m_IDs;
	};
}
