#include "rfpch.hpp"
#include "DXAccess.hpp"
#include "Graphics.hpp"

namespace Rimfrost
{

    Graphics* DXAccess::s_graphics = nullptr;

    Microsoft::WRL::ComPtr<ID3D11Device>& DXAccess::getDevice()
    {
        assert(DXAccess::s_graphics != nullptr);
        return DXAccess::s_graphics->GetDevice();
    }

    Microsoft::WRL::ComPtr<ID3D11DeviceContext>& DXAccess::getContext()
    {
        assert(DXAccess::s_graphics != nullptr);
        return DXAccess::s_graphics->GetDeviceContext();
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> DXAccess::getBackBuffer()
    {
        return s_graphics->getBackBuffer();
    }

    std::tuple<uint32_t, uint32_t> DXAccess::getBackBufferResoltion()
    {
        return s_graphics->getBackBufferResoltion();
    }

    FrameBufferRepo& DXAccess::getFrameBufferRepo()
    {
        return s_graphics->m_frameBuffers;
    }
    BufferRepo& DXAccess::getBufferRepo()
    {
        return s_graphics->m_buffers;
    }
}