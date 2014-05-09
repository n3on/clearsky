/*
*
*/

#ifndef _CLEARSKY_PRIMITIVE_DRAWER_
#define _CLEARSKY_PRIMITIVE_DRAWER_

#include <vector>

#include "core/utils.h"
#include "core/globals.h"
#include "rendersystem/irenderer.h"
#include "rendersystem/ibuffer.h"
#include "math/mathutils.h"

namespace clearsky
{
	class CLEARSKY_API PrimitiveDrawer
	{
		public:
			PrimitiveDrawer();
			~PrimitiveDrawer();

			RETURN_VALUE init(const unsigned int initSize);

			void addLine(const Vector3D start, const Vector3D end, const Color color);
			void commitLineBuffer();

			void addRect(const Vector3D position, const int width, const int height, const Color color);
			
			void addFilledRect(const Vector3D position, const int width, const int height, const Color color);
			void commitFilledRectBuffer();

			void addGrid();
			void addCircle();
			void addSphere();
			void addCross();
			void addTriangle();
			void addBox();

			void clear();
			void show();

		private:
			IRenderer *m_renderer;
			
			IBuffer   *m_lineVertexBuffer;
			std::vector<VertexPC> m_lineVerts;
			unsigned int m_maxLineVerts; //current maximum buffer size

			IBuffer   *m_filledRVertexBuffer;
			std::vector<VertexPC> m_filledRVerts;
			unsigned int m_maxFilledRVerts;
	};
}

#endif