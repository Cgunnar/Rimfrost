#include "rfpch.hpp"
#include "Scene.hpp"
#include "AssimpLoader.hpp"
#include "MeshStructures.hpp"
#include "Logger.hpp"
#include <imgui.h>
#include "EventSystem.hpp"
#include "PauseEvent.hpp"
#include "MouseEvent.hpp"
#include "AssetManager.hpp"


using namespace std;

namespace Rimfrost
{


	Scene::Scene() : m_keyboard(nullptr), m_mouse(nullptr), m_pointLightContainer(nullptr), m_isPaused(false)
	{
		EventSystem::addObserver(*this, PauseEvent::eventType);
		EventSystem::addObserver(*this, MouseButtonsEvent::eventType);
		m_frameData = {};
		m_frameData.mouseX = -1;
		m_frameData.mouseY = -1;
	}

	Scene::~Scene()
	{
		OutputDebugString(L"~Scene\n");
	}

	void Scene::update(double dt)
	{
		derivedSceneUpdate(dt);



		if (!m_isPaused)
		{
			static float ang = 0;
			ang += (float)dt;
			m_camera.update(static_cast<float>(dt), m_keyboard, m_mouse);
		}

		updateWorldMatrices();

		m_frameData.v = m_camera.GetViewMatrix();
		m_frameData.p = m_camera.GetPerspective();
		m_frameData.pos = m_camera.GetPosition();

		size_t prevSize = m_renderSubmits.size();
		m_renderSubmits.clear();
		m_renderSubmits.reserve(prevSize);

		for (auto& n : m_nodes)
		{
			if (n.m_subModel && !n.m_isHidden)
			{
				m_renderSubmits.push_back(n.m_ID);
			}
		}
	}

	void Scene::bindInput(const std::shared_ptr<Rimfrost::Keyboard>& keyboard, const std::shared_ptr<Rimfrost::Mouse>& mouse)
	{
		m_keyboard = keyboard;
		m_mouse = mouse;
	}

	PointLightContainer& Scene::getPointLights()
	{
		return *m_pointLightContainer;
	}

	std::vector<NodeID>& Scene::getRenderSubmits()
	{
		return m_renderSubmits;
	}

	const std::vector<Node>& Scene::getNodes() const
	{
		return m_nodes;
	}

	const Camera& Scene::getCamera() const noexcept
	{
		return m_camera;
	}

	const PerFrameData& Scene::getFrameCBufferData() const noexcept
	{
		return m_frameData;
	}

	NodeHandle Scene::addModel(const std::string& filePath, ModelSettings modelSettings)
	{
		return addModel(filePath, rootNode, modelSettings);
	}

	NodeHandle Scene::addModel(const string& filePath, NodeID parentNodeID, ModelSettings modelSettings)
	{
		static uint32_t modelID = 0;

		//loaad model
		/*AssimpLoader loader;
		auto meshData = loader.loadStaticModel(filePath);*/

		ModelID id = AssetManager::addModel(filePath, modelSettings);

		Model m = AssetManager::getModel(id);


		//model that the submodel belongs to
		/*SubModel::ModelData sharedModelData;
		sharedModelData.m_filePath = filePath;
		sharedModelData.m_settings = modelSettings;
		sharedModelData.m_vertexBuffer = std::make_shared<VertexBuffer>();
		sharedModelData.m_indexBuffer = std::make_shared<IndexBuffer>();

		sharedModelData.m_vertexBuffer->createBuffer(meshData.getVertextBuffer(), meshData.getVertexCount() * meshData.getVertexSize(), static_cast<uint32_t>(meshData.getVertexSize()));
		sharedModelData.m_indexBuffer->initialize((uint32_t*)meshData.getIndicesData(), static_cast<uint32_t>(meshData.getIndicesCount()));

		if (meshData.hasNormalMaps())
			sharedModelData.layout = InputLayout::Layout::POS_TEX_NOR_TAN_BITAN;
		else
			sharedModelData.layout = InputLayout::Layout::POS_TEX_NOR;*/

			//NodeID modelRoot = addNode(parentNodeID);
		NodeID modelRoot = this->traverseSubMeshTree(m.subModelTree, m, parentNodeID);
		m_nodes[modelRoot].m_isModelParent = true;

		modelID++;



		return NodeHandle(m_nodes, modelRoot);
	}

