/*
*
*/

#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_

#include <map>

#include "rendersystem/itexture2D.h"

#include "core/utils.h"
#include "core/globals.h"

namespace clearsky
{
	class CLEARSKY_API TextureManager
	{
		public:
			~TextureManager();

			ITexture2D* loadTexture2D(const char *filename);
			int removeTexture2D(const char *filename);

			static TextureManager* getInstance();
		private:
			TextureManager();

			static TextureManager *m_instance;
			std::map<std::string, ITexture2D*> m_textures;
	};

	#define TextureResource TextureManager::getInstance() 
}

#endif