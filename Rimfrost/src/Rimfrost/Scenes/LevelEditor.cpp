#include "rfpch.hpp"
#include "LevelEditor.hpp"
#include "RandGen.hpp"
#include "Logger.hpp"
#include "MouseEvent.hpp"
#include "EventSystem.hpp"
#include "SceneSerializer.hpp"
#include <imgui.h>

using namespace DirectX;



namespace Rimfrost
{
	Vector3 closestPointOnLineFromPoint(const Vector3& segmentEndPoint1, const Vector3& segmentEndPoint2, const Vector3& point);
	Vector3 rayFromMouse(float x, float y, float w, float h, Matrix projectionMatrix);
	std::pair<XMFLOAT3, XMFLOAT3> closestPointsBetweenLines(XMFLOAT3 line1Direction, XMFLOAT3 line1Point, XMFLOAT3 line2Direction, XMFLOAT3 line2Point);
	Vector3 closestPointOnLineFromMouseCursor(Vector3 lineDirection, Vector3 pointOnLine, const Camera& cam, MouseState ms);
	XMVECTOR intersectionWithPlaneFromMouseCursor(XMVECTOR plane, const Camera& cam, MouseState ms);
	


	LevelEditor::LevelEditor(bool saveOnExit) : Rimfrost::Scene(), m_whiteLight({ 0, 8, 0 }, { 1.0f, 1.0f, 1.0f }, 100, "whiteLight")
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
			Rimfrost::SceneSerializer::serialize("Scene2Save.json", *this);
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
		Transform parentWorldMatrixInv = inverse(m_selectedNode.getParentWorldMatrix());

		Vector3 p = closestPointOnLineFromMouseCursor(m_translationInfo.translateDirectionWorldSpace, m_translationInfo.onStartNodePositionWorldSpace, m_camera, m_mouseState);
		Vector3 deltaOnLine = p - m_translationInfo.onStartLinePointWorldSpace;
		Vector3 newPosWorldSpace = p - m_translationInfo.mouseToNodeOffsetWorldSpace;

		Vector3 correctedPos = parentWorldMatrixInv * Vector4(newPosWorldSpace, 1);
		m_selectedNode->localMatrix.setTranslation(correctedPos);

