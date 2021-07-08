#pragma	once
#include <wrl.h>
#include <d3d11.h>

namespace Rimfrost
{

	class Graphics;
	class FrameBufferRepo;
	class BufferRepo;

	class DXAccess
	{
		friend class Graphics;
	public:

	protected:
		static Microsoft::WRL::ComPtr<ID3D11Device>& getDevice();
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext>& getContext();
		static Microsoft::WRL::ComPtr<ID3D11Texture2D> getBackBuffer();
		static std::tuple<uint32_t, uint32_t> getBackBufferResoltion();
		static FrameBufferRepo& getFrameBufferRepo();
		static BufferRepo& getBufferRepo();
	private:
		static Graphics* s_graphics;

	};

}