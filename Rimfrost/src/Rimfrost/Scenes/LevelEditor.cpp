#include "rfpch.hpp"
#include "LevelEditor.hpp"
#include "RandGen.hpp"
#include "Logger.hpp"
#include "MouseEvent.hpp"
#include "EventSystem.hpp"
#include "SceneSerializer.hpp"
#include <imgui.h>

using namespace DirectX;



namespace Engine1
{
	XMVECTOR closestPointOnLineFromPoint(const XMVECTOR& segmentEndPoint1, const XMVECTOR& segmentEndPoint2, const XMVECTOR& point);
	XMVECTOR rayFromMouse(float x, float y, float w, float h, XMFLOAT4X4 projectionMatrix);
	std::pair<XMFLOAT3, XMFLOAT3> closestPointsBetweenLines(XMFLOAT3 line1Direction, XMFLOAT3 line1Point, XMFLOAT3 line2Direction, XMFLOAT3 line2Point);
	XMFLOAT3 closestPointOnLineFromMouseCursor(XMFLOAT3 lineDirection, XMFLOAT3 pointOnLine, const Camera& cam, MouseState ms);
	XMVECTOR intersectionWithPlaneFromMouseCursor(XMVECTOR plane, const Camera& cam, MouseState ms);
	


	LevelEditor::LevelEditor(bool saveOnExit) : Engine1::Scene(), m_whiteLight({ 0, 8, 0 }, { 1.0f, 1.0f, 1.0f }, 100, "whiteLight")
	{
		m_saveOnExit = saveOnExit;
		
	}

	LevelEditor::~LevelEditor()
	{
		if (m_saveOnExit)
		{
			if (m_gizmoRootNode.isValid()) removeNode(m_gizmoRootNode->m_ID);
			packSceneGraph();
			OutputDebugString(L"save level from leveleditor destructor\n");
			Engine1::SceneSerializer::serialize("Scene2Save.json", *this);
		}
		if (m_lightSphere) delete m_lightSphere;
		OutputDebugString(L"~LevelEditor\n");
	}

	void LevelEditor::setUpScene()
	{
		EventSystem::addObserver(*this, MousePickingEvent::eventType);

		m_pointLightContainer = std::make_shared<PointLightContainer>();
		m_pointLightContainer->addPointLight(m_whiteLight);

		m_gizmoRootNode = addNode();

		m_arrowX = addModel("Models/Arrows/DXarrowX.obj", m_gizmoRootNode);
		m_arrowY = addModel("Models/Arrows/DXarrowY.obj", m_gizmoRootNode);
		m_arrowZ = addModel("Models/Arrows/DXarrowZ.obj", m_gizmoRootNode);
		m_arrowX->localMatrix.setScale(4);
		m_arrowY->localMatrix.setScale(4);
		m_arrowZ->localMatrix.setScale(4);


		m_ringX = addModel("Models/ring/red_ring.obj", m_gizmoRootNode);
		m_ringY = addModel("Models/ring/green_ring.obj", m_gizmoRootNode);
		m_ringZ = addModel("Models/ring/blue_ring.obj", m_gizmoRootNode);
		m_ringX->localMatrix.scale(2.0f);
		m_ringY->localMatrix.scale(2.15f);
		m_ringZ->localMatrix.scale(2.3f);

		hideNode(m_gizmoRootNode->m_ID, true);

	}

	void LevelEditor::derivedSceneUpdate(double dt)
	{
		m_nodeEditGui.view();

		if (m_isTranslatingNode) translateSelectedNode();
		if (m_isRotatingNode) rotateSelectedNode(static_cast<float>(dt));

		updateGizmo();
	}

	template<typename T> int sgn(T val)
	{
		return (T(0) < val) - (val < T(0));
	}

