/*
*
*/

#include "core/precompiled.h"
#include "inputsystem/keyboardhandler.h"

namespace clearsky
{
	KeyboardHandler::KeyboardHandler()
	{
		for(int i=0; i<NUM_KEYS; ++i)
			this->m_keys[i] = false;
	}

	KeyboardHandler::~KeyboardHandler()
	{
	}

	bool KeyboardHandler::isKeyDown(int keyCode)
	{
		this->m_keys[keyCode] = (GetAsyncKeyState(keyCode) & 0x8000) ? true : false;
		return this->m_keys[keyCode];
	}

	bool KeyboardHandler::keyClicked(int keyCode)
	{
		if((this->m_lastKey == keyCode) && this->m_clicked)
		{
			this->m_lastKey = -1;
			this->m_clicked = false;

			return true;
		}

		return false;
	}

	int KeyboardHandler::setKey(int keyCode)
	{
		if(keyCode>=0 && keyCode<NUM_KEYS)
		{
			this->m_keys[keyCode] = true;

			this->m_lastKey = -1; //no key was clicked
			this->m_clicked = false; //for click we need to release the key
			return RETURN_OK;
		}

		return RETURN_ERROR;
	}

	int KeyboardHandler::releaseKey(int keyCode)
	{
		if(keyCode>=0 && keyCode<NUM_KEYS)
		{
			this->m_keys[keyCode] = false;

			this->m_clicked = true;
			this->m_lastKey = keyCode;

			return RETURN_OK;
		}

		return RETURN_ERROR;
	}

}