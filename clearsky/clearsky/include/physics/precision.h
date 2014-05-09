#pragma once
#ifndef _CLEARSKY_PRECISION_H_
#define _CLEARSKY_PRECISION_H_

#include "physicsIncludes.h"

namespace clearsky
{
	/*
	Precision clearsky physics should be
	compiled with. Use float or double.
	*************************************/

	typedef float real;

	/** Defines the precision of the square root operator. */

	#define real_sqrt sqrtf
}

#endif