#pragma once
#ifndef _CLEARSKY_SOUND_BASE_H_
#define _CLEARSKY_SOUND_BASE_H_

#include "soundsystem\XAudioIncludes.h"

#include "soundsystem\AudioBase.h"

namespace clearsky
{
	// Specify LFE level distance curve such that it rolls off much sooner than
	// all non-LFE channels, making use of the subwoofer more dramatic.
	static const X3DAUDIO_DISTANCE_CURVE_POINT m_emitter_LFE_CurvePoints[3] = { 0.0f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f };
	static const X3DAUDIO_DISTANCE_CURVE       m_emitter_LFE_Curve          = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&m_emitter_LFE_CurvePoints[0], 3 };

	// Specify reverb send level distance curve such that reverb send increases
	// slightly with distance before rolling off to silence.
	// With the direct channels being increasingly attenuated with distance,
	// this has the effect of increasing the reverb-to-direct sound ratio,
	// reinforcing the perception of distance.
	static const X3DAUDIO_DISTANCE_CURVE_POINT m_emitter_Reverb_CurvePoints[3] = { 0.0f, 0.5f, 0.75f, 1.0f, 1.0f, 0.0f };
	static const X3DAUDIO_DISTANCE_CURVE       m_emitter_Reverb_Curve          = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&m_emitter_Reverb_CurvePoints[0], 3 };

	class CLEARSKY_API Sound
	{
	public:
		Sound(AudioBase* initClass);
		virtual ~Sound(void);

		/*
		 * Const methods:
		 **********************/

		const bool isPlaying() const;
		void getVolume(float &fltVolume) const;
		void getFrequencyRatio(float& fltFreqRatio) const;
		const bool isRunning() const;
		const bool loop() const;
		const locationStringType getLoc() const;

		/*
		 * Control methods:
		 *
		 **************************/

		const bool play();
		void pause();
		void stop();

		//TODO: Implement this:
		void reload();
 
		// Sound3D::calculate overrides this with the doppler effect!
		void setFrequencyRatio(const float fltFreqRatio);
		void setLoop(const bool enabled = true);
		void alterVolume(const float fltVolume);
		void setLoc(const locationStringType loc);

		void setSourceVoice(IXAudio2SourceVoice* const newVoice);
		virtual const bool createSourceVoice(WAVEFORMATEX wfm);
		
		IXAudio2* const getIXAudio2();
		const IXAudio2MasteringVoice* const getMasteringVoice();
		IXAudio2SourceVoice* const getSourceVoice();

	private:
		Sound(const Sound&);
		Sound& operator=(const Sound&);

		locationStringType m_loc;

	protected:
		AudioBase* m_initClass;
		IXAudio2* m_XAudio2;
		IXAudio2MasteringVoice* m_masteringVoice;
		IXAudio2SourceVoice* m_sourceVoice;

		bool m_isRunning;
		bool m_isPaused;
		bool m_loop;

		virtual void resetParams();
	};

	class CLEARSKY_API Sound3D : public Sound
	{
	public:
		Sound3D(Audio3D* initClass, const short nInputChannels = 1);
		~Sound3D();

		void calculate(const float fElapsedTime);
		void setEmitterPos(const D3DXVECTOR3& emitterPos);

		const bool createSourceVoice(WAVEFORMATEX wfm);
		const bool setEmitterEcho(const bool enabled, const float feedback = 0.5, const float wdQuote = 0.6, const float delay = 300);
		const bool calculateEmitterEcho(const float echoDistance, const float emitterDistance, const float speedOfSound = X3DAUDIO_SPEED_OF_SOUND);
		void setVolumeCurve(X3DAUDIO_DISTANCE_CURVE* const VolumeCurve);
		void setDopplerScaler(const float dpScale);

	protected:

		virtual void resetParams();

		Audio3D* m_initClass;

		X3DAUDIO_DSP_SETTINGS m_dspSettings;
		X3DAUDIO_EMITTER m_emitter;
		X3DAUDIO_CONE m_emitterCone;
		X3DAUDIO_DISTANCE_CURVE m_distCurv;

		int m_frameToApplyAudio3D;
		short m_nInputChannels;

		D3DXVECTOR3 m_emitterPos;
		FLOAT32* m_emitterAzimuths;
		FLOAT32* m_matrixCoefficients;

		IXAudio2SubmixVoice* m_submixVoice;
	};

} //namespace clearsky


#endif