	void LevelEditor::translateSelectedNode()
	{
		TransformOld parentWorldMatrixInv = m_selectedNode.getParentWorldMatrix().getInverse();

		XMFLOAT3 p = closestPointOnLineFromMouseCursor(m_translationInfo.translateDirectionWorldSpace, m_translationInfo.onStartNodePositionWorldSpace, m_camera, m_mouseState);
		XMVECTOR deltaOnLine = XMLoadFloat3(&p) - XMLoadFloat3(&m_translationInfo.onStartLinePointWorldSpace);
		XMVECTOR newPosWorldSpace = XMLoadFloat3(&p) - XMLoadFloat3(&m_translationInfo.mouseToNodeOffsetWorldSpace);

		XMVECTOR correctedPos = XMVector3Transform(newPosWorldSpace, parentWorldMatrixInv.getXMMatrix());
		m_selectedNode->localMatrix.setPosition(correctedPos);

		m_gizmoRootNode->localMatrix.setPosition(newPosWorldSpace);
	}
	
	void LevelEditor::rotateSelectedNode(float dt)
	{
		XMVECTOR normalWorldSpace = XMVector3Normalize(XMLoadFloat3(&m_rotationInfo.rotationNormalWorldSpace));
		XMVECTOR tangentWorldSpace = XMLoadFloat3(&m_rotationInfo.tangentWorldSpace);
		XMVECTOR tangentViewSpace = XMVector4Transform(tangentWorldSpace, m_camera.GetViewMatrix());

		XMVECTOR tangentWithoutZ = XMVector2Normalize(tangentViewSpace);
		tangentWithoutZ.m128_f32[2] = 0;

		XMVECTOR onStartMousePointView = rayFromView(m_mouseCoordOnClick.x, m_mouseCoordOnClick.y);
		onStartMousePointView.m128_f32[2] = 0;

		m_rotationInfo.accRawMouseDelta.x += dt * m_mouseState.deltaX;
		m_rotationInfo.accRawMouseDelta.y -= dt * m_mouseState.deltaY;

		XMVECTOR mousePointView = onStartMousePointView + XMLoadFloat2(&m_rotationInfo.accRawMouseDelta);

		XMVECTOR closestPointOnLineView = closestPointOnLineFromPoint(onStartMousePointView, onStartMousePointView + tangentWithoutZ, mousePointView);
		float mouseTravelDistanceTangent = XMVector2Length(onStartMousePointView - closestPointOnLineView).m128_f32[0];


		float signOfTravelDistance = XMVector2Dot(tangentWithoutZ, mousePointView - onStartMousePointView).m128_f32[0];
		signOfTravelDistance = static_cast<float>(sgn(signOfTravelDistance));

		mouseTravelDistanceTangent *= 2.0f; //more speed

		float angle = (mouseTravelDistanceTangent - m_rotationInfo.lastFrameRotationAngle);

		TransformOld parentRotMatrixInv = m_selectedNode.getParentWorldMatrix().getRotationMatrix().getInverse();
		XMMATRIX deltaRotation = XMMatrixRotationNormal(normalWorldSpace, signOfTravelDistance * angle);
		TransformOld rot =  m_selectedNode->worldMatrix.getRotationMatrix() * deltaRotation;
		m_selectedNode->localMatrix.setRotation( rot * parentRotMatrixInv);
		m_rotationInfo.lastFrameRotationAngle = mouseTravelDistanceTangent;
	}

	void LevelEditor::selectTranslateFromGui(NodeEditGUI::RadioButtonRefSystem refSys)
	{
		m_selecteRefSysEnum = refSys;
		selectTranslate(getRefSysFromEnum(refSys));
	}

	void LevelEditor::selectTranslate(TransformOld referenceSystem)
	{
		deSelectTranslate();
		deSelectRotate();
		deSelectScale();

		m_translationIsSelected = true;

		hideNode(m_arrowX->m_ID, false);
		hideNode(m_arrowY->m_ID, false);
		hideNode(m_arrowZ->m_ID, false);

		m_selectedReferencSystem = referenceSystem;
	}

