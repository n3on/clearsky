
#include "core/precompiled.h"
#include "physics/physicsmath.h"


namespace clearsky
{
	/*
	real magnitude(PhVec3* Vec)
	{
		return real_sqrt(Vec->x * Vec->x + Vec->y * Vec->y + Vec->z * Vec->z);
	}
	*/

	PhVec3 componentProduct(const PhVec3& vec1, const PhVec3& vec2)
	{
		PhVec3 result;
		result.x = vec1.x * vec2.x;
		result.y = vec1.y * vec2.y;
		result.z = vec1.z * vec2.z;

		return result;
	}
}