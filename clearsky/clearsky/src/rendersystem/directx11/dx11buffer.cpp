/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/directx11/DX11Buffer.h"

namespace clearsky
{
	DX11Buffer::DX11Buffer(ID3D11DeviceContext *immediateContext):m_num(0),
					 m_size(0),
					 m_buffer(NULL),
					 m_data(NULL),
					 m_immediateContext(NULL),
					 m_bufferUsage(D3D11_USAGE_IMMUTABLE),
					 m_type(D3D11_BIND_VERTEX_BUFFER),
					 m_customType(BT_VERTEX),
					 m_cpuUsage((D3D11_CPU_ACCESS_FLAG)0),
					 m_format(FORMAT_R32_UINT)
	{
		
		if(immediateContext)
		{
			this->m_immediateContext = immediateContext;
			this->m_immediateContext->GetDevice(&this->m_device);
		}

	}

	DX11Buffer::~DX11Buffer()
	{
		this->release();
	}

	void DX11Buffer::release()
	{
		SAFE_RELEASE(this->m_buffer);
		SAFE_DELETE(this->m_data);

		this->m_num  = 0;
		this->m_size = 0;
	}

	RETURN_VALUE DX11Buffer::create(const void *data, int size, int num, BUFFER_TYPE type,DATA_FORMAT format)
	{
		if(!this->m_buffer && !data)
			return RETURN_ERROR;

		if(!this->m_device || !this->m_immediateContext)
			return RETURN_ERROR;

		this->m_size = size;
		this->m_num  = num;

		this->m_customType = type;
		this->m_format = format;

		memset(&this->m_bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		this->m_bufferDesc.Usage		= this->m_bufferUsage;
		this->m_bufferDesc.ByteWidth	= size*num;
		this->m_bufferDesc.BindFlags	= this->getDXType(type);
		this->m_bufferDesc.CPUAccessFlags	= this->m_cpuUsage;
		this->m_bufferDesc.MiscFlags		= 0;

		memset(&this->m_subresource, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		this->m_subresource.pSysMem = data;

		if(this->m_device->CreateBuffer(&this->m_bufferDesc, &this->m_subresource, &this->m_buffer)!=S_OK)
			return RETURN_ERROR;

		return RETURN_OK;
	}

	unsigned int DX11Buffer::getDXType(BUFFER_TYPE type)
	{
		switch(type)
		{
			case BT_VERTEX:
			{
				return D3D11_BIND_VERTEX_BUFFER;
			}break;
			case BT_INDEX:
			{
				return D3D11_BIND_INDEX_BUFFER;
			}break;
		}

		return 0;
	}

	RETURN_VALUE DX11Buffer::activate()
	{
		if(!this->m_buffer)
			return RETURN_ERROR;

		if(!this->m_immediateContext)
			return RETURN_ERROR;

		const unsigned int offset = 0;
		if(this->m_customType == BT_VERTEX)
			this->m_immediateContext->IASetVertexBuffers(0,1,&this->m_buffer,&this->m_size,&offset);
		else if(this->m_customType == BT_INDEX)
		{
			//fixe it: somehow FORMAT_R32_UINT make problems on dx9 hardware
			//this->m_format = FORMAT_R16_UINT;
			if(this->m_format==FORMAT_R32_UINT)
				this->m_immediateContext->IASetIndexBuffer(this->m_buffer, DXGI_FORMAT_R32_UINT, 0);
			else if(this->m_format==FORMAT_R16_UINT)
				this->m_immediateContext->IASetIndexBuffer(this->m_buffer, DXGI_FORMAT_R16_UINT, 0);
		}

		return RETURN_OK;
	}

	void* DX11Buffer::map(MAP_TYPE mapType)
	{
		HRESULT result;

		if(!this->m_buffer)
			return NULL;

		//this->m_buffer->Map(D3D11_MAP_READ,0,buffer);
		D3D11_MAP cpuMapType = this->convertMapType(mapType);
		D3D11_MAPPED_SUBRESOURCE buffer;
		memset(&buffer, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

		result = this->m_immediateContext->Map(this->m_buffer,0,cpuMapType,0,&buffer);
		if(FAILED(result))
			return NULL;

		return buffer.pData;
	}

	void DX11Buffer::unmap()
	{
		//if(this->m_data)
		//this->m_buffer->Unmap();
		
		if(this->m_immediateContext && this->m_buffer)
		{
			this->m_immediateContext->Unmap(this->m_buffer, 0);			
		}
	}

	DX11Buffer& DX11Buffer::setBufferUsage(BUFFER_USAGE usage)
	{
		D3D11_USAGE newUsage = convertDXUsage(usage);
		this->m_bufferUsage = newUsage;

		return *this;
	}

	DX11Buffer& DX11Buffer::setCPUUsage(BUFFER_CPU_USAGE usage)
	{
		switch(usage)
		{
			case BCU_READ:
			{
				this->m_cpuUsage = D3D11_CPU_ACCESS_READ;
			}break;
			case BCU_WRITE:
			{
				this->m_cpuUsage = D3D11_CPU_ACCESS_WRITE;
			}break;
		}

		return *this;
	}

	D3D11_USAGE DX11Buffer::convertDXUsage(BUFFER_USAGE usage)
	{
		switch(usage)
		{
			case BU_DEFAULT:
			{
				return D3D11_USAGE_DEFAULT;
			}break;
			case BU_IMMUTABLE:
			{
				return D3D11_USAGE_IMMUTABLE;
			}break;
			case BU_DYNAMIC:
			{
				return D3D11_USAGE_DYNAMIC;
			}break;
			case BU_STAGING:
			{
				return D3D11_USAGE_STAGING; 
			}break;
		};

		return D3D11_USAGE_DEFAULT;
	}

	D3D11_MAP DX11Buffer::convertMapType(MAP_TYPE mapType)
	{
		switch(mapType)
		{
			case MT_READ:
			{
				return D3D11_MAP_READ;
			}break;
			case MT_WRITE:
			{
				return D3D11_MAP_WRITE;
			}break;
			case MT_READ_WRITE:
			{
				return D3D11_MAP_READ_WRITE;
			}break;
			case MT_WRITE_DISCARD:
			{
				return D3D11_MAP_WRITE_DISCARD; 
			}break;
			case MT_WRITE_NO_OVERWRITE:
			{
				return D3D11_MAP_WRITE_NO_OVERWRITE;
			}break;

		};

		return D3D11_MAP_WRITE_DISCARD;
	}

	unsigned int DX11Buffer::getElementSize()
	{
		return this->m_size;
	}

	unsigned int DX11Buffer::getElementCount()
	{
		return this->m_num;
	}

}