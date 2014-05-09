/*
*
*/

#include "core/precompiled.h"
#include "gui/mouse.h"

namespace clearsky
{
	Mouse::Mouse(GUISkin *guiskin):m_visible(true),
									m_posX(0),
									m_posY(0),
									m_width(32),
									m_height(32),
									m_guiskin(NULL)
	{
		m_mouseHandler = GLOBALS->getMouseHandler();

		this->m_guiskin = guiskin;

		this->reloadSkin();
		this->m_renderer = GLOBALS->getRenderer();
	}

	Mouse::~Mouse()
	{

	}

	void Mouse::getCurPos(int &x, int &y)
	{
		x=this->m_posX;
		y=this->m_posY;\
	}

	void Mouse::reloadSkin()
	{
		if(this->m_guiskin)
			this->m_mouseTexture = this->m_guiskin->getCurrentSkin()->mouseTexture;
	}

	void Mouse::show()
	{
		if(this->m_visible)
		{
			
			this->m_renderer->begin2D();
			bool lastTexturing = this->m_renderer->isTexturing();
			this->m_renderer->setTexturing(true);

			//this->m_mouseTexture->show(this->m_posX-(this->m_width>>2), this->m_posY-(this->m_height>>2));
			this->m_renderer->setTexture(this->m_mouseTexture);
			this->m_renderer->apply(1,0);
			this->m_renderer->drawFilledRect((float)this->m_posX-(this->m_width>>2), (float)this->m_posY-(this->m_height>>2), (float)this->m_width, (float)this->m_height);

			//printf("PosX=%d PosY=%d\n", this->m_posX, this->m_posY);

			this->m_renderer->setTexturing(lastTexturing);
			this->m_renderer->end2D();
		}
	}

	bool Mouse::isVisible()
	{
		return this->m_visible;
	}

	void Mouse::setVisible(bool visible)
	{
		this->m_visible = visible;
	}

	void Mouse::update()
	{
		this->m_lastPosX = this->m_posX;
		this->m_lastPosY = this->m_posY;
		this->m_mouseHandler->getCurUnprojPos(this->m_posX,this->m_posY);
	}

	double Mouse::getDeltaX()
	{
		return this->m_lastPosX - this->m_posX;
	}

	double Mouse::getDeltaY()
	{
		return this->m_lastPosY - this->m_posY;
	}

	bool Mouse::isButtonDown(int keyCode)
	{
		return this->m_mouseHandler->isButtonDown(keyCode);
	}
}