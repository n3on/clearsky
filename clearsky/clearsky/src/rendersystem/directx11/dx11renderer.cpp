/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/directx11/DX11Renderer.h"

namespace clearsky
{

	DX11Renderer::DX11Renderer():m_device(NULL),
						 m_rasterizerState(NULL),
						 m_run(true),
						 m_keyboardHandler(NULL),
						 m_mouseHandler(NULL),
						 m_immediateContext(NULL),
						 m_blendState(NULL),
						 m_fixedFuncEffect(NULL),
						 m_fixedFuncInputLayout(NULL),
						 m_featureLevel(D3D_FEATURE_LEVEL_11_0),
						 m_driverType(D3D_DRIVER_TYPE_UNKNOWN),
						 m_currentChain(0)
	{

		ZeroMemory(&this->m_rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		this->m_rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		this->m_rasterizerDesc.CullMode = D3D11_CULL_BACK;
		this->m_rasterizerDesc.FrontCounterClockwise = false;
		this->m_rasterizerDesc.MultisampleEnable	 = true;
		this->m_rasterizerDesc.AntialiasedLineEnable = true;
		this->m_rasterizerDesc.DepthClipEnable = true;

		ZeroMemory(&this->m_blendDesc, sizeof(D3D11_BLEND_DESC));
		this->m_blendDesc.AlphaToCoverageEnable = false;
		this->m_blendDesc.IndependentBlendEnable	= false;
		for(int i=0; i<8; ++i)
		{
			this->m_blendDesc.RenderTarget[i].BlendEnable = true;
		
			this->m_blendDesc.RenderTarget[i].BlendOp			= D3D11_BLEND_OP_ADD;
			this->m_blendDesc.RenderTarget[i].BlendOpAlpha      = D3D11_BLEND_OP_ADD;
			this->m_blendDesc.RenderTarget[i].DestBlend			= D3D11_BLEND_INV_SRC_ALPHA;
			this->m_blendDesc.RenderTarget[i].DestBlendAlpha	= D3D11_BLEND_ZERO;
			this->m_blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			this->m_blendDesc.RenderTarget[i].SrcBlend			= D3D11_BLEND_SRC_ALPHA;
			this->m_blendDesc.RenderTarget[i].SrcBlendAlpha		= D3D11_BLEND_ZERO;
		}

		ZeroMemory(&this->m_depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		this->m_depthStencilDesc.DepthEnable = true;
		this->m_depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		this->m_depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

		this->m_depthStencilDesc.StencilEnable = true;
		this->m_depthStencilDesc.StencilWriteMask = 0xFF;
		this->m_depthStencilDesc.StencilReadMask = 0xFF;

		this->m_depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		this->m_depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		this->m_depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		this->m_depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		this->m_depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		this->m_depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		this->m_depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		this->m_depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

		this->m_depthStencilStateDesc = new DepthStencilState;
		this->m_rasterizerStateDesc   = new RasterizerState;
		this->m_samplerStateDesc = new SamplerState;

		D3DXMatrixIdentity(&this->m_view);
		D3DXMatrixIdentity(&this->m_proj);
		D3DXMatrixIdentity(&this->m_world);
		D3DXMatrixIdentity(&this->m_worldSaved);

		this->m_curEffect = NULL;
		this->m_inputLayout = NULL;
		this->m_camera = new Camera();
		this->m_timer  = new Timer();

		this->m_states.fullscreen = true;
		this->m_states.defaultEffect = true;
		this->m_states.lighting	= false;
		this->m_states.texturing	= false;
		this->m_states.blending	= false;
		this->m_states.normalsPosition = true; 
		this->m_states.cullMode = CULL_MODE_BACK;
		this->m_states.frontOrdnerCw = true;
		this->m_states.wireframe = false;
		this->m_states.fogging  = false;
		this->m_states.sampleCount = 2;
		this->m_states.sampleLevel = 0;
		this->m_states.backbufferFormat   = FORMAT_R8G8B8A8_UNORM;
		this->m_states.depthStencilFormat =  FORMAT_D24_UNORM_S8_UINT;//FORMAT_D32_FLOAT_S8X24_UINT; 
		this->m_states.clearColor = Color(0,0,0,1);
		this->m_states.nearPlane = 0.01f; 
		this->m_states.farPlane = 10000.0f;
		this->m_states.fov = 45;

	}

	DX11Renderer::~DX11Renderer()
	{
		this->release();
	}

	RETURN_VALUE DX11Renderer::create(const StartParams *startparams)
	{
		this->m_states.fullscreen = startparams->fullscreen;
		this->m_states.sampleCount = startparams->multiSamplingCount;
		this->m_states.sampleLevel = startparams->multiSamplingLevel;

		if(strlen(startparams->driverType)>0)
		{
			this->m_driverType = stringToDriverType(startparams->driverType);
		}

		if(strlen(startparams->featureLevel)>0)
		{
			this->m_featureLevel = stringToFeatureLevel(startparams->featureLevel);
		}

		//TCHAR *multibyteTitle = toMultibyteStr(startparams->title);

		return create(startparams->title, startparams->windowWidth, startparams->windowHeight);
	}

	//todo make const char *title
	RETURN_VALUE DX11Renderer::create(const char *title, int width, int height)
	{
		RETURN_VALUE returnValue;

		TCHAR *multibyteTitle = toMultibyteStr(title);
		
		//if no output window is specified, create default one
		if(this->m_outputWindows.size()<=0)
		{
			SwapChain *chain = new SwapChain;
			chain->id = 0;
			WindowDesc *windowDesc = new WindowDesc;
			windowDesc->width = width;
			windowDesc->height = height;
			windowDesc->fullscreen = this->m_states.fullscreen;
			windowDesc->multiSamplingCount = this->m_states.sampleCount;
			windowDesc->multiSamplingLevel = this->m_states.sampleLevel;

			windowDesc->hwnd = this->createWindow(multibyteTitle, width, height);
			if(!windowDesc->hwnd)
				return RETURN_INVALID_WINDOW_HANDLE;

			chain->windowDesc = windowDesc;
			this->m_outputWindows.push_back(chain);
		}

		GLOBALS->setWidth(width);
		GLOBALS->setHeight(height);
		GLOBALS->setWindowHandle(this->m_outputWindows.at(0)->windowDesc->hwnd);

		returnValue = this->createDevice();
		if(returnValue != RETURN_OK)
			return returnValue;
		
		//create vertexbuffer for 2d drawing
		this->m_buffer2DPrimitive = new DX11Buffer(this->m_immediateContext);
		this->m_buffer2DPrimitive->setCPUUsage(BCU_WRITE);
		this->m_buffer2DPrimitive->setBufferUsage(BU_DYNAMIC);
		VertexPCUV initData[10];
		memset(initData,0,sizeof(VertexPCUV)*10);
		this->m_buffer2DPrimitive->create(initData,sizeof(VertexPCUV),10);

		this->m_fixedFuncEffect = (DX11Effect*)this->createEffect();//new DX11Effect(this->m_device, this->m_immediateContext);
		if(this->m_fixedFuncEffect->create(DEFAULT_EFFECT_FILE)!=RETURN_OK)
			return RETURN_EFFECT_ERROR;

		this->m_curEffect = this->m_fixedFuncEffect;

		returnValue = this->setViewport(0,0,width,height);
		if(returnValue!=RETURN_OK)
			return returnValue;

		this->setProjection(width, height, this->m_states.nearPlane, this->m_states.farPlane, this->m_states.fov);
		this->setViewTransformation(this->m_camera->position,this->m_camera->lookDir,this->m_camera->up);

		this->m_fixedFuncInputLayout = new DX11InputLayout(this->m_immediateContext);

		//this->m_fixedFuncInputLayout->setEffect(this->m_effect);
		

		if(this->m_fixedFuncInputLayout->create(clearsky::VF_DEFAULT, this->m_fixedFuncEffect)!=RETURN_OK)
			return RETURN_INPUT_FORMAT_ERROR;

		this->m_inputLayout2D = new DX11InputLayout(this->m_immediateContext);
		if(this->m_inputLayout2D->create(clearsky::VF_PCUV, this->m_fixedFuncEffect,1)!=RETURN_OK)
			return RETURN_INPUT_FORMAT_ERROR;

		this->m_curEffect = this->m_fixedFuncEffect;//this->m_defaultEffect->getEffect();

		this->m_fixedFuncInputLayout->activate();

		this->m_inputLayout = this->m_fixedFuncInputLayout;

		if(FAILED(this->m_device->CreateRasterizerState(&this->m_rasterizerDesc, &this->m_rasterizerState)))
			return RETURN_CREATE_RASTERIZER_STATE_ERROR;
		this->m_immediateContext->RSSetState(this->m_rasterizerState);

		if(FAILED(this->m_device->CreateBlendState(&this->m_blendDesc,&m_blendState)))
			return RETURN_ERROR;

		
		if(FAILED(this->m_device->CreateDepthStencilState(&this->m_depthStencilDesc, &this->m_depthStencilState)))
			return RETURN_CREATE_BLEND_STATE_ERROR;
		this->m_immediateContext->OMSetDepthStencilState(this->m_depthStencilState, 0);

		Material material;
		this->setMaterial(material);
		this->setFrontOrder(true);

		GLOBALS->setRenderer(this);
		
		//read default setting for render states(lighting, texturing etc.) from the shader
		this->readDefaultShaderValues();

		/*//if we are in fullscreen store the windows resulution dimensions
		if(this->m_states.fullscreen)
		{
			RECT rc;
			GetWindowRect(GetDesktopWindow(), &rc);

			this->m_windowDesc.width  = (rc.right-rc.left);
			this->m_windowDesc.height = (rc.bottom-rc.top);
		}*/
		
		return RETURN_OK;
	}

	D3D_DRIVER_TYPE DX11Renderer::stringToDriverType(const char *driverType)
	{
		D3D_DRIVER_TYPE dxType = D3D_DRIVER_TYPE_UNKNOWN;

		if(strncmp(driverType, "D3D_DRIVER_TYPE_UNKNOWN",50)==0)
		{
			dxType = D3D_DRIVER_TYPE_UNKNOWN;
		}
		else if(strncmp(driverType, "D3D_DRIVER_TYPE_HARDWARE",50)==0)
		{
			dxType = D3D_DRIVER_TYPE_HARDWARE;
		}
		else if(strncmp(driverType, "D3D_DRIVER_TYPE_REFERENCE",50)==0)
		{
			dxType = D3D_DRIVER_TYPE_REFERENCE;
		}
		else if(strncmp(driverType, "D3D_DRIVER_TYPE_NULL",50)==0)
		{
			dxType =  D3D_DRIVER_TYPE_NULL;
		}
		else if(strncmp(driverType, "D3D_DRIVER_TYPE_SOFTWARE",50)==0)
		{
			dxType = D3D_DRIVER_TYPE_SOFTWARE;
		}
		else if(strncmp(driverType, "D3D_DRIVER_TYPE_WARP",50)==0)
		{
			dxType = D3D_DRIVER_TYPE_WARP;
		}

		return dxType;
	}

	D3D_FEATURE_LEVEL DX11Renderer::stringToFeatureLevel(const char *featureLevel)
	{
		D3D_FEATURE_LEVEL dxType = D3D_FEATURE_LEVEL_11_0;

		if(strncmp(featureLevel, "D3D_FEATURE_LEVEL_11_0",50)==0)
		{
			dxType = D3D_FEATURE_LEVEL_11_0;
		}
		else if(strncmp(featureLevel, "D3D_FEATURE_LEVEL_10_1",50)==0)
		{
			dxType = D3D_FEATURE_LEVEL_10_1;
		}
		else if(strncmp(featureLevel, "D3D_FEATURE_LEVEL_10_0",50)==0)
		{
			dxType = D3D_FEATURE_LEVEL_10_0;
		}
		else if(strncmp(featureLevel, "D3D_FEATURE_LEVEL_9_3",50)==0)
		{
			dxType =  D3D_FEATURE_LEVEL_9_3;
		}
		else if(strncmp(featureLevel, "D3D_FEATURE_LEVEL_9_2",50)==0)
		{
			dxType =D3D_FEATURE_LEVEL_9_2;
		}
		else if(strncmp(featureLevel, "D3D_FEATURE_LEVEL_9_1",50)==0)
		{
			dxType = D3D_FEATURE_LEVEL_9_1;
		}

		return dxType;
	}

	RETURN_VALUE DX11Renderer::createDevice()
	{
		SAFE_RELEASE(this->m_device);
		SAFE_RELEASE(this->m_immediateContext);

		 D3D_DRIVER_TYPE driverTypes[] =
		 {
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE
		};
		UINT numDriverTypes = 3;

		 D3D_FEATURE_LEVEL featureLevels[] =
		 {
			this->m_featureLevel, //user selected feature level
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};

		UINT numFeatureLevels = 4;

		//if user select feature level use it, instead of all levels
		if(this->m_featureLevel!=D3D_FEATURE_LEVEL_11_0)
			numFeatureLevels = 1;

		UINT deviceFlags = 0;


		#ifdef _DEBUG
			deviceFlags = D3D11_CREATE_DEVICE_DEBUG;
		#endif

		/*HRESULT result = D3D11CreateDeviceAndSwapChain(NULL,
							D3D_DRIVER_TYPE_HARDWARE,
							NULL,
							deviceFlags,
							D3D11_SDK_VERSION,
							&swapChainDesc,
							&this->m_swapChain,
							&this->m_device);*/

		HRESULT result = E_FAIL;

		//if no drive was set, try all devices
		if(this->m_driverType==D3D_DRIVER_TYPE_UNKNOWN)
		{
			for(unsigned int i=0; i<numDriverTypes; ++i)
			{
				this->m_driverType = driverTypes[i];

				result = D3D11CreateDevice(NULL,
									this->m_driverType,
									NULL,
									deviceFlags,
									featureLevels,
									numFeatureLevels,
									D3D11_SDK_VERSION,
									&this->m_device,
									&this->m_featureLevel,
									&this->m_immediateContext);
		
	
				if(SUCCEEDED(result))
					break;
			}
		}
		else//drive was already set
		{
			result = D3D11CreateDevice(NULL,
									this->m_driverType,
									NULL,
									deviceFlags,
									featureLevels,
									numFeatureLevels,
									D3D11_SDK_VERSION,
									&this->m_device,
									&this->m_featureLevel,
									&this->m_immediateContext);
		}

		if(FAILED(result))
				return RETURN_INVALIDE_DEVICE;

		IDXGIDevice* dxgiDevice = 0;
		IDXGIAdapter* dxgiAdapter = 0;
		IDXGIFactory1* dxgiFactory = 0;

		this->m_device->QueryInterface(&dxgiDevice);
		dxgiDevice->GetAdapter(&dxgiAdapter);
		dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

		for(int i=0; i<this->m_outputWindows.size(); ++i)
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

			swapChainDesc.Windowed = !this->m_outputWindows[i]->windowDesc->fullscreen;
			swapChainDesc.SampleDesc.Count = this->m_outputWindows[i]->windowDesc->multiSamplingCount;
			swapChainDesc.SampleDesc.Quality = this->m_outputWindows[i]->windowDesc->multiSamplingLevel;
			swapChainDesc.OutputWindow = this->m_outputWindows[i]->windowDesc->hwnd;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1; 
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 60; //TODO: make configurable to enable/disable vsync
			swapChainDesc.BufferDesc.Format = (DXGI_FORMAT)this->m_states.backbufferFormat;
			swapChainDesc.BufferDesc.Height = (int)this->m_outputWindows[i]->windowDesc->height;
			swapChainDesc.BufferDesc.Width = (int)this->m_outputWindows[i]->windowDesc->width;
			swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			swapChainDesc.BufferCount = 1;
			swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			dxgiFactory->CreateSwapChain(this->m_device, &swapChainDesc, &this->m_outputWindows[i]->dxSwapChain);

			ID3D11Texture2D *backBuffer;
			if(FAILED(this->m_outputWindows[i]->dxSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer)))
				return RETURN_GET_BUFFER_ERROR;

			if(FAILED(this->m_device->CreateRenderTargetView(backBuffer, NULL, &this->m_outputWindows[i]->renderTarget)))
				return RETURN_RENDER_TARGET_VIEW_ERROR;

			backBuffer->Release();

			ID3D11Texture2D *depthStencilTex;
			D3D11_TEXTURE2D_DESC depthStencil;

			//ZeroMemory(&depthStencil, sizeof(D3D11_TEXTURE2D_DESC));
			depthStencil.Width		= (int)this->m_outputWindows[i]->windowDesc->width;
			depthStencil.Height		= (int)this->m_outputWindows[i]->windowDesc->height;
			depthStencil.MipLevels	= 1;
			depthStencil.ArraySize	= 1;
			depthStencil.Format		= (DXGI_FORMAT)this->m_states.depthStencilFormat;
			depthStencil.SampleDesc.Count	= this->m_outputWindows[i]->windowDesc->multiSamplingCount;
			depthStencil.SampleDesc.Quality = this->m_outputWindows[i]->windowDesc->multiSamplingLevel;
			depthStencil.Usage		= D3D11_USAGE_DEFAULT;
			depthStencil.BindFlags	= D3D11_BIND_DEPTH_STENCIL;
			depthStencil.CPUAccessFlags	= 0;
			depthStencil.MiscFlags		= 0;

			if(FAILED(this->m_device->CreateTexture2D(&depthStencil,NULL,&depthStencilTex)))
				return RETURN_CREATE_TEXTURE_ERROR;

			D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
			ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			dsDesc.Format = depthStencil.Format;
			if(depthStencil.SampleDesc.Count > 1 )
				dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			else
				dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsDesc.Flags = 0;
			dsDesc.Texture2D.MipSlice = 0;

			if(FAILED(this->m_device->CreateDepthStencilView(depthStencilTex,&dsDesc,&this->m_outputWindows[i]->depthStencilView)))
				return RETURN_DEPTH_STENCIL_VIEW_ERROR;
		}

		this->setCurrentOutputWindow(this->m_outputWindows[0]->id);
		this->m_immediateContext->OMSetRenderTargets(1, &this->m_outputWindows[0]->renderTarget, this->m_outputWindows[0]->depthStencilView);
		//this->m_device->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_primitiveTopology = PT_TRIANGLELIST;
		this->setPrimitive(this->m_primitiveTopology);

		this->setCullMode(this->m_states.cullMode);

		return RETURN_OK;
	}

