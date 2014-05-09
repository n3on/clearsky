/*
*
*/

#include "core/precompiled.h"
#include "math/boundingbox.h"

namespace clearsky
{
	AABB::AABB()
	{
		this->min = Vector3D(0,0,0);
		this->max = Vector3D(0,0,0);
	}

	AABB::~AABB()
	{
	}

	bool AABB::pointInside(float x, float y, float z)
	{
		return  (x>=this->min.x) && (x<=this->max.x) &&
				(y>=this->min.y) && (y<=this->max.y) &&
				(z>=this->min.z) && (z<=this->max.z);
	}

	bool AABB::aabbCollision(AABB &aabb)
	{
		return this->aabbCollision(aabb.min, aabb.max);
	}
	
	bool AABB::aabbCollision(Vector3D &boxMin, Vector3D &boxMax)
	{
		return  boxMin.x>=this->min.x && boxMax.x<=this->max.x &&
				boxMin.y>=this->min.y && boxMax.y<=this->max.y &&
				boxMin.z>=this->min.z && boxMax.z<=this->max.z;

	}

	bool AABB::rayCollision(Vector3D rayOrigin, Vector3D rayDir)
	{
		float nearVal =  FLT_MIN;
		float farVal  =  FLT_MAX;

		float e = 0.00001f;
		for(int i=0; i<3; ++i)
		{
			//ray is parallel to x,y,z playes
			if(abs(rayDir[i])>e)
			{
				float inverseDir = 1/rayDir[i];

				float int1 = (this->min[i]-rayOrigin[i])*inverseDir; //first intersection time
				float int2 = (this->max[i]-rayOrigin[i])*inverseDir; //second intersetion time

				if(int1>int2)
					std::swap(int1, int2);

				if(int1>nearVal)
					nearVal=int1;

				if(int2<farVal)
					farVal=int2;

				if(nearVal>farVal)
					return false;

				if(farVal<0)
					return false;
			}
			else
			{
				if((rayOrigin[i]<this->min[i]) || (rayOrigin[i]>this->max[i]))
					return false;
			}
		}

		return true;
	}

	int AABB::loadFromPosition(const Vertex *positions, int num)
	{
		if(num<=0 || !positions)
			return 1;

		this->min = Vector3D(positions[0].position.x, positions[0].position.y, positions[0].position.z);
		this->max = Vector3D(positions[0].position.x, positions[0].position.y, positions[0].position.z);

		for(int i=0; i<num; ++i)
		{
			//printf("data %d: %f %f %f\n", i, positions[i].position.x, positions[i].position.y, positions[i].position.z);
			
			/*if(positions[i].position.x <= this->min.x)
				this->min.x = positions[i].position.x;
			if(positions[i].position.y <= this->min.y)
				this->min.y = positions[i].position.y;
			if(positions[i].position.z <= this->min.z)
				this->min.z = positions[i].position.z;
			
			if(positions[i].position.x >= this->max.x)
				this->max.x = positions[i].position.x;
			if(positions[i].position.y >= this->max.y)
				this->max.y = positions[i].position.y;
			if(positions[i].position.z >= this->max.z)
				this->max.z = positions[i].position.z;*/

			D3DXVec3Minimize(&this->min,&this->min,&positions[i].position);
			D3DXVec3Maximize(&this->max,&this->max,&positions[i].position);

		}

		return 0;
	}

	int AABB::load(const AABB &aabb)
	{
		D3DXVec3Minimize(&this->min,&this->min,&aabb.min);
		D3DXVec3Maximize(&this->max,&this->max,&aabb.max);

		return 0;
	}

	void AABB::recompute()
	{
		Vector3D newMin;
		Vector3D newMax;
		D3DXVec3Minimize(&newMin,&this->min,&this->max);
		D3DXVec3Maximize(&newMax,&this->min,&this->max);

		this->min = newMin;
		this->max = newMax;
		/*if(this->max.x <= this->min.x)
			this->min.x = this->max.x;
		if(this->max.y <= this->min.y)
			this->min.y = this->max.y;
		if(this->max.z <= this->min.z)
			this->min.z = this->max.z;
			
		if(this->min.x >= this->max.x)
			this->max.x = this->min.x;
		if(this->min.y >= this->max.y)
			this->max.y = this->min.y;
		if(this->min.z >= this->max.z)
			this->max.z = this->min.z;*/

	}

	AABB& AABB::operator=(const AABB &box)
	{
		this->min = box.min;
		this->max = box.max;

		return *this;
	}
}