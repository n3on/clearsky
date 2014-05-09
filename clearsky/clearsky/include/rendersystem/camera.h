/*
*
*/
#pragma once
#ifndef _CLEARSKY_CAMERA_H_
#define _CLEARSKY_CAMERA_H_

#include "core/utils.h"
//#include <d3dx10.h>

#include "core/globals.h"
#include "math/mathutils.h"

namespace clearsky
{
	enum CLEARSKY_API CAM_MOVE
	{
		CM_FORWARD,
		CM_BACK,
		CM_LEFT,
		CM_RIGHT,
		CM_UP,
		CM_DOWN,
		CM_TURN_RIGHT,
		CM_TURN_LEFT,
		CM_TURN_UP,
		CM_TURN_DOWN
	};

	class CLEARSKY_API Camera
	{
		public:
			Camera();
			Camera(Vector3D pos, Vector3D dir, Vector3D up);
			~Camera();

			void setPosition(float x, float y, float z);
			void setUp(float x, float y, float z);
			void setLookDir(float x, float y, float z);

			void move(CAM_MOVE dir, float val);
			//move in specific direction
			void move(Vector3D moveDir, float val);
			void move(Vector3D moveDir);

			Vector3D getLookDir();

			Matrix getRotationMatrix();

			Vector3D position;
			Vector3D up;
			Vector3D lookDir;
			Vector3D rightDir;
		private:

	};
}

#endif