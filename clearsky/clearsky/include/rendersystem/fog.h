/*
*
*/

#pragma once
#ifndef _CLEARSKY_FOG_H_
#define _CLEARSKY_FOG_H_

#include "core/utils.h"
#include "math/mathutils.h"

namespace clearsky
{

	//different fog types
	enum CLEARSKY_API FOG_TYPE
	{
		FT_LINEAR    = 0,
		FT_EXPONENT  = 1,
		FT_EXPONENT2 = 2
	};

	//struct to hold all fog information, will pass to shader
	struct CLEARSKY_API FogProperty
	{
		Color color; //fog Color
		int fogType;
		float start; //start of the fog
		float end; //end of fog
		float density;//for exponential fog
	};

	//this class represents the fixed function pipeline default fogging
	class CLEARSKY_API Fog
	{
		public:
			Fog();
			~Fog();

			FOG_TYPE setType(FOG_TYPE newType);
			FOG_TYPE getType();

			FogProperty *getProperty();
			void setProperty(FogProperty *newProperty);

			Color setColor(Color newColor);
			Color getColor();

			void setDimensions(float start, float end);
			float getStart();
			float getEnd();

			float setDensity(float newDensity);
			float getDensity();

		private:
			FogProperty *m_properties; //fog settings
	};
}
#endif