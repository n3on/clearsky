#ifndef _CLEARSKY_XAUDIO_INCLUDES_H_
#define _CLEARSKY_XAUDIO_INCLUDES_H_

//Built with:
//OGG version 1.1.4
//Vorbis version 1.2.3

//XAudio stuff
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <xapofx.h>
#include <x3DAudio.h>

#pragma comment(lib, "x3DAudio.lib")
#pragma comment(lib, "XAPOFX.lib")

//Engine's core components:
#include "core/utils.h"
#include "core/logger.h"
#include "core/globals.h"

namespace clearsky
{
	const static short NUM_PRESETS = 30;

	typedef std::wstring locationStringType;
};


#endif