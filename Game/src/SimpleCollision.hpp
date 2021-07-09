#pragma once

#include "Rimfrost\Math\RimfrostMath.hpp"

namespace SCOL
{
	struct CollisionInfo
	{
		bool collided = false;
		float penetration = 0;
		Rimfrost::Vector3 normal;
	};

	struct Collider
	{
		Rimfrost::Vector3 position;
		float radius;
	};

	CollisionInfo collisionDetection(Collider A, Collider B);

}