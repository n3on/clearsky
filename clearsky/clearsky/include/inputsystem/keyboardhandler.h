/*
*
*/
#pragma once
#ifndef _CLEARSKY_KEYBOARD_HANDLER_H_
#define _CLEARSKY_KEYBOARD_HANDLER_H_

#include "core/utils.h"

namespace clearsky
{
	#define NUM_KEYS 256

	#define VK_A 0x41
	#define VK_B 0x42
	#define VK_C 0x43
	#define VK_D 0x44
	#define VK_E 0x45
	#define VK_F 0x46
	#define VK_G 0x47
	#define VK_H 0x48
	#define VK_I 0x49
	#define VK_J 0x4A
	#define VK_K 0x4B
	#define VK_L 0x4C
	#define VK_M 0x4D
	#define VK_N 0x4E
	#define VK_O 0x4F
	#define VK_P 0x50
	#define VK_Q 0x51
	#define VK_R 0x52
	#define VK_S 0x53
	#define VK_T 0x54
	#define VK_U 0x55
	#define VK_V 0x56
	#define VK_W 0x57
	#define VK_X 0x58
	#define VK_Y 0x59
	#define VK_Z 0x5A
	#define VK_PLUS 0x6b
	#define VK_MINUS 0x6d

	class CLEARSKY_API KeyboardHandler
	{
		public:
			KeyboardHandler();
			~KeyboardHandler();

			bool isKeyDown(int keyCode);
			bool keyClicked(int keyCode);

			int setKey(int keyCode);
			int releaseKey(int keyCode);

		private:
			bool m_keys[NUM_KEYS];
			int m_lastKey; //lastkey that was down
			bool m_clicked; //do we have a keyclick?
	};
}

#endif