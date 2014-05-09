/*
*
*/

#ifndef _CLEARSKY_TGA_LOADER_H_
#define _CLEARSKY_TGA_LOADER_H_

#include <windows.h>
#include <stdio.h>
#include "core/utils.h"
#include "core/globals.h"

namespace clearsky
{
	class CLEARSKY_API TGALoader
	{
		public:
			TGALoader();
			~TGALoader();

			bool load(const char *filename);
			void unload();

			unsigned short getWidth();
			void setWidth(unsigned short width);
			unsigned short getHeight();
			void setHeight(unsigned short height);
			unsigned char* getData();
			int getType();

			unsigned int imageID;

		private:
			bool loadUncompressed(FILE *file);
			bool loadCompressed(FILE *file);

			unsigned char m_colorDepth;
			unsigned char m_ColorType;
			unsigned char m_dataFormat;
			unsigned char *m_imageData;
			unsigned short m_width;
			unsigned short m_height;
			unsigned long m_imageSize;
	};
}

#endif
