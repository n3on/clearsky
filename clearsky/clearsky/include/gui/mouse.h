/*
*
*/
#pragma once
#ifndef _CLEARSKY_GUI_MOUSE_H_
#define _CLEARSKY_GUI_MOUSE_H_

#include "core/globals.h"
#include "rendersystem/irenderer.h"
#include "inputsystem/mousehandler.h"
#include "gui/guiskin.h"
 
namespace clearsky
{
	class CLEARSKY_API Mouse
	{
		public:
			Mouse(GUISkin *guiskin);
			~Mouse();

			void update();
			void show();

			void reloadSkin();

			void getCurPos(int &x, int &y);
			double getDeltaX();
			double getDeltaY();

			bool isVisible();
			void setVisible(bool visible);

			bool isButtonDown(int keyCode);
		private:
			bool m_visible;

			MouseHandler *m_mouseHandler;
			ITexture2D *m_mouseTexture;
			IRenderer *m_renderer;
			GUISkin *m_guiskin;

			int m_posX;
			int m_posY;
			int m_lastPosX;
			int m_lastPosY;
			int m_width;
			int m_height;
	};
}

#endif