	RETURN_VALUE DX11Renderer::setViewport(int x, int y, int width, int height)
	{
		if(this->m_device!=NULL)
		{
			this->m_viewport.Width = (float)width;
			this->m_states.viewport.width = width;
			this->m_viewport.Height = (float)height;
			this->m_states.viewport.height = height;
			this->m_viewport.MinDepth = this->m_states.viewport.minDepth = 0.0f;
			this->m_viewport.MaxDepth = this->m_states.viewport.maxDepth = 1.0f;
			this->m_viewport.TopLeftX = (float)x;
			this->m_states.viewport.x = x;
			this->m_viewport.TopLeftY = (float)y;
			this->m_states.viewport.y = y;

			this->m_immediateContext->RSSetViewports(1, &this->m_viewport);

			this->m_curEffect->setData("gViewport", &this->m_states.viewport, sizeof(Viewport));
		}
		else
			return RETURN_INVALIDE_DEVICE;

		return RETURN_OK;
	}

	Viewport DX11Renderer::getViewport()
	{
		return this->m_states.viewport;
	}

	void DX11Renderer::begin(bool clearRenderTarget, bool clearDepthStencil)
	{
		if(this->m_device && this->m_currentChain->renderTarget)
		{
			this->updateView();

			//this->m_immediateContext->OMSetDepthStencilState(0, 0);
			//float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
			//this->m_device->OMSetBlendState(0, blendFactors, 0xffffffff);

			Color clearColor = this->m_states.clearColor;

			if(clearRenderTarget)
				this->m_immediateContext->ClearRenderTargetView(this->m_currentChain->renderTarget,Color(clearColor.r, clearColor.g, clearColor.b, clearColor.a));

			if(clearDepthStencil)
				this->m_immediateContext->ClearDepthStencilView(this->m_currentChain->depthStencilView,D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			/*if(this->m_states.defaultEffect)
			{
				DX11Effect *dxEffect = (DX11Effect*)this->m_effect;
				dxEffect->getTech(0)->GetPassByIndex(0)->Apply(0, this->m_immediateContext);
				this->m_inputLayout->activate();
			}*/
		}
	}

	void DX11Renderer::end()
	{
		//GLOBALS->setDeltaTime(this->m_timer->getDelta());
		this->m_currentChain->dxSwapChain->Present(0,0);
	}

	void DX11Renderer::clear(float red, float green, float blue)
	{
		if(this->m_device)
		{
			this->m_immediateContext->ClearRenderTargetView(this->m_currentChain->renderTarget,D3DXCOLOR(red, green, blue, 0));
			this->m_immediateContext->ClearDepthStencilView(this->m_currentChain->depthStencilView,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}

	void DX11Renderer::update()
	{
		POINT screenPos;
		GetCursorPos(&screenPos);

		this->m_mouseHandler->setCurScreenPos(screenPos.x, screenPos.y);

		ScreenToClient(this->m_currentChain->windowDesc->hwnd,&screenPos);
		this->m_mouseHandler->setCurClientPos(screenPos.x,screenPos.y);

		float newX = screenPos.x/(float)this->m_currentChain->windowDesc->width*this->m_states.viewport.width;
		float newY = screenPos.y/(float)this->m_currentChain->windowDesc->height*this->m_states.viewport.height;
		this->m_mouseHandler->setCurUnprojPos((int)newX, (int)newY);
	}

	void DX11Renderer::present()
	{
		this->m_currentChain->dxSwapChain->Present(0,0);
	}


	void DX11Renderer::setFullScreen(bool fullscreen)
	{
		this->m_states.fullscreen = fullscreen;
	}

	bool DX11Renderer::getFullScreen()
	{
		return this->m_states.fullscreen;
	}

	void DX11Renderer::setKeyboardHandler(KeyboardHandler *keyboardHandler)
	{
		SAFE_DELETE(this->m_keyboardHandler);
		this->m_keyboardHandler = keyboardHandler;
	}

	void DX11Renderer::setMouseHandler(MouseHandler *mouseHandler)
	{
		SAFE_DELETE(this->m_mouseHandler);
		this->m_mouseHandler = mouseHandler;
	}

	RETURN_VALUE DX11Renderer::setEffect(IEffect *newEffect)
	{
		if(!newEffect)
			return RETURN_EFFECT_ERROR;

		this->m_curEffect = newEffect;
		return RETURN_OK;
	}

	IEffect* DX11Renderer::getEffect()
	{
		return (IEffect*)this->m_curEffect;
	}

	void DX11Renderer::setDefaultEffect()
	{
		this->m_curEffect = this->m_fixedFuncEffect;
		this->m_inputLayout = this->m_fixedFuncInputLayout;
	}

	RETURN_VALUE DX11Renderer::setCurrentOutputWindow(int id)
	{
		for(int i=0; i<this->m_outputWindows.size(); ++i)
		{
			if(this->m_outputWindows[i]->id == id)
			{
				this->m_currentChain = this->m_outputWindows[i];
				return RETURN_OK;
			}
		}

		return RETURN_ERROR;
	}

	RETURN_VALUE DX11Renderer::setInputLayout(IInputLayout *newInputLayout)
	{
		this->m_inputLayout = (DX11InputLayout*)newInputLayout;
		return RETURN_OK;
	}

	IInputLayout* DX11Renderer::getInputLayout()
	{
		return (IInputLayout*)this->m_inputLayout;
	}

	IEffect* DX11Renderer::createEffect()
	{
		if(!this->m_device || !this->m_immediateContext)
			return NULL;

		return new DX11Effect(this->m_device, this->m_immediateContext);
	}

	IInputLayout* DX11Renderer::createInputLayout()
	{
		return new DX11InputLayout(this->m_immediateContext); 
	}

	ISprite* DX11Renderer::createSprite()
	{
		return new DX11Sprite(this->m_immediateContext);
	}

	void DX11Renderer::begin2D()
	{
		m_savedLighting = this->isLighting();
		this->setLighting(false);
		this->m_buffer2DPrimitive->activate();
		this->m_inputLayout2D->activate();
		this->apply(1,0);
	}

	void DX11Renderer::drawRect(float x, float y, float width, float height,Color color)
	{
		this->drawRect(x, y, width, height, 0.0f, color);
	}

	void DX11Renderer::drawRect(float x, float y, float width, float height, float depth, Color color)
	{
		VertexPCUV rectVertices[5];

		rectVertices[0].position.z = depth;
		rectVertices[1].position.z = depth;
		rectVertices[2].position.z = depth;
		rectVertices[3].position.z = depth;
		rectVertices[4].position.z = depth;

		//first and list points are the same to close the rect
		rectVertices[0].position.x = rectVertices[4].position.x = x;
		rectVertices[0].position.y = rectVertices[4].position.y = y;
		rectVertices[0].texCoord0 =  rectVertices[4].texCoord0 =  Vector2D(0,0);
		rectVertices[0].color = rectVertices[4].color = color;
		rectVertices[1].position.x = x+width;
		rectVertices[1].position.y = y;
		rectVertices[1].texCoord0 =  Vector2D(1,0);
		rectVertices[1].color = color;		
		rectVertices[2].position.x = x+width;
		rectVertices[2].position.y = y+height;
		rectVertices[2].texCoord0 =  Vector2D(1,1);
		rectVertices[2].color = color;
		rectVertices[3].position.x = x;
		rectVertices[3].position.y = y+height;
		rectVertices[1].texCoord0 =  Vector2D(0,1);
		rectVertices[3].color = color;

		VertexPCUV *verts = (VertexPCUV*)this->m_buffer2DPrimitive->map(MT_WRITE_DISCARD);
		memcpy(verts,rectVertices,sizeof(VertexPCUV)*5);
		this->m_buffer2DPrimitive->unmap();

		bool lastWireframe = this->isWireframe();
		this->setWireframe(true);
		this->setPrimitive(PT_LINESTRIP);
		this->draw(5,0);
		this->setWireframe(lastWireframe);
	}

	void DX11Renderer::drawFilledRect(float x, float y, float width, float height, Color color)
	{
		this->drawFilledRect(x,y,width,height,0,color);
	}

	void DX11Renderer::drawFilledRect(float x, float y, float width, float height, float depth, Color color)
	{
		VertexPCUV rectVertices[4];

		rectVertices[0].position.z = depth;
		rectVertices[1].position.z = depth;
		rectVertices[2].position.z = depth;
		rectVertices[3].position.z = depth;

		rectVertices[0].position.x = x;
		rectVertices[0].position.y = y;
		rectVertices[0].texCoord0 =  Vector2D(0,0);
		rectVertices[0].color = color;
		rectVertices[1].position.x = x+width;
		rectVertices[1].position.y = y;
		rectVertices[1].texCoord0 =  Vector2D(1,0);
		rectVertices[1].color = color;
		rectVertices[2].position.x = x;
		rectVertices[2].position.y = y+height;
		rectVertices[2].texCoord0 =  Vector2D(0,1);
		rectVertices[2].color = color;
		rectVertices[3].position.x = x+width;
		rectVertices[3].position.y = y+height;
		rectVertices[3].texCoord0 =  Vector2D(1,1);
		rectVertices[3].color = color;

		VertexPCUV *verts = NULL;
		verts = (VertexPCUV*)this->m_buffer2DPrimitive->map(MT_WRITE_DISCARD);
		if(verts!=NULL)
			memcpy(verts,rectVertices,sizeof(VertexPCUV)*4);
		this->m_buffer2DPrimitive->unmap();

		this->setPrimitive(PT_TRIANGLESTRIP);
		this->draw(4,0);
	}

	void DX11Renderer::drawFilledRect(float x, float y, float width, float height, float depth, Gradient gradient)
	{
		VertexPCUV rectVertices[4];

		//color for all four courners. enumerated clockwise
		Color color1;
		Color color2;
		Color color3;
		Color color4;

		switch(gradient.type)
		{
			case GRADIENT_VERTICAL:
			{
					color1 = gradient.startColor;
					color2 = gradient.startColor;
					color3 = gradient.endColor;
					color4 = gradient.endColor;
			}break;
			case GRADIENT_HORIZONTAL:
			{
					color1 = gradient.startColor;
					color2 = gradient.endColor;
					color3 = gradient.startColor;
					color4 = gradient.endColor;
			}break;
			case GRADIENT_MIDDLE_CROSS:
			{
					color1 = gradient.startColor;
					color2 = gradient.endColor;
					color3 = gradient.endColor;
					color4 = gradient.startColor;
			}break;
		}

		rectVertices[0].position.z = depth;
		rectVertices[1].position.z = depth;
		rectVertices[2].position.z = depth;
		rectVertices[3].position.z = depth;

		rectVertices[0].position.x = x;
		rectVertices[0].position.y = y;
		rectVertices[0].texCoord0 =  Vector2D(0,0);
		rectVertices[0].color = color1;
		rectVertices[1].position.x = x+width;
		rectVertices[1].position.y = y;
		rectVertices[1].texCoord0 =  Vector2D(1,0);
		rectVertices[1].color = color2;
		rectVertices[2].position.x = x;
		rectVertices[2].position.y = y+height;
		rectVertices[2].texCoord0 =  Vector2D(0,1);
		rectVertices[2].color = color3;
		rectVertices[3].position.x = x+width;
		rectVertices[3].position.y = y+height;
		rectVertices[3].texCoord0 =  Vector2D(1,1);
		rectVertices[3].color = color4;

		VertexPCUV *verts = (VertexPCUV*)this->m_buffer2DPrimitive->map(MT_WRITE_DISCARD);
		memcpy(verts,rectVertices,sizeof(VertexPCUV)*4);
		this->m_buffer2DPrimitive->unmap();

		this->setPrimitive(PT_TRIANGLESTRIP);
		this->draw(4,0);
	}

	//TODO: batch the primities to prevent locking vertexbuffer every frame!
	void DX11Renderer::drawLine(float xStart, float yStart, float xEnd, float yEnd, Color color)
	{
		this->drawLine(xStart, yStart, xEnd, yEnd, 0, color);
	}

	void DX11Renderer::drawLine(float xStart, float yStart, float xEnd, float yEnd, float depth, Color color)
	{
		/*Vertex lineVertices[2];

		lineVertices[0].position.z = depth;
		lineVertices[1].position.z = depth;

		lineVertices[0].position.x = xStart;
		lineVertices[0].position.y = yStart;
		lineVertices[0].color = color;
		lineVertices[1].position.x = xEnd;
		lineVertices[1].position.y = yEnd;
		lineVertices[1].color = color;

		Vertex *verts = (Vertex*)this->m_buffer2DPrimitive->map(MT_WRITE_DISCARD);
		memcpy(verts,lineVertices,sizeof(Vertex)*2);
		this->m_buffer2DPrimitive->unmap();

		this->setPrimitive(PT_LINELIST);
		this->draw(2,0);*/

		this->drawLine(Vector3D(xStart,yStart,depth),Vector3D(xEnd, yEnd, depth), color);
	}

	void DX11Renderer::drawLine(Vector3D start, Vector3D end, Color color)
	{
		VertexPCUV lineVertices[2];

		lineVertices[0].position = start;
		//lineVertices[0].normal   = start;
		lineVertices[0].color = color;
		lineVertices[1].position = end;
		//lineVertices[1].normal   = start;
		lineVertices[1].color = color;

		VertexPCUV *verts = (VertexPCUV*)this->m_buffer2DPrimitive->map(MT_WRITE_DISCARD);
		memcpy(verts,lineVertices,sizeof(VertexPCUV)*2);
		this->m_buffer2DPrimitive->unmap();

		this->setPrimitive(PT_LINELIST);
		this->draw(2,0);
	}

	void DX11Renderer::drawCircle(float xCenter, float yCenter, float radius)
	{
		//TODO
	}

	void DX11Renderer::drawCircle(float xCenter, float yCenter, float depth, float radius)
	{
		//TODO
	}

	void DX11Renderer::end2D()
	{
		this->setLighting(this->m_savedLighting);	
		this->apply(1,0);

		this->m_fixedFuncInputLayout->activate();
	}

	void DX11Renderer::release()
	{	
		if(this->m_immediateContext)
			this->m_immediateContext->ClearState();

		SAFE_RELEASE(this->m_immediateContext);
		SAFE_RELEASE(this->m_rasterizerState);
		SAFE_DELETE(this->m_inputLayout);
		SAFE_DELETE(this->m_curEffect);
		SAFE_DELETE(this->m_keyboardHandler);
		SAFE_DELETE(this->m_camera);
		SAFE_DELETE(this->m_timer);		
		SAFE_RELEASE(this->m_device);
	}

	HWND DX11Renderer::createWindow(TCHAR *title, int width, int height)
	{
		WNDCLASSEX wndclass;

		wndclass.cbSize = sizeof(WNDCLASSEX); 
		wndclass.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW; 
		wndclass.lpfnWndProc = (WNDPROC)DX11Renderer::staticWndProc; 
		wndclass.cbClsExtra = 0; 
		wndclass.cbWndExtra = 0; 
		wndclass.hInstance = GetModuleHandle(NULL);; 
		wndclass.hIcon = 0; 
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); 
		wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); 
		wndclass.lpszMenuName = NULL; 
		wndclass.lpszClassName = TEXT("clearskyClass"); 
		wndclass.hIconSm = 0;
	
		RegisterClassEx(&wndclass);

		RECT rec = {0,0,width, height};
		AdjustWindowRect(&rec,WS_OVERLAPPEDWINDOW,false);

		HWND hwnd = CreateWindow(TEXT("clearskyClass"),
								(LPCWSTR)title,
								WS_OVERLAPPEDWINDOW,
								0,
								0,
								rec.right-rec.left,
								rec.bottom-rec.top,
								0,
								0,
								wndclass.hInstance,
								NULL);
		if (!hwnd)
		{
			return NULL;
		}

		SetWindowLong(hwnd, GWL_USERDATA, (LONG)this);
		// Display the window on the screen
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);

		//center the cursor
		//TODO: center properly with changing window position etc.
		SetCursorPos(width>>1, height>>1);

		return hwnd;
	}

