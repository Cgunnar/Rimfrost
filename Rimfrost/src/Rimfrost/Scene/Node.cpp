#include "rfpch.hpp"
#include "Node.hpp"
#include "SceneGraph.hpp"

namespace Rimfrost
{

    Node::Node(NodeID ID, NodeID parentID, NodeID coldID, bool modelParent) : m_ID(ID), m_parentID(parentID),
        m_coldID(coldID), m_isModelParent(modelParent)
    {
    }

    NodeID Node::getColdID() const
    {
        return m_coldID;
    }

    NodeID Node::getID() const
    {
        return m_ID;
    }

    NodeID Node::getParentID() const
    {
        return m_parentID;
    }

    const std::vector<NodeID>& Node::getChildIDs() const
    {
        return m_childIDs;
    }

    bool Node::isParentToModel() const
    {
        return m_isModelParent;
    }

    std::optional<SubModel>& Node::operator->()
    {
        return m_subModel;
    }

    std::optional<SubModel>& Node::operator*()
    {
        return m_subModel;
    }



    NodeHandle::NodeHandle(SceneGraph& sceneGraph, NodeID nodeID, NodeID coldID) : m_sceneRef(&sceneGraph), m_nodeID(nodeID), m_coldNodeID(coldID)
    {
    }

    NodeHandle::NodeHandle(SceneGraph& sceneGraph, Node node) : m_sceneRef(&sceneGraph), m_nodeID(node.m_ID), m_coldNodeID(node.m_coldID)
    {
    }

    NodeHandle::NodeHandle() : m_sceneRef(nullptr), m_nodeID(-1), m_coldNodeID(-1)
    {
    }

    Node& NodeHandle::operator*() const
    {
        assert(m_nodeID != rootNode);
        return m_sceneRef->getNodes()[m_nodeID];
    }

    Node* NodeHandle::operator->() const
    {
        assert(m_nodeID != rootNode);
        return &m_sceneRef->getNodes()[m_nodeID];
    }

    bool NodeHandle::isValid() const
    {
        return (m_nodeID != rootNode) && m_sceneRef;
    }
    Node& NodeHandle::get() const
    {
        assert(m_nodeID != rootNode);
        return m_sceneRef->getNodes()[m_nodeID];
    }
    void NodeHandle::invalidateHandle()
    {
        m_nodeID = -1;
        m_sceneRef = nullptr;
    }
    void NodeHandle::removeNode()
    {
        assert(m_sceneRef);
        m_sceneRef->removeNode(m_nodeID);
        invalidateHandle();
    }
    Transform NodeHandle::getParentWorldMatrix() const
    {
        Transform M;
        NodeID parentID = this->get().m_parentID;
        if (parentID != rootNode)
        {
            M = m_sceneRef->getNodes()[parentID].worldMatrix;
        }
        return M;
    }
}