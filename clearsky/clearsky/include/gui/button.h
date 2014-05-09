/*
*
*/
#pragma once
#ifndef _CLEARSKY_CLEARSKY_GUI_BUTTON_H_
#define _CLEARSKY_CLEARSKY_GUI_BUTTON_H_

#include "core/utils.h"
#include "gui/window.h"

namespace clearsky
{

	class CLEARSKY_API Button : public Window
	{
		public:
			Button();
			Button(GUISkin *guiSkin);
			virtual ~Button();

			void init();
			virtual void reloadSkin();

			int paint();
			int mouseEnter();
			int mouseLeave();
			int mouseClicked(int x, int y, int keyCode);

	protected:
			GUI_ELEMENT_TYPE m_type;
			bool m_pushed;

			Gradient m_buttonColor;
			Gradient m_mouseOverColor;

			Font	*m_buttonFont;
			Gradient m_buttonFontColor;
			ITexture2D *m_buttonTexture;
			bool	 m_buttonTexturing;
	};
}

#endif