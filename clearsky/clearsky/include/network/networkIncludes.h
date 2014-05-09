#ifndef _CLEARSKY_NETWORK_INCLUDES_H_
#define _CLEARSKY_NETWORK_INCLUDES_H_

/*
 * STL Headers
 ****************/

#include <string>
#include <map>
//#include <boost/thread.hpp>
//#include <boost/ref.hpp>

/*
 * Platform Headers
 ****************/

#include <Windows.h>
#include <WinSock2.h>

/*
 * Static links
 ****************/

#pragma comment(lib, "ws2_32.lib")

/*
 * Engine headers
 ****************/

#include "core/utils.h"
#include "core/logger.h"
#include "core/globals.h"

#endif