/*
*
*/

#include "core/precompiled.h"
#include "gui/guiskin.h"

namespace clearsky
{
	GUISkin::GUISkin():m_currentSkin(NULL)
	{
		this->init();
	}

	GUISkin::~GUISkin()
	{

		for(unsigned int i=0; i<this->m_skinData.size(); ++i)
			SAFE_DELETE(this->m_skinData[i]);
	}

	void GUISkin::init()
	{
		//this->m_guiFont = new Font();
		//this->m_guiFont->load("..\\data\\fonts\\arial.ttf");

		this->insertDefaultSkin();
	}

	void GUISkin::insertDefaultSkin()
	{
		//add default skin
		SkinData *defaultSkin = new SkinData();
		defaultSkin->name = "Default";
		defaultSkin->guiFont = Resources->loadFont(16,"..\\data\\fonts\\arial.ttf");
		defaultSkin->topwindowColor.startColor = defaultSkin->topwindowColor.endColor = Color(1.0f,1.0f,1.0f,0.0f);
		defaultSkin->windowColor.startColor = Color(1.0f, 1.0f, 1.0f, 0.9f);
		defaultSkin->windowColor.endColor = Color(0.0f, 0.0f, 0.0f, 0.5f);
		defaultSkin->windowTitleColor.startColor = defaultSkin->windowTitleColor.endColor = Color(0.5f, 0.5f, 0.5f, 0.5f);
		defaultSkin->windowTitleHeight = 20;
		defaultSkin->windowFont = Resources->loadFont(16,"..\\data\\fonts\\arial.ttf");
		defaultSkin->buttonColor.startColor = Color(1.0f, 1.0f, 1.0f, 0.9f);
		defaultSkin->buttonColor.endColor =	Color(0.0f, 0.0f, 0.0f, 0.5f);
		defaultSkin->buttonMouseOverColor.startColor = defaultSkin->buttonColor.endColor;
		defaultSkin->buttonMouseOverColor.endColor   = defaultSkin->buttonColor.startColor;
		defaultSkin->buttonFont = Resources->loadFont(16,"..\\data\\fonts\\arial.ttf");
		defaultSkin->mouseTexture = Resources->loadTexture2D("..\\data\\gui\\mouse.png");
		defaultSkin->staticTextFont = Resources->loadFont(16,"..\\data\\fonts\\arial.ttf");

		this->m_skinData.push_back(defaultSkin);
		this->m_currentSkin = defaultSkin;
	}

	RETURN_VALUE GUISkin::load(const char* filename)
	{
		TiXmlDocument skinFile(filename);
		if(!skinFile.LoadFile())
			return RETURN_ERROR;

		//now parse the skin file
		TiXmlNode *guiRoot = skinFile.FirstChildElement("GUI");
		//do we have gui declaration?
		if(!guiRoot)
			return RETURN_ERROR;

		TiXmlNode *styleSheet = guiRoot->FirstChildElement("stylesheet");

		while(styleSheet)
		{
			//if(!styleSheet)
			//	return RETURN_ERROR;

			TiXmlAttribute *styleSheetAttribute = styleSheet->ToElement()->FirstAttribute();
			if(!styleSheetAttribute)
				return RETURN_ERROR;

			SkinData* newSkin = new SkinData;

			newSkin->name = new char[50];
			strncpy_s(newSkin->name, 50, styleSheetAttribute->Value(),50);

			this->m_skinData.push_back(newSkin);

			TiXmlElement *element = styleSheet->FirstChildElement();
			while(element)
			{
				if(strncmp(element->Value(),"topwindow", 50)==0)
				{
					this->parseTopWindow(element, newSkin);
				}
				else if(strncmp(element->Value(),"window", 50)==0)
				{
					this->parseWindow(element, newSkin);
				}
				else if(strncmp(element->Value(),"button", 50)==0)
				{
					this->parseButton(element, newSkin);
				}
				else if(strncmp(element->Value(),"cursor", 50)==0)
				{
					this->parseCursor(element, newSkin);
				}
				else if(strncmp(element->Value(),"statictext", 50)==0)
				{
					this->parseStaticText(element,newSkin);
				}

				element = element->NextSiblingElement();
			}//end while

			//read next stylesheet in file
			styleSheet = styleSheet->NextSibling("stylesheet");
		}//end while stylesheet
		
		return RETURN_OK;
	}

