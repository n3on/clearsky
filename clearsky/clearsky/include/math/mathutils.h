/*
*
*/
#pragma once
#ifndef _CLEARSKY_MATH_H_
#define _CLEARSKY_MATH_H_

//#include "core/precompiled.h"
//#include <d3dx10.h>
#include "core/utils.h"

#pragma warning(disable : 4251)

namespace clearsky
{
	typedef D3DXVECTOR2 Vector2D;
	typedef D3DXVECTOR3 Vector3D;
	typedef D3DXVECTOR4 Vector4D;
	typedef D3DXMATRIX Matrix;
	typedef D3DXPLANE Plane;
	typedef D3DXCOLOR Color;

	CLEARSKY_API Matrix* MatrixMultiply(Matrix *out, const Matrix *matrix1, const Matrix *matrix2);
	CLEARSKY_API Matrix* MatrixIdentity(Matrix *out);
	CLEARSKY_API Matrix* MatrixTranslation(Matrix *out, float x, float y, float z);
	CLEARSKY_API Matrix* MatrixRotation(Matrix *out, float x, float y, float z);
	CLEARSKY_API Matrix* MatrixScale(Matrix *out, float x, float y, float z);

	CLEARSKY_API Vector3D* VectorTransform(Vector3D *out, const Vector3D *in, const Matrix *matrix);

	enum CLEARSKY_API GRADIENT_TYPE
	{
		GRADIENT_VERTICAL,
		GRADIENT_HORIZONTAL,
		GRADIENT_MIDDLE_CROSS
	};

	//simple gradient
	struct CLEARSKY_API Gradient
	{
		Gradient()
		{
			this->type = GRADIENT_VERTICAL;
			this->startColor = Color(0,0,0,1);
			this->endColor   = Color(0,0,0,1);
		};

		Gradient(Color start, Color end, GRADIENT_TYPE grType)
		{
			this->startColor = start;
			this->endColor   = end;
			this->type = grType;
		}

		~Gradient(){};
		Color startColor;
		Color endColor;
		GRADIENT_TYPE type;
	};

	struct Rect
	{
		int left;
		int top;
		int right;
		int bottom;
	};


	inline float CLEARSKY_API DEG_TO_RAD(float var)
	{
		return (float)(var*0.0174532925);
	};

	class CLEARSKY_API Math
	{
		public:
			static inline float degToRad(float val)
			{
				return (float)(val*0.0174532925);
			}

			static inline float radToDeg(float val)
			{
				return (float)(val*57.29577951);
			}

			static inline float invSqrt (float x)
			{
				float xhalf = 0.5f*x;
				int i = *(int*)&x;
				i = 0x5f3759df - (i >> 1);
				x = *(float*)&i;
				x = x*(1.5f - xhalf*x*x);

				return x;
			}

			static inline bool pointInRect(float pointX, float pointY, float rectX, float rectY, float rectWidth, float rectHeight)
			{
				return pointX>rectX && pointX<(rectX+rectWidth) && pointY>rectY && pointY<(rectY+rectHeight);
			}

			static inline int absolute(int val) 
			{
				return val < 0 ? -val : val;
			}

		private:
			Math(){};
			~Math(){};

	};

}

#endif