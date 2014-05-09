/*
*
*/
#pragma once
#ifndef _CLEARSKY_CLEARSKY_IMOUSE_LISTENER_H_
#define _CLEARSKY_CLEARSKY_IMOUSE_LISTENER_H_

#include "core/utils.h"

namespace clearsky
{
	class Window;

	class CLEARSKY_API MouseEvent
	{
		public:
			Window *window;
			int x;
			int y;
			int xAbs;
			int yAbs;
	};

	class CLEARSKY_API MouseMoveEvent : public MouseEvent
	{
		public:
			double deltaX;
			double deltaY;
	};

	class CLEARSKY_API MouseKeyEvent : public MouseEvent
	{
		public:
			int	keyCode;
			bool down;
	};

	class CLEARSKY_API IMouseListener
	{
		public:
			virtual void mouseMove(MouseMoveEvent mouseEvent){};
			virtual void mouseKey(MouseKeyEvent mouseEvent){};
			virtual void mouseOver(MouseEvent mouseEvent){};
			virtual void mouseClicked(MouseKeyEvent mouseEvent){};
			virtual void mouseEnter(MouseEvent mouseEvent){};
			virtual void mouseLeave(MouseEvent mouseEvent){};

		private:
	};
}

#endif