	void LevelEditor::deSelectTranslate()
	{
		m_translationIsSelected = false;
		stopTranslate();

		hideNode(m_arrowX->m_ID, true);
		hideNode(m_arrowY->m_ID, true);
		hideNode(m_arrowZ->m_ID, true);
	}


	void LevelEditor::selectRotateFromGui(NodeEditGUI::RadioButtonRefSystem refSys)
	{
		m_selecteRefSysEnum = refSys;
		selectRotate(getRefSysFromEnum(refSys));
	}

	void LevelEditor::selectRotate(TransformOld referenceSystem)
	{
		deSelectRotate();
		deSelectTranslate();
		deSelectScale();

		m_rotationIsSelected = true;

		hideNode(m_ringX->m_ID, false);
		hideNode(m_ringY->m_ID, false);
		hideNode(m_ringZ->m_ID, false);

		m_selectedReferencSystem = referenceSystem;
	}

	void LevelEditor::deSelectRotate()
	{
		m_rotationIsSelected = false;
		stopRotate();

		hideNode(m_ringX->m_ID, true);
		hideNode(m_ringY->m_ID, true);
		hideNode(m_ringZ->m_ID, true);
	}

	void LevelEditor::selectScale()
	{
		deSelectScale();
		deSelectTranslate();
		deSelectRotate();

		m_scalingIsSelected = true;
	}

	void LevelEditor::deSelectScale()
	{
		m_scalingIsSelected = false;
		stopScaling();
	}


	TransformOld LevelEditor::getRefSysFromEnum(NodeEditGUI::RadioButtonRefSystem refSys) const
	{
		switch (refSys)
		{
		case Engine1::NodeEditGUI::RadioButtonRefSystem::WORLD:
			return TransformOld();
		case Engine1::NodeEditGUI::RadioButtonRefSystem::LOCAL:
			return m_selectedNode->worldMatrix;
		case Engine1::NodeEditGUI::RadioButtonRefSystem::PARENT:
			return m_selectedNode.getParentWorldMatrix();
		default:
			return TransformOld();
		}
	}

	void LevelEditor::startTranslate(XMVECTOR translationDirectionLocalSpace)
	{
		m_isTranslatingNode = true;

		XMVECTOR translationDirectionWorldSpace = XMVector4Transform(translationDirectionLocalSpace,
			m_selectedReferencSystem.getRotationMatrix().getXMMatrix());
		

		XMStoreFloat3(&m_translationInfo.translateDirectionWorldSpace, translationDirectionWorldSpace);

		XMVECTOR worldPosition = m_selectedNode->worldMatrix.getPositionVector();

		XMFLOAT3 closestPointOnLineWorldSpace = closestPointOnLineFromMouseCursor(m_translationInfo.translateDirectionWorldSpace,
			m_selectedNode->worldMatrix.getPositionFloat3(), m_camera, m_mouseState);

		XMStoreFloat3(&m_translationInfo.onStartNodePositionWorldSpace, worldPosition);
		XMStoreFloat3(&m_translationInfo.deltaTransformToParentOnMove, worldPosition - m_selectedNode->localMatrix.getPositionVector());

		XMVECTOR offset = XMLoadFloat3(&closestPointOnLineWorldSpace) - worldPosition;
		XMStoreFloat3(&m_translationInfo.mouseToNodeOffsetWorldSpace, offset);
	}

	void LevelEditor::stopTranslate(bool keepNewPos)
	{
		m_isTranslatingNode = false;
	}

