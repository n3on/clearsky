/*
*
*/

#include "core/precompiled.h"
#include "rendersystem/resourcemanager.h"

namespace clearsky
{

	ResourceManager* ResourceManager::m_instance=NULL;

	ResourceManager::ResourceManager()
	{
	}

	ResourceManager::~ResourceManager()
	{
		std::map<std::string, ITexture2D*>::const_iterator textureIter;
		for (textureIter = this->m_textures.begin(); textureIter != this->m_textures.end(); ++textureIter)
		{
			SAFE_DELETE((ITexture2D*)textureIter->second);
			this->m_textures.erase(textureIter);
		}
		this->m_textures.clear();


		std::map<std::string, MOFMesh*>::const_iterator objectIter;
		for(objectIter = this->m_objects.begin(); objectIter != this->m_objects.end(); ++objectIter)
		{
			objectIter->second->release();
			SAFE_DELETE((MOFMesh*)objectIter->second);
			this->m_objects.erase(objectIter);
		}
		this->m_objects.clear();

		for(unsigned int i=0; i<this->m_fonts.size(); ++i)
		{
			SAFE_DELETE(this->m_fonts[i].font);
		}
	}

	ITexture2D* ResourceManager::loadTexture2D(const char *filename)
	{
		ITexture2D *texture = NULL;
		
		//char *newFilename = toStr(filename);
		std::string strFilename(filename);

		std::map<std::string, ITexture2D*>::const_iterator textureIter;
		textureIter = this->m_textures.find(filename);
		if (textureIter != this->m_textures.end())
		{
			return textureIter->second;
		}
		else
		{
			texture = GLOBALS->getRenderer()->createTexture2D();
			texture->loadFile(filename);
	
			this->m_textures.insert(std::pair<std::string, ITexture2D*>(filename, texture));
		}

		return texture;
	}

	int ResourceManager::removeTexture2D(/*LPWSTR filename*/const char *filename)
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

	MOFMesh* ResourceManager::loadObject(const char *filename)
	{
		MOFMesh *newObject=NULL;

		//convert from LPWSTR to char*
		std::string strFilename(filename);

		std::map<std::string, MOFMesh*>::const_iterator meshIter;
		meshIter = this->m_objects.find(strFilename);
		if (meshIter != this->m_objects.end())
		{
			return meshIter->second;
		}
		else
		{
			newObject = new MOFMesh();
			if(newObject->load(filename)!=RETURN_OK)
				return NULL;

			this->m_objects.insert(std::pair<std::string, MOFMesh*>(strFilename, newObject));
		}

		return newObject;
	}

	int ResourceManager::removeObject(const char *filename)
	{
		std::map<std::string, MOFMesh*>::const_iterator meshIter;

		//convert from LPWSTR to char*
		//char *newFilename = toStr(filename);
		std::string strFilename(filename);

		meshIter = this->m_objects.find(strFilename);
		if(meshIter!=this->m_objects.end())
		{
			SAFE_DELETE((MOFMesh*)meshIter->second);
			this->m_objects.erase(meshIter);
		}

		//SAFE_DELETE(f);
		return 0;
	}


	IEffect* ResourceManager::loadEffect(const char *filename)
	{
		IEffect *newEffect=NULL;

		//convert from LPWSTR to char*
		std::string strFilename(filename);

		std::map<std::string, IEffect*>::const_iterator effectIter;
		effectIter = this->m_effects.find(strFilename);
		if (effectIter != this->m_effects.end())
		{
			return effectIter->second;
		}
		else
		{
			newEffect = GLOBALS->getRenderer()->createEffect();;
			if(newEffect->create(filename)!=RETURN_OK)
				return NULL;

			this->m_effects.insert(std::pair<std::string, IEffect*>(strFilename, newEffect));
		}

		return newEffect;
	}

	int ResourceManager::removeEffect(const char *filename)
	{
		std::map<std::string, IEffect*>::const_iterator effectIter;

		//convert from LPWSTR to char*
		std::string strFilename(filename);

		effectIter = this->m_effects.find(strFilename);
		if(effectIter!=this->m_effects.end())
		{
			SAFE_DELETE((IEffect*)effectIter->second);
			this->m_effects.erase(effectIter);
		}

		return 0;
	}

	Font* ResourceManager::loadFont(int size, const char* fontName)
	{
		Font *newFont=NULL;

		for(unsigned int i=0; i<this->m_fonts.size(); ++i)
		{
			if((strncmp(this->m_fonts[i].fontName,fontName,256)==0) && (this->m_fonts[i].size==size))
			{
				newFont = this->m_fonts[i].font;
				break;
			}	
		}
		
		if(newFont)
		{
			return newFont;
		}
		else
		{
			FontData fontData;
			//convert from LPCWSTR to char*
			newFont = new Font();
			newFont->setSize(size, size);
			newFont->load(fontName);
			
			fontData.fontName = new char[strlen(fontName)+1];
			strncpy_s(fontData.fontName, strlen(fontName)+1, fontName,256);
			fontData.size = size;
			fontData.font = newFont;

			this->m_fonts.push_back(fontData);
		}

		return newFont;
	}

	int ResourceManager::removeFont(const char *filename)
	{
		return 0;
	}

	ResourceManager* ResourceManager::getInstance()
	{
		if(ResourceManager::m_instance==NULL)
			ResourceManager::m_instance = new ResourceManager;

		return ResourceManager::m_instance;
	}
}
