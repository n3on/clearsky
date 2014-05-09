/*
*
*/
#pragma once
#ifndef _CLEARSKY_I_EFFECT_H_
#define _CLEARSKY_I_EFFECT_H_

#include "core/utils.h"
#include "math/mathutils.h"

namespace clearsky
{
	class ITexture2D;

	class CLEARSKY_API IEffect
	{
		public:
			virtual void release()=0;

			virtual RETURN_VALUE create(const char* filename)=0;

			virtual void apply(int technr, int pass)=0;
			virtual void apply(const char* name, int pass)=0;

			virtual int getNumPasses(const char* effectName)=0;
			virtual int getNumPasses(int nr)=0;

			virtual void setInt(const char *name, int value)=0;
			virtual int getInt(const char *name)=0;
			virtual void setFloat(const char *name, float value)=0;
			virtual float getFloat(const char *name)=0;
			virtual void setBool(const char *name, bool value)=0;
			virtual bool getBool(const char *name)=0;
			virtual void setMatrix(const char *name, Matrix value)=0;
			virtual Matrix getMatrix(const char *name)=0;
			virtual void setData(const char *name, void *data, int size,int offset=0)=0;

			virtual void setTexture(const char *name, ITexture2D *texture)=0;
	};
}

#endif