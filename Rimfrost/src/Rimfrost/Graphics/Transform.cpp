#include "pch.hpp"
#include "Transform.hpp"

using namespace DirectX;

namespace Engine1
{

	Transform::Transform()
	{
		m_matrix = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	}

	//Transform::Transform(float x, float y, float z) : Transform()
	//{
	//	setPosition(x, y, z);
	//}

	/*Transform::Transform(const XMVECTOR& position) : Transform()
	{
		setPosition(position);
	}

	Transform::Transform(const XMFLOAT3& position) : Transform()
	{
		setPosition(position);
	}*/

	Transform::Transform(const XMMATRIX& other)
	{
		XMStoreFloat4x4(&m_matrix, other);
	}

	Transform::Transform(const XMFLOAT4X4& other)
	{
		m_matrix = other;
	}

	Transform::~Transform()
	{
	}

	void Transform::operator=(const XMMATRIX& other)
	{
		XMStoreFloat4x4(&m_matrix, other);
	}

	void Transform::operator=(const XMFLOAT4X4& other)
	{
		m_matrix = other;
	}

	void Transform::operator*=(const Transform& other)
	{
		*this = *this * other; //it would be more practical to have *this = other * *this;
	}

	XMFLOAT4X4& Transform::operator*()
	{
		return this->m_matrix;
	}

	void Transform::setPosition(float x, float y, float z)
	{
		m_matrix._41 = x;
		m_matrix._42 = y;
		m_matrix._43 = z;
	}

	void Transform::setPosition(const XMVECTOR& position)
	{
		XMFLOAT3 temp;
		XMStoreFloat3(&temp, position);
		m_matrix._41 = temp.x;
		m_matrix._42 = temp.y;
		m_matrix._43 = temp.z;
	}

	void Transform::setPosition(const XMFLOAT3& position)
	{
		m_matrix._41 = position.x;
		m_matrix._42 = position.y;
		m_matrix._43 = position.z;
	}

	void Transform::setRotation(const XMVECTOR& rotation)
	{
		auto [S, R, T] = decomposeToSRT();
		R = XMMatrixRotationRollPitchYawFromVector(rotation);

		*this = S * R * T;
	}

	void Transform::setRotation(float x, float y, float z)
	{
		setRotation({ x,y,z });
	}

