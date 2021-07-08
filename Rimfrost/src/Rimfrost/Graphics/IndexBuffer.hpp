#pragma once
#include "DXAccess.hpp"
namespace Rimfrost
{
	class IndexBuffer : DXAccess
	{
	public:
		IndexBuffer() = default;
		~IndexBuffer() = default;

		void initialize(std::uint32_t* indices, std::uint32_t count);
		void bind();
		void unBind();

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	};
}
