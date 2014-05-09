/*
*
*/

#ifndef _CLEARSKY_BOUNDING_BOX_H_
#define _CLEARSKY_BOUNDING_BOX_H_

#include <float.h>

#include "math/mathutils.h"
#include "rendersystem/vertexformats.h"

namespace clearsky
{
	//Axis Aligned Bounding Box
	class CLEARSKY_API AABB
	{
		public:
			AABB();
			~AABB();
			
			bool pointInside(float x, float y, float z);

			//todo: move in extra method
			bool aabbCollision(AABB &aabb);
			bool aabbCollision(Vector3D &boxMin, Vector3D &boxMax);
			bool rayCollision(Vector3D rayOrigin, Vector3D rayDir);

			//given a set of position vertices, generate a bounding box of it
			int loadFromPosition(const Vertex *positions, int num);
			//create bounding box of two aabb(this one and parameter)
			int load(const AABB &aabb);

			//recompute the aabb given by its min, max positions(that should be recomputet)
			void recompute();

			AABB& operator=(const AABB &box);

			Vector3D min;
			Vector3D max;
	};
}

#endif