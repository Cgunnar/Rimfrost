#pragma once

#include <DirectXMath.h>

#include "Engine1.hpp"
namespace Engine1
{
	class Transform
	{
	public:
		Transform();
		//Transform(float x, float y, float z);
		//Transform(const DirectX::XMVECTOR& position);
		//Transform(const DirectX::XMFLOAT3& position);
		Transform(const DirectX::XMMATRIX& other);
		Transform(const DirectX::XMFLOAT4X4& other);
		~Transform();
		void operator=(const DirectX::XMMATRIX& other);
		void operator=(const DirectX::XMFLOAT4X4& other);
		void operator *=(const Transform& other);
		DirectX::XMFLOAT4X4& operator*();

		void setPosition(float x, float y, float z);
		void setPosition(const DirectX::XMVECTOR& position);
		void setPosition(const DirectX::XMFLOAT3& position);

		void setRotation(float x, float y, float z);
		void setRotation(const DirectX::XMVECTOR& rotation);
		void setRotationDeg(float x, float y, float z);
		void setRotation(Transform rotationMatrix);

		void rotate(const DirectX::XMVECTOR& rotation);
		void rotate(float x, float y, float z);
		void rotateDeg(float x, float y, float z);

		void scale(float scale);
		void scale(float x, float y, float z);
		void setScale(float scale);
		void setScale(float x, float y, float z);

		std::tuple<Transform, Transform, Transform> decomposeToSRT() const;
		std::tuple<Transform, Transform, Transform> decomposeToSRTInverse() const;

		

		Transform getRotationMatrix() const;

		void setZero();

		DirectX::XMFLOAT3 getPositionFloat3() const;
		DirectX::XMVECTOR getPositionVector() const;

		DirectX::XMVECTOR getForward();
		DirectX::XMVECTOR getUp();
		DirectX::XMVECTOR getRight();

		DirectX::XMFLOAT3 getForwardF3();
		DirectX::XMFLOAT3 getUpF3();
		DirectX::XMFLOAT3 getRightF3();

		DirectX::XMMATRIX getXMMatrix() const;

		Transform getInverse() const;

		DirectX::XMFLOAT4X4 m_matrix;
	};

	Transform operator *(const Transform& l, const Transform& r);

	Transform operator *(const Transform& l, const DirectX::XMMATRIX& r);
	Transform operator *(const DirectX::XMMATRIX& l, const Transform& r);

	Transform operator *(const Transform& l, const DirectX::XMFLOAT4X4& r);
	Transform operator *(const DirectX::XMFLOAT4X4& l, const Transform& r);

}