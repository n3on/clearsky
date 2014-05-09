/*
*
*/
#pragma once
#ifndef _CLEARSKY_GUI_H_
#define _CLEARSKY_GUI_H_

//#include "core/precompiled.h"

#include "math/mathutils.h"
#include "gui/guiskin.h"
#include "gui/mouse.h"
#include "gui/window.h"
#include "gui/button.h"
#include "gui/staticText.h"

//#include <boost/thread.hpp>

namespace clearsky
{
	class CLEARSKY_API GUI
	{
		public:
			GUI();
			~GUI();

			void init();
			void release();

			void reloadSkin();
			RETURN_VALUE deleteSkin(const char *skin);

			void detach(); //execute update and show in a new thread
			void join(); //thread joining

			RETURN_VALUE loadSkin(const char *filename);

			void update();
			void show();

			void run(); //invoke update+show in a single function

			Mouse* getMouse();
			
			GUISkin* getGUISkin();
			void setGUISkin(GUISkin *newGUISkin);
			RETURN_VALUE setCurrentSkin(const char* skinname);

			void setTopWindow(Window *window);
			Window* getTopWindow();

			Window* createWindow();
			Button* createButton();
			StaticText* createStaticText();

			

		private:
			GUISkin *m_guiSkin;
			const char *m_skinFileName;
			Mouse *m_mouse;
			IRenderer *m_renderer;
			DepthStencilState *m_depthState;

			Window *m_topWindow;
			//boost::thread m_guiThread;
	};

}

#endif