#include "rfpch.hpp"
#include "ForwardRenderer.hpp"
#include "RimfrostMath.hpp"
#include "Scene.hpp"
#include "Geometry.hpp"
#include "EventSystem.hpp"
#include "MouseEvent.hpp"
#include "FrameBufferRepo.hpp"
#include "Sampler.hpp"
#include "Engine1.hpp"

using namespace DirectX;
using namespace std;
namespace Rimfrost
{

	struct MousePickingStruct
	{
		bool picked;
		int id;
		XMFLOAT3 worldPosition;
	};

	ForwardRenderer::ForwardRenderer()
	{
		m_timer = Timer(Duration::MILLISECONDS);

		D3D11_BUFFER_DESC cBufferDesc = {};
		cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		cBufferDesc.StructureByteStride = 0;
		cBufferDesc.MiscFlags = 0;
		cBufferDesc.ByteWidth = sizeof(PerFrameData);

		HRESULT hr = this->getDevice()->CreateBuffer(&cBufferDesc, nullptr, &m_cameraCBuffer);
		assert(SUCCEEDED(hr));

		cBufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);

		hr = this->getDevice()->CreateBuffer(&cBufferDesc, nullptr, &m_worldMatrixCBuffer);
		assert(SUCCEEDED(hr));

		cBufferDesc.ByteWidth = sizeof(PerSubModelData);

		hr = this->getDevice()->CreateBuffer(&cBufferDesc, nullptr, &m_subModelMiscBuffer);
		assert(SUCCEEDED(hr));


		//create blendstate
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.IndependentBlendEnable = TRUE;
		auto& b = blendDesc.RenderTarget[0];

		b.BlendEnable = TRUE;
		b.BlendOp = D3D11_BLEND_OP_ADD;
		b.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		b.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		b.DestBlendAlpha = D3D11_BLEND_ZERO;
		b.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		b.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		b.SrcBlendAlpha = D3D11_BLEND_ZERO;

		//fix default for rest

		blendDesc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[2].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[3].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[4].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[5].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[6].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[7].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		


			//create rasterizers
		D3D11_RASTERIZER_DESC rzDesc = {};
		rzDesc.FillMode = D3D11_FILL_SOLID;
		rzDesc.CullMode = D3D11_CULL_BACK;

		hr = this->getDevice()->CreateRasterizerState(&rzDesc, &m_defaultRZ);
		assert(SUCCEEDED(hr));

		rzDesc.CullMode = D3D11_CULL_NONE;
		hr = this->getDevice()->CreateRasterizerState(&rzDesc, &m_noBackFaceCullRZ);
		assert(SUCCEEDED(hr));

		rzDesc.FillMode = D3D11_FILL_WIREFRAME;
		hr = this->getDevice()->CreateRasterizerState(&rzDesc, &m_wireFrameRZ);
		assert(SUCCEEDED(hr));


		D3D11_TEXTURE2D_DESC pos = { };
		pos.Width = 1280;
		pos.Height = 720;
		pos.MipLevels = 1;
		pos.ArraySize = 1;
		pos.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		pos.SampleDesc.Count = 1;
		pos.SampleDesc.Quality = 0;
		pos.Usage = D3D11_USAGE_DEFAULT;
		pos.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		pos.CPUAccessFlags = 0;
		pos.MiscFlags = 0;

		D3D11_TEXTURE2D_DESC col = { };
		col.Width = 1280;
		col.Height = 720;
		col.MipLevels = 1;
		col.ArraySize = 1;
		col.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		col.SampleDesc.Count = 1;
		col.SampleDesc.Quality = 0;
		col.Usage = D3D11_USAGE_DEFAULT;
		col.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		col.CPUAccessFlags = 0;
		col.MiscFlags = 0;

		D3D11_TEXTURE2D_DESC idTex = { };
		idTex.Width = 1280;
		idTex.Height = 720;
		idTex.MipLevels = 1;
		idTex.ArraySize = 1;
		idTex.Format = DXGI_FORMAT_R32_SINT;
		idTex.SampleDesc.Count = 1;
		idTex.SampleDesc.Quality = 0;
		idTex.Usage = D3D11_USAGE_DEFAULT;
		idTex.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		idTex.CPUAccessFlags = 0;
		idTex.MiscFlags = 0;

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		getBackBuffer()->GetDesc(&depthStencilDesc);
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;


