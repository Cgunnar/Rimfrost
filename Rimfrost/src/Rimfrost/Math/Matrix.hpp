#pragma once

#include <tuple>
#include "Vector.hpp"

namespace Rimfrost
{
	class Matrix
	{
	public:
		Matrix() = default;
		Matrix(float* mem);
		Matrix(float _00, float _01, float _02, float _03,
				float _10, float _11, float _12, float _13, 
				float _20, float _21, float _22, float _23, 
				float _30, float _31, float _32, float _33);
		Matrix(Vector4 columnVectors[4]);
		~Matrix() = default;


		Vector4& operator[] (int index) noexcept;		
		const Vector4& operator[] (int index) const noexcept;		
		


	private:
		Vector4 columns[4]{ Vector4(1, 0, 0, 0),
							Vector4(0, 1, 0, 0),
							Vector4(0, 0, 1, 0),
							Vector4(0, 0, 0, 1) };
	};
	Matrix transpose(const Matrix& matrix);
	Matrix inverse(const Matrix& matrix);
	std::tuple<Matrix, Matrix, Matrix> decomposeToTRS(const Matrix& matrix);
	Matrix scaleMatrix(float x, float y, float z);
	Matrix rotationFromAngles(float x, float y, float z);
	Matrix rotationXFromAngles(float a);
	Matrix rotationYFromAngles(float a);
	Matrix rotationZFromAngles(float a);
	Matrix rotationFromAnglesDeg(float x, float y, float z);
	Matrix rotationXFromAnglesDeg(float a);
	Matrix rotationYFromAnglesDeg(float a);
	Matrix rotationZFromAnglesDeg(float a);
	Matrix rotationMatrixFromNormal(Vector3 normal, float angle);
	Matrix operator*(const Matrix& l, const Matrix& r);
	Matrix operator+(const Matrix& l, const Matrix& r);
	Matrix operator-(const Matrix& l, const Matrix& r);
	Vector4 operator*(const Matrix& m, const Vector4& v);
}