		m_gizmoRootNode->localMatrix.setTranslation(newPosWorldSpace);
	}
	
	void LevelEditor::rotateSelectedNode(float dt)
	{
		Vector3 normalWorldSpace = m_rotationInfo.rotationNormalWorldSpace;
		normalWorldSpace.normalize();
		Vector3 tangentWorldSpace = m_rotationInfo.tangentWorldSpace;
		Vector3 tangentViewSpace =  m_camera.GetViewMatrix() * Vector4(tangentWorldSpace, 0);

		Vector2 tangentWithoutZ = Vector2(tangentViewSpace.x, tangentViewSpace.y);
		tangentWithoutZ.normalize();

		Vector2 onStartMousePointView = rayFromView(m_mouseCoordOnClick.x, m_mouseCoordOnClick.y);

		m_rotationInfo.accRawMouseDelta.x += dt * m_mouseState.deltaX;
		m_rotationInfo.accRawMouseDelta.y -= dt * m_mouseState.deltaY;

		Vector2 mousePointView = onStartMousePointView + m_rotationInfo.accRawMouseDelta;

		Vector3 closestPointOnLineView = closestPointOnLineFromPoint(Vector3(onStartMousePointView, 0), Vector3(onStartMousePointView + tangentWithoutZ, 0), Vector3(mousePointView, 0));
		float mouseTravelDistanceTangent = (onStartMousePointView - closestPointOnLineView).length();


		float signOfTravelDistance = dot(tangentWithoutZ, mousePointView - onStartMousePointView);
		signOfTravelDistance = static_cast<float>(sgn(signOfTravelDistance));

		mouseTravelDistanceTangent *= 2.0f; //more speed

		float angle = (mouseTravelDistanceTangent - m_rotationInfo.lastFrameRotationAngle);

		Transform parentRotMatrixInv = inverse(m_selectedNode.getParentWorldMatrix().getRotationMatrix());
		Matrix deltaRotation = XMMatrixRotationNormal(normalWorldSpace, signOfTravelDistance * angle);
		Transform rot =  m_selectedNode->worldMatrix.getRotationMatrix() * deltaRotation;
		m_selectedNode->localMatrix.setRotation( rot * parentRotMatrixInv); //flip order?
		m_rotationInfo.lastFrameRotationAngle = mouseTravelDistanceTangent;
	}

	void LevelEditor::selectTranslateFromGui(NodeEditGUI::RadioButtonRefSystem refSys)
	{
		m_selecteRefSysEnum = refSys;
		selectTranslate(getRefSysFromEnum(refSys));
	}

	void LevelEditor::selectTranslate(Transform referenceSystem)
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

	void LevelEditor::selectRotate(Transform referenceSystem)
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


	Transform LevelEditor::getRefSysFromEnum(NodeEditGUI::RadioButtonRefSystem refSys) const
	{
		switch (refSys)
		{
		case Rimfrost::NodeEditGUI::RadioButtonRefSystem::WORLD:
			return Transform();
		case Rimfrost::NodeEditGUI::RadioButtonRefSystem::LOCAL:
			return m_selectedNode->worldMatrix;
		case Rimfrost::NodeEditGUI::RadioButtonRefSystem::PARENT:
			return m_selectedNode.getParentWorldMatrix();
		default:
			return Transform();
		}
	}

	void LevelEditor::startTranslate(Vector3 translationDirectionLocalSpace)
	{
		m_isTranslatingNode = true;

		m_translationInfo.translateDirectionWorldSpace =
			m_selectedReferencSystem.getRotationMatrix() * Vector4(translationDirectionLocalSpace, 0);
		

		Vector3 worldPosition = m_selectedNode->worldMatrix.getTranslation();

		Vector3 closestPointOnLineWorldSpace = closestPointOnLineFromMouseCursor(m_translationInfo.translateDirectionWorldSpace,
			m_selectedNode->worldMatrix.getTranslation(), m_camera, m_mouseState);

		m_translationInfo.onStartNodePositionWorldSpace = worldPosition;
		m_translationInfo.deltaTransformToParentOnMove = worldPosition - m_selectedNode->localMatrix.getTranslation();

		Vector3 offset = closestPointOnLineWorldSpace - worldPosition;
		m_translationInfo.mouseToNodeOffsetWorldSpace = offset;
	}

	void LevelEditor::stopTranslate(bool keepNewPos)
	{
		m_isTranslatingNode = false;
	}

	void LevelEditor::startRotate(Vector3 rotationNormalLocalSpace)
	{
		Transform refSys = getRefSysFromEnum(m_selecteRefSysEnum);

		Matrix refSysRot = refSys.getRotationMatrix();
		Vector3 rotationNormalWorldSpace = refSysRot * Vector4(rotationNormalLocalSpace, 0);
		m_rotationInfo.rotationNormalWorldSpace = rotationNormalWorldSpace;

		m_isRotatingNode = true;
		m_selectedNodeTransformOnStart = m_selectedNode->localMatrix;
		m_rotationInfo.lastFrameRotationAngle = 0;
		m_rotationInfo.accRawMouseDelta = { 0,0 };

		Vector3 selNodePosW = m_selectedNode->worldMatrix.getTranslation();
		XMVECTOR selectedNodePosWorld = { selNodePosW.x, selNodePosW.y, selNodePosW.z };
		XMVECTOR rotationNormalWorldSpaceXM = { rotationNormalWorldSpace.x, rotationNormalWorldSpace.y, rotationNormalWorldSpace.z };
		XMVECTOR planeOfRotation = DirectX::XMPlaneFromPointNormal(selectedNodePosWorld, rotationNormalWorldSpaceXM);

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
		Vector3 translationDirection;
		if (m_arrowX.isValid() && m_arrowX->m_ID == id) //click in arrow
		{
			Logger::getLogger().debugLog("klick on arrowX\n");
			translationDirection = m_selectedNode->localMatrix.right();
			translationDirection = { 1,0,0 };
			clickedOnArrow = true;
		}
		else if (m_arrowY.isValid() && m_arrowY->m_ID == id) //click in arrow
		{
			Logger::getLogger().debugLog("klick on arrowY\n");
			translationDirection = m_selectedNode->localMatrix.up();
			translationDirection = { 0,1,0 };
			clickedOnArrow = true;
		}
		else if (m_arrowZ.isValid() && m_arrowZ->m_ID == id) //click in arrow
		{
			Logger::getLogger().debugLog("klick on arrowZ\n");
			translationDirection = m_selectedNode->localMatrix.forward();
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
			m_gizmoRootNode->localMatrix.setTranslation(m_selectedNode->worldMatrix.getTranslation());
			Transform refSys = getRefSysFromEnum(m_selecteRefSysEnum);
			m_gizmoRootNode->localMatrix.setRotation(refSys.getRotationMatrix());
		}
	}

	Vector3 LevelEditor::rayFromView(float x, float y)
	{
		return rayFromMouse(x, y, static_cast<float>(m_mouseState.width), static_cast<float>(m_mouseState.height), m_camera.GetPerspective());
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

	Vector3 rayFromMouse(float x, float y, float w, float h, Matrix projectionMatrix)
	{
		float viewSpaceX = (static_cast<float>(2 * x) / static_cast<float>(w) - 1.0f) / projectionMatrix[0][0];
		float viewSpaceY = (static_cast<float>(-2 * y) / static_cast<float>(h) + 1.0f) / projectionMatrix[1][1];
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

	Vector3 closestPointOnLineFromMouseCursor(Vector3 lineDirection, Vector3 pointOnLine, const Camera& cam, MouseState ms)
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
		return DirectX::XMPlaneIntersectLine(plane, cam.GetPosition(), cam.GetPosition() + rayDirInWorldSpace);
	}

	

	Vector3 closestPointOnLineFromPoint(const Vector3& segmentEndPoint1, const Vector3& segmentEndPoint2, const Vector3& point)
	{
		//closest point on linesegment to point
		//från boken Real time collision detection

		Vector3 ab = segmentEndPoint1 - segmentEndPoint2;
		//XMVECTOR t = ab.Dot(point - segmentEndPoint2) / ab.Dot(ab);
		Vector3 t = dot(ab, point - segmentEndPoint2) / dot(ab, ab);

		/*if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;*/

		Vector3 result = segmentEndPoint2 + ab * t;
		/*auto test = XMVector3Dot(segmentEndPoint1 - segmentEndPoint2, result - point).m128_f32[0];*/

		return result;
	}
}

