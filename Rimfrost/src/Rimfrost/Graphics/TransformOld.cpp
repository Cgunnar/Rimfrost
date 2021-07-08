#include "rfpch.hpp"
#include "TransformOld.hpp"

using namespace DirectX;

namespace Engine1
{

	TransformOld::TransformOld()
	{
		m_matrix = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	}

	TransformOld::TransformOld(const XMMATRIX& other)
	{
		XMStoreFloat4x4(&m_matrix, other);
	}

	TransformOld::TransformOld(const XMFLOAT4X4& other)
	{
		m_matrix = other;
	}

	TransformOld::~TransformOld()
	{
	}

	void TransformOld::operator=(const XMMATRIX& other)
	{
		XMStoreFloat4x4(&m_matrix, other);
	}

	void TransformOld::operator=(const XMFLOAT4X4& other)
	{
		m_matrix = other;
	}

	void TransformOld::operator*=(const TransformOld& other)
	{
		*this = *this * other; //it would be more practical to have *this = other * *this;
	}

	XMFLOAT4X4& TransformOld::operator*()
	{
		return this->m_matrix;
	}

	void TransformOld::setPosition(float x, float y, float z)
	{
		m_matrix._41 = x;
		m_matrix._42 = y;
		m_matrix._43 = z;
	}

	void TransformOld::setPosition(const XMVECTOR& position)
	{
		XMFLOAT3 temp;
		XMStoreFloat3(&temp, position);
		m_matrix._41 = temp.x;
		m_matrix._42 = temp.y;
		m_matrix._43 = temp.z;
	}

	void TransformOld::setPosition(const XMFLOAT3& position)
	{
		m_matrix._41 = position.x;
		m_matrix._42 = position.y;
		m_matrix._43 = position.z;
	}

	void TransformOld::setRotation(const XMVECTOR& rotation)
	{
		auto [S, R, T] = decomposeToSRT();
		R = XMMatrixRotationRollPitchYawFromVector(rotation);

		*this = S * R * T;
	}

	void TransformOld::setRotation(float x, float y, float z)
	{
		setRotation({ x,y,z });
	}

