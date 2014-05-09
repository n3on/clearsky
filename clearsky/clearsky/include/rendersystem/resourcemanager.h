/*
*
*/
#pragma once
#ifndef _CLEARSKY_RESOURCE_MANAGER_H_
#define _CLEARSKY_RESOURCE_MANAGER_H_

//#include "core/precompiled.h"
//#include "core/core.h"
#include <map>
#include "core/utils.h"
#include "rendersystem/itexture2D.h"
#include "rendersystem/ieffect.h"
#include "rendersystem/mesh.h"
#include "rendersystem/font.h"

namespace clearsky
{

	struct FontData
	{
		FontData()
		{
			font=NULL;
			fontName=NULL;
		}
		~FontData()
		{
			//SAFE_ARRAY_DELETE(fontName);
		}

		Font* font;
		int size;
		char* fontName;
	};

	class CLEARSKY_API ResourceManager
	{
		public:
			~ResourceManager();

			ITexture2D* loadTexture2D(const char *filename);
			int removeTexture2D(/*LPWSTR filename*/const char *filename);
		
			MOFMesh* loadObject(const char *filename);
			int removeObject(/*LPWSTR filename*/const char *filename);
			
			IEffect* loadEffect(/*LPWSTR filename*/const char *filename);
			int removeEffect(/*LPWSTR filename*/const char *filename);

			Font* loadFont(int size, const char* fontName);
			int removeFont(/*LPCWSTR filename*/const char *filename);

			static ResourceManager* getInstance();
		private:
			ResourceManager();

			std::map<std::string, ITexture2D*> m_textures;
			std::map<std::string, MOFMesh*> m_objects;
			std::map<std::string, IEffect*> m_effects;
			std::vector<FontData> m_fonts;

			static ResourceManager *m_instance;
	};

	#define Resources ResourceManager::getInstance() 
}

#endif