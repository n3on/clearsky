/*
*
*/
#pragma once
#ifndef _CLEARSKY_ENGINE_H_
#define _CLEARSKY_ENGINE_H_

#include <WinSock2.h>

#include "core/core.h"
#include "core/logger.h"
#include "core/timer.h"
#include "core/globals.h"
#include "core/PathExtractor.h"

#include "rendersystem/ResourceManager.h"
#include "rendersystem/box.h"
#include "rendersystem/light.h"
#include "rendersystem/fog.h"
#include "rendersystem/skybox.h"
#include "rendersystem/mesh.h"
#include "rendersystem/startparams.h"

#include "rendersystem/directx11/dx11renderer.h"
#include "rendersystem/directx11/dx11texture2D.h"
#include "rendersystem/directx11/dx11buffer.h"
#include "rendersystem/directx11/dx11sprite.h"

#include "rendersystem/primitivedrawer.h"

#include "rendersystem/font.h"
/*#include "rendersystem/directx10/dx10renderer.h"
#include "rendersystem/directx10/dx10texture2D.h"
#include "rendersystem/directx10/dx10font.h"
#include "rendersystem/directx10/dx10buffer.h"
#include "rendersystem/directx10/dx10sprite.h"
*/

#include "rendersystem/terrain.h"
#include "rendersystem/resourcemanager.h"

#include "soundsystem/Audio.h"

#include "inputsystem/keyboardhandler.h"
#include "inputsystem/mousehandler.h"

//#include "network/network.h"

#include "math/mathutils.h"
#include "math/boundingbox.h"

#endif
