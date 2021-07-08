#include "rfpch.hpp"
#include "Transform.hpp"

namespace Rimfrost
{

	Transform::Transform(const Matrix& matrix)
	{
		m_matrix = matrix;
	}

	void Transform::setTranslation(float x, float y, float z)
	{
		m_matrix[3].x = x;
		m_matrix[3].y = y;
		m_matrix[3].z = z;
		assert(abs(m_matrix[3].w - 1.0f) < 0.00001f);
	}

	void Transform::setTranslation(const Vector3& position)
	{
		m_matrix[3].x = position.x;
		m_matrix[3].y = position.y;
		m_matrix[3].z = position.z;
		assert(abs(m_matrix[3].w - 1.0f) < 0.0000f);
	}

	void Transform::translate(float x, float y, float z)
	{
		m_matrix[3].x += x;
		m_matrix[3].y += y;
		m_matrix[3].z += z;
	}

	void Transform::translate(const Vector3& position)
	{
		m_matrix[3] = m_matrix[3] + position;
	}

	void Transform::setRotation(float x, float y, float z)
	{
		setRotation(rotationFromAngles(x, y, z));
	}

	void Transform::setRotation(const Matrix& rotationMatrix)
	{
		auto [T, R, S] = decomposeToTRS(m_matrix);

		m_matrix = T * rotationMatrix * S;
	}

	void Transform::setRotationDeg(float x, float y, float z)
	{
		setRotation(DirectX::XMConvertToRadians(x), DirectX::XMConvertToRadians(y), DirectX::XMConvertToRadians(z));
	}

	void Transform::rotate(float x, float y, float z)
	{
		auto [T, R, S] = decomposeToTRS(m_matrix);
		R = rotationFromAngles(x, y, z) * R;

		m_matrix = T * R * S;
	}

	void Transform::rotate(const Matrix& rotationMatrix)
	{
		auto [T, R, S] = decomposeToTRS(m_matrix);
		R = rotationMatrix * R;

		m_matrix = T * R * S;
	}

	void Transform::rotateDeg(float x, float y, float z)
	{
		rotate(DirectX::XMConvertToRadians(x), DirectX::XMConvertToRadians(y), DirectX::XMConvertToRadians(z));
	}

	void Transform::setScale(float x, float y, float z)
	{
		auto [T, R, S] = decomposeToTRS(m_matrix);
		S = scaleMatrix(x, y, z);
		m_matrix = T * R * S;
	}
	void Transform::scale(float x, float y, float z)
	{
		auto [T, R, S] = decomposeToTRS(m_matrix);
		S = S * scaleMatrix(x, y, z);
		m_matrix = T * R * S;
	}
	Vector3 Transform::forward()
	{
		Vector3 v = m_matrix[2];
		v.normalize();
		return v;
	}
	Vector3 Transform::up()
	{
		Vector3 v = m_matrix[1];
		v.normalize();
		return v;
	}
	Vector3 Transform::right()
	{
		Vector3 v = m_matrix[0];
		v.normalize();
		return v;
	}
}