	//read default setting for render states(lighting, texturing etc.) from the shader
	void DX11Renderer::readDefaultShaderValues()
	{
		this->m_states.lighting			= this->m_curEffect->getBool("lightOn");
		this->m_states.fogging			= this->m_curEffect->getBool("fogOn");
		this->m_states.texturing		= this->m_curEffect->getBool("textureOn");
		this->m_states.specTexturing	= this->m_curEffect->getBool("gSpecTextureOn");
		this->m_states.heightTexturing	= this->m_curEffect->getBool("gHeightTextureOn");
		this->m_states.normalsPosition	= this->m_curEffect->getBool("gNormalsPosition");
		this->m_states.alphamapping		= this->m_curEffect->getBool("alphaMapOn");

	}

	void DX11Renderer::updateView()
	{
		this->setViewTransformation(this->m_camera->position,this->m_camera->lookDir,this->m_camera->up);
		//transform = this->m_world*this->m_view*this->m_proj;
		ID3DX11EffectMatrixVariable* eWorld = ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("world")->AsMatrix();
		ID3DX11EffectMatrixVariable* eView	= ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("view")->AsMatrix();
		ID3DX11EffectMatrixVariable* eProj	= ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("projection")->AsMatrix();
		ID3DX11EffectMatrixVariable* eMVP	= ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("mvp")->AsMatrix();

		this->m_mvp = this->m_world*this->m_view*this->m_proj;

		eWorld->SetMatrix((float*)&this->m_world);
		eView->SetMatrix((float*)&this->m_view);
		eProj->SetMatrix((float*)&this->m_proj);
		eMVP->SetMatrix((float*)&this->m_mvp);

		//ID3DX11EffectScalarVariable *time = this->m_effect->getEffect()->GetVariableByName("fTime")->AsScalar();
		//time->SetFloat(Timer::getTime()*0.001);
		//float time = this->m_timer->getTickCount()/100000000;

		this->m_curEffect->setData("gEyePosW",&this->m_camera->position,sizeof(4*3));

		
	}