	NodeHandle Scene::addModel(const std::string& filePath, const NodeHandle& parenthandle, ModelSettings modelSettings)
	{
		return addModel(filePath, parenthandle->m_ID, modelSettings);
	}



	NodeHandle Scene::addNode(const Transform& offset, NodeID parentNodeID)
	{

		NodeID newNodeID = m_nodes.size();

		if (parentNodeID != rootNode)
			m_nodes[parentNodeID].m_childIDs.push_back(newNodeID);

		m_nodes.emplace_back(Node(newNodeID, parentNodeID));
		m_nodes.back().localMatrix = offset;

		return NodeHandle(m_nodes, newNodeID);
	}

	NodeHandle Rimfrost::Scene::addNode(const Transform& offset, const NodeHandle& parenthandle)
	{
		return addNode(offset, parenthandle->m_ID);
	}

	void Rimfrost::Scene::removeNode(NodeID id)
	{
		assert(id != rootNode);
		for (auto childID : m_nodes[id].m_childIDs)
		{
			removeNode(childID);
		}
		m_nodes[id] = Node(-1, -2);
	}

	void Scene::hideNode(NodeID id, bool isHidden)
	{
		assert(id != rootNode && id < m_nodes.size());
		for (auto childID : m_nodes[id].m_childIDs)
		{
			hideNode(childID, isHidden);
		}
		m_nodes[id].m_isHidden = isHidden;
	}

	void Rimfrost::Scene::updateWorldMatrices()
	{
		for (auto& node : m_nodes)
		{
			if (node.m_parentID == rootNode)
			{
				updatedChildWorldMatrix(m_nodes, node.m_ID, Transform());
			}
		}
	}

	void Rimfrost::Scene::updatedChildWorldMatrix(std::vector<Node>& nodes, NodeID ID, const Transform& parentMatrix)
	{
		nodes[ID].worldMatrix = parentMatrix * nodes[ID].localMatrix;
		//nodes[ID].worldMatrix = nodes[ID].localMatrix * parentMatrix;
		for (auto& childID : nodes[ID].m_childIDs)
		{
			updatedChildWorldMatrix(nodes, childID, nodes[ID].worldMatrix);
		}
	}

	NodeID Rimfrost::Scene::traverseSubMeshTree(SubMeshTree tree, const Model& model, NodeID nodeID)
	{
		//every recursion is a new node

		/*for (auto& mesh : tree.subMeshes)
		{
			this->addSubModel(SubModel(mesh, model.commonData), nodeID);
		}*/

		for (auto& meshIndex : tree.subMeshesIndex)
		{
			this->addSubModel(SubModel(model.subModelData[meshIndex], model.commonData, static_cast<SubModelID>(meshIndex)), nodeID);
		}
		if (!tree.nodes.empty())
		{

			NodeID nextNode = addNode(nodeID);

			for (auto& node : tree.nodes)
			{
				//NodeID nextNode = addNode(nodeID);
				this->traverseSubMeshTree(node, model, nextNode);
			}
			return nextNode;
		}
		else
		{
			return -1;
		}

	}

	NodeID Rimfrost::Scene::addSubModel(SubModel subModel, NodeID parentID)
	{
		assert(!m_nodes[parentID].m_subModel);
		NodeID newNodeID = m_nodes.size();

		m_nodes.emplace_back(Node(newNodeID, parentID));
		m_nodes.back().m_subModel = std::make_optional<SubModel>(subModel);

		m_nodes[parentID].m_childIDs.push_back(newNodeID);
		return newNodeID;
	}

