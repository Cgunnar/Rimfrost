#pragma once
#include <DirectXMath.h>
#include "Mouse.hpp"
#include "Keyboard.hpp"

namespace Engine1
{
	class Camera
	{
	public:
		Camera();
		~Camera();
		void update(float dt, const std::shared_ptr<Keyboard>& keyboard = nullptr, const std::shared_ptr<Mouse>& mouse = nullptr);
		void SetPosition(DirectX::XMFLOAT3 newPosition);
		void SetOrientation(DirectX::XMFLOAT3X3 newOrientation);
		void SetWorldMatrix(DirectX::XMFLOAT4X4 newWorldMatrix);
		void SetAbsoluteEulerRotatation(float x, float y, float z);
		void ApplyEulerRotatation();
		void AddPitch(float deltaPitch);
		void AddYaw(float deltaYaw);
		void AddRoll(float deltaRoll);
		void AddPitchAndYaw(float deltaPitch, float deltaYaw);


		void MoveInLocalSpace(const float& distance, const DirectX::XMFLOAT3& direction);
		void MoveInLocalSpace(const float& distance, const DirectX::XMVECTOR& direction);

		void MoveInWorldSpace(const float& distance, const DirectX::XMVECTOR& direction);

		//void RotateInLocalSpace(const float& angle, const XMFLOAT3& localAxis);

		DirectX::XMMATRIX GetWorldMatrix() const;
		DirectX::XMMATRIX GetViewMatrix() const;
		DirectX::XMMATRIX GetPerspective() const;

		DirectX::XMVECTOR GetPosition() const;
		DirectX::XMVECTOR GetAxisX() const;
		DirectX::XMVECTOR GetAxisY() const;
		DirectX::XMVECTOR GetAxisZ() const;
		DirectX::XMFLOAT3 FgetAxisZ() const;
		float GetX() const;
		float GetY() const;
		float GetZ() const;

		float GetPitch() const;
		float GetYaw() const;
		float GetRoll() const;

	private:


		DirectX::XMFLOAT4X4 m_perspective;
		DirectX::XMFLOAT4X4 m_worldMatrix;
		float m_pitch = 0;
		float m_yaw = 0;
		float m_roll = 0;
	};
}
