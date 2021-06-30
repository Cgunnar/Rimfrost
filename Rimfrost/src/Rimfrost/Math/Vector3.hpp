#pragma once
class Vector3
{
public:
	Vector3(float x = 0, float y = 0, float z = 0);
	~Vector3() = default;

	float lengt() const;
	float x, y, z;
};

