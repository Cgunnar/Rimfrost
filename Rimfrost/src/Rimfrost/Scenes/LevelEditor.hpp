#pragma once
#include "Scene.hpp"
#include "PointLight.hpp"
#include "NodeEditGUI.hpp"

namespace Engine1
{

	class LevelEditor : public Engine1::Scene
	{
	public:
		LevelEditor(bool saveOnExit = false);
		~LevelEditor();

		void setUpScene() override;

	private:
		NodeEditGUI m_nodeEditGui;

		enum class GizmoXYZ
		{
			NONE = 0,
			X = 1,
			Y = 2,
			Z = 3
		};

		struct CurrentTranslationData
		{
			DirectX::XMFLOAT3 translateDirectionWorldSpace = { 0,0,0 };
			DirectX::XMFLOAT3 onStartNodePositionWorldSpace = { 0,0,0 };
			DirectX::XMFLOAT3 mouseToNodeOffsetWorldSpace = { 0,0,0 };
			DirectX::XMFLOAT3 deltaTransformToParentOnMove = { 0,0,0 };
			DirectX::XMFLOAT3 onStartLinePointWorldSpace = { 0,0,0 };


		} m_translationInfo;

		struct CurrentRotationData
		{
			DirectX::XMFLOAT3 tangentWorldSpace = { 0,0,0 };
			DirectX::XMFLOAT3 rotationNormalWorldSpace = { 0,0,0 };
			DirectX::XMFLOAT2 accRawMouseDelta = { 0,0 };
			float lastFrameRotationAngle = 0;
		} m_rotationInfo;


		bool m_translationIsSelected = false;
		bool m_rotationIsSelected = false;
		bool m_scalingIsSelected = false;
		
		bool m_isTranslatingNode = false;
		bool m_isRotatingNode = false;
		bool m_isScalingNode = false;


		bool m_saveOnExit;
		PointLight m_whiteLight;
		NodeHandle* m_lightSphere = nullptr;

		NodeHandle m_selectedNode{ m_nodes };
		NodeHandle m_gizmoRootNode{ m_nodes };

		Transform m_selectedNodeTransformOnStart;
		Transform m_selectedReferencSystem;
		NodeEditGUI::RadioButtonRefSystem m_selecteRefSysEnum = NodeEditGUI::RadioButtonRefSystem::WORLD;


		NodeHandle m_arrowX{ m_nodes };
		NodeHandle m_arrowY{ m_nodes };
		NodeHandle m_arrowZ{ m_nodes };

		NodeHandle m_ringX{ m_nodes };
		NodeHandle m_ringY{ m_nodes };
		NodeHandle m_ringZ{ m_nodes };
		
		DirectX::XMFLOAT2 m_mouseCoord = { 0, 0 };
		DirectX::XMFLOAT2 m_mouseCoordOnClick = { 0, 0 };
		MouseState m_mouseState;
		uint32_t m_winWidth = 0;
		uint32_t m_winHeight = 0;

	private:
		void derivedSceneUpdate(double dt) override;

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

		void startTranslate(DirectX::XMVECTOR translationDirectionLocalSpace);
		void stopTranslate(bool keepNewPos = true);

		void startRotate(DirectX::XMVECTOR rotationNormal);
		void stopRotate(bool keepNewAngle = true);

		void startScaling();
		void stopScaling(bool keeoNewScale = true);

		bool handleArrowSelection(Engine1::NodeID id);
		GizmoXYZ handleRingSelection(NodeID id);

		void derivedOnEvent(const Event& e) override;
		
		void updateGizmo();

		DirectX::XMVECTOR rayFromView(float x, float y);
		Transform getRefSysFromEnum(NodeEditGUI::RadioButtonRefSystem refSys) const;
	};

}