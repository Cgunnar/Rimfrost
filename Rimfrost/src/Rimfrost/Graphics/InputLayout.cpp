#include "pch.hpp"
#include "InputLayout.hpp"

namespace Engine1
{
	void InputLayout::init(VertexShader& vs, Layout layout)
	{
		switch (layout)
		{
		case InputLayout::Layout::POS_TEX_NOR:
		{
			D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			HRESULT hr = this->getDevice()->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc),
				vs.m_shaderBlob->GetBufferPointer(), vs.m_shaderBlob->GetBufferSize(), &m_inputLayout);
			assert(SUCCEEDED(hr));
			break;
		}
		case InputLayout::Layout::POS_TEX_NOR_TAN_BITAN:
		{
			D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			HRESULT hr = this->getDevice()->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc),
				vs.m_shaderBlob->GetBufferPointer(), vs.m_shaderBlob->GetBufferSize(), &m_inputLayout);
			assert(SUCCEEDED(hr));
			break;
		}
		case InputLayout::Layout::POS_TEX:
		{
			D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			HRESULT hr = this->getDevice()->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc),
				vs.m_shaderBlob->GetBufferPointer(), vs.m_shaderBlob->GetBufferSize(), &m_inputLayout);
			assert(SUCCEEDED(hr));
			break;
		}
		default:
			assert(false);
			break;
		}




	}

	void InputLayout::bind()
	{
		this->getContext()->IASetInputLayout(this->m_inputLayout.Get());
	}

	void InputLayout::unBind()
	{
		this->getContext()->IASetInputLayout(nullptr);
	}
}