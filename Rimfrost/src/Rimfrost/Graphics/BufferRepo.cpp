#include "rfpch.hpp"
#include "BufferRepo.hpp"

using namespace std;
using namespace Microsoft::WRL;

namespace Engine1
{
	BufferID BufferRepo::IDFromName(const std::string& name)
	{
		assert(!name.empty());
		assert(m_nameToIDMap.contains(name));
		return m_nameToIDMap[name];
	}
	BufferID BufferRepo::createBuffer(const BufferDescriptor& descriptor, D3D11_SUBRESOURCE_DATA* data)
	{
		BufferID bufferID = m_buffers.size();
		if (!descriptor.name.empty() && !m_nameToIDMap.contains(descriptor.name))
		{
			m_nameToIDMap[descriptor.name] = bufferID;
		}

		shared_ptr<BufferView> bufferView = make_shared<BufferView>();
		bufferView->desc = descriptor;

		internalCreateBuffer(bufferView, data);

		m_buffers.push_back(bufferView);
		return bufferID;
	}
	void BufferRepo::reCreateBuffer(BufferID ID, D3D11_BUFFER_DESC bufferDesc, D3D11_SUBRESOURCE_DATA* data)
	{
		auto view = getBuffer(ID);
		view->uav.Reset();
		view->srv.Reset();
		view->buffer.Reset();
		view->desc.bufferDesc = bufferDesc;

		internalCreateBuffer(view, data);
	}
	shared_ptr<BufferView> BufferRepo::getBuffer(BufferID id)
	{
		return m_buffers[id];
	}
	shared_ptr<BufferView> BufferRepo::getBuffer(const string& name)
	{
		auto iter = m_nameToIDMap.find(name);
		assert(iter != m_nameToIDMap.end());
		return getBuffer(iter->second);
	}
	void BufferRepo::internalCreateBuffer(std::shared_ptr<BufferView>& bufferView, D3D11_SUBRESOURCE_DATA* data)
	{
		assert((bufferView->desc.bufferDesc.BindFlags & D3D11_BIND_CONSTANT_BUFFER) == 0);


		HRESULT hr;
		hr = this->getDevice()->CreateBuffer(&bufferView->desc.bufferDesc, data, &bufferView->buffer);
		assert(SUCCEEDED(hr));


		if (bufferView->desc.bufferDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC desc = { }; // fix non hardcoded desc
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			desc.Buffer.Flags = 0;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.NumElements = bufferView->desc.bufferDesc.ByteWidth / bufferView->desc.bufferDesc.StructureByteStride;

			hr = this->getDevice()->CreateUnorderedAccessView(bufferView->buffer.Get(), &desc, &bufferView->uav);
			assert(SUCCEEDED(hr));
		}
		if (bufferView->desc.bufferDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc = { }; // fix non hardcoded desc
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.NumElements = bufferView->desc.bufferDesc.ByteWidth / bufferView->desc.bufferDesc.StructureByteStride;

			hr = this->getDevice()->CreateShaderResourceView(bufferView->buffer.Get(), &desc, &bufferView->srv);
			assert(SUCCEEDED(hr));
		}
	}
	void BufferView::updateBuffer(const void* data)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubRes;
		this->getContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubRes);
		memcpy(mappedSubRes.pData, data, desc.bufferDesc.ByteWidth);
		this->getContext()->Unmap(buffer.Get(), 0);
	}
}