	void LevelEditor::startRotate(XMVECTOR rotationNormalLocalSpace)
	{
		TransformOld refSys = getRefSysFromEnum(m_selecteRefSysEnum);

		XMMATRIX refSysRot = refSys.getRotationMatrix().getXMMatrix();
		XMVECTOR rotationNormalWorldSpace = XMVector4Transform(rotationNormalLocalSpace, refSysRot);
		XMStoreFloat3(&m_rotationInfo.rotationNormalWorldSpace, rotationNormalWorldSpace);

		m_isRotatingNode = true;
		m_selectedNodeTransformOnStart = m_selectedNode->localMatrix;
		m_rotationInfo.lastFrameRotationAngle = 0;
		m_rotationInfo.accRawMouseDelta = { 0,0 };

		XMVECTOR planeOfRotation = XMPlaneFromPointNormal(m_selectedNode->worldMatrix.getPositionVector(),
			rotationNormalWorldSpace);

		XMVECTOR mouseIntersectRotationPlaneOnStartRotate = intersectionWithPlaneFromMouseCursor(
			planeOfRotation, m_camera, m_mouseState);

		XMVECTOR clickPoint = mouseIntersectRotationPlaneOnStartRotate - m_selectedNode->worldMatrix.getPositionVector();
		clickPoint = XMVector3Normalize(clickPoint);

		XMVECTOR tangentWorldSpace = XMVector3Cross(rotationNormalWorldSpace, clickPoint);
		XMStoreFloat3(&m_rotationInfo.tangentWorldSpace, tangentWorldSpace);
	}

	void LevelEditor::stopRotate(bool keepNewAngle)
	{
		m_isRotatingNode = false;
	}

	void LevelEditor::startScaling()
	{
		m_isScalingNode = true;
	}

	void LevelEditor::stopScaling(bool keeoNewScale)
	{
		m_isScalingNode = false;
	}

	bool LevelEditor::handleArrowSelection(NodeID id)
	{
		bool clickedOnArrow = false;
		XMVECTOR translationDirection;
		if (m_arrowX.isValid() && m_arrowX->m_ID == id) //click in arrow
		{
			Logger::getLogger().debugLog("klick on arrowX\n");
			translationDirection = m_selectedNode->localMatrix.getRight();
			translationDirection = { 1,0,0 };
			clickedOnArrow = true;
		}
		else if (m_arrowY.isValid() && m_arrowY->m_ID == id) //click in arrow
		{
			Logger::getLogger().debugLog("klick on arrowY\n");
			translationDirection = m_selectedNode->localMatrix.getUp();
			translationDirection = { 0,1,0 };
			clickedOnArrow = true;
		}
		else if (m_arrowZ.isValid() && m_arrowZ->m_ID == id) //click in arrow
		{
			Logger::getLogger().debugLog("klick on arrowZ\n");
			translationDirection = m_selectedNode->localMatrix.getForward();
			translationDirection = { 0,0,1 };
			clickedOnArrow = true;
		}
		if (clickedOnArrow)
		{
			startTranslate(translationDirection);
		}

		return clickedOnArrow;
	}

	LevelEditor::GizmoXYZ LevelEditor::handleRingSelection(NodeID id)
	{
		LevelEditor::GizmoXYZ clickedRing = LevelEditor::GizmoXYZ::NONE;
		XMVECTOR rotationNormal;
		if (m_ringX.isValid() && m_ringX->m_ID == id)
		{
			Logger::getLogger().debugLog("klick on ringX\n");
			rotationNormal = { 1,0,0 };
			clickedRing = LevelEditor::GizmoXYZ::X;
		}
		else if (m_ringY.isValid() && m_ringY->m_ID == id)
		{
			Logger::getLogger().debugLog("klick on ringY\n");
			rotationNormal = { 0,1,0 };

			clickedRing = LevelEditor::GizmoXYZ::Y;
		}
		else if (m_ringZ.isValid() && m_ringZ->m_ID == id)
		{
			Logger::getLogger().debugLog("klick on ringZ\n");
			rotationNormal = { 0,0,1 };

			clickedRing = LevelEditor::GizmoXYZ::Z;
		}
		if (clickedRing != LevelEditor::GizmoXYZ::NONE)
		{
			startRotate(rotationNormal);
		}
		return clickedRing;
	}

