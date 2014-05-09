#ifndef _CLEARSKY_SOUND_LOADER_H_
#define _CLEARSKY_SOUND_LOADER_H_


#include "soundsystem\XAudioIncludes.h"

#include "soundsystem\AudioBase.h"
#include "soundsystem\SoundBase.h"

#include "soundsystem\codec.h"
#include "soundsystem\vorbisfile.h"
#include "soundsystem\SDKwavefile.h"

#pragma comment(lib, "libogg.lib")
#pragma comment(lib, "libvorbis.lib")
#pragma comment(lib, "libvorbisfile.lib")

namespace clearsky
{

	class CLEARSKY_API SoundLoader
	{
	public:
		SoundLoader();
		virtual ~SoundLoader();

		/*
		Change the Sound that should
		be filled with sound data:
		*****************************/
		virtual void setTargetSound(Sound* const targetSound);

		virtual const bool loadStaticSound(const locationStringType szSoundFileName) = 0;

		virtual const bool submitStaticSound() = 0;

		virtual const unsigned int bufferSize() const = 0;

	protected:
		bool m_fileOpened;
		Sound* m_Sound;
		IXAudio2* m_XAudio2;
		const IXAudio2MasteringVoice* m_masteringVoice;
		IXAudio2SourceVoice* m_sourceVoice;
	};


	/*****************************

	OggVorbisLoader class, derived from 
	SoundLoader

	Usage:

	- create new AudioBase
	- initialiseAudio 
	- create new Sound
	- create an ogg vorbis loader
	- ovl->setTargetSound(Sound/3D);
	- ovl->loadSound(path)
	- only 3d:
	sound3d->setEmitterPos(D3DXVECTOR3(0,0,0));
	- sound->play();
	- call update() from time to time

	Proper destroy:

	- stop Sound
	- delete ovl (or set new target sound)
	- delete Sound (or fill it with other data)
	- delete Audio/3D (if not needed by other sounds)

	*********************************/


	class CLEARSKY_API OggVorbisLoader : public SoundLoader
	{
	public:
		OggVorbisLoader();
		virtual ~OggVorbisLoader(void);

		/*
		Load ogg sound, buffered or complete:
		***************************************/
		const bool loadSound(const char* szSoundFilePath);

		/*
		Static sound, beta
		*****************************/
		const bool loadStaticSound(const locationStringType szSoundFileName);

		/*
		Submit static sound
		*****************************/
		const bool submitStaticSound();

		/*
		Update sound buffer, buffered sounds only:
		********************************************/
		void update();

		/*
		Retrieve static sound data for SoundPoolManager
		********************************************/
		const BYTE* const getWaveData() const;

		/*
		Returns current mem usage
		********************************************/
		const unsigned int bufferSize() const;

	private:
		static const unsigned int m_streamingBufferSize = 65536;//*10
		static const unsigned int m_maxBufferCount = 3;

	protected:
		bool m_almostDone;
		bool m_done;
		DWORD m_currentDiskReadBuffer;
		OggVorbis_File m_vf;
		WAVEFORMATEX wfm;

		unsigned int sz;
		char m_buffers[m_maxBufferCount][m_streamingBufferSize];
		char* m_staticBuffer;
		bool m_fileOpened;

		void resetParams();
	};

	class CLEARSKY_API WaveLoader : public SoundLoader
	{
	public:
		WaveLoader();
		virtual ~WaveLoader(void);

		const bool loadStaticSound(const locationStringType szSoundFilePath);

		const bool submitStaticSound();

		/*
		Retrieve static sound data for SoundPoolManager
		********************************************/
		const BYTE* getWaveData() const;

		/*
		Returns sound buffer size
		********************************************/
		const unsigned int bufferSize() const;

	protected:
		BYTE* m_waveData;
		bool m_fileOpened;
		WAVEFORMATEX pwfx;
		DWORD cbWaveSize;

		void resetParams();
	};

}

#endif