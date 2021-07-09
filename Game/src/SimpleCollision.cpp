#include "SimpleCollision.hpp"

using namespace Rimfrost;
namespace SCOL
{
	CollisionInfo collisionDetection(Collider A, Collider B)
	{
		CollisionInfo collResult;

		Vector3 diffVec = A.position - B.position;
		float dist = diffVec.length();
		float sumRadius = A.radius + B.radius;
		if (dist < sumRadius)
		{
			collResult.collided = true;
			collResult.penetration = sumRadius - dist;
			collResult.normal = normalize(diffVec);
		}
		return collResult;
	}

}