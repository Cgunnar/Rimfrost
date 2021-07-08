#pragma once


namespace Rimfrost
{
	class Vector3;
	class Vector4
	{
	public:
		Vector4(float x = 0, float y = 0, float z = 0, float w = 1);
		Vector4(const Vector3& v3, float w = 1);
		~Vector4() = default;

		float& operator[] (int index) noexcept;
		float operator[] (int index) const noexcept;
		float length3() const;
		float length4() const;
		float x, y, z, w;
	};


	float dot3(const Vector4& v, const Vector4& u);
	float dot4(const Vector4& v, const Vector4& u);
	Vector4 operator +(const Vector4& l, const Vector4& r);
	Vector4 operator +(const Vector4& l, const Vector3& r);
	Vector4 operator +(const Vector3& l, const Vector4& r);

	Vector4 operator -(const Vector4& l, const Vector4& r);
	Vector4 operator -(const Vector4& l, const Vector3& r);
	Vector4 operator -(const Vector3& l, const Vector4& r);



	//Vector3------------------------------------
	class Vector3
	{
	public:
		Vector3(float x = 0, float y = 0, float z = 0);
		Vector3(const Vector4& v);
		~Vector3() = default;

		float& operator[] (int index) noexcept;
		Vector3& operator +=(const Vector3& other);
		float length() const;
		void normalize();
		float x, y, z;
	};
	float dot(const Vector3& v, const Vector3& u);
	Vector3 cross(const Vector3& v, const Vector3& u);

	
	Vector3 operator +(const Vector3& l, const Vector3& r);
	Vector3 operator -(const Vector3& l, const Vector3& r);
	Vector3 operator *(float scale, const Vector3& v);
	Vector3 operator /(const Vector3& v, float scale);
}