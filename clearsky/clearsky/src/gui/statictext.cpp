/*
*
*/

#include "core/precompiled.h"
#include "gui/statictext.h"

namespace clearsky
{
	StaticText::StaticText()
	{
		this->init();
	}

	StaticText::StaticText(GUISkin *guiSkin):Window(guiSkin)
	{
		this->init();
	}

	StaticText::~StaticText()
	{
	}

	void StaticText::init()
	{
		this->m_staticTextFont = NULL;
		this->m_isMovable = false;

		this->reloadSkin();
	}

	void StaticText::reloadSkin()
	{
		if(!this->m_skin)
			return;

		SkinData *skin		= this->m_skin->getCurrentSkin();
		this->m_staticTextFont		= skin->staticTextFont;
		this->m_staticTextFontColor	= skin->staticTextFontColor;
	
	}

	int StaticText::paint()
	{
		if(this->m_staticTextFont)
		{
			Gradient lastColor = this->m_staticTextFont->getGradient();
			this->m_staticTextFont->setGradient(this->m_staticTextFontColor);

			this->m_staticTextFont->setString(255, this->m_caption);
			this->m_staticTextFont->show((int)this->m_positionAbs.x, (int)this->m_positionAbs.y);
		
			this->setSize((float)this->m_staticTextFont->getTextWidth(), (float)this->m_staticTextFont->getTextHeight());
		
			this->m_staticTextFont->setGradient(lastColor);
		}

		return SKIP_MSG;
	}
}