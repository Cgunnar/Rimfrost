#pragma once
#include "SubModel.hpp"
#include "Engine1.hpp"
#include "RimfrostMath.hpp"
#include <optional>
#include <vector>


namespace Rimfrost
{

	class SceneGraph;
	class ForwardRenderer;
	class SceneSerializer;

	class Node
	{
		friend SceneGraph;
		friend ForwardRenderer;
		friend SceneSerializer;
	public:

		Node(NodeID ID, NodeID parentID, NodeID coldID, bool modelRoot = false);

		std::optional<SubModel>& operator->();
		std::optional<SubModel>& operator*();

		Transform localMatrix;
		Transform worldMatrix;

		std::optional<SubModel> m_subModel;
		bool m_isModelParent;
		NodeID m_ID;
		NodeID m_parentID;
		std::vector<NodeID> m_childIDs;
	private:
		bool m_isHidden = false;
		NodeID m_coldID;
		
	};

	class NodeHandle
	{
	public:
		NodeHandle(SceneGraph& sceneGraph, NodeID nodeID = rootNode);
		NodeHandle();
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