	void DX11Renderer::apply(int tech, int pass)
	{
		this->updateView();
		this->m_curEffect->apply(tech,pass);
		//this->m_defaultEffect->apply(tech, pass);
	}

	RETURN_VALUE DX11Renderer::setViewTransformation(Vector3D position, Vector3D direction, Vector3D up)
	{
		D3DXMatrixLookAtLH(&this->m_view, &position, &direction, &up);

		return RETURN_OK;
	}

	Matrix DX11Renderer::getViewTransformation()
	{
		return this->m_view;
	}

	RETURN_VALUE DX11Renderer::setProjection(int width, int height, float nearPlane, float farPlane, float fov)
	{
		this->m_states.aspectRation = (float)width / (float)height;

		D3DXMatrixPerspectiveFovLH(&this->m_proj, float(DEG_TO_RAD(fov)),this->m_states.aspectRation, nearPlane, farPlane);
		
		this->m_currentChain->windowDesc->width  = width;
		this->m_currentChain->windowDesc->height = height;
		GLOBALS->setWidth(width);
		GLOBALS->setHeight(height);

		return RETURN_OK;
	}

	RETURN_VALUE DX11Renderer::setOrthoProjection(int width, int height, float nearPlane, float farPlane)
	{
		D3DXMatrixOrthoLH(&this->m_proj, (float)width, (float)height, nearPlane, farPlane);
		
		this->m_currentChain->windowDesc->width  = width;
		this->m_currentChain->windowDesc->height = height;

		return RETURN_OK;
	}

