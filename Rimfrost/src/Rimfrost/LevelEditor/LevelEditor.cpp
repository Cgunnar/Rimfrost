#include "rfpch.hpp"
#include <imgui.h>

#include "LevelEditor.hpp"
#include "RandGen.hpp"
#include "Logger.hpp"
#include "MouseEvent.hpp"
#include "Rimfrost\Events\KeyboardEvent.hpp"
#include "Rimfrost\Events\PauseEvent.hpp"
#include "Rimfrost\EventSystem\EventSystem.hpp"
#include "Rimfrost\Scene\SceneSerializer.hpp"
#include "Rimfrost\EntCom\SerializeECS.hpp"
#include "Rimfrost\EntCom\rfComponents.hpp"

#include "RfextendedMath.hpp"



namespace Rimfrost
{

	Vector3 rayFromMouse(float x, float y, float w, float h, Matrix projectionMatrix);
	Vector3 closestPointOnLineFromMouseCursor(Vector3 lineDirection, Vector3 pointOnLine, const Camera& cam, MouseState ms);
	Vector3 intersectionWithPlaneFromMouseCursor(Vector4 plane, const Camera& cam, MouseState ms);



	LevelEditor::LevelEditor(const std::string& inputMap, const std::string& outPutMap)
	{
		assert(outPutMap.back() == '\\' || outPutMap.back() == '/');
		assert(inputMap.back() == '\\' || inputMap.back() == '/');
		m_outPutMapFile = outPutMap;
		m_inputMap = inputMap;
	}

	LevelEditor::~LevelEditor()
	{
		OutputDebugString(L"~LevelEditor\n");
	}

	void LevelEditor::setUpScene()
	{
		EventSystem::addObserver(*this, MousePickingEvent::eventType);
		EventSystem::addObserver(*this, MouseButtonsEvent::eventType);
		EventSystem::addObserver(*this, PauseEvent::eventType);

		m_lights.pointLights = std::make_shared<PointLightContainer>();


		this->load(m_inputMap);


		m_saveOnExit = true;

		m_gizmoRootNode = m_sceneGraph.addNode();

		m_arrowX = m_sceneGraph.addModel("Models/Arrows/DXarrowX.obj", m_gizmoRootNode);
		m_arrowY = m_sceneGraph.addModel("Models/Arrows/DXarrowY.obj", m_gizmoRootNode);
		m_arrowZ = m_sceneGraph.addModel("Models/Arrows/DXarrowZ.obj", m_gizmoRootNode);
		m_arrowX->localMatrix.setScale(4);
		m_arrowY->localMatrix.setScale(4);
		m_arrowZ->localMatrix.setScale(4);


		m_ringX = m_sceneGraph.addModel("Models/ring/red_ring.obj", m_gizmoRootNode);
		m_ringY = m_sceneGraph.addModel("Models/ring/green_ring.obj", m_gizmoRootNode);
		m_ringZ = m_sceneGraph.addModel("Models/ring/blue_ring.obj", m_gizmoRootNode);
		m_ringX->localMatrix.scale(2.0f);
		m_ringY->localMatrix.scale(2.15f);
		m_ringZ->localMatrix.scale(2.3f);

		m_sceneGraph.hideNode(m_gizmoRootNode, true);


		//create a pair of lightGizmo and entityRef to entitys with PointLightComponent and NodeComponent
		for (const auto& p : EntityReg::getComponentArray<PointLightComponent>())
		{
			if (auto n = EntityReg::getComponent<NodeComponent>(p.getEntityID()); n)
			{
				NodeHandle lightGizmo = m_sceneGraph.addModel("Models/smallInvNormSphere.obj", n->nodeHandel->getParentID());
				lightGizmo->localMatrix = n->nodeHandel->localMatrix;
				m_pointLightGizmoHandles.emplace_back(p.getKey() , lightGizmo, std::make_shared<Entity>(p.getEntity()));
			}
		}


	}

