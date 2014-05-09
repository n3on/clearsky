/*
*
*/

#include "core/precompiled.h"
#include "core/tgaloader.h"

//TODO: add function 24bit->32bit
//TODO: add function Swap BGR->RBG
//TODO: check to free memory

namespace clearsky
{
	TGALoader::TGALoader()
	{
		this->imageID = 0;
		this->m_imageData = NULL;
		this->m_width = 0;
		this->m_height = 0;
	}

	TGALoader::~TGALoader()
	{
	}

	unsigned short TGALoader::getWidth()
	{
		return this->m_width;
	}
	void TGALoader::setWidth(unsigned short width)
	{
		this->m_width = width;
	}
	unsigned short TGALoader::getHeight()
	{
		return this->m_height;
	}
	void TGALoader::setHeight(unsigned short height)
	{
		this->m_height = height;
	}
	unsigned char* TGALoader::getData()
	{
		return this->m_imageData;
	}

	bool TGALoader::load(const char *filename)
	{
		FILE *file;
		unsigned char uncmpHeader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
		unsigned char compHeader[12] = {0,0,10,0,0,0,0,0,0,0,0,0};
		unsigned char tgaHeader[12];

		if(!filename) return false;
		fopen_s(&file, filename, "rb");
		if(!file) return false;

		fread(tgaHeader, 1, sizeof(tgaHeader), file);

		if(memcmp(uncmpHeader, tgaHeader, sizeof(uncmpHeader))==0)
		{
			return this->loadUncompressed(file);
		}
		else if(memcmp(compHeader, tgaHeader, sizeof(compHeader))==0)
		{
			 return this->loadCompressed(file);
		}

		return false;
	}

	bool TGALoader::loadUncompressed(FILE *file)
	{
		unsigned char header[6];

		fread(header,sizeof(header),1,file);
		this->m_width = header[1]*256+header[0];
		this->m_height = header[3]*256+header[2];

		unsigned char bitCount = header[4];
		int colorMode = bitCount/8;
		unsigned int tgaSize = this->m_width * this-> m_height * colorMode;

		this->m_imageData = new unsigned char[tgaSize];
		if(!this->m_imageData)
			return false;

		fread(this->m_imageData,1, tgaSize, file);

		//convertert 24bit data into 32bit
		if(colorMode == 3)
		{
			unsigned char *newData = new unsigned char[tgaSize];
			memcpy(newData,this->m_imageData,tgaSize);

			delete[] this->m_imageData;
			this->m_imageData = NULL;

			colorMode = 4;
			tgaSize = this->m_width*this->m_height*4;
			this->m_imageData = new unsigned char[tgaSize];

			int index24=0; //index for 24bit data
			for(unsigned int i=0; i<tgaSize; i+=4)
			{
				this->m_imageData[i] = newData[index24];
				this->m_imageData[i+1] = newData[index24+1];
				this->m_imageData[i+2] = newData[index24+2];
				this->m_imageData[i+3] = 255; //alpha value to 1

				index24 += 3;
			}

			delete[] newData;
			newData=NULL;
		}

		unsigned char tempColor;

		for(unsigned int index = 0; index<tgaSize; index+=colorMode)
		{
			//flip from BGR to RGB format
			tempColor = this->m_imageData[index];
			this->m_imageData[index] = this->m_imageData[index+2];
			this->m_imageData[index+2] = tempColor;
		}

		fclose(file);

		return true;
	}

