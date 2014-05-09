/*
*
*/

#pragma once
#ifndef _CLEARSKY_FONT_H_
#define _CLEARSKY_FONT_H_

#include "core/globals.h"
#include "core/utils.h"
#include "rendersystem/itexture2D.h"

namespace clearsky
{
	//#define NUM_CHARACTERS 256
	const int NUM_CHARACTERS=256;

	//
	//TODO: precompile needed font in single bitmap to prevent
	//texture settings for each glyph
	//

	struct GlyphData
	{
		GlyphData()
		{
			texture = NULL;
			glyph   = NULL;
		}

		~GlyphData()
		{
			SAFE_DELETE(texture);
		}

		ITexture2D *texture;
		FT_Glyph glyph;
		int width;
		int height;
	};

	//We are using freetype2 for font managing
	class CLEARSKY_API Font
	{
		public:
			Font();
			~Font();

			void init();
			void release();

			RETURN_VALUE load(const char* fontName);
			RETURN_VALUE setSize(int width, int height);

			RETURN_VALUE setString(int maxSize, const char* newString, ...);
			const char* getString();
			void setColor(Color newColor);
			Color getColor();
			void setGradient(Gradient newGradient);
			Gradient getGradient();

			const char* getFontPath();
			int getTextHeight();
			int getTextWidth();
			void show(int x, int y);

			

		private:
			const char *m_fontName;
			FT_Face m_face;
			FT_Library *m_ftLib;

			int m_fontWidth;
			int m_fontHeight;
			int m_textWidth;
			int m_textHeight;
			bool m_sizeChanged;

			Gradient m_color;

			char *m_string;

			GlyphData* m_glyphData[NUM_CHARACTERS];
	};
}

#endif