	void LevelEditor::updateGizmo()
	{
		if (m_gizmoRootNode.isValid() && m_selectedNode.isValid())
		{
			updateWorldMatrices();
			m_gizmoRootNode->localMatrix.setPosition(m_selectedNode->worldMatrix.getPositionFloat3());
			TransformOld refSys = getRefSysFromEnum(m_selecteRefSysEnum);
			m_gizmoRootNode->localMatrix.setRotation(refSys.getRotationMatrix());
		}
	}

	XMVECTOR LevelEditor::rayFromView(float x, float y)
	{
		XMFLOAT4X4 P;
		XMStoreFloat4x4(&P, m_camera.GetPerspective());
		return rayFromMouse(x, y, static_cast<float>(m_mouseState.width), static_cast<float>(m_mouseState.height), P);
	}

	void LevelEditor::derivedOnEvent(const Event& e)
	{
		if (e.type() == MouseButtonsEvent::eventType)
		{
			auto& mouse = static_cast<const MouseButtonsEvent&>(e).mouseState;
			m_mouseState = mouse;
			if (mouse.RMBClicked)
			{
				m_mouseCoordOnClick.x = static_cast<float>(mouse.x);
				m_mouseCoordOnClick.y = static_cast<float>(mouse.y);
			}
			if (mouse.RMBHeld)
			{
				m_mouseCoord.x = static_cast<float>(mouse.x);
				m_mouseCoord.y = static_cast<float>(mouse.y);
				m_winWidth = mouse.width;
				m_winHeight = mouse.height;
			}
			if (mouse.RMBReleased)
			{
				stopTranslate();
				stopRotate();
				stopScaling();
			}
		}


		if (e.type() == MousePickingEvent::eventType)
		{
			auto& mpe = static_cast<const MousePickingEvent&>(e);
			Logger::getLogger().debugLog("pickEvent: " + std::to_string(mpe.nodeID) + "\n");
			NodeHandle temp = NodeHandle(m_nodes, mpe.nodeID);
			if (temp.isValid() && **temp)
			{

				//find the root of the model and get the parent
				NodeID tempParent = temp->m_parentID;
				while (!m_nodes[tempParent].m_isModelParent)
				{
					tempParent = m_nodes[tempParent].m_parentID;
					assert(tempParent != rootNode);
				}

				if (handleArrowSelection(tempParent)) //click on arrow
				{
					Logger::getLogger().debugLog("clicked on Arrow\n");
				}
				else if (handleRingSelection(tempParent) != LevelEditor::GizmoXYZ::NONE)
				{
					Logger::getLogger().debugLog("clicked on ring\n");
				}
				else if (!(m_selectedNode.isValid() && m_selectedNode->m_ID == tempParent))//clicked on unselected
				{
					deSelectTranslate();
					deSelectRotate();
					deSelectScale();

					m_selectedNode = NodeHandle(m_nodes, tempParent);

					m_nodeEditGui = NodeEditGUI(m_selectedNode,
						std::bind(&LevelEditor::selectTranslateFromGui, this, std::placeholders::_1),
						std::bind(&LevelEditor::selectRotateFromGui, this, std::placeholders::_1),
						std::bind(&LevelEditor::selectScale, this));

					m_nodeEditGui.selectedTRS();
				}
			}
		}
	}

	XMVECTOR rayFromMouse(float x, float y, float w, float h, XMFLOAT4X4 projectionMatrix)
	{
		float viewSpaceX = (static_cast<float>(2 * x) / static_cast<float>(w) - 1.0f) / projectionMatrix._11;
		float viewSpaceY = (static_cast<float>(-2 * y) / static_cast<float>(h) + 1.0f) / projectionMatrix._22;
		return { viewSpaceX, viewSpaceY, 1 };
	}

