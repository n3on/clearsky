#pragma once
#ifndef _CLEARSKY_LOGGER_H_
#define _CLEARSKY_LOGGER_H_

//#include <stdio.h>	//for FILE
//#include <stdarg.h> //for va_list
//#include <string.h> //for strlen
//#include <time.h>   //for timestamping logs

//#include "core/precompiled.h"
#include "core/utils.h"


/****************************
TODO:

1) Add logMsg function for
std::wstring shit

***********************************/

/****************************
How to use Logger class:
*************************

1 setLoggingMode (file, cmd, both)
[2 setLogFilePath]
3 setTimeStamp (true, false)
[4 setTimestampFormat]
5 logMsg (logTypes below)
6 to save the file, call release
7 to destroy the class, call delete

*******************************************/

#define LOGMSG_LENGTH 1024

namespace clearsky
{

	#define CHECK_LOG(a, b) if(a!=clearsky::RETURN_OK) clearsky::LOG->logMsg(clearsky::LT_ERROR, "%s\n", b); return 1;

	#define LOG_PARAGRAPH LOG->logMsg(clearsky::LT_SUPPRESS, "\n\n\n\n");

	/****************
	Log to file or console or both
	******************************/
	enum CLEARSKY_API LOG_MODE 
	{
		LM_BOTH,
		LM_FILE,
		LM_STDOUT
	};

	/****************
	Type of log message
	********************/
	enum CLEARSKY_API LOG_TYPE
	{
		LT_STATUS,
		LT_WARNING,
		LT_ERROR,
		LT_CRITICAL,
		LT_MISC,
		LT_SUPPRESS,
		//LT_TIMESTAMP,  //obsolete

		/* these three are for individual use: */
		LT_CAT_1,
		LT_CAT_2,
		LT_CAT_3
	};

	class CLEARSKY_API Logger
	{
	public:

		Logger();

		~Logger();

		//works like printf
		void logMsg(const LOG_TYPE logType, const char* fmt, ...);

		//set path and create log file
		void setLogFilePath(const char* path);

		//set logging to cmd, file or both
		void setLoggingMode(const LOG_MODE lm);

		//insert timestamp before the logMsgs
		void setTimeStamp(const bool enabled);

		//close file without destroying the class:
		void release();


		/**********************************
		Timestamp formatting options:

		%a	Abbreviated weekday name *	Thu
		%A	Full weekday name * 	Thursday
		%b	Abbreviated month name *	Aug
		%B	Full month name *	August
		%c	Date and time representation *	Thu Aug 23 14:55:02 2001
		%d	Day of the month (01-31)	23
		%H	Hour in 24h format (00-23)	14
		%I	Hour in 12h format (01-12)	02
		%j	Day of the year (001-366)	235
		%m	Month as a decimal number (01-12)	08
		%M	Minute (00-59)	55
		%p	AM or PM designation	PM
		%S	Second (00-61)	02
		%U	Week number with the first Sunday as the first day of week one (00-53)	33
		%w	Weekday as a decimal number with Sunday as 0 (0-6)	4
		%W	Week number with the first Monday as the first day of week one (00-53)	34
		%x	Date representation *	08/23/01
		%X	Time representation *	14:55:02
		%y	Year, last two digits (00-99)	01
		%Y	Year	2001
		%Z	Timezone name or abbreviation	CDT
		%%	A % sign	%

		*****************************************************************************************/
		bool setTimeStampFormat(const char* fmt);


	private:

		// converts a logType into a char*
		const bool logTypeToStr(const LOG_TYPE logType, char* dest, const unsigned int dstsize, const char* fmt, const va_list args);

		FILE* m_logFile;
		char* m_logFilePath;
		char* m_tsFormat;
		bool m_timestamp;

		LOG_MODE m_logMode;

	};

	#define LOG GLOBALS->getLogger()

}
#endif