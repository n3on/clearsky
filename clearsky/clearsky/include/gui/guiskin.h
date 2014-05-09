/*
*
*/
#pragma once
#ifndef _CLEARSKY_GUI_SKIN_H_
#define _CLEARSKY_GUI_SKIN_H_

#include <tinyxml.h>

#include "core/globals.h"
#include "core/utils.h"
#include "rendersystem/irenderer.h"
#include "rendersystem/itexture2D.h"
#include "rendersystem/font.h"
#include "rendersystem/resourcemanager.h"

namespace clearsky
{
	const float DEPTH_OFFSET = 0.0f; //depth value for each new child in hirarchie
	const float START_DEPTH  = 0.0f; //stating depth value of controls

	struct CLEARSKY_API SkinData
	{
			SkinData()
			{
				name = NULL;
				mouseTexture = NULL;
				topwindowTexture = NULL;
				topwindowTexturing = false;

				windowTexture = NULL;
				windowTexturing = false;
				windowTitleTexturing = false;
				windowTitleHeight = 20;
				windowFont = NULL;
				guiFont = NULL;

				buttonFont = NULL;
				buttonTexture = NULL;
				buttonTexturing = false;

				windowFont = Resources->loadFont(16, "..\\data\\fonts\\arial.ttf");
				guiFont    = Resources->loadFont(16, "..\\data\\fonts\\arial.ttf");
				buttonFont = Resources->loadFont(16, "..\\data\\fonts\\arial.ttf");
				staticTextFont = Resources->loadFont(16, "..\\data\\fonts\\arial.ttf");
				windowTitleFont = Resources->loadFont(16, "..\\data\\fonts\\arial.ttf");
			}

			~SkinData()
			{
				//SAFE_ARRAY_DELETE(name);
			}

			char *name;

			ITexture2D	*mouseTexture;

			ITexture2D   *topwindowTexture;
			bool		 topwindowTexturing;
			Gradient	 topwindowColor;

			ITexture2D  *windowTexture;
			bool		 windowTexturing;
			Gradient	 windowColor;

			int			 windowTitleHeight;
			Gradient	 windowTitleColor;
			Font		 *windowTitleFont;
			Gradient	 windowTitleFontColor;
			ITexture2D   *windowTitleTexture;
			bool		 windowTitleTexturing;
			Font		 *windowFont;
			Gradient	 windowFontColor;

			Font		 *guiFont;
			Font		 *buttonFont; 
			ITexture2D   *buttonTexture;
			bool	     buttonTexturing;
			Gradient	 buttonFontColor;
			Gradient	 buttonColor;
			Gradient	 buttonMouseOverColor;

			Font		*staticTextFont;
			Gradient	staticTextFontColor;
	};

	class CLEARSKY_API GUISkin
	{
		public:
			GUISkin();
			~GUISkin();

			void init();

			//load from xml file
			RETURN_VALUE load(const char* filename);
			RETURN_VALUE remove(const char *name);

			SkinData* getSkin(const char *name);

			RETURN_VALUE setCurrentSkin(const char* skinname);
			SkinData* getCurrentSkin();
			const char* getCurrentSkinName();

		private:
			std::vector<SkinData*> m_skinData;
			SkinData *m_currentSkin;

			//for xml stylesheet parsing
			void parseTopWindow(TiXmlElement *topWindowElement, SkinData *skin);
			void parseWindow(TiXmlElement *windowElement, SkinData *skin);
			void parseButton(TiXmlElement *buttonElement, SkinData *skin);
			void parseCursor(TiXmlElement *cursorElement, SkinData *skin);
			void parseStaticText(TiXmlElement *staticElement, SkinData *skin);

			void parseGradient(TiXmlElement *element, Gradient &outGradient);
			void parseColor(TiXmlElement *colorElement, Color &outColor);
			void parseColorAndGradient(TiXmlElement *element, Gradient &outGradient);
			ITexture2D* parseTexture(TiXmlElement *element);
			Font* parseFont(TiXmlElement *element, Gradient &fontColor);

			void insertDefaultSkin();
	};

}

#endif