/*
*
*/

#ifndef _CLEARSKY_STATIC_TEXT_H_
#define _CLEARSKY_STATIC_TEXT_H_

#include "core/utils.h"
#include "gui/window.h"

namespace clearsky
{
	class CLEARSKY_API StaticText : public Window
	{
		public:
			StaticText();
			StaticText(GUISkin *guiSkin);
			~StaticText();

			void init();
			virtual void reloadSkin();

			int paint();

		private:
			Font *m_staticTextFont;
			Gradient m_staticTextFontColor;
			
	};
}

#endif