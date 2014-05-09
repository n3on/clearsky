#ifndef _CLEARSKY_AUDIO_BASE_H_
#define _CLEARSKY_AUDIO_BASE_H_

#include "soundsystem\XAudioIncludes.h"

namespace clearsky
{

	class CLEARSKY_API AudioBase 
	{
	public:
		AudioBase(void);
		virtual ~AudioBase(void);

	/*
		Creates audio device:
		*****************************/

		virtual bool initialiseAudio();

	/*
		Set master volume to fltVolume:
		*****************************/
		void alterVolume(const float fltVolume);

	/*
		Retrieves current master 
		volume level:
		*****************************/
		void getVolume(float& fltVolume) const;

		IXAudio2* m_XAudio2;
		IXAudio2MasteringVoice* m_masteringVoice;
	};


	class CLEARSKY_API Audio3D : public AudioBase
	{
	public:
		Audio3D(void); 
		~Audio3D(void); 

		/*
		Update current position of 
		the listener:
		*****************************/
		void setListenerPosition(const D3DXVECTOR3& listenerPos);

		/*
		Update direction the listener 
		is looking towards:
		*****************************/
		void setOrientFront(const D3DXVECTOR3& orientFront);

		/*
		Use inner sound radius with
		higher volume
		*****************************/
		void setUseInnerRadius(bool useIt = true);

		/*
		Change listener cone:
		*****************************/
		void setListenerCone(const X3DAUDIO_CONE& listenerCone);

		/*
		Change reverb (0 to 29)
		*****************************/
		HRESULT setReverb( int nReverb );

		/*
		Change curve distance scaler.
		Should be roughly set to the longest
		map dimension's length.
		*****************************/

		void setCurveDistanceScaler(float cds);

		/*
		Creates 3d audio device:
		*****************************/
		bool initialiseAudio();

		bool useListenerCone() const;
		bool useInnerRadius() const;
		bool isInitialized() const;
		bool useRedirectToLFE() const;
		const float getCurveDistanceScaler() const;
		const unsigned int getChannels() const;
		const X3DAUDIO_VECTOR& getListenerPosition() const;
		IXAudio2SubmixVoice* getSubmixVoice();


		X3DAUDIO_HANDLE m_x3DInstance;

		//	m_listener.position retrieves
		//	current position!!
		X3DAUDIO_LISTENER m_listener;

		//new position to be used for calculate:
		D3DXVECTOR3 m_listenerPos;

		IXAudio2SubmixVoice* m_submixVoice;

	private:
		bool  m_useRedirectToLFE;
		bool  m_initialized;
		bool  m_useInnerRadius;
		float m_curveDistanceScaler;
		DWORD m_channelMask;
		unsigned int m_channels;
		IUnknown* m_reverbEffect;

		X3DAUDIO_CONE m_Cone;
		

	};

}

#endif