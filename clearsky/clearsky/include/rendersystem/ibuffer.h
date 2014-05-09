/*
*
*/
#pragma once
#ifndef _CLEARSKY_I_BUFFER_H_
#define _CLEARSKY_I_BUFFER_H_

#include "core/utils.h"

namespace clearsky
{
	class CLEARSKY_API IBuffer
	{
		public:

			virtual RETURN_VALUE create(const void *data, int size, int num, BUFFER_TYPE type=BT_VERTEX, DATA_FORMAT format=FORMAT_R32_UINT)=0;
			virtual RETURN_VALUE activate()=0;

			virtual IBuffer& setBufferUsage(BUFFER_USAGE usage) = 0;
			virtual IBuffer& setCPUUsage(BUFFER_CPU_USAGE usage) = 0;

			virtual void* map(MAP_TYPE mapType=MT_WRITE_DISCARD) = 0;
			virtual void unmap() = 0;

			virtual unsigned int getElementSize()=0; //size of single element in the buffer
			virtual unsigned int getElementCount()=0; //number of elements

			virtual void release() = 0;

		private:
	};

}

#endif