	NodeID Rimfrost::Scene::addNode(NodeID parentNodeID)
	{
		NodeID newNodeID = m_nodes.size();

		if (parentNodeID != rootNode)
			m_nodes[parentNodeID].m_childIDs.push_back(newNodeID);

		m_nodes.emplace_back(Node(newNodeID, parentNodeID));

		return newNodeID;
	}

	//remove deleted nodes from the graph and make it compact
	void Rimfrost::Scene::packSceneGraph()
	{
		//find first invalid node
		int firstInvalidIndex = -1;
		for (int i = 0; i < m_nodes.size(); i++)
		{
			if (m_nodes[i].m_ID == -1 && firstInvalidIndex == -1)
			{
				firstInvalidIndex = i;
				break;
			}
		}



		//if no node was invalid
		if (firstInvalidIndex == -1) return;

		/*auto l = [](Node n) { return n.m_ID == -1; };
		auto beginItr = find_if(m_nodes.begin(), m_nodes.end(), l);
		auto invalidNodeItr = beginItr;
		while (invalidNodeItr != m_nodes.end())
		{
			invalidNodeItr = find_if(next(beginItr, 1), m_nodes.end(), l);

		}*/

		//find first valid node after the invalid nodes
		for (int i = firstInvalidIndex; i < m_nodes.size(); i++)
		{

			if (m_nodes[i].m_ID != -1)
			{
				assert(i == m_nodes[i].m_ID);
				m_nodes[firstInvalidIndex] = m_nodes[i];
				m_nodes[firstInvalidIndex].m_ID = firstInvalidIndex;

				m_nodes[i] = Node(-1, -2); //set to invalid Node, may do something not this expensive

				for (auto childID : m_nodes[firstInvalidIndex].m_childIDs)
				{
					m_nodes[childID].m_parentID = firstInvalidIndex;
				}
				firstInvalidIndex++;
			}
		}

		m_nodes.resize(firstInvalidIndex, Node(-1, -2));

	}

	void Rimfrost::Scene::onEvent(const Event& e)
	{
		if (e.type() == PauseEvent::eventType)
		{
			Logger::getLogger().debugLog("event\n");
			Logger::getLogger().debugLog((std::string)static_cast<const PauseEvent&>(e).type() + "\n");

			this->m_isPaused = static_cast<const PauseEvent&>(e).m_isPaused;
			Logger::getLogger().debugLog((std::to_string(m_isPaused) + "\n"));
		}
		if (e.type() == MouseButtonsEvent::eventType)
		{
			auto& mouse = static_cast<const MouseButtonsEvent&>(e).mouseState;
			if (mouse.RMBClicked)
			{
				m_frameData.mouseX = mouse.x;
				m_frameData.mouseY = mouse.y;
				//Logger::getLogger().debugLog("mouseX: " + std::to_string(mouse.x) + "\n");
			}
			else
			{
				m_frameData.mouseX = -1;
				m_frameData.mouseY = -1;
			}
		}
		derivedOnEvent(e);
	}

	//Scene::NodeHandle::NodeHandle(Scene& scene, NodeID nodeID) : m_sceneRef(scene), m_nodeID(nodeID)
	//{
	//}
	//
	//Node& Scene::NodeHandle::operator*() const
	//{
	//	assert(m_nodeID != rootNode);
	//	return m_sceneRef.get().m_nodes[m_nodeID];
	//}
	//
	//Node* Rimfrost::Scene::NodeHandle::operator->() const
	//{
	//	assert(m_nodeID != rootNode);
	//	return &m_sceneRef.get().m_nodes[m_nodeID];
	//}
	//
	//bool Rimfrost::Scene::NodeHandle::isValid() const
	//{
	//	return m_nodeID != rootNode;
	//}
}