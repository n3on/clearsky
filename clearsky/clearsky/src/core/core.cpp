/*
*
*/

#include "core/precompiled.h"
#include "vld.h" //visual leak detector
#include "core/core.h"

//include all needed external libraries
#if defined(_MSC_VER)
	#pragma comment(lib, "d3d10.lib")
	#pragma comment(lib, "d3d11.lib")
	#pragma comment(lib, "D3DCompiler.lib")

	#if defined(_DEBUG)
		#pragma comment(lib, "d3dx10d.lib")
		#pragma comment(lib, "d3dx11d.lib")
		#pragma comment(lib, "d3dx11effectsd.lib")
	#else
		#pragma comment(lib, "d3dx10.lib")
		#pragma comment(lib, "d3dx11.lib")
		#pragma comment(lib, "d3dx11effects.lib")
	#endif
#endif

namespace clearsky
{
	Engine::Engine():m_renderer(NULL),
					 m_run(true),
					 m_fps(-1),
					 m_ticks(0),
					 m_startTime(0),
					 m_mouseHandler(NULL),
					 m_keyboardHandler(NULL),
					 m_gui(NULL),
					 m_logger(NULL),
					 m_freetypeLib(NULL),
					 m_fpsCameraKeyboard(false),
					 m_fpsCameraMouse(false),
					 m_oldMouseX(1024<<1),
					 m_oldMouseY(768<<1),
					 m_curMouseX(0),
					 m_curMouseY(0)
	{
		this->init();
	}


	Engine::~Engine()
	{
		this->release();
	}

	RETURN_VALUE Engine::init()
	{
		this->m_logger = new Logger();
		m_logger->setLogFilePath("debug.log");
		m_logger->setLoggingMode(clearsky::LM_BOTH);
		m_logger->setTimeStamp(true);
	    m_logger->logMsg(clearsky::LT_STATUS, "Logging was started.");

		GLOBALS->setLogger(this->m_logger);
		GLOBALS->setEngine(this);

		this->m_keyboardHandler = new KeyboardHandler();
		this->m_mouseHandler = new MouseHandler();

		GLOBALS->setMouseHandler(this->m_mouseHandler);
		
		//init freetype library
		if(FT_Init_FreeType(&this->m_freetypeLib))
			return RETURN_FREETYPE_ERROR;
		GLOBALS->setFreetypeLib(&this->m_freetypeLib);

		this->m_cameraControl.keyboardMoveSpeed = Vector3D(50, 0, 50);
		this->m_cameraControl.keyboardRotationSpeed = Vector3D(0.3f,0.3f,0.3f);
		this->m_cameraControl.mouseRotSpeed = Vector3D(0.1f,0.1f,0.1f);

		return RETURN_OK;
	}

	KeyboardHandler* Engine::getKeyboard()
	{

		return this->m_keyboardHandler;
	}

	MouseHandler* Engine::getMouseHandler()
	{
		return this->m_mouseHandler;
	}

	GUI* Engine::getGUI()
	{
		if(!this->m_gui)
			this->m_gui = new GUI();

		return this->m_gui;
	}

	bool Engine::run()
	{
		GLOBALS->setDeltaTime(this->m_timer.getDelta());

		MSG msg = {0};
		
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return this->m_run;
	}

	void Engine::update()
	{
		if(this->m_fpsCameraKeyboard)
			this->keyboardCameraControl();
		if(this->m_fpsCameraMouse)
			this->mouseCameraControl();

		this->m_renderer->update();

	}

	void Engine::exit()
	{
		this->m_run = false;
	}

	IRenderer* Engine::getRenderer(RENDERER_TYPE renderer)
	{
		if(!this->m_renderer)
		{
			switch(renderer)
			{
				case RENDER_DX10:
				{
					//this->m_renderer = new DX10Renderer;
				}break;
				case RENDER_DX11:
				{
					this->m_renderer = new DX11Renderer;
				}break;
			}

			GLOBALS->setRenderer(this->m_renderer);

			this->m_renderer->setKeyboardHandler(this->m_keyboardHandler);
			this->m_renderer->setMouseHandler(this->m_mouseHandler);

		}

		return this->m_renderer;
	}

	void Engine::setConstFPS(int newFPS)
	{
		this->m_ticks = 1000/newFPS;
		this->m_fps = newFPS;
	}

	int Engine::getConstFPS()
	{
		return this->m_fps;
	}

	void Engine::makeConstFPS()
	{
		/*if(this->m_fps)
		{
			this->m_startTime += this->m_ticks;
			int sleep = (int)(this->m_startTime - GetTickCount());
			if(sleep >= 0)
			{
				Sleep(sleep);
			}

		}*/

		if(this->m_fps)
		{
			//if(this->m_startTime==0)
			//{
				this->m_startTime = GetTickCount();
			//}

			/*//this->m_startTime += this->m_ticks;
			int sleep = (int)(this->m_startTime - GetTickCount());
			if(sleep >= 0)
			{
				Sleep(sleep);
			}*/

			double nextTime = this->m_startTime + this->m_ticks;
			double now = GetTickCount();
			if(nextTime >=now)
			{
				Sleep((DWORD)(nextTime - now));
				this->m_startTime=0;
			}

		}

	}

