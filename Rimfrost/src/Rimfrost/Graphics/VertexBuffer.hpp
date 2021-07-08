#pragma once
#include "DXAccess.hpp"
#include <cstdint>
namespace Rimfrost
{
	class VertexBuffer : public DXAccess
	{
	public:
		VertexBuffer();
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		~VertexBuffer();



		void bind();
		void unBind();

		void createBuffer(const float* vertexArray, size_t arraySize, uint32_t vertexStride, uint32_t vertexOffset = 0);
		std::uint32_t getVertexCount() const;
		Microsoft::WRL::ComPtr<ID3D11Buffer>& getBuffer();


	private:
		float* m_vertexPtr = nullptr;
		std::uint32_t m_vertexStride = 0;
		std::uint32_t m_vertexCount = 0;
		std::uint32_t m_vertexOffset = 0;
		D3D11_BUFFER_DESC m_vertexBufferDesc = {};

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	};
}
