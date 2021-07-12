#pragma once
#include <DirectXMath.h>
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "RimfrostMath.hpp"


#include "EventObserver.hpp"

namespace Rimfrost
{
	class Camera : public EventObserver
	{
	public:
		Camera();
		~Camera() = default;
		void lockTranslation(bool status);
		void lockRotation(bool status);
		void update(float dt);
		void SetPosition(Vector3 newPosition);
		//void SetOrientation(Matrix newOrientation);
		void SetWorldMatrix(Matrix newWorldMatrix);
		void SetAbsoluteEulerRotatation(float x, float y, float z);
		void ApplyEulerRotatation();
		void AddPitch(float deltaPitch);
		void AddYaw(float deltaYaw);
		void AddRoll(float deltaRoll);
		void AddPitchAndYaw(float deltaPitch, float deltaYaw);


		void MoveInLocalSpace(const float& distance, const Vector3& direction);

		//void MoveInWorldSpace(const float& distance, const DirectX::XMVECTOR& direction);

		//void RotateInLocalSpace(const float& angle, const XMFLOAT3& localAxis);

		Matrix GetWorldMatrix() const;
		Matrix GetViewMatrix() const;
		Matrix GetPerspective() const;

		Vector3 GetPosition() const;
		Vector3 GetAxisX() const;
		Vector3 GetAxisY() const;
		Vector3 GetAxisZ() const;
		/*float GetX() const;
		float GetY() const;
		float GetZ() const;*/

		float GetPitch() const;
		float GetYaw() const;
		float GetRoll() const;

		void onEvent(const Rimfrost::Event& e) override;

	private:
		bool m_lockRotation = false;
		bool m_lockTranslation = false;

		float m_cameraRotationSpeed = 0.3f;
		float m_deltaPitch = 0;
		float m_deltaYaw = 0;

		float m_pitch = 0;
		float m_yaw = 0;
		float m_roll = 0;


		Vector3 m_moveDirection = { 0,0,0 };
		float m_moveSpeed = 2.5f;
		float m_normalSpeed = 2.5f;
		float m_fastSpeed = 8.5f;

		Matrix m_perspective;
		Transform m_worldMatrix;
		
	};
}
