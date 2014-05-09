/*
*
*/

#include "core/precompiled.h"
#include "math/viewfrustum.h"

namespace clearsky
{
	ViewFrustum::ViewFrustum()
	{

	}

	RETURN_VALUE ViewFrustum::build(Matrix view, Matrix projection)
	{
		Matrix viewProjection;
		D3DXMatrixMultiply( &viewProjection, &view, &projection);
		
		// Left
		this->m_planes[0].a = viewProjection._14 + viewProjection._11;
		this->m_planes[0].b = viewProjection._24 + viewProjection._21;
		this->m_planes[0].c = viewProjection._34 + viewProjection._31;
		this->m_planes[0].d = viewProjection._44 + viewProjection._41;

		// Right
		this->m_planes[1].a = viewProjection._14 - viewProjection._11;
		this->m_planes[1].b = viewProjection._24 - viewProjection._21;
		this->m_planes[1].c = viewProjection._34 - viewProjection._31;
		this->m_planes[1].d = viewProjection._44 - viewProjection._41;

		// Top
		this->m_planes[2].a = viewProjection._14 - viewProjection._12;
		this->m_planes[2].b = viewProjection._24 - viewProjection._22;
		this->m_planes[2].c = viewProjection._34 - viewProjection._32;
		this->m_planes[2].d = viewProjection._44 - viewProjection._42;

		// Bottom
		this->m_planes[3].a = viewProjection._14 + viewProjection._12;
		this->m_planes[3].b = viewProjection._24 + viewProjection._22;
		this->m_planes[3].c = viewProjection._34 + viewProjection._32;
		this->m_planes[3].d = viewProjection._44 + viewProjection._42;

		// Near
		this->m_planes[4].a = viewProjection._13;
		this->m_planes[4].b = viewProjection._23;
		this->m_planes[4].c = viewProjection._33;
		this->m_planes[4].d = viewProjection._43;

		// Far
		this->m_planes[5].a = viewProjection._14 - viewProjection._13;
		this->m_planes[5].b = viewProjection._24 - viewProjection._23;
		this->m_planes[5].c = viewProjection._34 - viewProjection._33;
		this->m_planes[5].d = viewProjection._44 - viewProjection._43; 

		for ( int i = 0; i < 6; i++ )
		{
			D3DXPlaneNormalize( &this->m_planes[i], &this->m_planes[i] );
		} 

		return RETURN_OK;
	}

	bool ViewFrustum::sphereInside(Vector3D position, unsigned int radius)
	{
		//test all 6 planes
		for(int i=0; i<6; ++i)
		{
			if ( D3DXPlaneDotCoord(&this->m_planes[i], &position) + radius < 0 )
			{
				//outside
				return false;
			}
		}

		//inside
		return true;
	}

	bool ViewFrustum::boxInside(Vector3D min, Vector3D max)
	{
		Vector3D  corner1 = min;
		Vector3D  corner2 = Vector3D(max.x,min.y,min.z);
		Vector3D  corner3 = Vector3D(min.x,max.y,min.z);
		Vector3D  corner4 = Vector3D(max.x,max.y,min.z);
		Vector3D  corner5 = Vector3D(min.x,min.y,max.z);
		Vector3D  corner6 = Vector3D(max.x,min.y,max.z);
		Vector3D  corner7 = Vector3D(min.x,max.y,max.z);
		Vector3D  corner8 = max;

		//test all 6 planes
		for(int i=0; i<6; ++i)
		{
			//check all 8 corners
			if(D3DXPlaneDotCoord(&this->m_planes[i], &corner1)>=0)
				continue;
			if(D3DXPlaneDotCoord(&this->m_planes[i], &corner2)>=0)
				continue;
			if(D3DXPlaneDotCoord(&this->m_planes[i], &corner3)>=0)
				continue;
			if(D3DXPlaneDotCoord(&this->m_planes[i], &corner4)>=0)
				continue;
			if(D3DXPlaneDotCoord(&this->m_planes[i], &corner5)>=0)
				continue;
			if(D3DXPlaneDotCoord(&this->m_planes[i], &corner6)>=0)
				continue;
			if(D3DXPlaneDotCoord(&this->m_planes[i], &corner7)>=0)
				continue;
			if(D3DXPlaneDotCoord(&this->m_planes[i], &corner8)>=0)
				continue;

			return false;
		}

		return true;
	}
}