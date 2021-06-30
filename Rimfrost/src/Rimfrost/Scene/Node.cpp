#include "rfpch.hpp"
#include "Node.hpp"
#include "Scene.hpp"

namespace Engine1
{

    Node::Node(NodeID ID, NodeID parentID, bool modelParent) : m_ID(ID), m_parentID(parentID), m_isModelParent(modelParent)
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



    NodeHandle::NodeHandle(std::vector<Node>& sceneGraph, NodeID nodeID) : m_sceneRef(sceneGraph), m_nodeID(nodeID)
    {
    }

    Node& NodeHandle::operator*() const
    {
        assert(m_nodeID != rootNode);
        return m_sceneRef.get()[m_nodeID];
    }

    Node* NodeHandle::operator->() const
    {
        assert(m_nodeID != rootNode);
        return &m_sceneRef.get()[m_nodeID];
    }

    bool NodeHandle::isValid() const
    {
        return m_nodeID != rootNode;
    }
    Node& NodeHandle::get() const
    {
        assert(m_nodeID != rootNode);
        return m_sceneRef.get()[m_nodeID];
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
            M = m_sceneRef.get()[parentID].worldMatrix;
        }
        return M;
    }
}