/*
*
*/
#pragma once
#ifndef _CLEARSKY_DX11_TEXTURE_2D_H_
#define _CLEARSKY_DX11_TEXTURE_2D_H_

//#include <d3d11.h>
//#include <d3dx11.h>
#include <string>
#include "core/globals.h"
#include "core/PathExtractor.h"
#include "core/tgaloader.h"
#include "rendersystem/itexture2d.h"

namespace clearsky
{
	class CLEARSKY_API DX11Texture2D : public ITexture2D
	{
		public:
			DX11Texture2D(ID3D11DeviceContext *immediateContext, IDXGISwapChain *swapChain);
			~DX11Texture2D();

			RETURN_VALUE create(void *data, int width, int height, DATA_FORMAT format=FORMAT_R8G8B8A8_UNORM);
			//depthstencil parameter to generate the depth texture with the colormap
			RETURN_VALUE createRenderTarget(int width, int height, bool colorMap=true, DATA_FORMAT format=FORMAT_R8G8B8A8_UNORM);

			RETURN_VALUE loadFile(const char *filename);
			void show(int x, int y);

			int getWidth();
			int getHeight();

			void setUsage(BUFFER_USAGE usage);
			void setCPUUsage(BUFFER_CPU_USAGE usage);

			//use the texture as render target
			void startRenderTarget();
			void endRenderTarget();

			void setPitch(int pitch);

			ID3D11ShaderResourceView* getShaderView();

			void release();
		private:
			RETURN_VALUE createColorMap(int width, int height, DATA_FORMAT format);
			RETURN_VALUE createDepthStencil(int width, int height);

			ID3D11Resource *m_textureResource; 
			ID3D11Texture2D *m_texture;
			D3D11_TEXTURE2D_DESC m_desc;
			D3D11_SUBRESOURCE_DATA m_subresourceData;
			ID3D11Device *m_dxdevice;
			IDXGISwapChain *m_swapChain;
			ID3D11DeviceContext *m_immediateContext;

			ID3D11ShaderResourceView *m_shaderView;
			ID3D11RenderTargetView *m_renderTargetView; //need if the texture is render target
			ID3D11Texture2D *m_depthTexture;
			ID3D11DepthStencilView *m_delphStencilView;
			ID3D11ShaderResourceView *m_depthStencilShaderView;

			D3D11_VIEWPORT m_viewport; //used if texture is a render target

			BUFFER_USAGE m_usage;
			BUFFER_CPU_USAGE m_cpuUsage;
	};
}

#endif