/*
*
*/

#include "core/precompiled.h"
#include "inputsystem/mousehandler.h"

namespace clearsky
{
	MouseHandler::MouseHandler():m_showWinCursor(false)
	{
		ShowCursor(false);

		for(int i=0; i<NUM_MOUSE_BUTTONS; ++i)
		{
			this->m_keyClicked[i] = false;
		}
	}

	MouseHandler::~MouseHandler()
	{

	}

	double MouseHandler::getDeltaX()
	{
		//static int lastX = this->m_unprojPosX;//GLOBALS->getWidth()>>1;

		//POINT mousePos;
		//GetCursorPos(&mousePos);
		//int newX, newY;
		//this->getCurUnprojPos(newX, newY);
		//SetCursorPos(lastX, mousePos.y);

		return this->m_unprojPosX - this->m_lastUnprojX;//mousePos.x-this->m_lastClientX;
	}

	double MouseHandler::getDeltaY()
	{
		//static int lastY = this->m_unprojPosY;//GLOBALS->getHeight()>>1;

		//POINT mousePos;
		//GetCursorPos(&mousePos);
		//int newX, newY;
		//this->getCurUnprojPos(newX, newY);
		//SetCursorPos(mousePos.x, lastY);
		
		return this->m_unprojPosY-this->m_lastUnprojY;//mousePos.y-this->m_lastClientY;
	}


	void MouseHandler::getCurScreenPos(int &x, int &y)
	{
		POINT point;
		GetCursorPos(&point);

		x=this->m_screenPosX=point.x;
		y=this->m_screenPosY=point.y;
	}


	void MouseHandler::setCurScreenPos(int x, int y)
	{
		this->m_screenPosX=x;
		this->m_screenPosY=y;

		SetCursorPos(x, y);
	}

	void MouseHandler::getCurClientPos(int &x, int &y)
	{
		x=this->m_clientPosX;
		y=this->m_clientPosY;

		POINT point;
		point.x = x;
		point.y = y;

		ClientToScreen(GLOBALS->getWindowHandle(),&point);
	}

	void MouseHandler::setCurClientPos(int x, int y)
	{
		//if(this->m_clientPosX!=x)
		//{
			this->m_lastClientX = this->m_clientPosX;
			this->m_clientPosX=x;
		//}
		
		//if(this->m_clientPosY!=y)
		//{
			this->m_lastClientY = this->m_clientPosY;
			this->m_clientPosY=y;
		//}
	}

	void MouseHandler::getCurUnprojPos(int &x, int &y)
	{
		x=this->m_unprojPosX;
		y=this->m_unprojPosY;
	}

	void MouseHandler::setCurUnprojPos(int x, int y)
	{
		//if(this->m_unprojPosX!=x)
		//{
			this->m_lastUnprojX = this->m_unprojPosX;
			this->m_unprojPosX=x;
		//}
		//if(this->m_unprojPosY!=y)
		//{
			this->m_lastUnprojY = this->m_unprojPosY;
			this->m_unprojPosY=y;
		//}

	}

	void MouseHandler::showCursor(bool show)
	{
		if(this->m_showWinCursor!=show)
		{
			ShowCursor(show);
			this->m_showWinCursor = show;
		}
	}

	bool MouseHandler::isCursorVisible()
	{
		return this->m_showWinCursor;
	}

	void MouseHandler::centerCursor()
	{
		SetCursorPos(GLOBALS->getWidth()>>1, GLOBALS->getHeight()>>1);
	}

	bool MouseHandler::isButtonDown(int keyCode)
	{
		//if(keyCode>=0 && keyCode<NUM_MOUSE_BUTTONS)
		//	return this->m_keyClicked[keyCode];

		 int key=0;
		 if(keyCode == 0x0) //LBUTTON
			 key = VK_LBUTTON;
		 else if(keyCode == 0x02) //RBUTTON
			 key = VK_RBUTTON;

		 bool down = (GetAsyncKeyState(key) & 0x8000) ? true : false;
		 if(down)
			 return true;

		return false;
	}

	RETURN_VALUE MouseHandler::setButton(int keyCode)
	{
		if(keyCode>=0 && keyCode<NUM_MOUSE_BUTTONS)
		{
			this->m_keyClicked[keyCode] = true;
			return RETURN_OK;
		}

		return RETURN_ERROR;
	}

	RETURN_VALUE MouseHandler::releaseButton(int keyCode)
	{
		if(keyCode>=0 && keyCode<NUM_MOUSE_BUTTONS)
		{
			this->m_keyClicked[keyCode] = false;
			return RETURN_OK;
		}

		return RETURN_ERROR;
	}
}