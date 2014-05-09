/*
*
*/
#pragma once
#ifndef _CLEARSKY_MOUSEHANDLER_H_
#define _CLEARSKY_MOUSEHANDLER_H_

//#include "core/precompiled.h"
//#include <windows.h>
#include "core/utils.h"
#include "core/globals.h"
#include "math/mathutils.h"

namespace clearsky
{
	#define NUM_MOUSE_BUTTONS 3

	#define MOUSE_LBUTTON 0x0
	#define MOUSE_MBUTTON 0x1
	#define MOUSE_RBUTTON 0x2

	class CLEARSKY_API MouseHandler
	{
		public:
			MouseHandler();
			~MouseHandler();

			double getDeltaX();
			double getDeltaY();

			void getCurScreenPos(int &x, int &y);
			void setCurScreenPos(int x, int y);

			void getCurClientPos(int &x, int &y);
			void setCurClientPos(int x, int y);

			void getCurUnprojPos(int &x, int &y);
			void setCurUnprojPos(int x, int y);

			void showCursor(bool show);
			bool isCursorVisible();

			void centerCursor();

			bool isButtonDown(int keyCode);
			RETURN_VALUE setButton(int keyCode);
			RETURN_VALUE releaseButton(int keyCode);

		private:
			bool m_showWinCursor;
			bool m_keyClicked[3];

			int m_screenPosX;
			int m_screenPosY;

			int m_clientPosX;
			int m_clientPosY;
			int m_lastClientX;
			int m_lastClientY;

			int m_unprojPosX;
			int m_unprojPosY;
			int m_lastUnprojX;
			int m_lastUnprojY;
	};
}
#endif