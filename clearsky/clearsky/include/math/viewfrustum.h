/*
*
*/

#ifndef _CLEARSKY_VIEW_FRUSTUM_H_
#define _CLEARSKY_VIEW_FRUSTUM_H_

#include "math/mathutils.h"

namespace clearsky
{
	class CLEARSKY_API ViewFrustum
	{
		public:
			ViewFrustum();
			RETURN_VALUE build(Matrix view, Matrix projection);
			bool boxInside(Vector3D min, Vector3D max);
			bool sphereInside(Vector3D position, unsigned int radius);

		private:
			Plane m_planes[6]; //bounding planes of the frustum
			
	};
}

#endif