#pragma once
#include "Node.hpp"
#include "Engine1.hpp"
#include "Transform.hpp"
#include "Camera.hpp"
#include "PointLightContainer.hpp"
#include "EventObserver.hpp"

class SceneSerializer;

namespace Engine1
{

	class Scene : public EventObserver
	{
		friend SceneSerializer;

	protected:
		/*class NodeHandle
		{
			friend Scene;
		public:
			NodeHandle(Scene& scene, NodeID nodeID = rootNode);
			Node& operator*() const;
			Node* operator->() const;
			bool isValid() const;

		private:
			std::reference_wrapper<Scene> m_sceneRef;
			NodeID m_nodeID;
		};*/

		struct NodeSerializationData
		{
			std::string modelPath;
			NodeID parentID = rootNode;
			NodeID nodeID = rootNode;
		};


	public:
		Scene();
		~Scene();
		virtual void setUpScene() {};
		void update(double dt);
		void bindInput(const std::shared_ptr<Keyboard>& keyboard, const std::shared_ptr<Mouse>& mouse);


		PointLightContainer& getPointLights();
		std::vector<NodeID>& getRenderSubmits();
		const std::vector<Node>& getNodes() const;
		const Camera& getCamera() const noexcept;
		const PerFrameData& getFrameCBufferData() const noexcept;
		
		[[nodiscard]] NodeHandle addModel(const std::string& filePath, ModelSettings modelSettings);
		[[nodiscard]] NodeHandle addModel(const std::string& filePath, NodeID parentNodeID = rootNode, ModelSettings modelSettings = ModelSettings::NONE);
		[[nodiscard]] NodeHandle addModel(const std::string& filePath, const NodeHandle& parenthandle, ModelSettings modelSettings = ModelSettings::NONE);
		[[nodiscard]] NodeHandle addNode(const Transform& offset = Transform(), NodeID parentNodeID = rootNode);
		[[nodiscard]] NodeHandle addNode(const Transform& offset, const NodeHandle& parenthandle);

		void removeNode(NodeID id);
		void hideNode(NodeID id, bool isHidden);
		

		void onEvent(const Event& e) override;
		virtual void derivedOnEvent(const Event& e) {}

	protected:
		Camera m_camera;
		std::vector<Node> m_nodes;
		std::vector<NodeID> m_renderSubmits;

		bool m_recordSceneToSerialization = true;
		std::vector<NodeSerializationData> m_serializationData;

		std::shared_ptr<Keyboard> m_keyboard;
		std::shared_ptr<Mouse> m_mouse;

		std::shared_ptr<PointLightContainer> m_pointLightContainer;

		bool m_isPaused;

		PerFrameData m_frameData;

	protected:
		//Model& addModel(const std::string& filePath);
		

		void updateWorldMatrices();
		void updatedChildWorldMatrix(std::vector<Node>& nodes, NodeID ID, const Transform& parentMatrix);
		virtual void derivedSceneUpdate(double dt) {}
		void packSceneGraph();

	private:
		

		NodeID traverseSubMeshTree(SubMeshTree tree, const Model& model, NodeID newNodeID);
		NodeID addSubModel(SubModel subModel, NodeID parentID);
		[[nodiscard]] NodeID addNode(NodeID parentNodeID);
		
	};

}
