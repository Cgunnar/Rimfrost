#include "rfpch.hpp"

#include "Camera.hpp"
#include <cstring>
#include <cmath>
using namespace DirectX;

namespace Engine1
{
	Camera::Camera()
	{
		XMStoreFloat4x4(&this->m_worldMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&this->m_perspective, XMMatrixPerspectiveFovLH(XM_PIDIV4, 16.0f / 9.0f, 0.01f, 1000.0f));
	}

	Camera::~Camera()
	{
	}

	void Camera::update(float dt, const std::shared_ptr<Keyboard>& keyboard, const std::shared_ptr<Mouse>& mouse)
	{

		if (keyboard)
		{
			DirectX::XMVECTOR moveDirection = { 0,0,0,0 };
			DirectX::XMVECTOR moveUp = { 0,1,0,0 };
			if (keyboard->IsKeyPressed(DIK_A))
			{
				moveDirection += {-1, 0, 0, 0};
			}
			if (keyboard->IsKeyPressed(DIK_D))
			{
				moveDirection += {1, 0, 0, 0};
			}
			if (keyboard->IsKeyPressed(DIK_W))
			{
				moveDirection += {0, 0, 1, 0};
			}
			if (keyboard->IsKeyPressed(DIK_S))
			{
				moveDirection += {0, 0, -1, 0};
			}
			if (keyboard->IsKeyPressed(DIK_SPACE))
			{
				moveDirection += XMVector4Transform(moveUp, GetViewMatrix());
			}
			if (keyboard->IsKeyPressed(DIK_LCONTROL))
			{
				moveDirection += XMVector4Transform(-moveUp, GetViewMatrix());
			}
			moveDirection = XMVector4Normalize(moveDirection);
			if (keyboard->IsKeyPressed(DIK_LSHIFT))
			{
				MoveInLocalSpace(dt * 8.0f, moveDirection);
			}
			else
			{
				MoveInLocalSpace(dt * 2.5f, moveDirection);
			}
		}

		if (mouse)
		{
			float speed = 0.3f * dt;
			AddPitchAndYaw((float)mouse->GetMouseState().deltaY * speed, (float)mouse->GetMouseState().deltaX * speed);
			SetAbsoluteEulerRotatation(GetPitch(), GetYaw(), GetRoll());
		}
	}

	void Camera::SetPosition(XMFLOAT3 newPosition)
	{
		this->m_worldMatrix.m[3][0] = newPosition.x;
		this->m_worldMatrix.m[3][1] = newPosition.y;
		this->m_worldMatrix.m[3][2] = newPosition.z;
	}

	void Camera::SetOrientation(XMFLOAT3X3 newOrientation)
	{
		this->m_worldMatrix.m[0][0] = newOrientation.m[0][0];
		this->m_worldMatrix.m[0][1] = newOrientation.m[0][1];
		this->m_worldMatrix.m[0][2] = newOrientation.m[0][2];
		this->m_worldMatrix.m[1][0] = newOrientation.m[1][0];
		this->m_worldMatrix.m[1][1] = newOrientation.m[1][1];
		this->m_worldMatrix.m[1][2] = newOrientation.m[1][2];
		this->m_worldMatrix.m[2][0] = newOrientation.m[2][0];
		this->m_worldMatrix.m[2][1] = newOrientation.m[2][1];
		this->m_worldMatrix.m[2][2] = newOrientation.m[2][2];
	}

	void Camera::SetWorldMatrix(DirectX::XMFLOAT4X4 newWorldMatrix)
	{
		m_worldMatrix = newWorldMatrix;
	}

