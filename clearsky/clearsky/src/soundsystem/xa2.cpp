#include "sound.h"

namespace clearsky
{

	Sound::Sound():m_dsDevice(NULL),
					m_deviceCount(0)
	{

		for (int i = 0; i < MAX_DS_DEVICES; i++)
		{
			this->m_deviceList[i].strDesc = (LPSTR) new TCHAR[MAX_DS_DEVICE_STRING_LENGTH];
			this->m_deviceList[i].strDrvName = (LPSTR) new TCHAR[MAX_DS_DEVICE_STRING_LENGTH];
		}
		
	}

	Sound::~Sound()
	{
		SAFE_RELEASE(this->m_dsDevice);
		for (int i = 0; i < MAX_DS_DEVICES; i++)
		{
			if (this->m_deviceList[i].strDesc)
			{
				delete this->m_deviceList[i].strDesc;
				this->m_deviceList[i].strDesc = NULL;
			}
			if (this->m_deviceList[i].strDrvName)
			{
				delete this->m_deviceList[i].strDrvName;
				this->m_deviceList[i].strDrvName = NULL;
			}
		}
	}

	unsigned int Sound::getDeviceCount()
	{
		return this->m_deviceCount;
	}

	BOOL CALLBACK Sound::DSCallback(GUID* pGUID, LPSTR strDesc,
								LPSTR strDrvName, VOID* pContext)
	{
		Sound* me = (Sound*) pContext;

		//TCHAR buffer[256] = {0};
		//wsprintf(buffer, L"%s", strDrvName, strDesc);

		if (me->m_deviceCount < MAX_DS_DEVICES)
		{
			me->m_deviceCount++;

			wsprintf((LPWSTR)me->m_deviceList[me->m_deviceCount-1].strDesc, L"%s", strDesc);
			wsprintf((LPWSTR)me->m_deviceList[me->m_deviceCount-1].strDrvName, L"%s", strDrvName);
			//strcpy_s(me->m_deviceList[me->m_deviceCount-1].strDesc, MAX_DS_DEVICE_STRING_LENGTH, strDesc);
			//strcpy_s(me->m_deviceList[me->m_deviceCount-1].strDrvName, MAX_DS_DEVICE_STRING_LENGTH, strDrvName);
			me->m_deviceList[me->m_deviceCount-1].pGUID = pGUID;
		}
		if (me->m_deviceCount < MAX_DS_DEVICES)
			return true;
			//only for debug use (function will not be called more than once):
			//return false;
		else 
			return false;
	}

	RETURN_VALUE Sound::getDeviceList(Sound* SoundClassHandle)
	{
		m_deviceCount = 0;

		for (int i = 0; i < MAX_DS_DEVICES; i++)
		{
			if (this->m_deviceList[i].strDesc == NULL)
			{
				this->m_deviceList[i].strDesc = new char[MAX_DS_DEVICE_STRING_LENGTH];
			}
			if (this->m_deviceList[i].strDrvName == NULL)
			{
				this->m_deviceList[i].strDrvName = new char[MAX_DS_DEVICE_STRING_LENGTH];
			}
		}

		HRESULT hr = DirectSoundEnumerate( (LPDSENUMCALLBACK) Sound::DSCallback, (void*) SoundClassHandle);

		if FAILED (hr)
		{
			m_lastError = hr;
			return RETURN_ERROR;
		}
		return RETURN_OK;
	}

	dsDevice* Sound::getDsDevice(unsigned int index)
	{
		if (index < (this->getDeviceCount()))
			return &this->m_deviceList[index];
		else 
			return NULL;
	}

	RETURN_VALUE Sound::setSoundDevice(unsigned int index, HWND hwnd, DWORD coLevel)
	{
		if (index < this->m_deviceCount)
		{
			if (this->m_dsDevice)
			{
				this->m_dsDevice->Release();
			}
			HRESULT hr = DirectSoundCreate8(this->getDsDevice(index)->pGUID, &this->m_dsDevice, NULL);

			if FAILED(hr)
			{
				this->m_lastError = hr;
				return RETURN_ERROR;
			}
			hr = this->m_dsDevice->SetCooperativeLevel(hwnd, coLevel);

			if FAILED(hr)
			{
				this->m_lastError = hr;
				return RETURN_ERROR;
			}

			return RETURN_OK;
						
		}
		else 
		{
			this->m_lastError = ERR_INDEX_OUT_OF_RANGE;
			return RETURN_ERROR;
		}
	}

	HRESULT Sound::getLastError()
	{
		return this->m_lastError;
	}

}