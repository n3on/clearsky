/*
*
*/

#include "core/precompiled.h"
#include "gui/button.h"

namespace clearsky
{
	Button::Button()
	{
		this->init();
	}

	Button::Button(GUISkin *guiSkin):Window(guiSkin)
	{
		this->init();

	}

	Button::~Button()
	{
	}

	void Button::init()
	{
		this->m_isMovable = false;
		strncpy_s(this->m_caption,"Button",50);
		this->m_width     = 120;
		this->m_height    = 50; 
		this->m_type	  = GUI_TYPE_BUTTON;
		this->m_pushed	  = false;
		this->m_showTitle = false;

		this->reloadSkin();
	}

	void Button::reloadSkin()
	{
		if(!this->m_skin)
			return;

		SkinData *skin = this->m_skin->getCurrentSkin();
		this->m_buttonColor = skin->buttonColor;
		this->m_buttonFont  = skin->buttonFont;
		this->m_mouseOverColor = skin->buttonMouseOverColor;
		this->m_buttonFontColor = skin->buttonFontColor;
		this->m_buttonTexture = skin->buttonTexture;
		this->m_buttonTexturing = skin->buttonTexturing;
	}

	int Button::paint()
	{
		int titleHeight = 0;
		if(this->m_showTitle)
			titleHeight = this->m_titleSize;

		Gradient color = this->m_buttonColor;
		if(this->m_mouseOver)
			color = this->m_mouseOverColor;

		bool lastTexturing = this->m_renderer->isTexturing();
		if(this->m_buttonTexturing)
		{
			this->m_renderer->setTexturing(true);
			this->m_renderer->setTexture(this->m_buttonTexture);
			this->m_renderer->apply(1,0);
		}
		else
			this->m_renderer->setTexturing(false);

		//draw button body
		this->m_renderer->drawFilledRect(this->m_positionAbs.x,
											this->m_positionAbs.y+titleHeight,
											this->m_width,
											this->m_height-titleHeight,
											this->m_position.z,
											color);

		//if(this->m_buttonTexturing)
		//{
			this->m_renderer->setTexturing(lastTexturing);
			//this->m_renderer->apply(1,0);
		//}

		//draw border rect
		this->m_renderer->drawRect(this->m_positionAbs.x,
											this->m_positionAbs.y,
											this->m_width,
											this->m_height,
											this->m_position.z,
											Color(0,0,0,1));

		if(strlen(this->m_caption)>0)
		{
			
			if(this->m_buttonFont)
			{
				Gradient lastColor = this->m_buttonFont->getGradient();
				this->m_buttonFont->setGradient(this->m_buttonFontColor);

				this->m_buttonFont->setString(255, this->m_caption);
				int deltaY = (int)(this->m_height-this->m_buttonFont->getTextHeight())>>1;
				int deltaX = (int)(this->m_width-this->m_buttonFont->getTextWidth())>>1;
				if(deltaX>0 && deltaY>0)
					this->m_buttonFont->show((int)this->m_positionAbs.x+deltaX, (int)this->m_positionAbs.y+deltaY);
				else
					this->m_buttonFont->show((int)this->m_positionAbs.x, (int)this->m_positionAbs.y+deltaY);

				this->m_buttonFont->setGradient(lastColor);

				//this->m_renderer->begin2D();
			}
		}

		WindowEvent windowEvent;
		windowEvent.window = this;

		for(unsigned int i=0; i<this->m_windowListener.size(); ++i)
			this->m_windowListener[i]->windowPaint(windowEvent);

		return SKIP_MSG;
	}


	int Button::mouseEnter()
	{
		this->m_mouseOver = true;

		/*Gradient temp = this->m_buttonColor;
		this->m_buttonColor = this->m_mouseOverColor;
		this->m_mouseOverColor = temp;*/

		MouseEvent mouseEvent;
		mouseEvent.window = this;
		mouseEvent.x = 0;
		mouseEvent.y = 0;
		mouseEvent.xAbs = 0;
		mouseEvent.yAbs = 0;

		for(unsigned int i=0; i<this->m_mouseListener.size(); ++i)
			this->m_mouseListener[i]->mouseEnter(mouseEvent);

		return PASS_MSG;
	}

	int Button::mouseLeave()
	{
		this->m_mouseOver = false;

		/*Gradient temp = this->m_buttonColor;
		this->m_buttonColor = this->m_mouseOverColor;
		this->m_mouseOverColor = temp;*/

		MouseEvent mouseEvent;
		mouseEvent.window = this;
		mouseEvent.x = 0;
		mouseEvent.y = 0;
		mouseEvent.xAbs = 0;
		mouseEvent.yAbs = 0;

		for(unsigned int i=0; i<this->m_mouseListener.size(); ++i)
			this->m_mouseListener[i]->mouseLeave(mouseEvent);

		return PASS_MSG;
	}

	int Button::mouseClicked(int x, int y, int keyCode)
	{

		if(this->m_type==GUI_TYPE_PUSHBUTTON)
		{
			Gradient temp = this->m_buttonColor;
			this->m_buttonColor = this->m_mouseOverColor;
			this->m_mouseOverColor = temp;

			this->m_pushed = !this->m_pushed;
		}

		MouseKeyEvent event;
		event.window = this;
		event.x = (int)(x - this->m_positionAbs.x);
		event.y = (int)(y - this->m_positionAbs.y);
		event.xAbs = x;
		event.yAbs = y;
		event.keyCode = keyCode;
		event.down = false;

		for(unsigned int i=0; i<this->m_mouseListener.size(); ++i)
			this->m_mouseListener[i]->mouseClicked(event);

		return PASS_MSG;
	}


};
