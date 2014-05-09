/*
*
*/
#pragma once
#ifndef _CLEARSKY_DX11_BUFFER_H_
#define _CLEARSKY_DX11_BUFFER_H_

//#include <d3d11.h>
//#include "core/precompiled.h"
//#include <cstdio>

#include "core/utils.h"
#include "core/globals.h"
#include "core/logger.h"

#include "rendersystem/ibuffer.h"

namespace clearsky
{

	class CLEARSKY_API DX11Buffer : public IBuffer
	{
		public:
			DX11Buffer(ID3D11DeviceContext *immediateContext);
			~DX11Buffer();

			RETURN_VALUE create(const void *data, int size, int num, BUFFER_TYPE type=BT_VERTEX, DATA_FORMAT format=FORMAT_R32_UINT);
			RETURN_VALUE activate();

			DX11Buffer& setBufferUsage(BUFFER_USAGE usage);
			DX11Buffer& setCPUUsage(BUFFER_CPU_USAGE usage);

			void* map(MAP_TYPE mapType=MT_WRITE_DISCARD);
			void unmap();

			unsigned int getElementSize();
			unsigned int getElementCount();

			void release();

		private:
			ID3D11Buffer *m_buffer;
			D3D11_BUFFER_DESC m_bufferDesc;
			D3D11_SUBRESOURCE_DATA m_subresource;
			D3D11_USAGE m_bufferUsage;
			D3D11_CPU_ACCESS_FLAG m_cpuUsage;
			DATA_FORMAT m_format;
			void *m_data;
			unsigned int m_size; //size of single element
			unsigned int m_num; //number of elements
			unsigned int m_type;
			BUFFER_TYPE m_customType;	
			
			unsigned int getDXType(BUFFER_TYPE type);
			D3D11_USAGE convertDXUsage(BUFFER_USAGE usage);
			D3D11_MAP convertMapType(MAP_TYPE mapType);

			ID3D11DeviceContext *m_immediateContext;
			ID3D11Device *m_device;
	};
}

#endif //end _DX11_BUFFER_H_