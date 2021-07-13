#include "rfpch.hpp"
#include <imgui.h>

#include "SceneGraph.hpp"
#include "AssimpLoader.hpp"
#include "MeshStructures.hpp"
#include "Logger.hpp"
#include "EventSystem.hpp"
#include "PauseEvent.hpp"
#include "MouseEvent.hpp"
#include "AssetManager.hpp"


using namespace std;

namespace Rimfrost
{

	SceneGraph::~SceneGraph()
	{
	#ifdef DEBUG
		OutputDebugString(L"~SceneGraph\n");
	#endif // DEBUG
	}

	void SceneGraph::update(double dt)
	{
		updateWorldMatrices();

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

	std::vector<NodeID>& SceneGraph::getRenderSubmits()
	{
		return m_renderSubmits;
	}

	const std::vector<Node>& SceneGraph::getNodes() const
	{
		return m_nodes;
	}

	std::vector<Node>& SceneGraph::getNodes()
	{
		return m_nodes;
	}

	NodeHandle SceneGraph::addModel(const std::string& filePath, ModelSettings modelSettings)
	{
		return addModel(filePath, rootNode, modelSettings);
	}

	NodeHandle SceneGraph::addModel(const string& filePath, NodeID parentNodeID, ModelSettings modelSettings)
	{
		static uint32_t modelID = 0;

		//loaad model

		ModelID id = AssetManager::addModel(filePath, modelSettings);

		Model m = AssetManager::getModel(id);

		NodeID modelRoot = this->traverseSubMeshTree(m.subModelTree, m, parentNodeID);
		m_nodes[modelRoot].m_isModelParent = true;

		modelID++;

		return NodeHandle(*this, modelRoot);
	}

	NodeHandle SceneGraph::addModel(const std::string& filePath, const NodeHandle& parenthandle, ModelSettings modelSettings)
	{
		return addModel(filePath, parenthandle->m_ID, modelSettings);
	}

	NodeHandle SceneGraph::addNode(const Transform& offset, NodeID parentNodeID)
	{

		NodeID newNodeID = m_nodes.size();

		if (parentNodeID != rootNode)
			m_nodes[parentNodeID].m_childIDs.push_back(newNodeID);

		m_nodes.emplace_back(Node(newNodeID, parentNodeID, generateColdID()));
		m_nodes.back().localMatrix = offset;

		return NodeHandle(*this, newNodeID);
	}

	NodeHandle Rimfrost::SceneGraph::addNode(const Transform& offset, const NodeHandle& parenthandle)
	{
		return addNode(offset, parenthandle->m_ID);
	}

	void Rimfrost::SceneGraph::removeNode(NodeID id)
	{
		assert(id != rootNode);
		for (auto childID : m_nodes[id].m_childIDs)
		{
			removeNode(childID);
		}
		m_nodes[id] = Node(-1, -2, -1);
	}

	void SceneGraph::hideNode(NodeID id, bool isHidden)
	{
		assert(id != rootNode && id < m_nodes.size());
		for (auto childID : m_nodes[id].m_childIDs)
		{
			hideNode(childID, isHidden);
		}
		m_nodes[id].m_isHidden = isHidden;
	}

	void Rimfrost::SceneGraph::updateWorldMatrices()
	{
		for (auto& node : m_nodes)
		{
			if (node.m_parentID == rootNode)
			{
				updatedChildWorldMatrix(m_nodes, node.m_ID, Transform());
			}
		}
	}

	void Rimfrost::SceneGraph::updatedChildWorldMatrix(std::vector<Node>& nodes, NodeID ID, const Transform& parentMatrix)
	{
		nodes[ID].worldMatrix = parentMatrix * nodes[ID].localMatrix;
		for (auto& childID : nodes[ID].m_childIDs)
		{
			updatedChildWorldMatrix(nodes, childID, nodes[ID].worldMatrix);
		}
	}

	NodeID SceneGraph::generateColdID() const
	{
		NodeID id = 0;
		for (const auto& n : m_nodes)
		{
			assert(n.m_coldID != rootNode);
			if (n.m_coldID >= id) id = n.m_coldID + 1;
		}
		return id;
	}

	NodeID Rimfrost::SceneGraph::traverseSubMeshTree(SubMeshTree tree, const Model& model, NodeID nodeID)
	{
		//every recursion is a new node

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

	NodeID Rimfrost::SceneGraph::addSubModel(SubModel subModel, NodeID parentID)
	{
		assert(!m_nodes[parentID].m_subModel);
		NodeID newNodeID = m_nodes.size();

		m_nodes.emplace_back(Node(newNodeID, parentID, generateColdID()));
		m_nodes.back().m_subModel = std::make_optional<SubModel>(subModel);

		m_nodes[parentID].m_childIDs.push_back(newNodeID);
		return newNodeID;
	}

	NodeID Rimfrost::SceneGraph::addNode(NodeID parentNodeID)
	{
		NodeID newNodeID = m_nodes.size();

		if (parentNodeID != rootNode)
			m_nodes[parentNodeID].m_childIDs.push_back(newNodeID);

		m_nodes.emplace_back(Node(newNodeID, parentNodeID, generateColdID()));

		return newNodeID;
	}

	//remove deleted nodes from the graph and make it compact
	void Rimfrost::SceneGraph::packSceneGraph()
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

		//find first valid node after the invalid nodes
		for (int i = firstInvalidIndex; i < m_nodes.size(); i++)
		{

			if (m_nodes[i].m_ID != -1)
			{
				assert(i == m_nodes[i].m_ID);
				m_nodes[firstInvalidIndex] = m_nodes[i];
				m_nodes[firstInvalidIndex].m_ID = firstInvalidIndex;

				m_nodes[i] = Node(-1, -2, -1); //set to invalid Node, may do something not this expensive

				for (auto childID : m_nodes[firstInvalidIndex].m_childIDs)
				{
					m_nodes[childID].m_parentID = firstInvalidIndex;
				}
				firstInvalidIndex++;
			}
		}

		m_nodes.resize(firstInvalidIndex, Node(-1, -2, -1));

	}
}