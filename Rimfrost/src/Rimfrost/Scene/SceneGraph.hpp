#pragma once
#include "Rimfrost\Scene\Node.hpp"
#include "Rimfrost\Graphics\Engine1.hpp"
#include "Rimfrost\Math\RimfrostMath.hpp"


namespace Rimfrost
{
	class SceneGraph
	{

	public:
		SceneGraph() = default;
		virtual ~SceneGraph();
		virtual void setUpScene() {};
		void update(double dt);

		std::vector<NodeID>& getRenderSubmits();
		const std::vector<Node>& getNodes() const;
		std::vector<Node>& getNodes();
		
		[[nodiscard]] NodeHandle addModel(const std::string& filePath, ModelSettings modelSettings);
		[[nodiscard]] NodeHandle addModel(const std::string& filePath, NodeID parentNodeID = rootNode, ModelSettings modelSettings = ModelSettings::NONE);
		[[nodiscard]] NodeHandle addModel(const std::string& filePath, const NodeHandle& parenthandle, ModelSettings modelSettings = ModelSettings::NONE);
		[[nodiscard]] NodeHandle addNode(const Transform& offset = Transform(), NodeID parentNodeID = rootNode);
		[[nodiscard]] NodeHandle addNode(const Transform& offset, const NodeHandle& parenthandle);

		void removeNode(NodeID id);
		void hideNode(NodeID id, bool isHidden);
		void packSceneGraph();
		void updateWorldMatrices();

	private:
		std::vector<Node> m_nodes;
		std::vector<NodeID> m_renderSubmits;
		

	private:
		
		void updatedChildWorldMatrix(std::vector<Node>& nodes, NodeID ID, const Transform& parentMatrix);
		

		NodeID traverseSubMeshTree(SubMeshTree tree, const Model& model, NodeID newNodeID);
		NodeID addSubModel(SubModel subModel, NodeID parentID);
		[[nodiscard]] NodeID addNode(NodeID parentNodeID);
		
	};

}