	void Transform::setRotationDeg(float x, float y, float z)
	{
		setRotation(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z));
	}

	void Transform::setRotation(Transform rotationMatrix)
	{
		auto [S, R, T] = decomposeToSRT();
		R = rotationMatrix;

		*this = S * R * T;
	}

	void Transform::rotate(const XMVECTOR& rotation)
	{
		auto [S, R, T] = decomposeToSRT();
		R *= XMMatrixRotationRollPitchYawFromVector(rotation);

		*this = S * R * T;
	}

	void Transform::rotate(float x, float y, float z)
	{
		rotate({ x,y,z });
	}

	void Transform::rotateDeg(float x, float y, float z)
	{
		rotate(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z));
	}



	void Transform::scale(float scale)
	{
		this->scale(scale, scale, scale);
	}

	void Transform::scale(float x, float y, float z)
	{
		auto [S, R, T] = decomposeToSRT();
		S *= XMMatrixScaling(x, y, z);

		*this = S * R * T;
	}

	void Transform::setScale(float scale)
	{
		setScale(scale, scale, scale);
	}

	void Transform::setScale(float x, float y, float z)
	{
		auto [S, R, T] = decomposeToSRT();
		S = XMMatrixScaling(x, y, z);

		*this = S * R * T;
	}

	std::tuple<Transform, Transform, Transform> Transform::decomposeToSRT() const
	{
		XMVECTOR scale, rotationQuat, translation;
		XMMatrixDecompose(&scale, &rotationQuat, &translation, getXMMatrix());

		XMMATRIX S{ XMMatrixScalingFromVector(scale) };
		XMMATRIX R{ XMMatrixRotationQuaternion(rotationQuat) };
		XMMATRIX T{ XMMatrixTranslationFromVector(translation) };

		return std::make_tuple<Transform, Transform, Transform>(Transform(S), Transform(R), Transform(T));
	}

	std::tuple<Transform, Transform, Transform> Transform::decomposeToSRTInverse() const
	{
		auto [S, R, T] = decomposeToSRT();

		//inverse of scale
		(*S)._11 = 1.0f / (*S)._11;
		(*S)._22 = 1.0f / (*S)._22;
		(*S)._33 = 1.0f / (*S)._33;

		//inverse if rotation
		R = XMMatrixTranspose(R.getXMMatrix());

		//inverce of translation
		T.setPosition(-T.getPositionVector());
		return std::tuple<Transform, Transform, Transform>(S, R, T);
	}



	Transform Transform::getRotationMatrix() const
	{
		XMVECTOR scale, rotationQuat, translation;
		XMMatrixDecompose(&scale, &rotationQuat, &translation, getXMMatrix());

		XMMatrixRotationQuaternion(rotationQuat);
		return Transform(XMMatrixRotationQuaternion(rotationQuat));
	}

	void Transform::setZero()
	{
		m_matrix = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	}

	XMFLOAT3 Transform::getPositionFloat3() const
	{
		return { m_matrix._41, m_matrix._42, m_matrix._43 };
	}

	XMVECTOR Transform::getPositionVector() const
	{
		XMFLOAT3 temp{ m_matrix._41, m_matrix._42, m_matrix._43 };
		return XMLoadFloat3(&temp);
	}

	XMVECTOR Transform::getForward()
	{
		XMVECTOR e = XMVectorSet(m_matrix.m[2][0], m_matrix.m[2][1], m_matrix.m[2][2], 0);
		e = XMVector3Normalize(e);
		return e;
	}

	XMVECTOR Transform::getUp()
	{
		XMVECTOR e = XMVectorSet(m_matrix.m[1][0], m_matrix.m[1][1], m_matrix.m[1][2], 0);
		e = XMVector3Normalize(e);
		return e;
	}

	XMVECTOR Transform::getRight()
	{
		XMVECTOR e = XMVectorSet(m_matrix.m[0][0], m_matrix.m[0][1], m_matrix.m[0][2], 0);
		e = XMVector3Normalize(e);
		return e;
	}

	XMFLOAT3 Transform::getForwardF3()
	{
		XMFLOAT3 v;
		XMStoreFloat3(&v, getForward());
		return v;
	}

	XMFLOAT3 Transform::getUpF3()
	{
		XMFLOAT3 v;
		XMStoreFloat3(&v, getUp());
		return v;
	}

	XMFLOAT3 Transform::getRightF3()
	{
		XMFLOAT3 v;
		XMStoreFloat3(&v, getRight());
		return v;
	}

	XMMATRIX Transform::getXMMatrix() const
	{
		return XMLoadFloat4x4(&m_matrix);
	}

	Transform Transform::getInverse() const
	{
		XMMATRIX inverse = XMLoadFloat4x4(&this->m_matrix);
		XMVECTOR det = XMMatrixDeterminant(inverse);
		inverse = XMMatrixInverse(&det, inverse);

		return Transform(inverse);
	}

	Transform operator*(const Transform& l, const Transform& r)
	{
		return Transform(XMLoadFloat4x4(&l.m_matrix) * XMLoadFloat4x4(&r.m_matrix));
	}

	Transform operator*(const Transform& l, const XMMATRIX& r)
	{
		return Transform(XMLoadFloat4x4(&l.m_matrix) * r);
	}

	Transform operator*(const XMMATRIX& l, const Transform& r)
	{
		return Transform(l * XMLoadFloat4x4(&r.m_matrix));
	}

	Transform operator*(const Transform& l, const XMFLOAT4X4& r)
	{
		return Transform(XMLoadFloat4x4(&l.m_matrix) * XMLoadFloat4x4(&r));
	}

	Transform operator*(const XMFLOAT4X4& l, const Transform& r)
	{
		return Transform(XMLoadFloat4x4(&l) * XMLoadFloat4x4(&r.m_matrix));
	}
}