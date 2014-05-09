/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/directx11/DX11Texture2D.h"

namespace clearsky
{
	DX11Texture2D::DX11Texture2D(ID3D11DeviceContext *immediateContext,IDXGISwapChain *swapChain):m_textureResource(NULL),
						   m_texture(NULL),
						   m_depthTexture(NULL),
						   m_shaderView(NULL),
						   m_renderTargetView(NULL),
						   m_delphStencilView(NULL),
						   m_usage(BU_DEFAULT),
						   m_cpuUsage((BUFFER_CPU_USAGE)0),
						   m_depthStencilShaderView(NULL)
	{
		this->m_immediateContext = immediateContext;
		this->m_immediateContext->GetDevice(&this->m_dxdevice);
		this->m_swapChain = swapChain;

		memset(&this->m_subresourceData,0,sizeof(D3D11_SUBRESOURCE_DATA));
	}

	DX11Texture2D::~DX11Texture2D()
	{
		this->release();
	}

	void DX11Texture2D::release()
	{
		SAFE_RELEASE(this->m_texture);
		SAFE_RELEASE(this->m_depthTexture);
		SAFE_RELEASE(this->m_textureResource);
		SAFE_RELEASE(this->m_shaderView);
		SAFE_RELEASE(this->m_renderTargetView);
		SAFE_RELEASE(this->m_delphStencilView);
		SAFE_RELEASE(this->m_depthStencilShaderView);
	}

	RETURN_VALUE DX11Texture2D::create(void *data, int width, int height, DATA_FORMAT format)
	{
		//hardcoded for dx feature level 10.1
		//TODO: make better solution
		if((width<1) || (width>8192) || (height<1) || (height>8191))
			return RETURN_ERROR;

		UINT bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		UINT miscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		//printf("\ntest\n");
		memset(&this->m_desc,0,sizeof(D3D11_TEXTURE2D_DESC));
		this->m_desc.Width = width;
		this->m_desc.Height = height;
		this->m_desc.MipLevels = 0;
		this->m_desc.ArraySize = 1;
		this->m_desc.SampleDesc.Count   = 1;
		this->m_desc.SampleDesc.Quality = 0;
		this->m_desc.Usage = (D3D11_USAGE)this->m_usage;
		this->m_desc.BindFlags = bindFlags;
		this->m_desc.Format = (DXGI_FORMAT)format;
		this->m_desc.MiscFlags = miscFlags;
		this->m_desc.CPUAccessFlags = this->m_cpuUsage;
	
		this->m_subresourceData.pSysMem				= data;

		HRESULT result =this->m_dxdevice->CreateTexture2D(&this->m_desc,/*&this->m_subresourceData*/NULL,&this->m_texture);
		if(result!=S_OK)
		{
			LOG->logMsg(clearsky::LT_ERROR, "Error creating Texture %d",result);
			return RETURN_CREATE_TEXTURE_ERROR;
		}

		//need to initilize texture after creatertexture2d for later mipmap generation
		this->m_immediateContext->UpdateSubresource(this->m_texture,0,NULL,this->m_subresourceData.pSysMem,this->m_subresourceData.SysMemPitch,this->m_subresourceData.SysMemSlicePitch);


		//don't need the stuff
		/*D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDesc;

		ZeroMemory(&shaderViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderViewDesc.Format = this->m_desc.Format;
		shaderViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderViewDesc.Texture2D.MipLevels = 1;
		shaderViewDesc.Texture2D.MostDetailedMip = 0;
		*/
		if(this->m_dxdevice->CreateShaderResourceView(this->m_texture, /*&shaderViewDesc*/0, &this->m_shaderView)!=S_OK)
		{
			LOG->logMsg(clearsky::LT_ERROR, "Error creating Texture: CreateShaderResourceView failed");
			return RETURN_CREATE_SHADER_RESOURCE_ERROR;
		}

		this->m_immediateContext->GenerateMips(this->m_shaderView);

		return RETURN_OK;
	}

	RETURN_VALUE DX11Texture2D::createRenderTarget(int width, int height, bool colorMap, DATA_FORMAT format)
	{
	
		RETURN_VALUE result;

		if(colorMap)
		{
			result = this->createColorMap(width, height, format);
			if(result!=RETURN_OK)
				return result;
		}

		result = this->createDepthStencil(width, height);
		if(result!=RETURN_OK)
			return result;

		this->m_viewport.TopLeftX = this->m_viewport.TopLeftY = 0;
		this->m_viewport.Width    = (float)width;
		this->m_viewport.Height   = (float)height;
		this->m_viewport.MinDepth = 0;
		this->m_viewport.MaxDepth = 1;

		return RETURN_OK;
	}