	void TransformOld::setRotationDeg(float x, float y, float z)
	{
		setRotation(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z));
	}

	void TransformOld::setRotation(TransformOld rotationMatrix)
	{
		auto [S, R, T] = decomposeToSRT();
		R = rotationMatrix;

		*this = S * R * T;
	}

	void TransformOld::rotate(const XMVECTOR& rotation)
	{
		auto [S, R, T] = decomposeToSRT();
		R *= XMMatrixRotationRollPitchYawFromVector(rotation);

		*this = S * R * T;
	}

	void TransformOld::rotate(float x, float y, float z)
	{
		rotate({ x,y,z });
	}

	void TransformOld::rotateDeg(float x, float y, float z)
	{
		rotate(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z));
	}



	void TransformOld::scale(float scale)
	{
		this->scale(scale, scale, scale);
	}

	void TransformOld::scale(float x, float y, float z)
	{
		auto [S, R, T] = decomposeToSRT();
		S *= XMMatrixScaling(x, y, z);

		*this = S * R * T;
	}

	void TransformOld::setScale(float scale)
	{
		setScale(scale, scale, scale);
	}

	void TransformOld::setScale(float x, float y, float z)
	{
		auto [S, R, T] = decomposeToSRT();
		S = XMMatrixScaling(x, y, z);

		*this = S * R * T;
	}

	std::tuple<TransformOld, TransformOld, TransformOld> TransformOld::decomposeToSRT() const
	{
		XMVECTOR scale, rotationQuat, translation;
		XMMatrixDecompose(&scale, &rotationQuat, &translation, getXMMatrix());

		XMMATRIX S{ XMMatrixScalingFromVector(scale) };
		XMMATRIX R{ XMMatrixRotationQuaternion(rotationQuat) };
		XMMATRIX T{ XMMatrixTranslationFromVector(translation) };

		return std::make_tuple<TransformOld, TransformOld, TransformOld>(TransformOld(S), TransformOld(R), TransformOld(T));
	}

	std::tuple<TransformOld, TransformOld, TransformOld> TransformOld::decomposeToSRTInverse() const
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
		return std::tuple<TransformOld, TransformOld, TransformOld>(S, R, T);
	}



	TransformOld TransformOld::getRotationMatrix() const
	{
		XMVECTOR scale, rotationQuat, translation;
		XMMatrixDecompose(&scale, &rotationQuat, &translation, getXMMatrix());

		XMMatrixRotationQuaternion(rotationQuat);
		return TransformOld(XMMatrixRotationQuaternion(rotationQuat));
	}

	void TransformOld::setZero()
	{
		m_matrix = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	}

	XMFLOAT3 TransformOld::getPositionFloat3() const
	{
		return { m_matrix._41, m_matrix._42, m_matrix._43 };
	}

	XMVECTOR TransformOld::getPositionVector() const
	{
		XMFLOAT3 temp{ m_matrix._41, m_matrix._42, m_matrix._43 };
		return XMLoadFloat3(&temp);
	}

	XMVECTOR TransformOld::getForward()
	{
		XMVECTOR e = XMVectorSet(m_matrix.m[2][0], m_matrix.m[2][1], m_matrix.m[2][2], 0);
		e = XMVector3Normalize(e);
		return e;
	}

	XMVECTOR TransformOld::getUp()
	{
		XMVECTOR e = XMVectorSet(m_matrix.m[1][0], m_matrix.m[1][1], m_matrix.m[1][2], 0);
		e = XMVector3Normalize(e);
		return e;
	}

	XMVECTOR TransformOld::getRight()
	{
		XMVECTOR e = XMVectorSet(m_matrix.m[0][0], m_matrix.m[0][1], m_matrix.m[0][2], 0);
		e = XMVector3Normalize(e);
		return e;
	}

	XMFLOAT3 TransformOld::getForwardF3()
	{
		XMFLOAT3 v;
		XMStoreFloat3(&v, getForward());
		return v;
	}

	XMFLOAT3 TransformOld::getUpF3()
	{
		XMFLOAT3 v;
		XMStoreFloat3(&v, getUp());
		return v;
	}

	XMFLOAT3 TransformOld::getRightF3()
	{
		XMFLOAT3 v;
		XMStoreFloat3(&v, getRight());
		return v;
	}

	XMMATRIX TransformOld::getXMMatrix() const
	{
		return XMLoadFloat4x4(&m_matrix);
	}

	TransformOld TransformOld::getInverse() const
	{
		XMMATRIX inverse = XMLoadFloat4x4(&this->m_matrix);
		XMVECTOR det = XMMatrixDeterminant(inverse);
		inverse = XMMatrixInverse(&det, inverse);

		return TransformOld(inverse);
	}

	TransformOld operator*(const TransformOld& l, const TransformOld& r)
	{
		return TransformOld(XMLoadFloat4x4(&l.m_matrix) * XMLoadFloat4x4(&r.m_matrix));
	}

	TransformOld operator*(const TransformOld& l, const XMMATRIX& r)
	{
		return TransformOld(XMLoadFloat4x4(&l.m_matrix) * r);
	}

	TransformOld operator*(const XMMATRIX& l, const TransformOld& r)
	{
		return TransformOld(l * XMLoadFloat4x4(&r.m_matrix));
	}

	TransformOld operator*(const TransformOld& l, const XMFLOAT4X4& r)
	{
		return TransformOld(XMLoadFloat4x4(&l.m_matrix) * XMLoadFloat4x4(&r));
	}

	TransformOld operator*(const XMFLOAT4X4& l, const TransformOld& r)
	{
		return TransformOld(XMLoadFloat4x4(&l) * XMLoadFloat4x4(&r.m_matrix));
	}
}