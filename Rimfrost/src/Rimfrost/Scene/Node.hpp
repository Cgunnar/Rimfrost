#pragma once
#include "SubModel.hpp"
#include "Engine1.hpp"
#include "RimfrostMath.hpp"
#include <optional>
#include <vector>


namespace Rimfrost
{
	class SceneGraph;
	class SceneSerializer;
	class NodeHandle;

	class Node
	{
		friend SceneGraph;
		friend SceneSerializer;
		friend NodeHandle;
	public:

		Node(NodeID ID, NodeID parentID, NodeID coldID, bool modelRoot = false);
		operator const NodeID() const { return m_ID; }
		NodeID getColdID() const;
		NodeID getID() const;
		NodeID getParentID() const;
		const std::vector<NodeID>& getChildIDs() const;
		bool isParentToModel() const;
		std::optional<SubModel>& operator->();
		std::optional<SubModel>& operator*();

		Transform localMatrix;
		Transform worldMatrix;

		std::optional<SubModel> m_subModel;
	private:
		std::vector<NodeID> m_childIDs;
		NodeID m_ID;
		NodeID m_parentID;
		//coldID is a persisting id that survives when the scenegraph gets serialized
		NodeID m_coldID;

		bool m_isModelParent;
		bool m_isHidden = false;
		
	};

	class ECSSerializer;
	class NodeHandle
	{
		friend ECSSerializer;
	public:
		NodeHandle(SceneGraph& sceneGraph, NodeID nodeID, NodeID coldID = -1);
		NodeHandle(SceneGraph& sceneGraph, Node node);
		NodeHandle();
		operator const NodeID() const { assert(this->isValid()); return m_nodeID; }
		Node& operator*() const;
		Node* operator->() const;
		bool isValid() const;
		Node& get() const;
		void invalidateHandle();
		Transform getParentWorldMatrix() const;

	private:
		std::optional<std::reference_wrapper<SceneGraph>> m_sceneRef;
		NodeID m_nodeID;
		NodeID m_coldNodeID = -1;
	};


}