	RETURN_VALUE GUISkin::remove(const char *name)
	{
		//skin to be removed is current skin
		bool isCurrentSkin = false;
		if(strncmp(this->m_currentSkin->name,name,50)==0)
			isCurrentSkin = true;

		//search for index
		int index=-1;
		for(unsigned int i=0; i<this->m_skinData.size(); ++i)
		{
			if(strncmp(this->m_skinData[i]->name,name,50)==0)
			{
				index=i;
				break;
			}
		}

		//if found
		if(index>=0)
		{
			SAFE_DELETE(this->m_skinData[index]);
			this->m_skinData.erase(this->m_skinData.begin()+index);
			//reset to default skin if current is removed
			if(isCurrentSkin)
				this->setCurrentSkin("Default");
			
			return RETURN_OK;
		}

		return RETURN_ERROR;
	}

	SkinData* GUISkin::getSkin(const char *name)
	{
		SkinData *skin = NULL;

		for(unsigned int i=0; i<this->m_skinData.size(); ++i)
		{
			if(strncmp(this->m_skinData[i]->name, name, 50)==0)
			{
				skin = this->m_skinData[i];
				break;
			}
		}

		return skin;
	}

	void GUISkin::parseTopWindow(TiXmlElement *topWindowElement, SkinData *skin)
	{
		TiXmlElement *element = topWindowElement->FirstChildElement();

		while(element)
		{
			this->parseColorAndGradient(element, skin->topwindowColor);

			skin->topwindowTexture = this->parseTexture(element);
			if(skin->topwindowTexture)
				skin->topwindowTexturing = true;

			element = element->NextSiblingElement();
		}
	}

	void GUISkin::parseWindow(TiXmlElement *windowElement, SkinData *skin)
	{
		TiXmlElement *element = windowElement->FirstChildElement();

		//check all window elements
		while(element)
		{
			//parse color or gradient data
			this->parseColorAndGradient(element, skin->windowColor);
			
			skin->windowTexture = this->parseTexture(element);
			if(skin->windowTexture)
				skin->windowTexturing = true;

			skin->windowFont = this->parseFont(element, skin->windowFontColor);

			//parse title
			if(strncmp(element->Value(), "title", 50)==0)
			{
				TiXmlElement *titleElement = element->FirstChildElement();

				//check all title elements: 
				//TODO unroll the loop if there would be no more title elements
				while(titleElement)
				{
					this->parseColorAndGradient(titleElement, skin->windowTitleColor);
					skin->windowTitleTexture = this->parseTexture(titleElement);
					if(skin->windowTitleTexture)
						skin->windowTitleTexturing = true;

					skin->windowTitleFont = this->parseFont(titleElement, skin->windowTitleFontColor);
					if(strncmp(titleElement->Value(), "size", 10)==0)
					{
						const char* sizeStr = titleElement->GetText();
						if(sizeStr)
						{
							int titleSize = atoi(sizeStr);
							skin->windowTitleHeight = titleSize;
						}
					}

					titleElement = titleElement->NextSiblingElement();
				}//end while titleelement
			}//end if

			element = element->NextSiblingElement();
		}//end while element
	}

	void GUISkin::parseButton(TiXmlElement *buttonElement, SkinData *skin)
	{
		TiXmlElement *element = buttonElement->FirstChildElement();

		//check all button elements
		while(element)
		{
			//parse color or gradient data
			this->parseColorAndGradient(element, skin->buttonColor);

			skin->buttonTexture = this->parseTexture(element);
			if(skin->buttonTexture)
				skin->buttonTexturing = true;

			skin->buttonFont = this->parseFont(element, skin->buttonFontColor);

			//parse mouseover
			if(strncmp(element->Value(), "mouseover", 50)==0)
			{
				TiXmlElement *mouseElement = element->FirstChildElement();
				//check mouseover color
				if(mouseElement)
					this->parseColorAndGradient(mouseElement, skin->buttonMouseOverColor);

			}//end if

			element = element->NextSiblingElement();
		}//end while element
	}

	void GUISkin::parseCursor(TiXmlElement *cursorElement, SkinData *skin)
	{
		TiXmlElement *element = cursorElement->FirstChildElement();

		skin->mouseTexture = this->parseTexture(element);
	}

	void GUISkin::parseStaticText(TiXmlElement *staticElement, SkinData *skin)
	{
		TiXmlElement *element = staticElement->FirstChildElement();

		skin->staticTextFont = this->parseFont(element,skin->staticTextFontColor);
	}

