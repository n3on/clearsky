
#ifndef _SOUND_H_
#define _SOUND_H_

#include <DSound.h>
#include <stdio.h> // printf debugging

#include "core\utils.h"

namespace clearsky
{

	struct CLEARSKY_API dsDevice
	{
		LPSTR strDesc;
		LPSTR strDrvName;
		GUID* pGUID;
	};

	class CLEARSKY_API Sound
	{

		public:
			Sound();
			~Sound();

			
			HRESULT getLastError();
			RETURN_VALUE getDeviceList(Sound* SoundClassHandle);
			unsigned int getDeviceCount();
			dsDevice* getDsDevice(unsigned int index);

			RETURN_VALUE setSoundDevice(unsigned int index, HWND hwnd, DWORD coLevel = DSSCL_PRIORITY);
			RETURN_VALUE setSoundDeviceEx(GUID* pGUID, HWND hwnd, DWORD coLevel = DSSCL_PRIORITY);

		private:

			static BOOL CALLBACK DSCallback(GUID* pGUID, LPSTR strDesc,
										LPSTR strDrvName, VOID* pContext);

			LPDIRECTSOUND8 m_dsDevice;
			HRESULT m_lastError;

			dsDevice m_deviceList[MAX_DS_DEVICES];

			unsigned int m_deviceCount;
	};

}

#endif