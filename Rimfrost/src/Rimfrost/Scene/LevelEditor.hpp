#pragma once
#include "Rimfrost\Graphics\Lights.hpp"
#include "Rimfrost\Scene\NodeEditGUI.hpp"
#include "Rimfrost\Math\RimfrostMath.hpp"
#include "Rimfrost\Scene\SceneGraph.hpp"
#include "Rimfrost\Scene\IScene.hpp"
#include "Rimfrost\EventSystem\EventObserver.hpp"
#include "Rimfrost\EntCom\rfEntity.hpp"

namespace Rimfrost
{

	class LevelEditor : public IScene, public EventObserver
	{
	public:
		LevelEditor(const std::string& inputMap = "", const std::string& outPutMap = "tempLevelEditorSave/");
		~LevelEditor();

		void setUpScene() override;
		void load(std::string path) override;
		void save(std::string path) override;
		Camera& camera() override;
		SceneGraph& sceneGraph() override;
		Lights& lights() override;

		void onEvent(const Event& e) override;

	private:
		SceneGraph m_sceneGraph;
		Camera m_camera;
		Lights m_lights;

		NodeEditGUI m_nodeEditGui;

		std::string m_inputMapFile;
		std::string m_outPutMapFile;
		bool m_saveOnExit = false;

		//only used to work the scene, shall not be saved
		std::vector<Entity> m_tempEntitis;

		// uesed to map pointLightComponents to PointLights
		std::unordered_map<int64_t, Rimfrost::PointLight> m_poinLightMap;

		enum class GizmoXYZ
		{
			NONE = 0,
			X = 1,
			Y = 2,
			Z = 3
		};

		struct CurrentTranslationData
		{
			Vector3 translateDirectionWorldSpace = { 0,0,0 };
			Vector3 onStartNodePositionWorldSpace = { 0,0,0 };
			Vector3 mouseToNodeOffsetWorldSpace = { 0,0,0 };
			Vector3 deltaTransformToParentOnMove = { 0,0,0 };
			Vector3 onStartLinePointWorldSpace = { 0,0,0 };


		} m_translationInfo;

		struct CurrentRotationData
		{
			Vector3 tangentWorldSpace = { 0,0,0 };
			Vector3 rotationNormalWorldSpace = { 0,0,0 };
			Vector2 accRawMouseDelta = { 0,0 };
			float lastFrameRotationAngle = 0;
		} m_rotationInfo;


		bool m_translationIsSelected = false;
		bool m_rotationIsSelected = false;
		bool m_scalingIsSelected = false;
		
		bool m_isTranslatingNode = false;
		bool m_isRotatingNode = false;
		bool m_isScalingNode = false;

		bool m_acceptMousePickingEvent = false;

		PointLight m_whiteLight;
		NodeHandle* m_lightSphere = nullptr;

		NodeHandle m_selectedNode;
		NodeHandle m_gizmoRootNode;

		Transform m_selectedNodeTransformOnStart;
		Transform m_selectedReferencSystem;
		NodeEditGUI::RadioButtonRefSystem m_selecteRefSysEnum = NodeEditGUI::RadioButtonRefSystem::WORLD;

		std::vector<NodeHandle> m_pointLightGizmoHandles;

		NodeHandle m_arrowX;
		NodeHandle m_arrowY;
		NodeHandle m_arrowZ;

		NodeHandle m_ringX;
		NodeHandle m_ringY;
		NodeHandle m_ringZ;
		
		Vector2 m_mouseCoord = { 0, 0 };
		Vector2 m_mouseCoordOnClick = { 0, 0 };
		MouseState m_mouseState;
		uint32_t m_winWidth = 0;
		uint32_t m_winHeight = 0;

	private:
		void update(double dt) override;

		void translateSelectedNode();
		void rotateSelectedNode(float dt);

		void selectTranslateFromGui(NodeEditGUI::RadioButtonRefSystem refSys);
		void selectTranslate(Transform referenceSystem);
		void deSelectTranslate();

		void selectRotateFromGui(NodeEditGUI::RadioButtonRefSystem refSys);
		void selectRotate(Transform referenceSystem);
		void deSelectRotate();

		void selectScale();
		void deSelectScale();

		void startTranslate(Vector3 translationDirectionLocalSpace);
		void stopTranslate(bool keepNewPos = true);

		void startRotate(Vector3 rotationNormal);
		void stopRotate(bool keepNewAngle = true);

		void startScaling();
		void stopScaling(bool keeoNewScale = true);

		bool handleArrowSelection(Rimfrost::NodeID id);
		GizmoXYZ handleRingSelection(NodeID id);

		
		
		void updateGizmo();

		Vector3 rayFromView(float x, float y);
		Transform getRefSysFromEnum(NodeEditGUI::RadioButtonRefSystem refSys) const;
	};

}