	void LevelEditor::load(std::string path)
	{
		if (path.empty())
			throw std::runtime_error("Path to savefolder was not given.");
		SceneSerializer::deSerialize(path, *this);
		ECSSerializer::deSerialize(path);
		ECSSerializer::reCoupleWithSceneGraph(m_sceneGraph);
		for (const auto& compL : EntityReg::getComponentArray<PointLightComponent>())
		{
			assert(!m_poinLightMap.contains(compL.getKey()));
			m_poinLightMap.insert_or_assign(compL.getKey(), PointLight(compL.position, compL.color, compL.strength));
			this->lights().pointLights->addPointLight(m_poinLightMap[compL.getKey()]);
		}
	}

	void LevelEditor::save(std::string path)
	{
		assert(!path.empty());
		if (path == "tempLevelEditorSave/")
		{
			Logger::getLogger().debugLog("[WARNING] LevelEditor::save(std::string path) uses default argument: [ " + path + " ], saves will be overitten next call.\n");
		}

		m_tempEntitis.clear();


		if (!m_outPutMapFile.empty() && m_saveOnExit)
		{

			if (m_gizmoRootNode.isValid()) m_sceneGraph.removeNode(m_gizmoRootNode);
			for (auto& p : m_pointLightGizmoHandles)
			{
				m_sceneGraph.removeNode(p.gizmoNode);
			}
			m_sceneGraph.packSceneGraph();
			ECSSerializer::reCoupleWithSceneGraph(m_sceneGraph);
			ECSSerializer::serialize(m_outPutMapFile);
			SceneSerializer::serialize(m_outPutMapFile, *this);
			m_pointLightGizmoHandles.clear();
		}


	}

	Camera& LevelEditor::camera()
	{
		return m_camera;
	}

	SceneGraph& LevelEditor::sceneGraph()
	{
		return m_sceneGraph;
	}

	Lights& LevelEditor::lights()
	{
		return m_lights;
	}

	void LevelEditor::update(double dt)
	{
		updateEntitysAndLights();



		m_camera.update(static_cast<float>(dt));
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
		m_nodeEditGui.view();
		m_entityEditGui.view();

		if (m_isTranslatingNode) translateSelectedNode();
		if (m_isRotatingNode) rotateSelectedNode(static_cast<float>(dt));

		updateGizmo();
	}

	void LevelEditor::deSelectNode()
	{
		deSelectTranslate();
		deSelectRotate();
		deSelectScale();
		m_selectedNode.invalidateHandle();
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
		Vector3 tangentViewSpace = m_camera.GetViewMatrix() * Vector4(tangentWorldSpace, 0);

		Vector2 tangentWithoutZ = Vector2(tangentViewSpace.x, tangentViewSpace.y);
		tangentWithoutZ.normalize();

		Vector2 onStartMousePointView = rayFromView(m_mouseCoordOnClick.x, m_mouseCoordOnClick.y);

		m_rotationInfo.accRawMouseDelta.x += dt * m_mouseState.deltaX;
		m_rotationInfo.accRawMouseDelta.y -= dt * m_mouseState.deltaY;

		Vector2 mousePointView = onStartMousePointView + m_rotationInfo.accRawMouseDelta;

		Vector3 closestPointOnLineView = rfm::closestPointOnLineFromPoint(Vector3(onStartMousePointView, 0), Vector3(onStartMousePointView + tangentWithoutZ, 0), Vector3(mousePointView, 0));
		float mouseTravelDistanceTangent = (onStartMousePointView - closestPointOnLineView).length();


		float signOfTravelDistance = dot(tangentWithoutZ, mousePointView - onStartMousePointView);
		signOfTravelDistance = static_cast<float>(sgn(signOfTravelDistance));

		mouseTravelDistanceTangent *= 2.0f; //more speed

		float angle = (mouseTravelDistanceTangent - m_rotationInfo.lastFrameRotationAngle);

		Transform parentRotMatrixInv = inverse(m_selectedNode.getParentWorldMatrix().getRotationMatrix());

		Matrix deltaRotation = rotationMatrixFromNormal(normalWorldSpace, signOfTravelDistance * angle);
		Transform rot = deltaRotation * m_selectedNode->worldMatrix.getRotationMatrix();
		m_selectedNode->localMatrix.setRotation(parentRotMatrixInv * rot);
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

		m_sceneGraph.hideNode(m_arrowX, false);
		m_sceneGraph.hideNode(m_arrowY, false);
		m_sceneGraph.hideNode(m_arrowZ, false);

		m_selectedReferencSystem = referenceSystem;
	}