	bool TGALoader::loadCompressed(FILE *file)
	{
		unsigned char header[6];
		memset(header,0,6);
		if(fread(header,1,6,file)!=6)
		{
			LOG->logMsg(clearsky::LT_ERROR, "Error creating Texture: can't read tga header");
			return false;
		}

		this->m_width = header[1]*256+header[0];
		this->m_height = header[3]*256+header[2];

		unsigned char bitCount = header[4];
		int colorMode = bitCount/8;

		unsigned int tgaSize = this->m_width *this-> m_height * colorMode;
		this->m_imageData = new unsigned char[tgaSize];
		if(!this->m_imageData)
		{
			LOG->logMsg(clearsky::LT_ERROR, "Error creating Texture: Low Memory");
			return false;
		}

		unsigned int curPixel    = 0;
		unsigned int curImgIndex = 0;

		unsigned char *pixel = new unsigned char[colorMode];

		unsigned int numPixels = this->m_width*this->m_height;

		do
		{
			unsigned char curHeader = 0;
        
			if(fread(&curHeader, sizeof(unsigned char), 1, file) == 0)
			{
				LOG->logMsg(clearsky::LT_ERROR, "Error creating TGA Texture: can't read chunk Header");
				return false;
			}
        
			if(curHeader < 128) //raw stuff
			{
				++curHeader;
				for (unsigned int i=0; i < curHeader; ++i)
				{
					//if(curImgIndex>=tgaSize)
					//	break;

					if(fread(pixel, 1, colorMode, file) != (unsigned int)colorMode)
					{
						LOG->logMsg(clearsky::LT_ERROR, "Error creating TGA Texture: can't read raw pixel");
						return false;
					}
                    
					//flip from BGRA to RGBA format
					this->m_imageData[curImgIndex] = pixel[2];
					this->m_imageData[curImgIndex + 1] = pixel[1];
					this->m_imageData[curImgIndex + 2] = pixel[0];
					if(colorMode==4)
						this->m_imageData[curImgIndex + 3] = pixel[3];
                
					curImgIndex += colorMode;
					++curPixel;
                
				}//end for
			}//end if(curHeader < 128)
			else //RLE encoding
			{
				curHeader -= 127;
        
				if (fread(pixel, 1, colorMode, file) != (unsigned int)colorMode)
				{
					LOG->logMsg(clearsky::LT_ERROR, "Error creating TGA Texture: can't read rle pixel");
					return false;
				}

				for (unsigned int i = 0; i < curHeader; ++i)
				{
					//if(curImgIndex>=tgaSize)
					//	break;

					//flip from BGRA to RGBA format
					this->m_imageData[curImgIndex] = pixel[2];
					this->m_imageData[curImgIndex + 1] = pixel[1];
					this->m_imageData[curImgIndex + 2] = pixel[0];
					if(colorMode==4)
						this->m_imageData[curImgIndex + 3] = pixel[3];
                
					curImgIndex += colorMode;
					++curPixel;
                
				}//end for   
				//printf("curImgIndex %d curPixel %d\n",curImgIndex, curPixel);
			}//end RLE

		}while(curPixel < numPixels);

		SAFE_ARRAY_DELETE(pixel);
		fclose(file);

		//convertert 24bit data into 32bit
		if(colorMode == 3)
		{
			int imageSize24 = this->m_width*this->m_height*3;
			char *newData = new char[imageSize24];
			memset(newData,0,imageSize24);
			memcpy(newData,this->m_imageData,imageSize24);

			SAFE_ARRAY_DELETE(this->m_imageData);

			colorMode = 4;
			int imageSize32 = this->m_width*this->m_height*4;
			this->m_imageData = new unsigned char[imageSize32];

			int index24=0; //index for 24bit data
			for(int i=0; i<imageSize32; i+=4)
			{
				this->m_imageData[i] = newData[index24];
				this->m_imageData[i+1] = newData[index24+1];
				this->m_imageData[i+2] = newData[index24+2];
				this->m_imageData[i+3] = 255; //alpha value to 1

				index24 += 3;

				//printf("index imageData %d, index oldData %d\n",i,index24);
			}

			delete[] newData;
			newData=NULL;
		}

		return true;
	}

	int TGALoader::getType()
	{
		return this->m_ColorType;
	}

	void TGALoader::unload()
	{
		if(this->m_imageData)
		{
			if(this->m_imageData)
			{
				delete[] this->m_imageData;
				this->m_imageData = NULL;
			}
		}
	}
}
