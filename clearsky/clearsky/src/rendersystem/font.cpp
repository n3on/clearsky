/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/font.h"

namespace clearsky
{
	Font::Font():m_fontWidth(12),
				 m_fontHeight(12),
				 m_string(NULL),
				 m_sizeChanged(false),
				 m_face(NULL),
				 m_textWidth(0),
				 m_textHeight(0)
	{
		this->init();
	}

	Font::~Font()
	{
		this->release();
	}

	void Font::init()
	{
		this->m_color.startColor = Color(0,0,0,1);
		this->m_color.endColor   = Color(0,0,0,1);

		for(int i=0; i<NUM_CHARACTERS; ++i)
		{
			this->m_glyphData[i] = NULL;
		}
	}

	void Font::release()
	{
		for(int i=0; i<NUM_CHARACTERS; ++i)
			SAFE_DELETE(this->m_glyphData[i]);
		
		//if(this->m_face)
		//	FT_Done_Face(this->m_face);
	}


	RETURN_VALUE Font::load(const char* fontName)
	{
		this->m_ftLib = GLOBALS->getFreetypeLib();

		if(FT_New_Face(*this->m_ftLib,fontName,0,&this->m_face))
			return RETURN_FREETYPE_ERROR;

		FT_Set_Char_Size(this->m_face,
						this->m_fontWidth<<6, this->m_fontHeight<<6,
						96, 96);

		this->m_fontName = fontName;


		return RETURN_OK;
	}

	RETURN_VALUE Font::setSize(int width, int height)
	{
		this->m_fontWidth  = width;
		this->m_fontHeight = height;

		return RETURN_OK;
	}

	RETURN_VALUE Font::setString(int maxSize, const char* newString, ...)
	{
		SAFE_ARRAY_DELETE(this->m_string);
		
		this->m_string    = new char[maxSize];

		//TODO: Make unicode
		va_list vaList;
		va_start(vaList, newString);
		vsprintf_s(this->m_string, maxSize, newString, vaList);
		va_end(vaList);

		if(this->m_face)
			FT_Set_Char_Size(this->m_face , this->m_fontWidth<<6, this->m_fontHeight<<6, 96, 96);

		for(unsigned int i=0; i<strlen(this->m_string); ++i)
		{
			int charIndex = this->m_string[i];

			if(this->m_glyphData[charIndex]==NULL)
			{			
				if(FT_Load_Glyph(this->m_face, FT_Get_Char_Index(this->m_face, charIndex), FT_LOAD_DEFAULT))
					continue;
				
				this->m_glyphData[charIndex] = new GlyphData;

				GlyphData *glyphData = this->m_glyphData[charIndex];

				FT_Set_Char_Size(this->m_face , this->m_fontWidth<<6, this->m_fontHeight<<6, 96, 96);

				if(FT_Render_Glyph(this->m_face->glyph, FT_RENDER_MODE_NORMAL))
					continue;

				//this->m_face = this->m_glyphData[charIndex]->face;
				FT_Get_Glyph(this->m_face->glyph, &glyphData->glyph);
				FT_Glyph_To_Bitmap( &glyphData->glyph, ft_render_mode_normal, 0, 1);

				glyphData->texture = GLOBALS->getRenderer()->createTexture2D();
				glyphData->texture->setPitch(this->m_face->glyph->bitmap.pitch);
				if(glyphData->texture->create(this->m_face->glyph->bitmap.buffer, 
												this->m_face->glyph->bitmap.pitch, 
												this->m_face->glyph->bitmap.rows, 
												FORMAT_R8_UNORM)!=RETURN_OK)
				{
					continue;
				}

				glyphData->width = this->m_face->glyph->bitmap.width;
				glyphData->height = this->m_face->glyph->bitmap.rows;
			}//end if
		}//end for

		return RETURN_OK;
	}

	const char* Font::getString()
	{
		return this->m_string;
	}

	const char* Font::getFontPath()
	{
		return this->m_fontName;
	}

	int Font::getTextHeight()
	{
		return this->m_fontHeight;
	}

	int Font::getTextWidth()
	{
		int strWidth=0;
		int charWidth=0;
		for(unsigned int i=0; i<strlen(this->m_string); ++i)
		{
			//spaces are evil
			if(this->m_glyphData[this->m_string[i]] && this->m_string[i]!=' ')
			{
				charWidth = this->m_glyphData[this->m_string[i]]->width;
				strWidth += charWidth;
			}
			else
			{
				strWidth += this->m_fontWidth;
				continue;
			}
		}

		return strWidth;//this->m_fontWidth*strlen(this->m_string);
	}

	void Font::setColor(Color newColor)
	{
		this->m_color.startColor = newColor;
		this->m_color.endColor   = newColor;
	}

	Color Font::getColor()
	{
		return this->m_color.startColor;
	}

	void Font::setGradient(Gradient newGradient)
	{
		this->m_color = newGradient;
	}

	Gradient Font::getGradient()
	{
		return this->m_color;
	}

	void Font::show(int x, int y)
	{
		if(this->m_string==NULL)
			return;

		IRenderer *renderer = GLOBALS->getRenderer();

		int curPosX = x;
		int curPosY = y+this->m_fontHeight;

		FT_Matrix textTransform;
		textTransform.xx = 26;
		textTransform.yy = 26;
		textTransform.xy = textTransform.yx = 0;

		FT_Vector vector;
		vector.x = vector.y = 0;

		bool oldAlphaMapping = renderer->isAlphaMapping();
		bool oldTexturing = renderer->isTexturing();

		renderer->setAlphaMapping(true);
		renderer->setTexturing(false);

		DepthStencilState *depthStencil = renderer->getDepthStencilState();
		bool oldDepthTest = depthStencil->depthEnable;
		depthStencil->depthEnable = false;
		renderer->setDepthStencilState(depthStencil);

		for(unsigned int i=0; i<strlen(this->m_string); ++i)
		{
			
			int charIndex = this->m_string[i];
			GlyphData *glyphData = this->m_glyphData[charIndex];
			if(glyphData==NULL)
				continue;
			
			FT_BitmapGlyph glyphBitmap = (FT_BitmapGlyph)glyphData->glyph;
			renderer->setAlphaTexture(glyphData->texture);
			renderer->begin2D();
			renderer->drawFilledRect((float)(curPosX + glyphBitmap->left),
									(float)(curPosY - glyphBitmap->top),
									(float)glyphBitmap->bitmap.pitch,
									(float)glyphBitmap->bitmap.rows,
									0.0f,
									this->m_color);
			renderer->end2D();			
			renderer->apply(1,0);	
			curPosX += glyphData->glyph->advance.x>>16;
			curPosY += glyphData->glyph->advance.y>>16;
		}

		depthStencil->depthEnable = oldDepthTest;
		renderer->setDepthStencilState(depthStencil);
		renderer->setAlphaMapping(oldAlphaMapping);
		renderer->setTexturing(oldTexturing);
		renderer->apply(1,0);
	}

}