	void DX11Renderer::addOutputWindow(WindowDesc *window, int id)
	{
		SwapChain *chain = new SwapChain;
		chain->id = id;
		chain->windowDesc = window;

		this->m_outputWindows.push_back(chain);
	}

	Matrix DX11Renderer::getProjection()
	{
		return this->m_proj;
	}

	float DX11Renderer::setFOV(float newFOV)
	{
		float oldFOV = this->m_states.fov;

		setProjection((int)this->m_currentChain->windowDesc->width, (int)this->m_currentChain->windowDesc->height, this->m_states.nearPlane, this->m_states.farPlane,newFOV);
		this->m_states.fov = newFOV;

		return oldFOV;
	}

	float DX11Renderer::getFOV()
	{
		return this->m_states.fov;
	}

	float DX11Renderer::getAspectRation()
	{
		return this->m_states.aspectRation;
	}

	//transform ray from screenspace to world space
	void DX11Renderer::rayFromScreen(const int x, const int y, Vector3D &rayOrigin, Vector3D &rayDir)
	{
		//float d1 = 1/(this->m_states.aspectRation*tan(this->m_states.fov/2));
		//float d2 = 1/(tan(this->m_states.fov/2));
		Matrix proj = this->getProjection();

		//getRay from the screenpoints in viewspace
		float viewX = ((2.0f*x)/this->m_currentChain->windowDesc->width-1.0f)/proj(0,0);//d1;
		float viewY = ((-2.0f*y)/this->m_currentChain->windowDesc->height+1.0f)/proj(1,1);//d2;

		Vector3D rayOriginV = Vector3D(0,0,0);
		Vector3D rayDirV    = Vector3D(viewX, viewY, 1.0f);

		//std::cout<<"oX: "<<rayOrigin.x<<" oY: "<<rayOrigin.y<<" oZ: "<<rayOrigin.z<<"\n"<<" dX: "<<rayDir.x<<" dY: "<<rayDir.y<<" dZ: "<<rayDir.z<<std::endl;
		//std::cout<<"ascpect: "<<this->m_states.aspectRation<<std::endl;
		Matrix view = this->getViewTransformation();
		Matrix inverseView;
		D3DXMatrixInverse(&inverseView,0,&view);

		D3DXVec3TransformCoord(&rayOrigin,&rayOriginV,&inverseView);
		D3DXVec3TransformNormal(&rayDir, &rayDirV, &inverseView);
	}

	void DX11Renderer::unproject(const int x, const int y, Vector3D &worldPoint)
	{
		Matrix proj = this->getProjection();
		Vector3D pointView = Vector3D(0,0,0);
		Matrix view = this->getViewTransformation();
		Matrix inverseView;
		D3DXMatrixInverse(&inverseView,0,&view);

		D3DXVec3TransformCoord(&worldPoint,&pointView,&inverseView);

	}

	ID3D11Device *DX11Renderer::getDevice()
	{
		return this->m_device;
	}

	bool DX11Renderer::isWireframe()
	{
		return this->m_rasterizerDesc.FillMode==FILL_MODE_WIREFRAME?true:false;
	}

	RETURN_VALUE DX11Renderer::setWireframe(bool wireframe)
	{
		if(!this->m_device)
			return RETURN_ERROR;

		if(this->m_states.wireframe != wireframe)
		{
			this->m_rasterizerStateDesc->fillMode =  wireframe?FILL_MODE_WIREFRAME:FILL_MODE_SOLID;
			//if(this->m_device->CreateRasterizerState(&this->m_rasterizerDesc, &this->m_rasterizerState)!=S_OK)
			//	return RETURN_ERROR;

			//this->m_immediateContext->RSSetState(this->m_rasterizerState);
			this->setRasterizerState(this->m_rasterizerStateDesc);
			this->m_states.wireframe = wireframe;
		}

		return RETURN_OK;
	}

	bool DX11Renderer::setFrontOrder(bool cw)
	{
		if(!this->m_device)
			return RETURN_ERROR;

		bool lastOrdner = this->m_states.frontOrdnerCw;
		if(this->m_states.frontOrdnerCw != cw)
		{
			

			this->m_rasterizerDesc.FrontCounterClockwise = !cw;
			if(this->m_device->CreateRasterizerState(&this->m_rasterizerDesc, &this->m_rasterizerState)!=S_OK)
				return RETURN_ERROR;

			this->m_immediateContext->RSSetState(this->m_rasterizerState);
			this->m_states.frontOrdnerCw = cw;
		}

		return lastOrdner;
	}

	RETURN_VALUE DX11Renderer::setLighting(bool enable)
	{
		if((this->m_states.lighting!=enable) && this->m_curEffect)
		{
			ID3DX11EffectScalarVariable* lightOn = ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("lightOn")->AsScalar();
			lightOn->SetBool(enable);
			this->m_states.lighting = enable;
		}
		return RETURN_OK;
	}

	bool DX11Renderer::isLighting()
	{
		return this->m_states.lighting;
	}

	RETURN_VALUE DX11Renderer::setTexturing(bool enableTex)
	{
		if((this->m_states.texturing!=enableTex) && this->m_curEffect)
		{

			ID3DX11EffectScalarVariable* texOn = ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("textureOn")->AsScalar();
			texOn->SetBool(enableTex);
			this->m_states.texturing = enableTex;
		}

		return RETURN_OK;
	}

	bool DX11Renderer::isTexturing()
	{
		return this->m_states.texturing;
	}

	RETURN_VALUE DX11Renderer::setSpecTexturing(bool enableTex)
	{
		if((this->m_states.specTexturing!=enableTex) && this->m_curEffect)
		{

			ID3DX11EffectScalarVariable* texOn = ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("specTextureOn")->AsScalar();
			texOn->SetBool(enableTex);
			this->m_states.specTexturing = enableTex;
		}

		return RETURN_OK;
	}

	bool DX11Renderer::isSpecTexturing()
	{
		return this->m_states.specTexturing;
	}

	RETURN_VALUE DX11Renderer::setHeightTexturing(bool enableTex)
	{
		if((this->m_states.heightTexturing!=enableTex) && this->m_curEffect)
		{
			this->m_curEffect->setBool("heightTextureOn",enableTex);
			this->m_states.heightTexturing = enableTex;
		}

		return RETURN_OK;
	}

	bool DX11Renderer::isHeightTexturing()
	{
		return this->m_states.heightTexturing;
	}

	RETURN_VALUE DX11Renderer::setAlphaMapping(bool enable)
	{
		if((this->m_states.alphamapping!=enable) && this->m_curEffect)
		{

			ID3DX11EffectScalarVariable* alphaOn = ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("alphaMapOn")->AsScalar();
			alphaOn->SetBool(enable);
			this->m_states.alphamapping = enable;
		}

		return RETURN_OK;
	}

	bool DX11Renderer::isAlphaMapping()
	{
		return this->m_states.alphamapping;
	}

