#pragma once
#ifndef _CLEARSKY_NETWORK_H_
#define _CLEARSKY_NETWORK_H_


#include "networkIncludes.h"

#include "networkData.h"
#include "networkQueue.h"
#include "networkSocket.h"


/*

Socket concept:

Socket class
 --> Client
 --> Server
	--> TCP
	--> UDP

NetworkQueue, thread safe

Helpers:

LinMutexFactory
WinMutexFactory
Mutex class

*/

#endif