		FrameBufferDescriptor positionGBuffer(pos, "g_pos");
		FrameBufferDescriptor depthStencilBuffer(depthStencilDesc, "g_dsv");
		FrameBufferDescriptor nodeIDsForPicking(idTex, "nodeIDs");

		getFrameBufferRepo().createBuffer(positionGBuffer);
		getFrameBufferRepo().createBuffer(depthStencilBuffer);
		getFrameBufferRepo().createBuffer(nodeIDsForPicking);

		m_renderPasses.forwardPhongPass.AttachFrameBufferViews({ View("backBuffer", ViewType::RTV), View("g_pos", ViewType::RTV),
			View("g_dsv", ViewType::DSV), View("nodeIDs", ViewType::RTV) });

		m_renderPasses.forwardPhongPass.acquireResources();

		m_renderPasses.forwardPhongPass.addSamplers({ standardSamplers::linear_wrap }, 0, ShaderEnum::PIXELSHADER);

		//trnasparent only differs in blendstate
		m_renderPasses.transparencyPass = m_renderPasses.forwardPhongPass;
		m_renderPasses.transparencyPass.addBlendState(blendDesc);

		blendDesc.AlphaToCoverageEnable = TRUE;
		blendDesc.RenderTarget[0].BlendEnable = FALSE;

		m_renderPasses.forwardPhongPass.addBlendState(blendDesc);


		/*this->m_bufferDesc.ByteWidth = structCount * structSize;
		this->m_bufferDesc.StructureByteStride = structSize;
		this->m_bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		this->m_bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		this->m_bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		this->m_bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		getBufferRepo().createBuffer()*/


		//second pass setup
		m_p2vertexShader = ShaderRepo::addShader("../Rimfrost/src/Rimfrost/Shaders/QuadPassVS.hlsl", ShaderEnum::VERTEXSHADER);
		m_p2pixelShader = ShaderRepo::addShader("../Rimfrost/src/Rimfrost/Shaders/MousePickingPS.hlsl", ShaderEnum::PIXELSHADER);

		m_quadVertexBuffer = std::make_unique<VertexBuffer>();
		Geometry::Quad quad = {};
		m_quadVertexBuffer->createBuffer(quad.data, quad.arraySize, quad.vertexStride);
		m_p2inputLayout = std::make_unique<InputLayout>();
		m_p2inputLayout->init(reinterpret_cast<VertexShader&>(ShaderRepo::getShader(m_p2vertexShader)), quad.layout);


		D3D11_BUFFER_DESC mpDesc = {};
		mpDesc.ByteWidth = sizeof(MousePickingStruct);
		mpDesc.StructureByteStride = sizeof(MousePickingStruct);
		mpDesc.Usage = D3D11_USAGE_DEFAULT;
		mpDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		mpDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		mpDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;

		D3D11_TEXTURE2D_DESC backBufferTtv = { };
		backBufferTtv.Width = 1280;
		backBufferTtv.Height = 720;
		backBufferTtv.MipLevels = 1;
		backBufferTtv.ArraySize = 1;
		backBufferTtv.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		backBufferTtv.SampleDesc.Count = 1;
		backBufferTtv.SampleDesc.Quality = 0;
		backBufferTtv.Usage = D3D11_USAGE_DEFAULT;
		backBufferTtv.BindFlags = D3D11_BIND_RENDER_TARGET;
		backBufferTtv.CPUAccessFlags = 0;
		backBufferTtv.MiscFlags = 0;


		BufferDescriptor mousePickReturnFromGPU(mpDesc, 0, "UAVNodeID");
		getBufferRepo().createBuffer(mousePickReturnFromGPU);

		m_renderPasses.mousePickingPass.AttachFrameBufferViews({ View("g_pos", ViewType::SRV), View("nodeIDs", ViewType::SRV) });

		m_renderPasses.mousePickingPass.AttachBufferViews({ View("UAVNodeID", ViewType::UAV) });
		m_renderPasses.mousePickingPass.acquireResources();

