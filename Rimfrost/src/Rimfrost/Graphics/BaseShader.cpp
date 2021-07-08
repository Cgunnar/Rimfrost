#include "rfpch.hpp"
#include "BaseShader.hpp"
namespace Rimfrost
{
	BaseShader::BaseShader(Rimfrost::ShaderEnum shaderType) : m_shaderType(shaderType)
	{
	}

	BaseShader::~BaseShader()
	{
	}

	ID3DBlob* BaseShader::internalCompile()
	{
		UINT sFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		sFlags |= D3DCOMPILE_DEBUG;
#endif

		std::wstringstream wFileNameStream;
		wFileNameStream << m_filePath.c_str();
		std::wstring wFileName = wFileNameStream.str();

		ID3DBlob* shaderBlobPtr = nullptr;
		ID3DBlob* errorBlobPtr = nullptr;
		HRESULT	hr = D3DCompileFromFile
		(
			wFileName.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			m_entryPoint.c_str(),
			m_shaderModel.c_str(),
			sFlags,
			0,
			&shaderBlobPtr,
			&errorBlobPtr
		);
		if (FAILED(hr))
		{
			if (errorBlobPtr)
			{
				OutputDebugStringA((char*)errorBlobPtr->GetBufferPointer());
				errorBlobPtr->Release();
			}
			if (shaderBlobPtr)
			{
				shaderBlobPtr->Release();
			}
			assert(false);
		}
		return shaderBlobPtr;
	}
}