	//activate default keyboard camera control (movement)
	void Engine::setCameraKeyboard(bool activate)
	{
		this->m_fpsCameraKeyboard=activate;
	}

	//activate default mouse camera control (rotation)
	void Engine::setCameraMouse(bool activate)
	{
		this->m_fpsCameraMouse = activate;
	}

	void Engine::setCameraControlData(Vector3D keyboardMoveSpeed, Vector3D keyboardRotSpeed, Vector3D mouseRotSpeed)
	{
		this->m_cameraControl.keyboardMoveSpeed		= keyboardMoveSpeed;
		this->m_cameraControl.keyboardRotationSpeed = keyboardRotSpeed;
		this->m_cameraControl.mouseRotSpeed			= mouseRotSpeed;
	}

	void Engine::keyboardCameraControl()
	{
		if(!this->m_keyboardHandler || !this->m_renderer)
			return;

		if(this->m_keyboardHandler->isKeyDown(VK_W))
			this->m_renderer->getCamera()->move(CM_FORWARD, this->m_cameraControl.keyboardMoveSpeed.z);
		
		if(this->m_keyboardHandler->isKeyDown(VK_S))
			this->m_renderer->getCamera()->move(CM_BACK, this->m_cameraControl.keyboardMoveSpeed.z);
		
		if(this->m_keyboardHandler->isKeyDown(VK_A))
			this->m_renderer->getCamera()->move(CM_LEFT,this->m_cameraControl.keyboardMoveSpeed.x);

		if(this->m_keyboardHandler->isKeyDown(VK_D))
			this->m_renderer->getCamera()->move(CM_RIGHT,this->m_cameraControl.keyboardMoveSpeed.x);

		if(this->m_keyboardHandler->isKeyDown(VK_LEFT))
			this->m_renderer->getCamera()->move(CM_TURN_LEFT, this->m_cameraControl.keyboardRotationSpeed.x);
		
		if(this->m_keyboardHandler->isKeyDown(VK_RIGHT))
			this->m_renderer->getCamera()->move(CM_TURN_RIGHT, this->m_cameraControl.keyboardRotationSpeed.x);
		
		if(this->m_keyboardHandler->isKeyDown(VK_UP))
			this->m_renderer->getCamera()->move(CM_TURN_UP, this->m_cameraControl.keyboardRotationSpeed.y);
		
		if(this->m_keyboardHandler->isKeyDown(VK_DOWN))
			this->m_renderer->getCamera()->move(CM_TURN_DOWN, this->m_cameraControl.keyboardRotationSpeed.y);
		
	}

	void Engine::mouseCameraControl()
	{
		if(!this->m_mouseHandler)
			return;

		RECT rect;
		GetWindowRect(GLOBALS->getWindowHandle(),&rect);
		//(rect->right-rect->left)>>1;

		this->m_oldMouseX = (rect.right+rect.left)>>1;//GLOBALS->getWidth()>>1;
		this->m_oldMouseY = (rect.bottom+rect.top)>>1;//GLOBALS->getHeight()>>1;

		this->m_mouseHandler->getCurScreenPos(this->m_curMouseX, this->m_curMouseY);
		this->m_mouseHandler->setCurScreenPos(this->m_curMouseX, this->m_oldMouseY);
		this->m_renderer->getCamera()->move(CM_TURN_DOWN, (this->m_curMouseY-this->m_oldMouseY)/*mouse->getDeltaY()*//**0.1f*/*this->m_cameraControl.mouseRotSpeed.y);

		this->m_mouseHandler->getCurScreenPos(this->m_curMouseX, this->m_curMouseY);
		this->m_mouseHandler->setCurScreenPos(this->m_oldMouseX, this->m_curMouseY);
		this->m_renderer->getCamera()->move(CM_TURN_RIGHT, (this->m_curMouseX-this->m_oldMouseX)*this->m_cameraControl.mouseRotSpeed.y/*0.1f*/);
	}

	void Engine::release()
	{
		
		if(this->m_renderer)
		{
			this->m_renderer->release();
			this->m_renderer = NULL;
		}
		
		//SAFE_DELETE(this->m_keyboardHandler);
		if(this->m_logger)
		{
			this->m_logger->release();
			SAFE_DELETE(this->m_logger);
		}
		
		//SAFE_DELETE(this->m_gui);
		//this->m_gui->release();
		SAFE_DELETE(this->m_gui);

		if(this->m_freetypeLib)
		{
			FT_Done_FreeType(this->m_freetypeLib);
			this->m_freetypeLib=NULL;
		}
	}

}
