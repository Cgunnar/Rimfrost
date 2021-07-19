#include "rfpch.hpp"
#include "Node.hpp"
#include "SceneGraph.hpp"

namespace Rimfrost
{

    Node::Node(NodeID ID, NodeID parentID, NodeID coldID, bool modelParent) : m_ID(ID), m_parentID(parentID),
        m_coldID(coldID), m_isModelParent(modelParent)
    {
    }

    std::optional<SubModel>& Node::operator->()
    {
        return m_subModel;
    }

    std::optional<SubModel>& Node::operator*()
    {
        return m_subModel;
    }



    NodeHandle::NodeHandle(SceneGraph& sceneGraph, NodeID nodeID, NodeID coldID) : m_sceneRef(sceneGraph), m_nodeID(nodeID), m_coldNodeID(coldID)
    {
    }

    NodeHandle::NodeHandle(SceneGraph& sceneGraph, Node node) : m_sceneRef(sceneGraph), m_nodeID(node.m_ID), m_coldNodeID(node.m_coldID)
    {
    }

    NodeHandle::NodeHandle() : m_sceneRef(std::nullopt), m_nodeID(-1), m_coldNodeID(-1)
    {
    }

    Node& NodeHandle::operator*() const
    {
        assert(m_nodeID != rootNode);
        return m_sceneRef->get().getNodes()[m_nodeID];
    }

    Node* NodeHandle::operator->() const
    {
        assert(m_nodeID != rootNode);
        return &m_sceneRef->get().getNodes()[m_nodeID];
    }

    bool NodeHandle::isValid() const
    {
        return (m_nodeID != rootNode) && m_sceneRef;
    }
    Node& NodeHandle::get() const
    {
        assert(m_nodeID != rootNode);
        return m_sceneRef->get().getNodes()[m_nodeID];
    }
    void NodeHandle::invalidateHandle()
    {
        m_nodeID = -1;
    }
    Transform NodeHandle::getParentWorldMatrix() const
    {
        Transform M;
        NodeID parentID = this->get().m_parentID;
        if (parentID != rootNode)
        {
            M = m_sceneRef->get().getNodes()[parentID].worldMatrix;
        }
        return M;
    }
}