#include "rfpch.hpp"
#include "Vector3.hpp"

Vector3::Vector3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;

	
}
float Vector3::lengt() const
{
	return sqrt(x * x + y * y + z * z);
}