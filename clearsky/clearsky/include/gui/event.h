/*
*
*/

#pragma once
#ifndef _CLEARSKY_EVENT_H_
#define _CLEARSKY_EVENT_H_

#include "core/utils.h"

namespace clearsky
{
	class Window;

	enum EVENT_TYPE
	{
		EVENT_MOUSE_MOVE,
		EVENT_MOUSE_DOWN,
		EVENT_MOUSE_UP,
		EVENT_WINDOW_MOVE,
		EVENT_WINDOW_RESIZE,
		EVENT_WINDOW_CLICKED
	};

	class Event
	{
		public:
			EVENT_TYPE type;
			Window *sender;
			float fData[4];
			float iData[4];
	};

	class EventListener
	{
		public:
			virtual void process(Event event){};
	};
}

#endif