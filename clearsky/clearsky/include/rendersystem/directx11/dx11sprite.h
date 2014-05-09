/*
*
*/
#pragma once
#ifndef _CLEARSKY_DX11_SPRITE_H_
#define _CLEARSKY_DX11_SPRITE_H_

//#include <d3dx11.h>
#include "core/globals.h"
#include "rendersystem/isprite.h"
#include "rendersystem/directx11/dx11texture2D.h"
#include "math/mathutils.h"

namespace clearsky
{

	class CLEARSKY_API DX11Sprite : public ISprite
	{
		public:
			DX11Sprite(ID3D11DeviceContext *immediateContext);
			DX11Sprite(const char *filename, int spriteWidth, int spriteHeight, int num,int maxFramesX, int maxFramesY);
			~DX11Sprite();

			RETURN_VALUE init(const char *filename, int spriteWidth, int spriteHeight, int num,int maxFramesX, int maxFramesY);

			void resetTransform();
			void translate(float x, float y, float z);
			void scale(float x, float y, float z);
			void transform(Matrix *mat);

			void show();
			void drawAll();

			void setVisible(bool visible);
			bool getVisible();

			void nextFrame();
			void prevFrame();
			void setCurrentFrame(int frame);
			int getCurrentFrame();
			int getNumFrames();

			void setNumFrames(int numFrames);
			void setFrameType(FRAME_TYPE frameType);

			void billboard(Matrix *camMatrix);
			void unbillboard();

			void release();

		private:
			//D3DX11_SPRITE m_spriteDesc;
			//ID3DX11Sprite *m_sprite;
			ITexture2D *m_texture;
			IBuffer *m_vertexBuffer;
			
			Vector2D m_deltaUV;//uv to reach next frame
			Vector2D m_startUV;
			Vector2D m_endUV;

			int m_width;
			int m_height;
			int m_maxFramesX;
			int m_maxFramesY;
			bool m_visible;

			Matrix m_world;
			Matrix *m_cameraMatrix;
			DepthStencilState *m_depthState;

			int m_curFrame;
			int m_numFrames;

			FRAME_TYPE m_frameType;

			ID3D11DeviceContext *m_immediateContext;
			ID3D11Device *m_dxdevice;
			IRenderer *m_renderer;
	};
}

#endif