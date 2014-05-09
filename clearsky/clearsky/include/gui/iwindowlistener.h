/*
*
*/
#pragma once
#ifndef _CLEARSKY_I_WINDOW_LISTENER_H_
#define _CLEARSKY_I_WINDOW_LISTENER_H_

#include "core/utils.h"

namespace clearsky
{
	class Window;

	class CLEARSKY_API WindowEvent
	{
		public:
			WindowEvent()
			{
				window = NULL;
				x=y=0;
				xAbs=yAbs=0;
				deltaX=deltaY=0;
			}

			Window *window;
			int x;
			int y;
			int xAbs;
			int yAbs;
			double deltaX;
			double deltaY;
	};

	class CLEARSKY_API IWindowListener
	{
		public:
			virtual void windowMove(WindowEvent windowEvent){};
			virtual void windowPaint(WindowEvent windowPaint){};

	};
}

#endif