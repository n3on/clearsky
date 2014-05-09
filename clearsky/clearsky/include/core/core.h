/*
*
*/
#pragma once
#ifndef _CLEARSKY_CORE_H_
#define _CLEARSKY_CORE_H_

#define WIN32_LEAN_AND_MEAN     // include only minimal functionality
#define NOGDICAPMASKS           // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
// #define NOWINSTYLES                  // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_* // "WS_VISIBLE"
#define NOSYSMETRICS            // SM_*
#define NOMENUS                         // MF_*
#define NOICONS                         // IDI_*
#define NOKEYSTATES                     // MK_*
#define NOSYSCOMMANDS           // SC_*
#define NORASTEROPS                     // Binary and Tertiary raster ops
#define NOSHOWWINDOW            // SW_*
#define OEMRESOURCE                     // OEM Resource values
#define NOATOM                          // Atom Manager routines
#define NOCLIPBOARD                     // Clipboard routines
#define NOCOLOR                         // Screen colors
#define NOCTLMGR                        // Control and Dialog routines
#define NODRAWTEXT                      // DrawText() and DT_*
#define NOGDI                           // All GDI defines and routines
#define NOKERNEL                        // All KERNEL defines and routines
#define NOUSER                          // All USER defines and routines
#define NONLS                           // All NLS defines and routines
#define NOMB                            // MB_* and MessageBox()
#define NOMEMMGR                        // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE                      // typedef METAFILEPICT
#define NOMINMAX                        // Macros min(a,b) and max(a,b)
// #define NOMSG                                // typedef MSG and associated routines // "::LPMSG" is required for some D3D11 functions
#define NOOPENFILE                      // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL                        // SB_* and scrolling routines
#define NOSERVICE                       // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND                         // Sound driver routines
#define NOTEXTMETRIC            // typedef TEXTMETRIC and associated routines
#define NOWH                            // SetWindowsHook and WH_*
#define NOWINOFFSETS            // GWL_*, GCL_*, associated routines
#define NOCOMM                          // COMM driver routines
#define NOKANJI                         // Kanji support stuff.
#define NOHELP                          // Help engine interface.
#define NOPROFILER                      // Profiler interface.
#define NODEFERWINDOWPOS        // DeferWindowPos routines
#define NOMCX                           // Modem Configuration Extensions

#include <windows.h>

//#include "core/precompiled.h"

#include "rendersystem/directx11/dx11renderer.h"
//#include "rendersystem/directx10/dx10renderer.h"
#include "rendersystem/irenderer.h"
#include "gui/gui.h"
#include "inputsystem/keyboardhandler.h"
#include "inputsystem/mousehandler.h"
#include "core/utils.h"
#include "core/logger.h"
#include "core/timer.h"

namespace clearsky
{
	//possible rendering apis
	enum CLEARSKY_API RENDERER_TYPE
	{
		RENDER_DX10, //direc3d 10 renderer
 		RENDER_DX11 //direct3d 11 renderer
	};

	struct CameraControl
	{
		Vector3D keyboardMoveSpeed;
		Vector3D keyboardRotationSpeed;
		Vector3D mouseRotSpeed;
	};

	//core class to control the engine
	class CLEARSKY_API Engine
	{
		public:
			Engine();
			~Engine();

			RETURN_VALUE init();

			bool run(); //true while engine is executing
			void update(); //update all engine components
			void exit(); //exit engine

			//get graphics renderer of specific type
			IRenderer* getRenderer(RENDERER_TYPE renderer);
			KeyboardHandler* getKeyboard();//get keyboard access
			MouseHandler* getMouseHandler();//get mouse access
			GUI* getGUI();

			//set fps to constant number,
			void setConstFPS(int newFPS);
			int getConstFPS();//current fps count(set by setConstFps)

			//enable the constant fps count
			void makeConstFPS();

			//activate default keyboard camera control (movement)
			void setCameraKeyboard(bool activate=true);
			//activate default mouse camera control (rotation)
			void setCameraMouse(bool activate=true);
			//
			void setCameraControlData(Vector3D keyboardMoveSpeed, Vector3D keyboardRotSpeed, Vector3D mouseRotSpeed);

			//release all resources
			void release();

		private:
			//called in update function, when the default camera control is activated
			void keyboardCameraControl();
			void mouseCameraControl();
			int m_oldMouseX; //for mouse control, to center the curser while ratating camera
			int m_oldMouseY;
			int m_curMouseX;
			int m_curMouseY;

			CameraControl m_cameraControl;

			IRenderer *m_renderer; //graphic renderer
			KeyboardHandler *m_keyboardHandler; //keyboard
			MouseHandler *m_mouseHandler; //mouse
			GUI *m_gui;

			//main logger
			Logger *m_logger;
			bool m_run;//is engine running?

			int m_fps; //constant fps counter
			double m_ticks; //need fot constant fps
			double m_startTime; //need for constant fps

			bool m_fpsCameraKeyboard;
			bool m_fpsCameraMouse;

			Timer m_timer; //need to get frame duration

			FT_Library m_freetypeLib;
	};

};

#endif _CORE_H_