		m_renderPasses.mousePickingPass.addSamplers({ standardSamplers::linear_wrap }, 0, ShaderEnum::PIXELSHADER);
	}

	ForwardRenderer::~ForwardRenderer()
	{
		OutputDebugString(L"~ForwardRenderer\n");
	}

	uint32_t ForwardRenderer::renderScene(const std::shared_ptr<Scene>& scene)
	{
		m_timer.start();

		auto& nodes = scene->getNodes();
		auto& renderSubmits = scene->getRenderSubmits();
		auto& camera = scene->getCamera();


		D3D11_MAPPED_SUBRESOURCE mousePickResource = {};
		MousePickingStruct myPicking = {};

		auto& mousePickingUAVBuffer = getBufferRepo().getBuffer("UAVNodeID")->buffer;
		this->getContext()->Map(mousePickingUAVBuffer.Get(), 0, D3D11_MAP_READ, 0, &mousePickResource);
		memcpy(&myPicking, mousePickResource.pData, sizeof(MousePickingStruct));
		this->getContext()->Unmap(mousePickingUAVBuffer.Get(), 0);

		m_renderPasses.forwardPhongPass.clearRenderTargets();
		m_renderPasses.forwardPhongPass.clearDepthStencil();
		m_renderPasses.transparencyPass.clearRenderTargets();
		m_renderPasses.transparencyPass.clearDepthStencil();
		//m_renderPasses.forwardPhongPass.bind();

		m_renderPasses.mousePickingPass.unBind();
		m_renderPasses.mousePickingPass.clearUnorderdAccessViews();

		if (myPicking.picked)
		{
			Logger::getLogger().debugLog("mousePicking: id1: " + std::to_string(myPicking.id) + "\n");
			Logger::getLogger().debugLog(myPicking.worldPosition, "pos ");
			if (nodes[myPicking.id].m_subModel)
				Logger::getLogger().debugLog(nodes[myPicking.id].m_subModel->getMaterialName() + "\n");

			EventSystem::postInstantly(MousePickingEvent(myPicking.id, myPicking.worldPosition));
		}



		scene->getPointLights().getStructuredLightBuffer()->bind(4);

		

		auto indexToChangeRenderPass = sortRenderSubmits(*camera, nodes, renderSubmits);

		D3D11_MAPPED_SUBRESOURCE mappedSubRes;

		this->getContext()->Map(m_cameraCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubRes);
		memcpy(mappedSubRes.pData, &scene->getFrameCBufferData(), sizeof(PerFrameData));
		this->getContext()->Unmap(m_cameraCBuffer.Get(), 0);


		this->getContext()->VSSetConstantBuffers(5, 1, m_cameraCBuffer.GetAddressOf());
		this->getContext()->PSSetConstantBuffers(5, 1, m_cameraCBuffer.GetAddressOf());


		m_renderPasses.reset();
		for (int i = 0; i < renderSubmits.size(); i++)
		{
			auto& r = renderSubmits[i];

			if (nodes[r].m_ID == -1) continue;
			if (!indexToChangeRenderPass.empty() && indexToChangeRenderPass.front() == i)
			{
				m_renderPasses.next();
				indexToChangeRenderPass.pop();
			}

			D3D11_MAPPED_SUBRESOURCE mappedSubRes;
			this->getContext()->Map(m_worldMatrixCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubRes);
			memcpy(mappedSubRes.pData, &nodes[r].worldMatrix, sizeof(Matrix)); //gpu matrix
			this->getContext()->Unmap(m_worldMatrixCBuffer.Get(), 0);

			m_subModelCBuffer.id = (int)r;
			this->getContext()->Map(m_subModelMiscBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubRes);
			memcpy(mappedSubRes.pData, &m_subModelCBuffer, sizeof(PerSubModelData));
			this->getContext()->Unmap(m_subModelMiscBuffer.Get(), 0);

			this->getContext()->VSSetConstantBuffers(1, 1, m_worldMatrixCBuffer.GetAddressOf());
			this->getContext()->PSSetConstantBuffers(1, 1, m_worldMatrixCBuffer.GetAddressOf());
			this->getContext()->PSSetConstantBuffers(2, 1, m_subModelMiscBuffer.GetAddressOf());

			auto& subModel = nodes[r].m_subModel;

			subModel->bind();

			//set rasterizer
			if ((subModel->getMaterialType() & Rimfrost::MaterialProperties::WIREFRAME) == Rimfrost::MaterialProperties::WIREFRAME)
			{
				this->getContext()->RSSetState(m_wireFrameRZ.Get());
			}
			else if ((subModel->getMaterialType() & Rimfrost::MaterialProperties::NO_BACKFACE_CULLING) == Rimfrost::MaterialProperties::NO_BACKFACE_CULLING)
			{
				this->getContext()->RSSetState(m_noBackFaceCullRZ.Get());
			}
			else
			{
				this->getContext()->RSSetState(m_defaultRZ.Get());
			}


			this->getContext()->DrawIndexed(subModel->getIndexCount(), subModel->getStartIndexLocation(), subModel->getBaseVertexLocation());
		}


		//m_renderPasses.forwardPhongPass.unBind();

		//-------------------------------------
		//second render pass
	/*	this->getContext()->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFFu);*/
		this->getContext()->RSSetState(m_defaultRZ.Get());

		//m_renderPasses.mousePickingPass.clearRenderTargets();
		//m_renderPasses.mousePickingPass.bind();

		m_renderPasses.next();

		m_p2inputLayout->bind();
		m_quadVertexBuffer->bind();
		this->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		ShaderRepo::getShader(m_p2vertexShader).bind();
		ShaderRepo::getShader(m_p2pixelShader).bind();


		this->getContext()->Draw(m_quadVertexBuffer->getVertexCount(), 0);



		return m_timer.stop();
	}

	std::queue<int> ForwardRenderer::sortRenderSubmits(const Camera& camera, const std::vector<Node>& nodes, std::vector<NodeID>& submits)
	{
		Vector3 forward = camera.GetAxisZ();

		auto&& frontToBack = [&nodes, &forward](NodeID a, NodeID b)
		{
			const auto& posA = nodes[a].worldMatrix.getTranslation();
			const auto& posB = nodes[b].worldMatrix.getTranslation();
			//dot(cameraForward, meshPosition)
			return forward.x * posA.x + forward.y * posA.y + forward.z * posA.z < forward.x* posB.x + forward.y * posB.y + forward.z * posB.z;
		};
		auto&& backToFront = [&nodes, &forward](NodeID a, NodeID b)
		{
			const auto& posA = nodes[a].worldMatrix.getTranslation();
			const auto& posB = nodes[b].worldMatrix.getTranslation();
			//dot(cameraForward, meshPosition)
			return forward.x * posA.x + forward.y * posA.y + forward.z * posA.z > forward.x * posB.x + forward.y * posB.y + forward.z * posB.z;
		};


		auto&& transparency = [&nodes](NodeID a, NodeID b)
		{
			const auto& posA = nodes[a].worldMatrix.getTranslation();
			const auto& posB = nodes[b].worldMatrix.getTranslation();

			//dot(cameraForward, meshPosition)


			int isTransparentA = static_cast<int>((nodes[a].m_subModel->getMaterialType() & MaterialProperties::ALPHA_BLENDING));
			int isTransparentB = static_cast<int>((nodes[b].m_subModel->getMaterialType() & MaterialProperties::ALPHA_BLENDING));

			return isTransparentA < isTransparentB;
		};

		auto&& findTransparent = [&nodes](NodeID n)
		{
			return (nodes[n].m_subModel->getMaterialType() & MaterialProperties::ALPHA_BLENDING) == MaterialProperties::ALPHA_BLENDING;
		};

		std::sort(submits.begin(), submits.end(), transparency);

		auto firstTransparentIter = std::ranges::find_if(submits.begin(), submits.end(), [&nodes](NodeID n) {
			return (nodes[n].m_subModel->getMaterialType() & MaterialProperties::ALPHA_BLENDING)
				== MaterialProperties::ALPHA_BLENDING; });

		std::sort(firstTransparentIter, submits.end(), backToFront);
		std::sort(submits.begin(), firstTransparentIter, frontToBack);
		std::queue<int> q;
		q.push(static_cast<int>(firstTransparentIter - submits.begin()));
		return q;
	}
}