	CULL_MODE DX11Renderer::setCullMode(CULL_MODE newMode)
	{
		CULL_MODE oldMode = this->m_states.cullMode;

		if(this->m_states.cullMode != newMode)
		{
			D3D11_CULL_MODE cullMode = D3D11_CULL_BACK;

			switch(newMode)
			{
				case CULL_MODE_FRONT:
				{
					cullMode = D3D11_CULL_FRONT;
				}break;
				case CULL_MODE_BACK:
				{
					cullMode = D3D11_CULL_BACK;
				}break;
				case CULL_MODE_NONE:
				{
					cullMode = D3D11_CULL_NONE;
				}break;
			}

			this->m_rasterizerDesc.CullMode = cullMode;
			this->m_device->CreateRasterizerState(&this->m_rasterizerDesc, &this->m_rasterizerState);
			this->m_immediateContext->RSSetState(this->m_rasterizerState);

			this->m_states.cullMode = newMode;
		}

		return oldMode;
	}

	CULL_MODE DX11Renderer::getCullMode()
	{
		return this->m_states.cullMode;
	}

	RETURN_VALUE DX11Renderer::setBlending(bool enable)
	{
		if(this->m_states.blending!=enable)
		{
			this->m_blendDesc.IndependentBlendEnable = enable;
			this->m_blendDesc.RenderTarget[0].BlendEnable = enable;
			this->m_device->CreateBlendState(&this->m_blendDesc,&m_blendState);

			float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
			this->m_immediateContext->OMSetBlendState(this->m_blendState,blendFactors,0xffffff);
			
			this->m_states.blending=enable;
		}

		return RETURN_OK;
	}

	bool DX11Renderer::isBlending()
	{
		return this->m_states.blending;
	}

	RETURN_VALUE DX11Renderer::setFogging(bool enable)
	{
		if(this->m_states.fogging!=enable)
		{
			ID3DX11EffectScalarVariable* fogOn = ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("fogOn")->AsScalar();
			fogOn->SetBool(enable);
			this->m_states.fogging = enable;
		}

		return RETURN_OK;
	}

	bool DX11Renderer::isFogging()
	{
		return this->m_states.fogging;
	}

	Color DX11Renderer::setClearColor(float r, float g, float b, float a)
	{
		Color oldColor = this->m_states.clearColor;
		this->m_states.clearColor = Color(r,g,b,a);

		return oldColor;
	}

	Color DX11Renderer::getClearColor()
	{
		return this->m_states.clearColor;
	}

	void DX11Renderer::depthEnable(bool enable)
	{
		//TODO
	}

	bool DX11Renderer::isDepthEnabled()
	{
		//TODO
		return true;
	}

	RETURN_VALUE DX11Renderer::setMultisampling(int sampleCount, int qualityLevel)
	{
		this->m_states.sampleCount = sampleCount;
		this->m_states.sampleLevel = qualityLevel;

		return RETURN_OK;
	}

	RETURN_VALUE DX11Renderer::setBackbufferFormat(DATA_FORMAT format)
	{
		this->m_states.backbufferFormat = format;
		return RETURN_OK;
	}

	RETURN_VALUE DX11Renderer::setDepthStencilFormat(DATA_FORMAT format)
	{
		this->m_states.depthStencilFormat = format;
		return RETURN_OK;
	}

	RETURN_VALUE DX11Renderer::setNormalsPosition(bool enable)
	{
		if(this->m_states.normalsPosition!=enable)
			this->m_curEffect->setBool("gNormalsPosition",enable);

		return RETURN_OK;
	}

	bool DX11Renderer::isNormalsPosition()
	{
		return this->m_states.normalsPosition;
	}

	RETURN_VALUE DX11Renderer::setBlendState(BlendState *newBlendState)
	{
		this->m_blendStateDesc = newBlendState;

		return RETURN_OK;
	}

	BlendState* DX11Renderer::getBlendState()
	{
		return this->m_blendStateDesc;
	}

	D3D11_RASTERIZER_DESC DX11Renderer::convertRasterizerDesc(RasterizerState *newRasterizerState)
	{
		D3D11_RASTERIZER_DESC rasterizerDesc;

		rasterizerDesc.FillMode = (D3D11_FILL_MODE)newRasterizerState->fillMode;
		rasterizerDesc.CullMode = (D3D11_CULL_MODE)newRasterizerState->cullMode;
		rasterizerDesc.FrontCounterClockwise = newRasterizerState->frontCCW;
		rasterizerDesc.DepthBias = newRasterizerState->depthBias;
		rasterizerDesc.DepthBiasClamp  = newRasterizerState->depthBiasClamp;
		rasterizerDesc.SlopeScaledDepthBias = newRasterizerState->slopeScaledDepthBias;
		rasterizerDesc.DepthClipEnable      = newRasterizerState->depthClip;
		rasterizerDesc.ScissorEnable        = newRasterizerState->scissor;
		rasterizerDesc.MultisampleEnable    = newRasterizerState->multisampling;
		rasterizerDesc.AntialiasedLineEnable = newRasterizerState->antialisedLine;

		return rasterizerDesc;
	}