	RETURN_VALUE DX11Texture2D::createColorMap(int width, int height, DATA_FORMAT format)
	{
		memset(&this->m_desc,0,sizeof(D3D11_TEXTURE2D_DESC));
		this->m_desc.Width = width;
		this->m_desc.Height = height;
		this->m_desc.MipLevels = 1;
		this->m_desc.ArraySize = 1;
		this->m_desc.SampleDesc.Count   = 1;
		this->m_desc.SampleDesc.Quality = 0;
		this->m_desc.Usage = (D3D11_USAGE)this->m_usage;
		this->m_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		this->m_desc.Format = (DXGI_FORMAT)format;
		this->m_desc.MiscFlags = 0;
		this->m_desc.CPUAccessFlags = this->m_cpuUsage;

		if(this->m_dxdevice->CreateTexture2D(&this->m_desc,0,&this->m_texture)!=S_OK)
				return RETURN_CREATE_TEXTURE_ERROR;
		if(!this->m_texture)
			return RETURN_CREATE_TEXTURE_ERROR;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDesc;

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = this->m_desc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		if(this->m_dxdevice->CreateRenderTargetView(this->m_texture, &renderTargetViewDesc, &this->m_renderTargetView)!=S_OK)
			return RETURN_RENDER_TARGET_VIEW_ERROR;

		ZeroMemory(&shaderViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderViewDesc.Format = this->m_desc.Format;
		shaderViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderViewDesc.Texture2D.MipLevels = 1;
		shaderViewDesc.Texture2D.MostDetailedMip = 0;

		if(this->m_dxdevice->CreateShaderResourceView(this->m_texture, &shaderViewDesc, &this->m_shaderView)!=S_OK)
			return RETURN_CREATE_SHADER_RESOURCE_ERROR;

		return RETURN_OK;
	}

	RETURN_VALUE DX11Texture2D::createDepthStencil(int width, int height)
	{
		D3D11_TEXTURE2D_DESC depthTextureDesc;

		memset(&depthTextureDesc,0,sizeof(depthTextureDesc));
		depthTextureDesc.Width = width;
		depthTextureDesc.Height = height;
		depthTextureDesc.MipLevels = 1;
		depthTextureDesc.ArraySize = 1;
		depthTextureDesc.SampleDesc.Count   = 1;
		depthTextureDesc.SampleDesc.Quality = 0;
		depthTextureDesc.Usage = (D3D11_USAGE)this->m_usage;
		depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthTextureDesc.Format = (DXGI_FORMAT)FORMAT_R32_TYPELESS;
		depthTextureDesc.MiscFlags = 0;
		depthTextureDesc.CPUAccessFlags = 0;

		HRESULT result = this->m_dxdevice->CreateTexture2D(&depthTextureDesc,0,&this->m_depthTexture);
		if(result!=S_OK)
		{
			LOG->logMsg(clearsky::LT_ERROR, "Error creating Texture %d",result);
			return RETURN_CREATE_TEXTURE_ERROR;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsDesc.Texture2D.MipSlice = 0;
		dsDesc.Format = (DXGI_FORMAT)FORMAT_D32_FLOAT;
		dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		
		if(this->m_dxdevice->CreateDepthStencilView(this->m_depthTexture,&dsDesc,&this->m_delphStencilView)!= S_OK)
			return RETURN_DEPTH_STENCIL_VIEW_ERROR;


		D3D11_SHADER_RESOURCE_VIEW_DESC depthShaderViewDesc;

		ZeroMemory(&depthShaderViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		depthShaderViewDesc.Format = (DXGI_FORMAT)FORMAT_R32_FLOAT;
		depthShaderViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		depthShaderViewDesc.Texture2D.MipLevels = 1;
		depthShaderViewDesc.Texture2D.MostDetailedMip = 0;

		if(this->m_dxdevice->CreateShaderResourceView(this->m_depthTexture, &depthShaderViewDesc, &this->m_depthStencilShaderView)!=S_OK)
			return RETURN_CREATE_SHADER_RESOURCE_ERROR;

		return RETURN_OK;
	}

	RETURN_VALUE DX11Texture2D::loadFile(const char *filename)
	{
		if(!this->m_dxdevice || !this->m_swapChain)
			return RETURN_ERROR;

		StringPathExtractor extractor(std::string(filename),'\\');
		//Use own TGA loader
		if(extractor.extractFileExt()==".tga")
		{
			TGALoader tgaLoader;
			if(!tgaLoader.load(filename))
			{
				LOG->logMsg(clearsky::LT_ERROR, "Error loading texture %s\n",filename);
				return RETURN_ERROR;
			}
			unsigned char *data = tgaLoader.getData();
			if(!data)
			{
				LOG->logMsg(clearsky::LT_ERROR, "Error loading texture %s\n",filename);
				return RETURN_ERROR;
			}

			int width = tgaLoader.getWidth();
			this->setPitch(width*4);
			RETURN_VALUE result = this->create(data, width, tgaLoader.getHeight(),FORMAT_R8G8B8A8_UNORM);
			if(result!=RETURN_OK)
			{
				LOG->logMsg(clearsky::LT_ERROR, "Error loading texture %s width: %d height: %d\n",filename,
																								tgaLoader.getWidth(),
																								tgaLoader.getHeight());
				return result;
			}

			//tgaLoader.unload();
		}
		else
		{
			D3DX11_IMAGE_LOAD_INFO imageInfo;
			HRESULT result = D3DX11CreateTextureFromFile(this->m_dxdevice,
													toMultibyteStr(filename),
													&imageInfo,
													NULL,
													&this->m_textureResource,
													NULL);

			if(FAILED(result) || !m_textureResource)
			{
				LOG->logMsg(clearsky::LT_ERROR, "Error loading texture %s",filename);
				return RETURN_ERROR;
			}
		
			this->m_textureResource->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&this->m_texture);
			this->m_textureResource->Release();

			this->m_texture->GetDesc(&this->m_desc);

			//create shader view
			if(this->m_texture!=NULL)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDesc;

				ZeroMemory(&shaderViewDesc, sizeof(shaderViewDesc));
				shaderViewDesc.Format = this->m_desc.Format;
				shaderViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderViewDesc.Texture2D.MipLevels = this->m_desc.MipLevels;

				this->m_dxdevice->CreateShaderResourceView(this->m_texture, &shaderViewDesc, &this->m_shaderView);
			}
		}

		return RETURN_OK;
	}
	
	void DX11Texture2D::show(int x, int y)
	{
		ID3D11Texture2D *backBuffer;
		this->m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

		D3D11_BOX sourceRegion;
		sourceRegion.left = 0;
		sourceRegion.right = this->m_desc.Width;
		sourceRegion.top = 0;
		sourceRegion.bottom = this->m_desc.Height;
		sourceRegion.front = 0;
		sourceRegion.back = 1;

		this->m_immediateContext->CopySubresourceRegion(backBuffer, 0, x, y, 0, this->m_texture, 0, &sourceRegion);
	}

	ID3D11ShaderResourceView* DX11Texture2D::getShaderView()
	{
		return this->m_shaderView;
	}

	int DX11Texture2D::getWidth()
	{
		return this->m_desc.Width;
	}

	int DX11Texture2D::getHeight()
	{
		return this->m_desc.Height;
	}

	void DX11Texture2D::setUsage(BUFFER_USAGE usage)
	{
		this->m_usage = usage;
	}

	void DX11Texture2D::setCPUUsage(BUFFER_CPU_USAGE usage)
	{
		this->m_cpuUsage = usage;
	}

	void DX11Texture2D::setPitch(int pitch)
	{
		this->m_subresourceData.SysMemPitch = pitch;
	}

	void DX11Texture2D::startRenderTarget()
	{
		ID3D11RenderTargetView *targets[1] = {this->m_renderTargetView};
		this->m_immediateContext->OMSetRenderTargets(1,targets,this->m_delphStencilView);
		this->m_immediateContext->RSSetViewports(1, &this->m_viewport);

		if(this->m_renderTargetView)
			this->m_immediateContext->ClearRenderTargetView(this->m_renderTargetView,Color(0,0,1,1));
		
		if(this->m_delphStencilView)
			this->m_immediateContext->ClearDepthStencilView(this->m_delphStencilView,D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0f,0);
	}


	void DX11Texture2D::endRenderTarget()
	{
		GLOBALS->getRenderer()->setDefaultRenderTarget();

		//this->m_immediateContext->GenerateMips(this->m_shaderView);
	}

}