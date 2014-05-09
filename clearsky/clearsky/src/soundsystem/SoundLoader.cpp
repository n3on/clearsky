#include "core/precompiled.h"
#include "soundsystem\SoundLoader.h"

namespace clearsky
{

	SoundLoader::SoundLoader() : m_fileOpened(false)
	{
		this->m_Sound = NULL;
	}

	SoundLoader::~SoundLoader()
	{

	}

	void SoundLoader::setTargetSound(Sound* const targetSound)
	{
		this->m_Sound = targetSound;
		this->m_masteringVoice = targetSound->getMasteringVoice();
		this->m_sourceVoice = targetSound->getSourceVoice();
	}


	/*****************************

	OggVorbisLoader class, derived from 
	SoundLoader

	*********************************/

	OggVorbisLoader::OggVorbisLoader() : SoundLoader()

	{
		resetParams();
		this->m_staticBuffer = NULL;
	}

	OggVorbisLoader::~OggVorbisLoader(void)
	{
		if (m_fileOpened)
			ov_clear(&m_vf);

		SAFE_ARRAY_DELETE(m_staticBuffer);
	}

	void OggVorbisLoader::resetParams()
	{
		m_fileOpened = false;
		m_done = false;
		m_almostDone = false;
		m_currentDiskReadBuffer = 0;
		sz = 0;
	}


