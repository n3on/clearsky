/*
*
*/
#pragma once
#ifndef _CLEARSKY_SKYBOX_H_
#define _CLEARSKY_SKYBOX_H_

#include "rendersystem/resourcemanager.h"

#include "math/mathutils.h"

#include "rendersystem/camera.h"

#include "rendersystem/irenderer.h"
#include "rendersystem/ibuffer.h"
#include "rendersystem/itexture2D.h"


namespace clearsky
{
	class CLEARSKY_API Skybox
	{
		public:
			Skybox();
			~Skybox();


			RETURN_VALUE create(Camera *camera);
			void draw();

			RETURN_VALUE setTextures(ITexture2D *north, ITexture2D *south,
									 ITexture2D *west, ITexture2D *east,
									 ITexture2D *up, ITexture2D *down);
			RETURN_VALUE setTextures(const char *north, const char *south,
									 const char *west, const char *east,
									 const char *up, const char *down);

			void setScaleFactor(float scale);

		private:
			IBuffer *m_vertexBuffer;
			IBuffer *m_indexBuffer;
			Camera *m_camera;
			ITexture2D *m_texture;
			float m_scaleFactor;

			ITexture2D *m_textureNorth;
			ITexture2D *m_textureSouth;
			ITexture2D *m_textureEast;
			ITexture2D *m_textureWest;
			ITexture2D *m_textureUp;
			ITexture2D *m_textureDown;

			SamplerState m_samplerState;
			IRenderer *m_renderer;
	};
}

#endif