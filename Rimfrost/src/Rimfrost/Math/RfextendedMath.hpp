#pragma once

#include "RimfrostMath.hpp"

namespace Rimfrost
{
	namespace rfm
	{
		//wrappers to directXmath
		Vector3 planeIntersectLine(Vector4 plane, Vector3 linePoint1, Vector3 linePoint2);
		Vector4 planeFromPointNormal(Vector3 point, Vector3 normal);
	}
}