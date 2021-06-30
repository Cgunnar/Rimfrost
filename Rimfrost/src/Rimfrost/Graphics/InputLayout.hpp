#pragma once
#include "DXAccess.hpp"
#include "VertexShader.hpp"

namespace Engine1
{
	class InputLayout : public DXAccess
	{
	public:
		InputLayout() = default;
		~InputLayout() = default;
		enum class Layout
		{
			POS_TEX_NOR = 1,
			POS_TEX_NOR_TAN_BITAN,
			POS_TEX,
		};
		void init(VertexShader& vs, Layout layout = Layout::POS_TEX_NOR);

		void bind();
		void unBind();



	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	};
}