	std::pair<XMFLOAT3, XMFLOAT3> closestPointsBetweenLines(XMFLOAT3 L1Dir, XMFLOAT3 L1P, XMFLOAT3 L2Dir, XMFLOAT3 L2P)
	{
		float L3P[3] = { L1P.x - L2P.x, L1P.y - L2P.y, L1P.z - L2P.z };

		float B[2] = { -(L1Dir.x * L3P[0] + L1Dir.y * L3P[1] + L1Dir.z * L3P[2]),
						-(L2Dir.x * L3P[0] + L2Dir.y * L3P[1] + L2Dir.z * L3P[2]) };

		float A[4] = { L1Dir.x * L1Dir.x + L1Dir.y * L1Dir.y + L1Dir.z * L1Dir.z,
						L1Dir.x * -L2Dir.x + L1Dir.y * -L2Dir.y + L1Dir.z * -L2Dir.z,
						L1Dir.x * L2Dir.x + L1Dir.y * L2Dir.y + L1Dir.z * L2Dir.z,
						L2Dir.x * -L2Dir.x + L2Dir.y * -L2Dir.y + L2Dir.z * -L2Dir.z };

		float detInv = 1.0f / (A[0] * A[3] - A[1] * A[2]);

		float x0 = detInv * A[3] * B[0] + detInv * -A[1] * B[1];
		float x1 = detInv * -A[2] * B[0] + detInv * A[0] * B[1];

		XMFLOAT3 L1CP = { L1Dir.x * x0 + L1P.x, L1Dir.y * x0 + L1P.y, L1Dir.z * x0 + L1P.z };
		XMFLOAT3 L2CP = { L2Dir.x * x1 + L2P.x, L2Dir.y * x1 + L2P.y, L2Dir.z * x1 + L2P.z };
		return std::pair<XMFLOAT3, XMFLOAT3>(L1CP, L2CP);
	}

	XMFLOAT3 closestPointOnLineFromMouseCursor(XMFLOAT3 lineDirection, XMFLOAT3 pointOnLine, const Camera& cam, MouseState ms)
	{
		XMFLOAT4X4 P;
		XMStoreFloat4x4(&P, cam.GetPerspective());
		XMVECTOR ray = rayFromMouse(static_cast<float>(ms.x), static_cast<float>(ms.y), static_cast<float>(ms.width), static_cast<float>(ms.height), P);

		XMFLOAT3 V2;
		XMStoreFloat3(&V2, XMVector4Transform(ray, cam.GetWorldMatrix()));
		XMFLOAT3 P2;
		XMStoreFloat3(&P2, cam.GetPosition());
		auto [CP1, CP2] = closestPointsBetweenLines(lineDirection, pointOnLine, V2, P2);
		return CP1;
	}
	XMVECTOR intersectionWithPlaneFromMouseCursor(XMVECTOR plane, const Camera& cam, MouseState ms)
	{
		XMFLOAT4X4 P;
		XMStoreFloat4x4(&P, cam.GetPerspective());
		XMVECTOR ray = rayFromMouse(static_cast<float>(ms.x), static_cast<float>(ms.y), static_cast<float>(ms.width), static_cast<float>(ms.height), P);
		XMVECTOR rayDirInWorldSpace = XMVector4Transform(ray, cam.GetWorldMatrix());
		return XMPlaneIntersectLine(plane, cam.GetPosition(), cam.GetPosition() + rayDirInWorldSpace);
	}

	

	XMVECTOR closestPointOnLineFromPoint(const XMVECTOR& segmentEndPoint1, const XMVECTOR& segmentEndPoint2, const XMVECTOR& point)
	{
		//closest point on linesegment to point
		//från boken Real time collision detection

		XMVECTOR ab = segmentEndPoint1 - segmentEndPoint2;
		//XMVECTOR t = ab.Dot(point - segmentEndPoint2) / ab.Dot(ab);
		XMVECTOR t = XMVector3Dot(ab, point - segmentEndPoint2) / XMVector3Dot(ab, ab);

		/*if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;*/

		auto result = segmentEndPoint2 + XMVectorMultiply(ab, t);
		/*auto test = XMVector3Dot(segmentEndPoint1 - segmentEndPoint2, result - point).m128_f32[0];*/

		return result;
	}
}

