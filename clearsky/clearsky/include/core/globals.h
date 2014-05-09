/*
*
*/
#pragma once
#ifndef _CLEARSKY_GLOBALS_H_
#define _CLEARSKY_GLOBALS_H_

//#include <windows.h>
//#include "core/precompiled.h"

#include "core/utils.h"
#include "core/logger.h"

#include "rendersystem/irenderer.h"

namespace clearsky
{
	class IRenderer;
	class Engine;
	class MouseHandler;

	//holds all global data
	class CLEARSKY_API Globals
	{
		public:
			static Globals* getInstance();

			//main window width
			void setWidth(int width);
			int getWidth();

			//main window height
			void setHeight(int height);
			int getHeight();

			//handle to the main window
			void setWindowHandle(HWND hwnd);
			HWND getWindowHandle();

			//time difference between two frames in seconds
			void setDeltaTime(double delta);
			double getDeltaTime();

			//primary logger
			Logger* getLogger();
			void setLogger(Logger* newLogger);

			//current graphics renderer
			void setRenderer(IRenderer *renderer);
			IRenderer* getRenderer();

			//engine interface
			void setEngine(Engine *engine);
			Engine* getEngine();

			MouseHandler* getMouseHandler();
			void setMouseHandler(MouseHandler *newMouseHandler);

			void setFreetypeLib(FT_Library *ftLib);
			FT_Library* getFreetypeLib();

		private:
			static Globals *m_instance;

			Globals();
			~Globals();

			IRenderer *m_renderer; //current renderer
			Logger *m_logger; //main logger
			Engine *m_engine; //engine interface
			MouseHandler *m_mouseHandler;
			HWND m_hwnd; //handle to the main window
			int m_width; //main window width
			int m_height; //main window height

			double m_delta; //time between two frames

			FT_Library *m_freetypeLib;
	};
}

#define GLOBALS Globals::getInstance()

#endif