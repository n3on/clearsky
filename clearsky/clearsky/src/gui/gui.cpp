/*
*
*/

#include "core/precompiled.h"
#include "gui/gui.h"

namespace clearsky
{
	GUI::GUI():m_guiSkin(NULL),
				m_topWindow(NULL),
				m_mouse(NULL)
	{
		this->init();
		
	}

	GUI::~GUI()
	{
		//SAFE_DELETE(this->m_mouse);
		this->release();
	}

	void GUI::release()
	{
		SAFE_DELETE(this->m_depthState);
		SAFE_DELETE(this->m_mouse);
		SAFE_DELETE(this->m_topWindow);
		SAFE_DELETE(this->m_guiSkin);
	}

	void GUI::init()
	{
		this->m_guiSkin = new GUISkin();
		this->m_mouse = new Mouse(this->m_guiSkin);
		this->m_renderer = GLOBALS->getRenderer();

		this->m_depthState = new DepthStencilState;
		this->m_depthState->depthEnable = false;

		this->m_topWindow = new Window(this->m_guiSkin);
		this->m_topWindow->setType(GUI_TYPE_TOPWINDOW);
		this->m_topWindow->setSize((float)GLOBALS->getWidth(), (float)GLOBALS->getHeight());
	}

	void GUI::reloadSkin()
	{
		if(this->m_mouse)
			this->m_mouse->reloadSkin();

		if(this->m_topWindow)
			this->m_topWindow->reloadSkin();
	}

	RETURN_VALUE GUI::deleteSkin(const char *skin)
	{
		return this->m_guiSkin->remove(skin);
	}

	GUISkin* GUI::getGUISkin()
	{
		return this->m_guiSkin;
	}

	void GUI::setGUISkin(GUISkin *newGUISkin)
	{
		this->m_guiSkin = newGUISkin;
	}

	RETURN_VALUE GUI::setCurrentSkin(const char* skinname)
	{
		if(!this->m_guiSkin)
			return RETURN_ERROR;

		RETURN_VALUE result = this->m_guiSkin->setCurrentSkin(skinname);
		if(result==RETURN_OK)
			this->reloadSkin();

		return result;
	}

	Mouse* GUI::getMouse()
	{
		return this->m_mouse;
	}

	void GUI::detach()
	{
		//this->m_guiThread = boost::thread(&GUI::run, this);
	}

	void GUI::join()
	{
		//this->m_guiThread.join();
	}

	RETURN_VALUE GUI::loadSkin(const char *filename)
	{
		if(filename)
		{
			if(this->m_guiSkin->load(filename)==RETURN_OK)
			{
				this->m_skinFileName = filename;
			}
			else
				return RETURN_ERROR;
		}

		return RETURN_OK;
	}

	void GUI::update()
	{
		this->m_mouse->update();

		if(this->m_topWindow)
			this->m_topWindow->processEvents();

		/*if(this->checkMouseMove());
			this->checkWindowMove();

		this->checkMouseKey();*/
	}

	void GUI::show()
	{
		//DepthStencilState *oldDepthState = this->m_renderer->getDepthStencilState();
		bool oldTexturing = this->m_renderer->isTexturing();
		this->m_renderer->setTexturing(false);
		this->m_renderer->pushDepthStencilState();
		this->m_renderer->setDepthStencilState(this->m_depthState);

		this->m_renderer->begin2D();
		
		if(this->m_topWindow)
			this->m_topWindow->show();

		this->m_mouse->show();

		this->m_renderer->end2D();
		//this->m_renderer->setDepthStencilState(oldDepthState);
		this->m_renderer->setTexturing(oldTexturing);
		this->m_renderer->popDepthStencilState();
	}

	void GUI::run()
	{
		/*while(1)
		{
			this->update();
			this->show();
		}*/
	}

	void GUI::setTopWindow(Window *window)
	{
		if(window)	
			this->m_topWindow = window;
	}

	Window* GUI::getTopWindow()
	{
		return this->m_topWindow;
	}

	Window* GUI::createWindow()
	{
		return new Window(this->m_guiSkin);
	}

	Button* GUI::createButton()
	{
		return new Button(this->m_guiSkin);
	}

	StaticText* GUI::createStaticText()
	{
		return new StaticText(this->m_guiSkin);
	}
}
