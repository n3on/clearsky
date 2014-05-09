#pragma once
#ifndef _CLEARSKY_PHYSICS_MATH_H_
#define _CLEARSKY_PHYSICS_MATH_H_

#include "physics/physicsIncludes.h"
#include "physics/precision.h"

namespace clearsky
{
	#define nVec			D3DXVec3Normalize
	#define magnitude		D3DXVec3Length
	#define	squareMagnitude D3DXVec3LengthSq

	// These are the same:
	#define	dotP			D3DXVec3Dot
	#define scalarProduct	D3DXVec3Dot

	//These two too
	#define vectorProduct	D3DXVec3Cross
	#define crossProduct	D3DXVec3Cross

	typedef Vector3D PhVec3;

	PhVec3 componentProduct(const PhVec3& vec1, const PhVec3& vec2);

}

#endif