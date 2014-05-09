/*
*
*/
#pragma once
#ifndef _CLEARSKY_I_TEXTURE_2D_H_
#define _CLEARSKY_I_TEXTURE_2D_H_

//#include "core/precompiled.h"
#include "core/utils.h"

namespace clearsky
{
	class CLEARSKY_API ITexture2D
	{
		public:
			
			virtual RETURN_VALUE create(void *data, int width, int height, DATA_FORMAT format)=0;
			virtual RETURN_VALUE createRenderTarget(int width, int height, bool colorMap, DATA_FORMAT format)=0;

			virtual RETURN_VALUE loadFile(const char *filename)=0;
			virtual void show(int x, int y)=0;

			virtual int getWidth()=0;
			virtual int getHeight()=0;
			
			virtual void setUsage(BUFFER_USAGE usage)=0;
			virtual void setCPUUsage(BUFFER_CPU_USAGE usage)=0;

			virtual void startRenderTarget()=0;
			virtual void endRenderTarget()=0;

			virtual void setPitch(int pitch)=0;
			
			virtual void release()=0;
	};
}

#endif