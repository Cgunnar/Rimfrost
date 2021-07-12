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

		Node(NodeID ID, NodeID parentID, bool modelRoot = false);

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
		
	};

	class NodeHandle
	{
	public:
		NodeHandle(std::vector<Node>& sceneGraph, NodeID nodeID = rootNode);
		Node& operator*() const;
		Node* operator->() const;
		bool isValid() const;
		Node& get() const;
		void invalidateHandle();
		Transform getParentWorldMatrix() const;

	private:
		std::reference_wrapper<std::vector<Node>> m_sceneRef;
		NodeID m_nodeID;
	};


}