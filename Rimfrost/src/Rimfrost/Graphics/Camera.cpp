#include "rfpch.hpp"

#include "Camera.hpp"
#include <cstring>
#include <cmath>
#include <EventSystem.hpp>
#include "Rimfrost\EventSystem\KeyboardEvent.hpp"
#include "Rimfrost\EventSystem\MouseEvent.hpp"
using namespace DirectX;

namespace Rimfrost
{
	Camera::Camera()
	{
		EventSystem::addObserver(*this, KeyboardEvent::eventType);
		EventSystem::addObserver(*this, MouseMoveEvent::eventType);

		XMFLOAT4X4 pt;
		XMStoreFloat4x4(&pt, XMMatrixPerspectiveFovLH(XM_PIDIV4, 16.0f / 9.0f, 0.01f, 1000.0f));

		m_perspective = Matrix((float*)pt.m);
		m_worldMatrix = Transform();
	}

	void Camera::lockTranslation(bool status)
	{
		m_lockTranslation = status;
	}

	void Camera::lockRotation(bool status)
	{
		m_lockRotation = status;
	}

	void Camera::update(float dt)
	{
		if (!m_lockTranslation)
		{
			m_moveDirection.normalize();
			MoveInLocalSpace(dt * m_moveSpeed, m_moveDirection);
		}
		m_moveSpeed = m_normalSpeed;
		m_moveDirection = { 0,0,0 };

		if (!m_lockRotation)
		{
			float rotationSpeed = m_cameraRotationSpeed * dt;
			AddPitchAndYaw(m_deltaPitch * rotationSpeed, m_deltaYaw * rotationSpeed);
			SetAbsoluteEulerRotatation(GetPitch(), GetYaw(), GetRoll());
		}
		m_deltaPitch = 0;
		m_deltaYaw = 0;
	}

	void Camera::SetPosition(Vector3 newPosition)
	{
		m_worldMatrix.setTranslation(newPosition);
	}

	void Camera::SetWorldMatrix(Matrix newWorldMatrix)
	{
		m_worldMatrix = newWorldMatrix;
	}

	void Camera::SetAbsoluteEulerRotatation(float x, float y, float z)
	{
		m_worldMatrix.setRotation(x, y, z);

	}

	void Camera::ApplyEulerRotatation()
	{
		this->SetAbsoluteEulerRotatation(this->m_pitch, this->m_yaw, this->m_roll);
	}

	void Camera::AddPitch(float deltaPitch)
	{
		this->m_pitch += deltaPitch;
		if (this->m_pitch >= XM_PIDIV2)
			this->m_pitch = XM_PIDIV2 - 0.0001f;
		if (this->m_pitch <= -XM_PIDIV2)
			this->m_pitch = -XM_PIDIV2 + 0.0001f;
	}

	void Camera::AddYaw(float deltaYaw)
	{
		this->m_yaw += deltaYaw;
		this->m_yaw = fmod(this->m_yaw, 2.0f * XM_PI);
	}

	void Camera::AddRoll(float deltaRoll)
	{
		this->m_roll += deltaRoll;
	}

	void Camera::AddPitchAndYaw(float deltaPitch, float deltaYaw)
	{
		this->AddPitch(deltaPitch);
		this->AddYaw(deltaYaw);
	}


	void Camera::MoveInLocalSpace(const float& distance, const Vector3& direction)
	{
		/*XMVECTOR dir = { direction.x, direction.y, direction.z , 0.0f };
		dir = XMVector4Normalize(dir);
		dir = XMVector4Transform(dir, this->GetWorldMatrix());
		dir = XMVector4Normalize(dir);
		this->m_worldMatrix._41 = this->m_worldMatrix._41 + distance * dir.m128_f32[0];
		this->m_worldMatrix._42 = this->m_worldMatrix._42 + distance * dir.m128_f32[1];
		this->m_worldMatrix._43 = this->m_worldMatrix._43 + distance * dir.m128_f32[2];*/
		Vector3 dir = direction;
		dir.normalize();

		dir = m_worldMatrix * Vector4(dir, 0);
		dir.normalize();
		m_worldMatrix.translate(distance * dir);
	}

	//void Camera::MoveInWorldSpace(const float& distance, const DirectX::XMVECTOR& direction)
	//{/*
	//	XMVECTOR dir = direction;
	//	dir = XMVector4Normalize(dir);
	//	this->m_worldMatrix._41 = this->m_worldMatrix._41 + distance * dir.m128_f32[0];
	//	this->m_worldMatrix._42 = this->m_worldMatrix._42 + distance * dir.m128_f32[1];
	//	this->m_worldMatrix._43 = this->m_worldMatrix._43 + distance * dir.m128_f32[2];*/
	//}

	Matrix Camera::GetWorldMatrix() const
	{
		return this->m_worldMatrix;
	}

	Matrix Camera::GetViewMatrix() const
	{
		return inverse(m_worldMatrix);
	}

	Matrix Camera::GetPerspective() const
	{
		return m_perspective;
	}

	Vector3 Camera::GetPosition() const
	{
		return m_worldMatrix.getTranslation();
	}

	Vector3 Camera::GetAxisX() const
	{
		return m_worldMatrix.right();
	}

	Vector3 Camera::GetAxisY() const
	{
		return m_worldMatrix.up();
	}

	Vector3 Camera::GetAxisZ() const
	{
		return m_worldMatrix.forward();
	}

	/*float Camera::GetX() const
	{
		return this->m_worldMatrix._41;
	}
	float Camera::GetY() const
	{
		return this->m_worldMatrix._42;
	}
	float Camera::GetZ() const
	{
		return this->m_worldMatrix._43;
	}*/

	float Camera::GetPitch() const
	{
		return this->m_pitch;
	}

	float Camera::GetYaw() const
	{
		return this->m_yaw;
	}

	float Camera::GetRoll() const
	{
		return this->m_roll;
	}
	void Camera::onEvent(const Rimfrost::Event& e)
	{
		if (e.type() == MouseMoveEvent::eventType)
		{
			auto& mouse = static_cast<const MouseMoveEvent&>(e);
			m_deltaPitch = mouse.mouseState.deltaY;
			m_deltaYaw = mouse.mouseState.deltaX;
		}
		if (e.type() == KeyboardEvent::eventType)
		{
			auto& keyboard = static_cast<const KeyboardEvent&>(e);
			if (keyboard.keyAndState.second == KeyState::KEY_DOWN)
			{
				switch (keyboard.keyAndState.first)
				{
				case Key::W:
					m_moveDirection += {0, 0, 1};
					break;
				case Key::A:
					m_moveDirection += {-1, 0, 0};
					break;
				case Key::S:
					m_moveDirection += {0, 0, -1};
					break;
				case Key::D:
					m_moveDirection += {1, 0, 0};
					break;
				case Key::SPACE:
					m_moveDirection += GetViewMatrix() * Vector4(0, 1, 0, 0);
					break;
				case Key::LCTRL:
					m_moveDirection += GetViewMatrix() * Vector4(0, -1, 0, 0);
					break;
				case Key::LSHIFT:
					m_moveSpeed = m_fastSpeed;
					break;
				default:
					break;
				}
			}
		}
	}
};