	void LevelEditor::deSelectTranslate()
	{
		m_translationIsSelected = false;
		stopTranslate();

		m_sceneGraph.hideNode(m_arrowX, true);
		m_sceneGraph.hideNode(m_arrowY, true);
		m_sceneGraph.hideNode(m_arrowZ, true);
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

		m_sceneGraph.hideNode(m_ringX, false);
		m_sceneGraph.hideNode(m_ringY, false);
		m_sceneGraph.hideNode(m_ringZ, false);

		m_selectedReferencSystem = referenceSystem;
	}

	void LevelEditor::deSelectRotate()
	{
		m_rotationIsSelected = false;
		stopRotate();

		m_sceneGraph.hideNode(m_ringX, true);
		m_sceneGraph.hideNode(m_ringY, true);
		m_sceneGraph.hideNode(m_ringZ, true);
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

		Vector4 planeOfRotation = rfm::planeFromPointNormal(m_selectedNode->worldMatrix.getTranslation(), rotationNormalWorldSpace);

		Vector3 mouseIntersectRotationPlaneOnStartRotate = intersectionWithPlaneFromMouseCursor(
			planeOfRotation, m_camera, m_mouseState);

		Vector3 clickPoint = mouseIntersectRotationPlaneOnStartRotate - m_selectedNode->worldMatrix.getTranslation();
		//clickPoint = XMVector3Normalize(clickPoint);

		Vector3 tangentWorldSpace = cross(rotationNormalWorldSpace, clickPoint);
		tangentWorldSpace.normalize();

		m_rotationInfo.tangentWorldSpace = tangentWorldSpace;
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
		if (m_arrowX.isValid() && m_arrowX->getID() == id) //click in arrow
		{
			Logger::getLogger().debugLog("klick on arrowX\n");
			translationDirection = m_selectedNode->localMatrix.right();
			translationDirection = { 1,0,0 };
			clickedOnArrow = true;
		}
		else if (m_arrowY.isValid() && m_arrowY->getID() == id) //click in arrow
		{
			Logger::getLogger().debugLog("klick on arrowY\n");
			translationDirection = m_selectedNode->localMatrix.up();
			translationDirection = { 0,1,0 };
			clickedOnArrow = true;
		}
		else if (m_arrowZ.isValid() && m_arrowZ->getID() == id) //click in arrow
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
		Vector3 rotationNormal;
		if (m_ringX.isValid() && m_ringX->getID() == id)
		{
			Logger::getLogger().debugLog("klick on ringX\n");
			rotationNormal = { 1,0,0 };
			clickedRing = LevelEditor::GizmoXYZ::X;
		}
		else if (m_ringY.isValid() && m_ringY->getID() == id)
		{
			Logger::getLogger().debugLog("klick on ringY\n");
			rotationNormal = { 0,1,0 };

			clickedRing = LevelEditor::GizmoXYZ::Y;
		}
		else if (m_ringZ.isValid() && m_ringZ->getID() == id)
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

	void LevelEditor::selectFromMousePicking(NodeID clickedID)
	{
		Logger::getLogger().debugLog("selectFromMousePicking, pickEvent: " + std::to_string(clickedID) + "\n");

		if (clickedID == 0) 
		{
			return;
		}

		NodeHandle temp = NodeHandle(m_sceneGraph, clickedID);
		if (temp.isValid() && **temp) //*NodeHandle returns Node, *Node returns optional<SubModel>
		{
			//find the root of the model and get the parent
			NodeID tempParent = temp->getParentID();
			while (!m_sceneGraph.getNodes()[tempParent].isParentToModel())
			{
				tempParent = m_sceneGraph.getNodes()[tempParent].getParentID();
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
			else if (!(m_selectedNode.isValid() && m_selectedNode->getID() == tempParent))//clicked on unselected
			{
				deSelectTranslate();
				deSelectRotate();
				deSelectScale();
				m_entityEditGui.deSelectEntity();

				m_selectedNode = NodeHandle(m_sceneGraph, tempParent);

				//select entity from node clicked on
				bool nodeHasEntity = m_entityEditGui.trySelectEntityFromNode(m_selectedNode);
				if (!nodeHasEntity)
				{
					//pointlight gizmo have sibling nodes
					for (auto& p : m_pointLightGizmoHandles)
					{
						if (p.gizmoNode == m_selectedNode)
						{
							m_entityEditGui.selectEntity(p.entityRef);
							nodeHasEntity = true;
							break;
						}
					}
				}

				m_nodeEditGui = NodeEditGUI(m_selectedNode, nodeHasEntity,
					std::bind(&LevelEditor::selectTranslateFromGui, this, std::placeholders::_1),
					std::bind(&LevelEditor::selectRotateFromGui, this, std::placeholders::_1),
					std::bind(&LevelEditor::selectScale, this),
					std::bind(&LevelEditor::deSelectNode, this));

				m_nodeEditGui.selectedTRS();
			}
		}
	}



	void LevelEditor::updateEntitysAndLights()
	{
		//if entity is empty it has been deleted and the light should also be deleted
		auto& plMap = m_poinLightMap;
		auto&& removeLight = [&](auto& l)
		{
			if (l.entityRef->empty())
			{
				l.gizmoNode.removeNode();
				assert(plMap.contains(l.lightKey));
				plMap[l.lightKey].setOnOff(false); // this does not remove the pointlight from the pointlightContainer or structuredbuffer submitted to gpu
				plMap.erase(l.lightKey);
				return true;
			}
			return false;
		};
		m_pointLightGizmoHandles.erase(std::remove_if(
			m_pointLightGizmoHandles.begin(),
			m_pointLightGizmoHandles.end(),
			[&plMap, &removeLight](auto& l) { return removeLight(l); }),
			m_pointLightGizmoHandles.end());


		//move pointlights to lightGizmo
		for (auto& l : m_pointLightGizmoHandles)
		{
			assert(!l.entityRef->empty());
			assert(l.entityRef->getComponent<NodeComponent>());
			NodeComponent* lightNode = l.entityRef->getComponent<NodeComponent>();
			lightNode->nodeHandel->localMatrix = l.gizmoNode->localMatrix;
			
		}

		for (auto& pcComp : EntityReg::getComponentArray<PointLightComponent>())
		{
			if (auto nodeComp = EntityReg::getComponent<NodeComponent>(pcComp.getEntityID()); nodeComp)
			{
				//assert(m_poinLightMap.contains(pcComp.getKey()));
				if (m_poinLightMap.contains(pcComp.getKey()))
				{
					auto& pointLight = m_poinLightMap[pcComp.getKey()];

					pcComp.position = nodeComp->nodeHandel->worldMatrix.getTranslation();
					pointLight.setPosition(pcComp.position);
				}
			}
		}
	}

	void LevelEditor::updateGizmo()
	{
		if (m_gizmoRootNode.isValid() && m_selectedNode.isValid())
		{
			m_sceneGraph.updateWorldMatrices();
			m_gizmoRootNode->localMatrix.setTranslation(m_selectedNode->worldMatrix.getTranslation());
			Transform refSys = getRefSysFromEnum(m_selecteRefSysEnum);
			m_gizmoRootNode->localMatrix.setRotation(refSys.getRotationMatrix());
		}
	}

	Vector3 LevelEditor::rayFromView(float x, float y)
	{
		return rayFromMouse(x, y, static_cast<float>(m_mouseState.windowWidth), static_cast<float>(m_mouseState.windowHeight), m_camera.GetPerspective());
	}

	void LevelEditor::onEvent(const Event& e)
	{
		if (e.type() == PauseEvent::eventType)
		{
			if (static_cast<const PauseEvent&>(e).m_isPaused)
			{
				m_acceptMousePickingEvent = true;
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			}
			else
			{
				m_acceptMousePickingEvent = false;
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
			}
		}

		if (e.type() == MouseButtonsEvent::eventType)
		{
			auto& mouse = static_cast<const MouseButtonsEvent&>(e).mouseState;
			m_mouseState = mouse;
			if (mouse.LMBClicked)
			{
				m_mouseCoordOnClick.x = static_cast<float>(mouse.x);
				m_mouseCoordOnClick.y = static_cast<float>(mouse.y);

				EventSystem::postTOQueue(MousePickingRequestEvent(mouse.x, mouse.y));
				m_acceptMousePickingEvent = true;
			}
			if (mouse.LMBHeld)
			{
				m_mouseCoord.x = static_cast<float>(mouse.x);
				m_mouseCoord.y = static_cast<float>(mouse.y);
				m_winWidth = mouse.windowWidth;
				m_winHeight = mouse.windowHeight;
			}
			if (mouse.LMBReleased)
			{
				stopTranslate();
				stopRotate();
				stopScaling();
			}
		}


		if (e.type() == MousePickingEvent::eventType && m_acceptMousePickingEvent)
		{
			m_acceptMousePickingEvent = false;
			NodeID nodeID = static_cast<const MousePickingEvent&>(e).nodeID;
			selectFromMousePicking(nodeID);
		}

		if (e.type() == KeyboardEvent::eventType)
		{
			auto& keyboard = static_cast<const KeyboardEvent&>(e);
			if (keyboard.keyAndState.state == KeyState::KEY_CLICKED)
			{
				if ((*keyboard.boardState)[Key::LCTRL] == KeyState::KEY_DOWN)
				{
					switch (keyboard.keyAndState.key)
					{
					case Key::S:
						//this->save();
						break;
					case Key::Y:
						break;
					case Key::Z:
						break;
					default:
						break;
					}
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



	Vector3 closestPointOnLineFromMouseCursor(Vector3 lineDirection, Vector3 pointOnLine, const Camera& cam, MouseState ms)
	{
		Vector3 ray = rayFromMouse(static_cast<float>(ms.x), static_cast<float>(ms.y), static_cast<float>(ms.windowWidth), static_cast<float>(ms.windowHeight), cam.GetPerspective());

		Vector3 V2 = cam.GetWorldMatrix() * Vector4(ray, 0);
		auto [CP1, CP2] = rfm::closestPointsBetweenLines(lineDirection, pointOnLine, V2, cam.GetPosition());
		return CP1;
	}
	Vector3 intersectionWithPlaneFromMouseCursor(Vector4 plane, const Camera& cam, MouseState ms)
	{
		Vector3 ray = rayFromMouse(static_cast<float>(ms.x), static_cast<float>(ms.y), static_cast<float>(ms.windowWidth), static_cast<float>(ms.windowHeight), cam.GetPerspective());
		Vector3 rayDirInWorldSpace = cam.GetWorldMatrix() * Vector4(ray, 0);

		rayDirInWorldSpace.normalize(); //not needed, i think

		return rfm::planeIntersectLine(plane, cam.GetPosition(), cam.GetPosition() + rayDirInWorldSpace);
	}
}

