/*
*
*/

#pragma once
#ifndef _START_PARAMS_H_
#define _START_PARAMS_H_

#include "core/utils.h"

namespace clearsky
{
	class OutputWindow
	{
		char title[150];
		int windowWidth;
		int windowHeight;
		int multiSamplingCount;
		int multiSamplingLevel;
		bool fullscreen;

		int id;
	};

	class CLEARSKY_API StartParams
	{
		public:
			StartParams();
			~StartParams();

			RETURN_VALUE load(const char *filename);

			std::vector<OutputWindow *> outputWindows;
			char title[150];
			int windowWidth;
			int windowHeight;
			int multiSamplingCount;
			int multiSamplingLevel;
			bool fullscreen;

			char driverType[150];
			char featureLevel[150];

	};
}

#endif