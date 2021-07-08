#pragma once

#include <DirectXMath.h>

#include "Engine1.hpp"
namespace Engine1
{
	class TransformOld
	{
	public:
		TransformOld();
		//Transform(float x, float y, float z);
		//Transform(const DirectX::XMVECTOR& position);
		//Transform(const DirectX::XMFLOAT3& position);
		TransformOld(const DirectX::XMMATRIX& other);
		TransformOld(const DirectX::XMFLOAT4X4& other);
		~TransformOld();
		void operator=(const DirectX::XMMATRIX& other);
		void operator=(const DirectX::XMFLOAT4X4& other);
		void operator *=(const TransformOld& other);
		DirectX::XMFLOAT4X4& operator*();

		void setPosition(float x, float y, float z);
		void setPosition(const DirectX::XMVECTOR& position);
		void setPosition(const DirectX::XMFLOAT3& position);

		void setRotation(float x, float y, float z);
		void setRotation(const DirectX::XMVECTOR& rotation);
		void setRotationDeg(float x, float y, float z);
		void setRotation(TransformOld rotationMatrix);

		void rotate(const DirectX::XMVECTOR& rotation);
		void rotate(float x, float y, float z);
		void rotateDeg(float x, float y, float z);

		void scale(float scale);
		void scale(float x, float y, float z);
		void setScale(float scale);
		void setScale(float x, float y, float z);

		std::tuple<TransformOld, TransformOld, TransformOld> decomposeToSRT() const;
		std::tuple<TransformOld, TransformOld, TransformOld> decomposeToSRTInverse() const;

		

		TransformOld getRotationMatrix() const;

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

		TransformOld getInverse() const;

		DirectX::XMFLOAT4X4 m_matrix;
	};

	TransformOld operator *(const TransformOld& l, const TransformOld& r);

	TransformOld operator *(const TransformOld& l, const DirectX::XMMATRIX& r);
	TransformOld operator *(const DirectX::XMMATRIX& l, const TransformOld& r);

	TransformOld operator *(const TransformOld& l, const DirectX::XMFLOAT4X4& r);
	TransformOld operator *(const DirectX::XMFLOAT4X4& l, const TransformOld& r);

}