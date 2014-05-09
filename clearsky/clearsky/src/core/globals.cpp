/*
*
*/

#include "core/precompiled.h"
#include "core/globals.h"

namespace clearsky
{
	Globals* Globals::m_instance=NULL;

	Globals::Globals():m_delta(1.0),
					   m_renderer(NULL),
					   m_logger(NULL),
					   m_engine(NULL),
					   m_mouseHandler(NULL),
					   m_freetypeLib(NULL)
	{
	}

	Globals::~Globals()
	{

	}

	Globals* Globals::getInstance()
	{
		if(!Globals::m_instance)
			Globals::m_instance = new Globals;

		return Globals::m_instance;
	}


	void Globals::setWindowHandle(HWND hwnd)
	{
		this->m_hwnd = hwnd;
	}

	HWND Globals::getWindowHandle()
	{
		return this->m_hwnd;
	}


	void Globals::setWidth(int width)
	{
		this->m_width = width;
	}

	int Globals::getWidth()
	{
		return this->m_width;
	}

	void Globals::setHeight(int height)
	{
		this->m_height = height;
	}

	int Globals::getHeight()
	{
		return this->m_height;
	}

	void Globals::setDeltaTime(double delta)
	{
		this->m_delta = delta;
	}
	
	double Globals::getDeltaTime()
	{
		return this->m_delta;
	}

	Logger* Globals::getLogger()
	{
		return this->m_logger;
	}
			
	void Globals::setLogger(Logger* newLogger)
	{
		this->m_logger=newLogger;
	}

	void Globals::setRenderer(IRenderer *renderer)
	{
		this->m_renderer = renderer;
	}

	IRenderer* Globals::getRenderer()
	{
		return this->m_renderer;
	}

	void Globals::setEngine(Engine *engine)
	{
		this->m_engine = engine;
	}

	Engine* Globals::getEngine()
	{
		return this->m_engine;
	}

	MouseHandler* Globals::getMouseHandler()
	{
		return this->m_mouseHandler;
	}

	void Globals::setMouseHandler(MouseHandler *newMouseHandler)
	{
		this->m_mouseHandler = newMouseHandler;
	}

	void Globals::setFreetypeLib(FT_Library *ftLib)
	{
		this->m_freetypeLib = ftLib;
	}

	FT_Library* Globals::getFreetypeLib()
	{
		return this->m_freetypeLib;
	}
}