	D3D11_SAMPLER_DESC DX11Renderer::convertSamplerDesc(SamplerState *newSamplerState)
	{
		D3D11_SAMPLER_DESC samplerDesc;

		samplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)newSamplerState->addressU;
		samplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)newSamplerState->addressV;
		samplerDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)newSamplerState->addressW;
		samplerDesc.BorderColor[0] = newSamplerState->borderColor[0];
		samplerDesc.BorderColor[1] = newSamplerState->borderColor[1];
		samplerDesc.BorderColor[2] = newSamplerState->borderColor[2];
		samplerDesc.BorderColor[3] = newSamplerState->borderColor[3];
		samplerDesc.ComparisonFunc = (D3D11_COMPARISON_FUNC)newSamplerState->comparisonFunc;
		samplerDesc.Filter		   = (D3D11_FILTER)newSamplerState->filter;
		samplerDesc.MaxAnisotropy  = newSamplerState->maxAnisotropy;
		samplerDesc.MaxLOD         = newSamplerState->maxLOD;
		samplerDesc.MinLOD		   = newSamplerState->minLOD;
		samplerDesc.MipLODBias	   = newSamplerState->mipLODBias;

		return samplerDesc;
	}

	RETURN_VALUE DX11Renderer::setRasterizerState(RasterizerState *newRasterizerState)
	{
		this->m_rasterizerStateDesc = newRasterizerState;
		this->m_rasterizerDesc = this->convertRasterizerDesc(newRasterizerState);

		if(FAILED(this->m_device->CreateRasterizerState(&this->m_rasterizerDesc, &this->m_rasterizerState)))
			return RETURN_ERROR;

		this->m_immediateContext->RSSetState(this->m_rasterizerState);

		return RETURN_OK;
	}

	RasterizerState* DX11Renderer::getRasterizerState()
	{
		return this->m_rasterizerStateDesc;
	}

	D3D11_DEPTH_STENCILOP_DESC DX11Renderer::convertDepthStencilFuncDesc(DepthStencilFuncDesc funcDesc)
	{
		D3D11_DEPTH_STENCILOP_DESC dxDesc;
		dxDesc.StencilDepthFailOp = (D3D11_STENCIL_OP)funcDesc.stencilDepthFailFunc;
		dxDesc.StencilFailOp      = (D3D11_STENCIL_OP)funcDesc.stencilFailFunc;
		dxDesc.StencilFunc		  = (D3D11_COMPARISON_FUNC)funcDesc.stencilFunc;
		dxDesc.StencilPassOp      = (D3D11_STENCIL_OP)funcDesc.stencilPassFunc;

		return dxDesc;
	}

	D3D11_DEPTH_STENCIL_DESC DX11Renderer::convertDepthStencilDesc(DepthStencilState *depthStencilState)
	{
		D3D11_DEPTH_STENCIL_DESC dxDesc;

		dxDesc.BackFace         = this->convertDepthStencilFuncDesc(depthStencilState->backFace);
		dxDesc.DepthEnable		 = depthStencilState->depthEnable;
		dxDesc.DepthFunc        = (D3D11_COMPARISON_FUNC)depthStencilState->depthFunc;
		dxDesc.DepthWriteMask   = (D3D11_DEPTH_WRITE_MASK)depthStencilState->depthWriteMask;
		dxDesc.FrontFace        = this->convertDepthStencilFuncDesc(depthStencilState->frontFace);
		dxDesc.StencilEnable	 = depthStencilState->stencilEnable;
		dxDesc.StencilReadMask  = (UINT8)depthStencilState->stencilReadMask;
		dxDesc.StencilWriteMask = (UINT8)depthStencilState->stencilWriteMask;

		return dxDesc;
	
	}

	RETURN_VALUE DX11Renderer::setDepthStencilState(DepthStencilState *newDepthStencilState)
	{
		this->m_depthStencilStateDesc = newDepthStencilState;
		this->m_depthStencilDesc = this->convertDepthStencilDesc(newDepthStencilState);

		if(FAILED(this->m_device->CreateDepthStencilState(&this->m_depthStencilDesc, &this->m_depthStencilState)))
			return RETURN_ERROR;

		this->m_immediateContext->OMSetDepthStencilState(this->m_depthStencilState, 0);

		return RETURN_OK;
	}

	DepthStencilState* DX11Renderer::getDepthStencilState()
	{
		return this->m_depthStencilStateDesc;
	}

	RETURN_VALUE DX11Renderer::setSamplerState(SamplerState *newSamplerState)
	{
		this->m_samplerStateDesc = newSamplerState;
		this->m_samplerDesc = this->convertSamplerDesc(newSamplerState);

		if(FAILED(this->m_device->CreateSamplerState(&this->m_samplerDesc, &this->m_samplerState)))
			return RETURN_ERROR;

		//this->m_immediateContext->PSSetSamplers(0,1,&this->m_samplerState);

		return RETURN_OK;
	}

	SamplerState* DX11Renderer::getSamplerState()
	{
		return this->m_samplerStateDesc;
	}

	void DX11Renderer::pushBlendState()
	{
		this->m_savedBlendStates.push_back(this->m_blendStateDesc);
	}

	void DX11Renderer::popBlendState()
	{
		if(!this->m_savedBlendStates.empty())
		{
			BlendState *lastBlendState=this->m_savedBlendStates.back();
			this->setBlendState(lastBlendState);
			this->m_savedBlendStates.pop_back();
		}
	}

	void DX11Renderer::pushRasterizerState()
	{
		this->m_savedRasterizerStates.push_back(this->m_rasterizerStateDesc);
	}

	void DX11Renderer::popRasterizerState()
	{
		if(!this->m_savedRasterizerStates.empty())
		{
			RasterizerState *lastRasterizerState=this->m_savedRasterizerStates.back();
			this->setRasterizerState(lastRasterizerState);
			this->m_savedRasterizerStates.pop_back();
		}
	}

	void DX11Renderer::pushDepthStencilState()
	{
		this->m_savedDepthStencilStates.push_back(this->m_depthStencilStateDesc);
	}

	void DX11Renderer::popDepthStencilState()
	{
		if(!this->m_savedDepthStencilStates.empty())
		{
			DepthStencilState *lastDepthStencilState=this->m_savedDepthStencilStates.back();
			this->setDepthStencilState(lastDepthStencilState);
			this->m_savedDepthStencilStates.pop_back();
		}
	}

	void DX11Renderer::pushSamplerState()
	{
		this->m_savedSamplerStates.push_back(this->m_samplerStateDesc);
	}

	void DX11Renderer::popSamplerState()
	{
		if(!this->m_savedSamplerStates.empty())
		{
			SamplerState *lastSamplerState=this->m_savedSamplerStates.back();
			this->setSamplerState(lastSamplerState);
			this->m_savedSamplerStates.pop_back();
		}
	}

	void DX11Renderer::pushEffect()
	{
		this->m_savedEffects.push_back(this->m_curEffect);
	}

	void DX11Renderer::popEffect()
	{
		if(!this->m_savedEffects.empty())
		{
			IEffect *effect = this->m_savedEffects.back();
			this->setEffect(effect);
			this->m_savedEffects.pop_back();
		}
	}

	void DX11Renderer::SetScissorRect(Rect rect)
	{
		this->m_scissorRect = rect;

		D3D11_RECT dxRect[1];
		dxRect[0].bottom = rect.bottom;
		dxRect[0].left   = rect.left;
		dxRect[0].right  = rect.right;
		dxRect[0].top    = rect.top;

		this->m_immediateContext->RSSetScissorRects(1,dxRect);
	}

	void DX11Renderer::SetScissorRect(int left, int top, int right, int bottom)
	{
		D3D11_RECT dxRect[1];
		this->m_scissorRect.left	= dxRect[0].left   = left;
		this->m_scissorRect.right	= dxRect[0].right  = right;
		this->m_scissorRect.top		= dxRect[0].top    = top;
		this->m_scissorRect.bottom	= dxRect[0].bottom = bottom;

		this->m_immediateContext->RSSetScissorRects(1, dxRect);
	}

	Rect DX11Renderer::getScissorRect()
	{
		return this->m_scissorRect;
	}


	PRIMITIVE_TOPOLOGY DX11Renderer::setPrimitive(PRIMITIVE_TOPOLOGY topology)
	{
		if(this->m_device)
		{
			D3D11_PRIMITIVE_TOPOLOGY newTopology = this->convertTopology(topology);
			this->m_immediateContext->IASetPrimitiveTopology(newTopology);

			this->m_primitiveTopology = topology;
		}

		return this->m_primitiveTopology;
	}

	PRIMITIVE_TOPOLOGY DX11Renderer::getPrimitive()
	{
		return this->m_primitiveTopology;
	}

	RETURN_VALUE DX11Renderer::setLight(int index, Light *light)
	{
		if(index>=0 && index<MAX_NUM_LIGHTS)
		{
			ID3DX11EffectVariable* lightVar = ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("gLight");
			lightVar->SetRawValue(light->getProperty(),index*sizeof(LightProperty),sizeof(LightProperty));
			return RETURN_OK;
		}

		return RETURN_ERROR;
	}

	RETURN_VALUE DX11Renderer::setFog(Fog *fog)
	{
		ID3DX11EffectVariable* fogVar = ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("gFog");
		fogVar->SetRawValue(fog->getProperty(),0,sizeof(FogProperty));

		return RETURN_OK;
	}

	RETURN_VALUE DX11Renderer::setTexture(ITexture2D *texture, const unsigned int layer)
	{
		if(texture && layer>=0 && layer<MAX_NUM_TEXTURE_LAYERS)
		{
			DX11Texture2D *dxTexture = NULL;
			dxTexture = (DX11Texture2D*)texture;
			if(!dxTexture)
				return RETURN_ERROR;

			char textureName[50];
			sprintf_s(textureName, 49, "gTexture2D%d",layer);

			//add number of texture layers. because layers are numbered from zero, we need to add 1
			this->m_curEffect->setInt("gNumTextureLayers", layer+1);

			ID3DX11EffectShaderResourceVariable *effectTex = ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName(textureName)->AsShaderResource();
			if(!effectTex)
				return RETURN_ERROR;

			ID3D11ShaderResourceView *shaderView = dxTexture->getShaderView();
			if(!shaderView)
				return RETURN_ERROR;

			effectTex->SetResource(shaderView);

			return RETURN_OK;
		}

		return RETURN_ERROR;
	}

	RETURN_VALUE DX11Renderer::setSpecTexture(ITexture2D *texture)
	{
		this->m_curEffect->setTexture("gSpecTexture2D",texture);

		return RETURN_OK;
	}

	RETURN_VALUE DX11Renderer::setHeightTexture(ITexture2D *texture)
	{
		this->m_curEffect->setTexture("gHeightTexture2D",texture);

		return RETURN_OK;
	}

	void DX11Renderer::setAlphaTexture(ITexture2D *texture)
	{
		DX11Texture2D *alphaTexture = (DX11Texture2D*)texture;

		ID3DX11EffectShaderResourceVariable *effectTex = ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("gAlphaMap")->AsShaderResource();
		effectTex->SetResource(alphaTexture->getShaderView());
	}

	Material DX11Renderer::setMaterial(Material newMaterial)
	{
		EffectMaterial effectMat;
		effectMat.diffuse = newMaterial.diffuse;
		effectMat.ambient = newMaterial.ambient;
		effectMat.specular = newMaterial.specular;

		ID3DX11EffectVariable* material = ((DX11Effect*)this->m_curEffect)->getEffect()->GetVariableByName("gMaterial");
		material->SetRawValue(&effectMat,0,sizeof(EffectMaterial));

		Material oldMaterial = this->m_material;
		this->m_material = newMaterial;

		return oldMaterial;
	}

	Material DX11Renderer::getMaterial()
	{
		return this->m_material;
	}

	const char* DX11Renderer::getFeatureLevel()
	{
		switch(this->m_featureLevel)
		{
			case D3D_FEATURE_LEVEL_11_0:
			{
				return "D3D_FEATURE_LEVEL_11_0";
			}break;
			case D3D_FEATURE_LEVEL_10_1:
			{
				return "D3D_FEATURE_LEVEL_10_1";
			}break;
			case D3D_FEATURE_LEVEL_10_0:
			{
				return "D3D_FEATURE_LEVEL_10_0";
			}break;
			case D3D_FEATURE_LEVEL_9_3:
			{
				return "D3D_FEATURE_LEVEL_9_3";
			}break;
			case D3D_FEATURE_LEVEL_9_2:
			{
				return "D3D_FEATURE_LEVEL_9_2";
			}break;
			case D3D_FEATURE_LEVEL_9_1:
			{
				return "D3D_FEATURE_LEVEL_9_1";
			}break;
		}

		return NULL;
	}

	const char* DX11Renderer::getDriverType()
	{
		switch(this->m_driverType)
		{
			case D3D_DRIVER_TYPE_HARDWARE:
			{
				return "D3D_DRIVER_TYPE_HARDWARE";
			}break;
			case D3D_DRIVER_TYPE_WARP:
			{
				return "D3D_DRIVER_TYPE_WARP";
			}break;
			case D3D_DRIVER_TYPE_REFERENCE:
			{
				return "D3D_DRIVER_TYPE_REFERENCE";
			}
		};

		return NULL;
	}

	LRESULT CALLBACK DX11Renderer::staticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		DX11Renderer *window = (DX11Renderer*)GetWindowLong(hwnd,GWL_USERDATA);

		return window->wndProc(hwnd, msg, wParam, lParam);
	}

	/*void DX11Renderer::setEffect(IEffect *effect)
	{
		this->m_effect = effect;
	}

	IEffect* DX11Renderer::getEffect()
	{
		return this->m_effect;
	}*/

	void DX11Renderer::draw(int numVertexes, int startVertex)
	{
		this->m_immediateContext->Draw(numVertexes,startVertex);
	}

	void DX11Renderer::drawIndexed(int num, int startIndex, int baseIndex)
	{
		
		this->m_immediateContext->DrawIndexed(num,startIndex,baseIndex);
	}

	Camera *DX11Renderer::getCamera()
	{
		return this->m_camera;
	}

	void DX11Renderer::rotateY(float deg)
	{
		Matrix yRotation;
		D3DXMatrixRotationY(&yRotation, DEG_TO_RAD(deg));
		this->m_world *= yRotation;
	}

	void DX11Renderer::rotateX(float deg)
	{
		Matrix xRotation;
		D3DXMatrixRotationX(&xRotation, DEG_TO_RAD(deg));
		this->m_world *= xRotation;
	}

	void DX11Renderer::rotateZ(float deg)
	{
		Matrix zRotation;
		D3DXMatrixRotationZ(&zRotation, DEG_TO_RAD(deg));
		this->m_world *= zRotation;
	}

	void DX11Renderer::translate(float x, float y, float z)
	{
		Matrix translation;
		D3DXMatrixTranslation(&translation, x, y, z);
		this->m_world *= translation;
	}

	void DX11Renderer::scale(float factorX, float factorY, float factorZ)
	{
		Matrix scaling;
		D3DXMatrixScaling(&scaling, factorX, factorY, factorZ);
		this->m_world *= scaling;
	}

	void DX11Renderer::setWorld(Matrix world)
	{	
		this->m_world = world;
	}

	void DX11Renderer::multWorld(Matrix world)
	{
		this->m_world *= world;
	}

	Matrix DX11Renderer::getWorld()
	{
		return this->m_world;
	}

	void DX11Renderer::saveWorld()
	{
		this->m_worldSaved=this->m_world;
	}

	void DX11Renderer::restoreWorld()
	{
		//this->m_world = this->m_worldSaved;
		D3DXMatrixIdentity(&this->m_world);
	}

	LRESULT CALLBACK DX11Renderer::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{	
			case WM_SIZE:
			{
				//TODO: recreate the backbuffer properly

				/*int width  = (int)(short)LOWORD(lParam);
				int height = (int)(short)HIWORD(lParam);
				//this->setProjection(width, height, this->m_states.nearPlane, this->m_states.farPlane, this->m_states.fov);
				//this->setViewport(0,0,width,height);
				this->m_windowDesc.width	= width;
				this->m_windowDesc.height	= height;*/

				//TODO: center properly-> set mouse coords unprojected
				//SetCursorPos(width>>1, height>>1);

				int width  = (int)(short)LOWORD(lParam);
				int height = (int)(short)HIWORD(lParam);

				if(!this->m_states.fullscreen)
					this->resize(width,height);
			}break;
			case WM_KEYDOWN:
			{
				this->m_keyboardHandler->setKey(wParam);
	
				return 0;
			}break;
			case WM_KEYUP:
			{
				this->m_keyboardHandler->releaseKey(wParam);

				return 0;
			}break;
			case WM_LBUTTONDOWN:
			{
				//this->m_mouseHandler->setButton(MOUSE_LBUTTON);

				return 0;
			}break;
			case WM_RBUTTONDOWN:
			{
				//this->m_mouseHandler->setButton(MOUSE_RBUTTON);

				return 0;
			}break;
			case WM_MOUSEMOVE:
			{
				/*TRACKMOUSEEVENT track;
				track.cbSize = sizeof(TRACKMOUSEEVENT);
				track.dwFlags = TME_LEAVE;
				track.hwndTrack = this->m_windowDesc.hwnd;
				TrackMouseEvent(&track);*/
				//this->m_mouseHandler->setCurClientPos((int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam)));
				return 0;
			}break;
			case WM_LBUTTONUP:
			{
				//this->m_mouseHandler->releaseButton(MOUSE_LBUTTON);
				return 0;
			}break;
			case WM_RBUTTONUP:
			{
				//this->m_mouseHandler->releaseButton(MOUSE_RBUTTON);
				return 0;
			}break;
			case WM_MOUSELEAVE:
			{
				//this->m_mouseHandler->releaseButton(MOUSE_LBUTTON);
				//this->m_mouseHandler->releaseButton(MOUSE_RBUTTON);
			}break;
			case WM_CLOSE:
			{
				GLOBALS->getEngine()->exit();
				return 0;
			}break;
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}break;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	D3D11_PRIMITIVE_TOPOLOGY DX11Renderer::convertTopology(PRIMITIVE_TOPOLOGY topology)
	{
		D3D11_PRIMITIVE_TOPOLOGY newTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		switch(topology)
		{
			case PT_TRIANGLELIST:
			{
				newTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; 
			}break;
			case PT_TRIANGLESTRIP:
			{
				newTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			}break;
			case PT_POINTLIST:
			{
				newTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
			}break;
			case PT_LINELIST:
			{
				newTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			}break;
			case PT_LINESTRIP:
			{
				newTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
			}break;
			case PT_LINELIST_ADJ:
			{
				newTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
			}break;
			case PT_LINESTRIP_ADJ:
			{
				newTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
			}break;
			case PT_TRIANGLELIST_ADJ:
			{
				newTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
			}break;
			case PT_TRIANGLESTRIP_ADJ:
			{
				newTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
			}break;
		}

		return newTopology;
	}

	DX11Texture2D* DX11Renderer::createTexture2D()
	{
		if(!this->m_device || !this->m_currentChain->dxSwapChain)
			return NULL;

		return new DX11Texture2D(this->m_immediateContext, this->m_currentChain->dxSwapChain);
	}

	DX11Buffer* DX11Renderer::createBuffer()
	{
		if(!this->m_immediateContext)
			return NULL;

		return new DX11Buffer(this->m_immediateContext);
	}


	bool DX11Renderer::isRunning()
	{
		return this->m_run;
	}

	void DX11Renderer::setDefaultRenderTarget()
	{
		this->m_immediateContext->OMSetRenderTargets(1, &this->m_currentChain->renderTarget, this->m_currentChain->depthStencilView);
		this->setViewport(0,0,this->m_currentChain->windowDesc->width, this->m_currentChain->windowDesc->height);
	}

	void DX11Renderer::resize(int width, int height)
	{
		//this->setProjection(width, height, this->m_states.nearPlane, this->m_states.farPlane, this->m_states.fov);
		//this->setViewport(0,0,width,height);
		
		//SAFE_RELEASE(this->m_renderTarget)
		//SAFE_RELEASE(this->m_depthStencilView)
		//SAFE_RELEASE(this->m_depthStencilState)
		if(!this->m_currentChain)
			return;

		if(this->m_currentChain->renderTarget)
			this->m_currentChain->renderTarget->Release();
		if(this->m_currentChain->depthStencilView)
			this->m_currentChain->depthStencilView->Release();

		if(!this->m_device)
			return;

		this->m_currentChain->dxSwapChain->ResizeBuffers(1, width, height, (DXGI_FORMAT)this->m_states.backbufferFormat, 0);
		ID3D11Texture2D* backBuffer;
		this->m_currentChain->dxSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
		if(!backBuffer)
			return;

		this->m_device->CreateRenderTargetView(backBuffer, NULL, &this->m_currentChain->renderTarget);
		backBuffer->Release();


		D3D11_TEXTURE2D_DESC depthStencilDesc;
	
		depthStencilDesc.Width     = width;
		depthStencilDesc.Height    = height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format    = (DXGI_FORMAT)this->m_states.depthStencilFormat;
		depthStencilDesc.SampleDesc.Count   = this->m_states.sampleCount; 
		depthStencilDesc.SampleDesc.Quality = this->m_states.sampleLevel;
		depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0; 
		depthStencilDesc.MiscFlags      = 0;

		ID3D11Texture2D *depthStencilTex;
		this->m_device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilTex);

		D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsDesc.Format = depthStencilDesc.Format;
		if(depthStencilDesc.SampleDesc.Count > 1 )
            dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        else
            dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsDesc.Flags = 0;
		dsDesc.Texture2D.MipSlice = 0;

		this->m_device->CreateDepthStencilView(depthStencilTex, &dsDesc, &this->m_currentChain->depthStencilView);

		this->m_immediateContext->OMSetRenderTargets(1, &this->m_currentChain->renderTarget, this->m_currentChain->depthStencilView);

		this->setViewport(0,0,width,height);

		this->setProjection(width, height, this->m_states.nearPlane, this->m_states.farPlane, this->m_states.fov);
	}

	void DX11Renderer::exit(bool exit)
	{
		this->m_run = !exit;
	}
}
