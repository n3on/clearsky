/*
*
*/
#pragma once
#ifndef _CLEARSKY_BOX_H_
#define _CLEARSKY_BOX_H_

#include "core/utils.h"
#include "core/globals.h"

#include "rendersystem/ibuffer.h"
#include "rendersystem/itexture2D.h"
#include "rendersystem/iinputlayout.h"

#include "math/mathutils.h"

namespace clearsky
{
	class CLEARSKY_API Box
	{
		public:
			Box();
			~Box();

			RETURN_VALUE create();

			void draw();
			void release();

			void setTexture(ITexture2D *texture);
			ITexture2D* getTexture();

			IBuffer* getVertexBuffer();
			IBuffer* getIndexBuffer();

		private:
			IBuffer *m_vertexBuffer;
			IBuffer *m_indexBuffer;
			ITexture2D *m_texture;

			IRenderer *m_renderer;
	};
}

#endif