	void GUISkin::parseGradient(TiXmlElement *element, Gradient &outGradient)
	{
		TiXmlElement *gradientData = element->FirstChildElement();
		while(gradientData)
		{
			if(strncmp(gradientData->Value(),"startcolor",50)==0)
			{
				this->parseColor(gradientData,outGradient.startColor);
			}
			else if(strncmp(gradientData->Value(),"endcolor",50)==0)
			{
				this->parseColor(gradientData,outGradient.endColor);
			}
			else if(strncmp(gradientData->Value(),"type",50)==0)
			{
				if(strncmp(gradientData->GetText(),"VERTICAL",50)==0)
				{
					outGradient.type = GRADIENT_VERTICAL;
				}
				else if(strncmp(gradientData->GetText(),"HORIZONTAL",50)==0)
				{
					outGradient.type = GRADIENT_HORIZONTAL;
				}
				else if(strncmp(gradientData->GetText(),"CROSS",50)==0)
				{
					outGradient.type = GRADIENT_MIDDLE_CROSS;
				}
			}

			gradientData = gradientData->NextSiblingElement();
		}
	}

	void GUISkin::parseColor(TiXmlElement *colorElement, Color &outColor)
	{
		char* colorStr = const_cast<char*>(colorElement->GetText());

		char* context	= NULL;
			
		//read 4 color values
		char* colorVal = NULL;
		colorVal = strtok_s(colorStr, " ", &context);
		
		for(int i=0; i<4; ++i)
		{	
			if(!colorVal)
				return;
			
			outColor[i] = (float)atof(colorVal);

			colorVal = strtok_s(NULL, " ", &context);
		}
	}

	void GUISkin::parseColorAndGradient(TiXmlElement *element, Gradient &outGradient)
	{
		if(strncmp(element->Value(),"color",50)==0)
		{
			this->parseColor(element, outGradient.startColor);
			//make start and endcolor of a gradient the same
			outGradient.endColor = outGradient.startColor;
		}
		
		if(strncmp(element->Value(), "gradient", 50)==0)
		{
			this->parseGradient(element, outGradient);
		}
	}

	ITexture2D* GUISkin::parseTexture(TiXmlElement *element)
	{
		ITexture2D *texture = NULL;
		if(strncmp(element->Value(), "texture", 50)==0)
		{
			
			const char *texFilename = element->GetText();
			int strSize = MultiByteToWideChar(CP_ACP,0,texFilename ,strlen(texFilename)+1,NULL,0);
			wchar_t *newTexFilename = new wchar_t[strSize];
			MultiByteToWideChar(CP_ACP,0,texFilename,strlen(texFilename)+1,newTexFilename,strSize);

			texture = Resources->loadTexture2D(/*newTexFilename*/texFilename);

			SAFE_ARRAY_DELETE(newTexFilename);

		}

		return texture;
	}

	Font* GUISkin::parseFont(TiXmlElement *element, Gradient &fontColor)
	{
		Font *font=NULL;
		if(strncmp(element->Value(), "font", 50)==0)
		{
			TiXmlElement *fontElement = element->FirstChildElement();
			//TODO: unroll the loop
			const char *fontName = NULL;
			int fontSize = 0;

			while(fontElement)
			{
				parseColorAndGradient(fontElement,fontColor);

				if(strncmp(fontElement->Value(), "name", 50)==0)
				{
					fontName = fontElement->GetText();
				}
				else if(strncmp(fontElement->Value(), "size", 50)==0)
				{
					fontSize = atoi(fontElement->GetText());
				}

				fontElement = fontElement->NextSiblingElement();
			}

			if(fontName)
				font = Resources->loadFont(fontSize, fontName);
		}

		return font;
	}

	RETURN_VALUE GUISkin::setCurrentSkin(const char* skinname)
	{
		SkinData *skin = NULL;

		for(unsigned int i=0; i<this->m_skinData.size(); ++i)
		{
			if(strncmp(this->m_skinData[i]->name, skinname,50)==0)
			{
				skin=this->m_skinData[i];
				break;
			}
		}

		if(skin)
		{
			this->m_currentSkin = skin;
			return RETURN_OK;
		}

		return RETURN_ERROR;
	}

	SkinData* GUISkin::getCurrentSkin()
	{
		return this->m_currentSkin;
	}

	const char* GUISkin::getCurrentSkinName()
	{
		if(this->m_currentSkin)
		{
			return this->m_currentSkin->name;
		}

		return NULL;
	}
}
