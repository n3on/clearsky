/*
*
*/

#include "core/precompiled.h"
#include "core/logger.h"


namespace clearsky
{

	Logger::Logger():m_logMode(LM_FILE),
						 m_logFile(NULL),
						 m_timestamp(true)
	{
		this->m_logFilePath = new char[FILENAME_MAX];
		strcpy_s(this->m_logFilePath, FILENAME_MAX, "default.log");
		this->m_tsFormat = new char[256];
		strcpy_s(this->m_tsFormat, 256, "%d.%m.%Y %H:%M:%S> ");
	}

	Logger::~Logger()
	{
		this->release();

		SAFE_ARRAY_DELETE(this->m_logFilePath);
		SAFE_ARRAY_DELETE(this->m_tsFormat);
	}

	void Logger::release()
	{
		if(this->m_logFile!=NULL)
		{
			fclose(this->m_logFile);
			this->m_logFile=NULL;
		}
	}


	//see logger.h for formatting options
	bool Logger::setTimeStampFormat(const char* fmt)
	{
		if (strcpy_s(this->m_tsFormat, 256, fmt) == 0)
			return true;

		return false;
	}

	const bool Logger::logTypeToStr(const LOG_TYPE logType, char* dest, const unsigned int dstsize, const char* fmt, const va_list args)
	{

	char* logmsg = new char[2*LOGMSG_LENGTH];

	vsprintf_s(logmsg, 2*LOGMSG_LENGTH, fmt, args);

	switch (logType)
		{
			case LT_STATUS:
				sprintf_s(dest, dstsize, "Current state:\t\t%s", logmsg);
				SAFE_ARRAY_DELETE(logmsg);
				return true;

			case LT_WARNING:
				sprintf_s(dest, dstsize, "Warning:\t\t\t%s", logmsg);
				SAFE_ARRAY_DELETE(logmsg);
				return true;

			case LT_ERROR:
				sprintf_s(dest, dstsize, "Error:\t\t\t%s", logmsg);
				SAFE_ARRAY_DELETE(logmsg);
				return true;

			case LT_CRITICAL:
				sprintf_s(dest, dstsize, "Critical error:\t\t%s", logmsg);
				SAFE_ARRAY_DELETE(logmsg);
				return true;

			case LT_MISC:
				sprintf_s(dest, dstsize, "Misc:\t\t\t%s", logmsg);
				SAFE_ARRAY_DELETE(logmsg);
				return true;

			case LT_SUPPRESS:
				sprintf_s(dest, dstsize, "%s", logmsg);
				SAFE_ARRAY_DELETE(logmsg);
				return false;

			default:
				sprintf_s(dest, dstsize, "Uncategorized:\t\t%s", logmsg);
				SAFE_ARRAY_DELETE(logmsg);
				return true;
		}
	}

	void Logger::setLoggingMode(const LOG_MODE lm)
	{
		this->m_logMode = lm;
	}

	void Logger::setTimeStamp(const bool enabled)
	{
		this->m_timestamp = enabled;
	}

	void Logger::logMsg(const LOG_TYPE logType, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		char* message = new char[LOGMSG_LENGTH];

		time_t rawtime;
		struct tm timeinfo;
		char timestring[100];

		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);
		strftime (timestring,80,this->m_tsFormat,&timeinfo);


		if (m_logMode == LM_FILE || m_logMode == LM_BOTH)
		{
			if (this->m_logFile == NULL) //this is nearly impossible!
			{
				fopen_s(&this->m_logFile, this->m_logFilePath, "a");

				if (this->m_logFile != NULL) //did it work ?
				{
					if (this->logTypeToStr(logType, message, LOGMSG_LENGTH, fmt, args))
					{
						if (this->m_timestamp)
						{
							fprintf_s(this->m_logFile, "\n%s%s", timestring, message);
							fflush(this->m_logFile);
						}
						else 
						{
							fprintf_s(this->m_logFile, "\n%s", message);
							fflush(this->m_logFile);
						}
					}
				}
				else						//it didn't...
				{
					if (this->logTypeToStr(logType, message, LOGMSG_LENGTH, fmt, args))
						printf_s("\n%s%s", timestring, message);
				}
			}
			else  // this should be the normal case for file or both:
			{
				if (this->logTypeToStr(logType, message, LOGMSG_LENGTH, fmt, args))
				{
					if (this->m_timestamp)
					{
						fprintf_s(this->m_logFile, "\n%s%s", timestring, message);
						fflush(this->m_logFile);
					}
					else
					{
						fprintf_s(this->m_logFile, "\n%s", message);
						fflush(this->m_logFile);
					}
				}
			}

		}
		if (m_logMode == LM_STDOUT || m_logMode == LM_BOTH)
		{
			if (this->logTypeToStr(logType, message, LOGMSG_LENGTH, fmt, args))
			{
				if (this->m_timestamp)
				{
					printf_s("\n%s%s", timestring, message);
				}
				else
				{
					printf_s("\n%s", message);
				}
			}
		}

		va_end(args);
		SAFE_ARRAY_DELETE(message);
	}

	void Logger::setLogFilePath(const char* path)
	{
			strcpy_s(this->m_logFilePath, FILENAME_MAX, path);

			if(this->m_logFile)
				fclose(this->m_logFile);

			fopen_s(&this->m_logFile, this->m_logFilePath, "w");
	}

}
