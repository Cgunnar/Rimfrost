#pragma once
#include <queue>

#include "Engine1.hpp"
#include "Node.hpp"
#include "Timer.hpp"
#include "Camera.hpp"
#include "DXAccess.hpp"
#include "RenderPass.hpp"
#include "Rimfrost\Scene\IScene.hpp"
#include "Rimfrost\EventSystem\EventObserver.hpp"

namespace Rimfrost
{
	

	class ForwardRenderer : DXAccess, public EventObserver
	{
	public:
		ForwardRenderer();
		~ForwardRenderer();

		void renderScene(IScene& scene);
		void onEvent(const Event& e) override;

	private:
		std::queue<int> sortRenderSubmits(const Camera& camera, const std::vector<Rimfrost::Node>& nodes, std::vector<Rimfrost::NodeID>& submits);
	private:
		Timer m_timer;
		PerFrameData m_frameData;

		struct RenderPasses
		{
			RenderPasses() { m_passes = { &forwardPhongPass, &transparencyPass, &mousePickingPass }; }
			RenderPass forwardPhongPass;
			RenderPass transparencyPass;
			RenderPass mousePickingPass;

			inline void reset()
			{
				m_passes[m_counter]->unBind();
				m_counter = 0;
				m_passes[m_counter]->bind();
			}
			inline void next()
			{
				assert(static_cast<size_t>(m_counter) + 1 < m_passes.size());
				m_passes[m_counter]->unBind();
				m_passes[++m_counter]->bind();
			}

		private:
			std::vector<RenderPass*> m_passes;
			int m_counter = 0;

			
		} m_renderPasses;


		D3D11_SUBRESOURCE_DATA m_subRes = {};
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_cameraCBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_worldMatrixCBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_subModelMiscBuffer;

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_defaultRZ;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_noBackFaceCullRZ;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireFrameRZ;

		struct alignas(16) PerSubModelData
		{
			int id = -1;
		}m_subModelCBuffer;
		
		
		//for the second pass;
		ShaderID m_p2vertexShader;
		ShaderID m_p2pixelShader;

		std::unique_ptr<VertexBuffer> m_quadVertexBuffer;
		std::unique_ptr<InputLayout> m_p2inputLayout;

	};
}