	const bool OggVorbisLoader::loadSound(const char* szSoundFileName)
	{
		//WCHAR wstrSoundPath[MAX_PATH];
		//char strSoundPath[MAX_PATH];

		/*Get the our applications "sounds" directory.
		GetCurrentDirectory(MAX_PATH, (LPWSTR)strSoundPath);
		strcat_s(strSoundPath, MAX_PATH, "\\");
		strcat_s(strSoundPath, MAX_PATH, szSoundFileName);
		*/

		//Convert the path to unicode.
		//MultiByteToWideChar(CP_ACP, 0, strSoundPath, -1, wstrSoundPath, MAX_PATH);

		//If we already have a file open then kill the current voice setup
		if (m_fileOpened)
		{

			ov_clear(&m_vf);

			resetParams();
		}


		FILE *f;
		errno_t err;

		if ((err  = fopen_s( &f, szSoundFileName, "rb" )) != 0)
		{
			LOG->logMsg(LT_ERROR, "Failed to open audio (does file exist?): %s", szSoundFileName);

			char szBuffer[MAX_PATH];
			_strerror_s(szBuffer, MAX_PATH, NULL);
			LOG->logMsg(LT_ERROR, "\tReason: %s", szBuffer);
			return false;
		}


		//ov_open(f, &m_vf, NULL, 0);	//Windows does not like this function so we use ov_open_callbacks() instead
		if (ov_open_callbacks(f, &m_vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
		{
			fclose(f);
			return false;
		}
		else
			m_fileOpened = true;

		//The vorbis_info struct keeps the most of the interesting format info
		vorbis_info *vi = ov_info(&m_vf, -1);

		//Set the wave format
		WAVEFORMATEX wfm;
		memset(&wfm, 0, sizeof(wfm));

		wfm.cbSize          = sizeof(wfm);
		wfm.nChannels       = (WORD)vi->channels;
		wfm.wBitsPerSample  = 16;                    //Ogg vorbis is always 16 bit
		wfm.nSamplesPerSec  = vi->rate;
		wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nChannels * 2;
		wfm.nBlockAlign     = 2 * wfm.nChannels;
		wfm.wFormatTag      = 1;


		DWORD pos = 0;
		int sec = 0;
		int ret = 1;

		memset(&m_buffers[m_currentDiskReadBuffer], 0, sizeof(m_buffers[m_currentDiskReadBuffer]));

		//Read in the bits
		while(ret && pos<this->m_streamingBufferSize)
		{
			ret = ov_read(&m_vf, m_buffers[m_currentDiskReadBuffer]+pos, this->m_streamingBufferSize-pos, 0, 2, 1, &sec);
			pos += ret;
		}

		HRESULT hr;

		if (!m_Sound->createSourceVoice(wfm))
		{
			LOG->logMsg(clearsky::LT_ERROR, "Source voice was not created!");
			return false;
		}

		this->m_sourceVoice = m_Sound->getSourceVoice();

		XAUDIO2_BUFFER buffer = {0};
		buffer.pAudioData = (BYTE*)&m_buffers[m_currentDiskReadBuffer];
		buffer.AudioBytes = this->m_streamingBufferSize;

		if(FAILED(hr = m_sourceVoice->SubmitSourceBuffer(&buffer)))
		{
			LOG->logMsg(LT_ERROR, "Error %#X submitting source buffer", hr);
			return false;
		}

		m_currentDiskReadBuffer++;

		return true;
	}

	const bool OggVorbisLoader::loadStaticSound(const locationStringType szSoundFileName)
	{
		//WCHAR wstrSoundPath[MAX_PATH];
		//char strSoundPath[MAX_PATH];

		/*Get the our applications "sounds" directory.
		GetCurrentDirectory(MAX_PATH, (LPWSTR)strSoundPath);
		strcat_s(strSoundPath, MAX_PATH, "\\");
		strcat_s(strSoundPath, MAX_PATH, szSoundFileName);
		*/

		//Convert the path to unicode.
		//MultiByteToWideChar(CP_ACP, 0, strSoundPath, -1, wstrSoundPath, MAX_PATH);

		//If we already have a file open then kill the current voice setup
		/*
		int len =  wcslen(szSoundFileName.c_str());
		char *szTo = new char[len + 1];
		szTo[len] = '\0';
		WideCharToMultiByte(CP_ACP, 0, szSoundFileName, -1, szTo, len, NULL, NULL);
		*/

		if (m_fileOpened)
		{

			ov_clear(&m_vf);

			resetParams();
		}


		FILE *f;
		errno_t err;

		if ((err  = _wfopen_s( &f, szSoundFileName.c_str(), L"rb" )) != 0)
		{
			LOG->logMsg(LT_ERROR, "Failed to open audio: %s", szSoundFileName);

			char szBuffer[MAX_PATH];
			_strerror_s(szBuffer, MAX_PATH, NULL);
			LOG->logMsg(LT_WARNING, "\tReason: %s", szBuffer);
			return false;
		}

		//ov_open(f, &m_vf, NULL, 0);	//Windows does not like this function so we use ov_open_callbacks() instead
		if (ov_open_callbacks(f, &m_vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
		{
			fclose(f);
			return false;
		}
		else
			m_fileOpened = true;

		//The vorbis_info struct keeps the most of the interesting format info
		vorbis_info *vi = ov_info(&m_vf, -1);

		//Set the wave format
		memset(&wfm, 0, sizeof(wfm));

		wfm.cbSize          = sizeof(wfm);
		wfm.nChannels       = (WORD)vi->channels;
		wfm.wBitsPerSample  = 16;                    //Ogg vorbis is always 16 bit
		wfm.nSamplesPerSec  = vi->rate;
		wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nChannels * 2;
		wfm.nBlockAlign     = 2 * wfm.nChannels;
		wfm.wFormatTag      = 1;


		unsigned long pos = 0;
		int sec = 0;
		int ret = 1;

		unsigned int samples = (unsigned int) ov_pcm_total(&m_vf, -1);
		sz = 2 * wfm.nChannels * samples;

		SAFE_ARRAY_DELETE(m_staticBuffer);

		m_staticBuffer = new char[sz];

		memset(m_staticBuffer, 0, sz);

		//LOG->logMsg(clearsky::LT_STATUS, "Loading static ogg...");
		//Read in the bits
		while(ret && pos<sz)
		{
			ret = ov_read(&m_vf, m_staticBuffer+pos, 4096, 0, 2, 1, &sec);
			pos += ret;
		}
		//LOG->logMsg(clearsky::LT_STATUS, "Ogg file was loaded.");

		return true;
	}

	const bool OggVorbisLoader::submitStaticSound()
	{
		HRESULT hr;

		if (!m_Sound->createSourceVoice(wfm))
		{
			LOG->logMsg(clearsky::LT_ERROR, "Source voice was not created!");
			return false;
		}

		this->m_sourceVoice = m_Sound->getSourceVoice();

		XAUDIO2_BUFFER buffer = {0};
		buffer.pAudioData = (BYTE*)m_staticBuffer;
		buffer.AudioBytes = sz;
		buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer

		if (m_Sound->loop())
		{
			buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
		}

		if(FAILED(hr = m_sourceVoice->SubmitSourceBuffer(&buffer)))
		{
			LOG->logMsg(LT_ERROR, "Error %#X submitting source buffer", hr);
			return false;
		}

		return true;
	}

	void OggVorbisLoader::update()
	{
		if (m_sourceVoice == NULL)
			return;

		if (!m_Sound->isRunning())
			return;

		//Do we have any free buffers?
		XAUDIO2_VOICE_STATE state;
		m_sourceVoice->GetState( &state );
		if ( state.BuffersQueued < this->m_maxBufferCount - 1 )
		{
			if (m_done && !m_Sound->loop())
			{
				m_sourceVoice->Stop(0);
			}

			//Got to use this trick because otherwise all the bits wont play
			if (m_almostDone && !m_Sound->loop())
				m_done = true;

			memset(&m_buffers[m_currentDiskReadBuffer], 0, sizeof(m_buffers[m_currentDiskReadBuffer]));

			DWORD pos = 0;
			int sec = 0;
			int ret = 1;

			while(ret && pos<this->m_streamingBufferSize)
			{
				ret = ov_read(&m_vf, m_buffers[m_currentDiskReadBuffer]+pos, this->m_streamingBufferSize-pos, 0, 2, 1, &sec);
				pos += ret;
			}

			//Reached the end?
			if (!ret && m_Sound->loop())
			{
				//We are looping so restart from the beginning
				//NOTE: sound with sizes smaller than BUFSIZE may be cut off

				ret = 1;
				ov_pcm_seek(&m_vf, 0);
				while(ret && pos<this->m_streamingBufferSize)
				{
					ret = ov_read(&m_vf, m_buffers[m_currentDiskReadBuffer]+pos, this->m_streamingBufferSize-pos, 0, 2, 1, &sec);
					pos += ret;
				}
			}
			else if (!ret && !(m_Sound->loop()))
			{
				//Not looping so fill the rest with 0
				//while(pos<size)
				//    *(buffers[currentDiskReadBuffer]+pos)=0; pos ++;

				//And say that after the current section no other section follows
				m_almostDone = true;
			}

			XAUDIO2_BUFFER buffer = {0};
			buffer.pAudioData = (BYTE*)&m_buffers[m_currentDiskReadBuffer];
			if (m_almostDone)
				buffer.Flags = XAUDIO2_END_OF_STREAM;	//Tell the source voice not to expect any data after this buffer
			buffer.AudioBytes = this->m_streamingBufferSize;

			HRESULT hr;
			if( FAILED(hr = m_sourceVoice->SubmitSourceBuffer( &buffer ) ) )
			{
				LOG->logMsg(LT_ERROR, "Error %#X submitting source buffer", hr );
				return;
			}

			m_currentDiskReadBuffer++;
			m_currentDiskReadBuffer %= this->m_maxBufferCount;
		}
	}

	const BYTE* const OggVorbisLoader::getWaveData() const
	{
		return (BYTE*)this->m_staticBuffer;
	}

	const unsigned int OggVorbisLoader::bufferSize() const
	{
		return sz;
	}


	/*****************************

	WaveLoader class, derived from 
	SoundLoader

	*********************************/

	WaveLoader::WaveLoader():SoundLoader()
	{
		m_waveData = NULL;
		resetParams();
		//LOG->logMsg(LT_STATUS, "WaveLoader class was constructed.");
	}

	WaveLoader::~WaveLoader(void)
	{
	
		SAFE_ARRAY_DELETE( m_waveData );

		//LOG->logMsg(LT_STATUS, "WaveLoader class was destroyed.");
	}

	void WaveLoader::resetParams()
	{
		m_fileOpened = false;
	}


	const bool WaveLoader::loadStaticSound(const locationStringType szSoundFileName)
	{
		if (m_fileOpened)
		{
			resetParams();
			SAFE_ARRAY_DELETE( m_waveData );
		}
		HRESULT hr = S_OK;
		//
		// Read in the wave file
		//
		CWaveFile wav;
		std::vector<wchar_t> vec(szSoundFileName.begin(), szSoundFileName.end());
		vec.push_back(L'\0');

		// Extract wchar_t*
		LPWSTR c_string = &vec[0];
		if( FAILED( hr = wav.Open(c_string, NULL, WAVEFILE_READ ) ) )
		{
			LOG->logMsg(LT_ERROR, "Failed reading wave file: %#X", hr);
			return false;
		}

		// Get format of wave file
		pwfx = *wav.GetFormat();

		// Calculate how many bytes and samples are in the wave
		cbWaveSize = wav.GetSize();

		// Read the sample data into memory

		m_waveData = new BYTE[ cbWaveSize ];

		if( FAILED( hr = wav.Read( m_waveData, cbWaveSize, &cbWaveSize ) ) )
		{
			LOG->logMsg(LT_ERROR, "Failed to read wave data: %#X", hr);
			SAFE_ARRAY_DELETE( m_waveData );
			return false;
		}

		return true;
	}

	const bool WaveLoader::submitStaticSound()
	{
	//
		// Play the wave using a XAudio2SourceVoice
		//

		if (!m_Sound->createSourceVoice(pwfx))
		{
			LOG->logMsg(clearsky::LT_ERROR, "Source voice was not created!");
			SAFE_ARRAY_DELETE( m_waveData );
			return false;
		}

		this->m_sourceVoice = m_Sound->getSourceVoice();

		// Submit the wave sample data using an XAUDIO2_BUFFER structure
		XAUDIO2_BUFFER buffer = {0};
		buffer.pAudioData = m_waveData;
		buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
		buffer.AudioBytes = cbWaveSize;
		if (m_Sound->loop())
		{
			buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
		}

		HRESULT hr;

		if( FAILED( hr = m_sourceVoice->SubmitSourceBuffer( &buffer ) ) )
		{
			LOG->logMsg(LT_ERROR, "Error submitting source buffer: %#X", hr);
			m_sourceVoice->DestroyVoice();
			SAFE_ARRAY_DELETE( m_waveData );
			return false;
		}

		m_fileOpened = true;

		return true;
	}
	
	const BYTE* WaveLoader::getWaveData() const
	{
		return this->m_waveData;
	}

	const unsigned int WaveLoader::bufferSize() const
	{
		return this->cbWaveSize;
	}

} //namespace clearsky