	void Camera::SetAbsoluteEulerRotatation(float x, float y, float z)
	{
		this->m_worldMatrix._11 = 1;
		this->m_worldMatrix._21 = 0;
		this->m_worldMatrix._31 = 0;

		this->m_worldMatrix._12 = 0;
		this->m_worldMatrix._22 = 1;
		this->m_worldMatrix._32 = 0;

		this->m_worldMatrix._13 = 0;
		this->m_worldMatrix._23 = 0;
		this->m_worldMatrix._33 = 1;


		XMMATRIX rot = XMMatrixRotationRollPitchYaw(x, y, z);
		XMMATRIX world = this->GetWorldMatrix();

		world = rot * world;


		XMStoreFloat4x4(&this->m_worldMatrix, world);

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


	void Camera::MoveInLocalSpace(const float& distance, const XMFLOAT3& direction)
	{
		XMVECTOR dir = { direction.x, direction.y, direction.z , 0.0f };
		dir = XMVector4Normalize(dir);
		dir = XMVector4Transform(dir, this->GetWorldMatrix());
		dir = XMVector4Normalize(dir);
		this->m_worldMatrix._41 = this->m_worldMatrix._41 + distance * dir.m128_f32[0];
		this->m_worldMatrix._42 = this->m_worldMatrix._42 + distance * dir.m128_f32[1];
		this->m_worldMatrix._43 = this->m_worldMatrix._43 + distance * dir.m128_f32[2];
	}

	void Camera::MoveInLocalSpace(const float& distance, const DirectX::XMVECTOR& direction)
	{
		XMVECTOR dir = direction;
		dir = XMVector4Normalize(dir);
		dir = XMVector4Transform(dir, this->GetWorldMatrix());
		dir = XMVector4Normalize(dir);
		this->m_worldMatrix._41 = this->m_worldMatrix._41 + distance * dir.m128_f32[0];
		this->m_worldMatrix._42 = this->m_worldMatrix._42 + distance * dir.m128_f32[1];
		this->m_worldMatrix._43 = this->m_worldMatrix._43 + distance * dir.m128_f32[2];
	}

	void Camera::MoveInWorldSpace(const float& distance, const DirectX::XMVECTOR& direction)
	{
		XMVECTOR dir = direction;
		dir = XMVector4Normalize(dir);
		this->m_worldMatrix._41 = this->m_worldMatrix._41 + distance * dir.m128_f32[0];
		this->m_worldMatrix._42 = this->m_worldMatrix._42 + distance * dir.m128_f32[1];
		this->m_worldMatrix._43 = this->m_worldMatrix._43 + distance * dir.m128_f32[2];
	}

	//void Camera::RotateInLocalSpace(const float& angle, const XMFLOAT3& localAxis)
	//{
	//	XMVECTOR worldAxis = XMLoadFloat3(&localAxis);
	//	worldAxis = XMVector4Transform(worldAxis, this->GetWorldMatrix());
	//	XMMATRIX rotM =	XMMatrixRotationAxis(worldAxis, angle);
	//	XMMATRIX translationM = XMMatrixTranslation(this->GetX(), this->GetY(), this->GetZ());
	//	XMMATRIX invTranslationM = XMMatrixInverse(nullptr, translationM);
	//	
	//	XMMATRIX worldM = this->GetWorldMatrix() * invTranslationM * rotM * translationM;
	//
	//	XMStoreFloat4x4(&this->worldMatrix, worldM);
	//}

	XMMATRIX Camera::GetWorldMatrix() const
	{
		return XMLoadFloat4x4(&this->m_worldMatrix);
	}

	XMMATRIX Camera::GetViewMatrix() const
	{
		XMMATRIX viewMatrix = XMLoadFloat4x4(&this->m_worldMatrix);
		XMVECTOR det = XMMatrixDeterminant(viewMatrix);
		viewMatrix = XMMatrixInverse(&det, viewMatrix);

		return viewMatrix;
	}

	DirectX::XMMATRIX Camera::GetPerspective() const
	{
		return XMLoadFloat4x4(&this->m_perspective);
	}

	XMVECTOR Camera::GetPosition() const
	{
		XMFLOAT4 vector = (XMFLOAT4)this->m_worldMatrix.m[3];
		vector.w = 1;
		return XMLoadFloat4(&vector);
	}

	XMVECTOR Camera::GetAxisX() const
	{
		XMVECTOR vec;
		vec.m128_f32[0] = this->m_worldMatrix.m[0][0];
		vec.m128_f32[1] = this->m_worldMatrix.m[0][1];
		vec.m128_f32[2] = this->m_worldMatrix.m[0][2];
		vec.m128_f32[3] = 0.0f;
		return vec;
	}

	XMVECTOR Camera::GetAxisY() const
	{
		XMVECTOR vec;
		vec.m128_f32[0] = this->m_worldMatrix.m[1][0];
		vec.m128_f32[1] = this->m_worldMatrix.m[1][1];
		vec.m128_f32[2] = this->m_worldMatrix.m[1][2];
		vec.m128_f32[3] = 0.0f;
		return vec;
	}

	XMVECTOR Camera::GetAxisZ() const
	{
		XMVECTOR vec;
		vec.m128_f32[0] = this->m_worldMatrix.m[2][0];
		vec.m128_f32[1] = this->m_worldMatrix.m[2][1];
		vec.m128_f32[2] = this->m_worldMatrix.m[2][2];
		vec.m128_f32[3] = 0.0f;
		return vec;
	}

	XMFLOAT3 Camera::FgetAxisZ() const
	{
		XMFLOAT3 vec;
		vec.x = this->m_worldMatrix.m[2][0];
		vec.y = this->m_worldMatrix.m[2][1];
		vec.z = this->m_worldMatrix.m[2][2];
		return vec;
	}

	float Camera::GetX() const
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
	}

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
}