/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/texturemanager.h"

namespace clearsky
{
	TextureManager* TextureManager::m_instance=NULL;

	TextureManager::TextureManager()
	{
	}

	TextureManager::~TextureManager()
	{
		std::map<std::string, ITexture2D*>::const_iterator textureIter;
		for (textureIter = this->m_textures.begin(); textureIter != this->m_textures.end(); ++textureIter)
		{
			SAFE_DELETE((ITexture2D*)textureIter->second);
			this->m_textures.erase(textureIter);
		}
		this->m_textures.clear();
	}

	ITexture2D* TextureManager::loadTexture2D(const char *filename)
	{
		ITexture2D *texture = NULL;
		
		//char *newFilename = toStr(filename);
		std::string strFilename(filename);

		std::map<std::string, ITexture2D*>::const_iterator textureIter;
		textureIter = this->m_textures.find(filename);
		if (textureIter != this->m_textures.end())
		{
			//printf("Manager: found texture: %s\n", filename);
			return textureIter->second;
		}
		else
		{
			//printf("Manager: create new texture: %s\n", filename);
			texture = GLOBALS->getRenderer()->createTexture2D();
			texture->loadFile(filename);
	
			this->m_textures.insert(std::pair<std::string, ITexture2D*>(filename, texture));
		}

		return texture;
	}

	int TextureManager::removeTexture2D(/*LPWSTR filename*/const char *filename)
	{
		//char *newFilename = toStr(filename);
		std::string strFilename(/*newFilename*/filename);

		std::map<std::string, ITexture2D*>::const_iterator textureIter;
		textureIter = this->m_textures.find(/*newFilename*/filename);
		if(textureIter!=this->m_textures.end())
		{
			SAFE_DELETE((ITexture2D*)textureIter->second);
			this->m_textures.erase(textureIter);
		}

		return 0;
	}

	TextureManager* TextureManager::getInstance()
	{
		if(TextureManager::m_instance==NULL)
			TextureManager::m_instance = new TextureManager;

		return TextureManager::m_instance;
	}
}