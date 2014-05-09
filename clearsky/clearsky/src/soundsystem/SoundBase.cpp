#include "core/precompiled.h"
#include "soundsystem\SoundBase.h"


namespace clearsky
{



	/*****************************

	Sound class implementation
	- requires instance of AudioBase
	- base class for Sound3D class

	*********************************/

	Sound::Sound(clearsky::AudioBase *initClass):m_loop(false)
	{
		m_sourceVoice = NULL;
		if (!initClass)
		{
			LOG->logMsg(LT_ERROR, "Error code: IP1000");
			return;
		}
		this->m_XAudio2 = initClass->m_XAudio2;
		this->m_masteringVoice = initClass->m_masteringVoice;

		//LOG->logMsg(LT_STATUS, "Sound class was constructed (correctly).");
	}

	Sound::~Sound(void)
	{
		if (m_sourceVoice != NULL)
		{
			m_sourceVoice->Stop(0);
			m_sourceVoice->DestroyVoice();
		}	
	}

	void Sound::resetParams()
	{
		m_isRunning = false;
		m_isPaused = false;
		m_loop = false;
	}


	const bool Sound::play()
	{
		if (m_sourceVoice == NULL)
		{
			LOG->logMsg(LT_ERROR, "Error: m_sourceVoice NOT created");
			LOG->logMsg(LT_STATUS, "Did you forget to createSourceVoice or loadSound ?");
			return false;
		}

		HRESULT hr;
		
		if(FAILED(hr = m_sourceVoice->Start(0)))
		{
			LOG->logMsg(LT_ERROR, "Error %#X submitting source buffer.", hr);
		}

		XAUDIO2_VOICE_STATE state;
		m_sourceVoice->GetState(&state);
		m_isRunning = (state.BuffersQueued > 0) != 0;
		m_isPaused = false;

		return m_isRunning;
	}

	const locationStringType Sound::getLoc() const
	{
		return this->m_loc;
	}

	void Sound::setLoc(const locationStringType loc)
	{
		this->m_loc = loc;
	}

	void Sound::stop()
	{
		m_isPaused = false;
		m_isRunning = false;

		if (m_sourceVoice == NULL)
			return;

		m_sourceVoice->Stop(0);
		m_sourceVoice->FlushSourceBuffers();
	}

	const bool Sound::isPlaying()	const
	{
		/*XAUDIO2_VOICE_STATE state;
		m_sourceVoice->GetState(&state);
		return (state.BuffersQueued > 0) != 0;
		*/

		return m_isRunning;
	}

	void Sound::alterVolume(const float fltVolume)
	{
		if (m_sourceVoice == NULL)
			return;

		m_sourceVoice->SetVolume(fltVolume);
	}

	void Sound::getVolume(float &fltVolume) const
	{
		if (m_sourceVoice == NULL)
			return;

		m_sourceVoice->GetVolume(&fltVolume);
	}

	
	void Sound::setFrequencyRatio(const float fltFreqRatio)
	{
		if (m_sourceVoice == NULL)
			return;
		m_sourceVoice->SetFrequencyRatio(fltFreqRatio);
	}

	void Sound::getFrequencyRatio(float& fltFreqRatio)	const
	{
		if (m_sourceVoice == NULL)
		{
			fltFreqRatio = 0;
			return;
		}
		m_sourceVoice->GetFrequencyRatio(&fltFreqRatio);
	}

	void Sound::pause()
	{
		if (m_sourceVoice == NULL)
		{
			m_isPaused = false;
			return;
		}

		if (m_isPaused)
		{
			m_sourceVoice->Start(0);	//Unless we tell it otherwise the voice resumes playback from its last position
			m_isPaused = false;
		}
		else
		{
			m_sourceVoice->Stop(0);
			m_isPaused = true;
		}
	}

	const bool Sound::isRunning()	const
	{
		return m_isRunning;
	}

	const bool Sound::loop() const
	{
		return m_loop;
	}

	void Sound::setLoop(bool enabled)
	{
		this->m_loop = enabled;
	}

	IXAudio2* const Sound::getIXAudio2()
	{
		return this->m_XAudio2;
	}

	IXAudio2SourceVoice* const Sound::getSourceVoice()
	{
		return this->m_sourceVoice;
	}

	// Will deliver a pointer to m_masteringVoice, which cannot be 
	// used to change the m_masteringVoice object:
	const IXAudio2MasteringVoice* const Sound::getMasteringVoice()
	{
		return this->m_masteringVoice;
	}

	void Sound::setSourceVoice(IXAudio2SourceVoice* newVoice)
	{
		if (!newVoice)
		{
			LOG->logMsg(LT_ERROR, "Error code: IP1001");
			return;
		}
		this->m_sourceVoice = newVoice;
	}

	const bool Sound::createSourceVoice(WAVEFORMATEX wfm)
	{
		if (this->m_sourceVoice != NULL)
		{
			this->m_sourceVoice->Stop(0);
			this->m_sourceVoice->DestroyVoice();
		}

		HRESULT hr;

		XAUDIO2_SEND_DESCRIPTOR sendDescriptors[1];
		sendDescriptors[0].Flags = XAUDIO2_SEND_USEFILTER; // LPF direct-path
		sendDescriptors[0].pOutputVoice = m_masteringVoice;
		const XAUDIO2_VOICE_SENDS sendList = { 1, sendDescriptors };

		//Create the source voice
		if (FAILED(hr = this->m_XAudio2->CreateSourceVoice( &m_sourceVoice, &wfm, 0,
			2.0f, NULL, &sendList )))
		{
			LOG->logMsg(LT_ERROR, "Error %#X creating source voice", hr);
			return false;
		}

		return true;
	}

	/*****************************

	Sound3D class
	- derived from Sound class
	- requires an instance Audio3D

	********************************/

	Sound3D::Sound3D(Audio3D* initClass, const short nInputChannels):Sound(initClass)
	{
		if (!initClass)
		{
			LOG->logMsg(LT_ERROR, "Error code: IP1000");
			return;
		}
		
		this->m_initClass = initClass;
		
		m_nInputChannels = nInputChannels;

		m_matrixCoefficients = new float[m_nInputChannels * m_initClass->getChannels()];
		m_emitterAzimuths = new float[m_nInputChannels];

		resetParams();

		//LOG->logMsg(LT_STATUS, "Sound3D class was constructed.");
	}

	Sound3D::~Sound3D()
	{
		//LOG->logMsg(LT_STATUS, "Sound3D class was destroyed.");
		SAFE_DELETE(this->m_matrixCoefficients);
		SAFE_DELETE(this->m_emitterAzimuths);
	}

	void Sound3D::resetParams()
	{
		m_isRunning = false;
		m_isPaused = false;

		m_submixVoice = m_initClass->getSubmixVoice();
		m_emitter.CurveDistanceScaler = m_initClass->getCurveDistanceScaler();

		m_emitterPos = D3DXVECTOR3( 0, 0, 0 );

		m_emitter.pCone = &m_emitterCone;
		m_emitter.pCone->InnerAngle = 0.0f;

		// Setting the inner cone angles to X3DAUDIO_2PI and
		// outer cone other than 0 causes
		// the m_emitter to act like a point m_emitter using the
		// INNER cone settings only.
		m_emitter.pCone->OuterAngle = 0.0f;
		// Setting the outer cone angles to zero causes
		// the m_emitter to act like a point m_emitter using the
		// OUTER cone settings only.
		m_emitter.pCone->InnerVolume = 0.0f;
		m_emitter.pCone->OuterVolume = 1.0f;
		m_emitter.pCone->InnerLPF = 0.0f;
		m_emitter.pCone->OuterLPF = 1.0f;
		m_emitter.pCone->InnerReverb = 0.0f;
		m_emitter.pCone->OuterReverb = 1.0f;

		m_emitter.Position = m_emitterPos;
		m_emitter.OrientFront = D3DXVECTOR3( 0, 0, 1 );
		m_emitter.OrientTop = D3DXVECTOR3( 0, 1, 0 );
		m_emitter.ChannelCount = m_nInputChannels;
		m_emitter.ChannelRadius = 1.0f;
		m_emitter.pChannelAzimuths = m_emitterAzimuths;

		// Use of Inner radius allows for smoother transitions as
		// a sound travels directly through, above, or below the m_listener.
		// It also may be used to give elevation cues.
		m_emitter.InnerRadius = 2.0f;
		m_emitter.InnerRadiusAngle = X3DAUDIO_PI/4.0f;;

		m_emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
		m_emitter.pLFECurve    = (X3DAUDIO_DISTANCE_CURVE*)&m_emitter_LFE_Curve;
		m_emitter.pLPFDirectCurve = NULL; // use default curve
		m_emitter.pLPFReverbCurve = NULL; // use default curve
		m_emitter.pReverbCurve    = (X3DAUDIO_DISTANCE_CURVE*)&m_emitter_Reverb_Curve;

		m_emitter.DopplerScaler = 1.0f;

		m_dspSettings.SrcChannelCount = m_nInputChannels;
		m_dspSettings.DstChannelCount = m_initClass->getChannels();
		m_dspSettings.pMatrixCoefficients = m_matrixCoefficients;
	}

	void Sound3D::setDopplerScaler(const float dpScale)
	{
		m_emitter.DopplerScaler = dpScale;
	}


	void Sound3D::calculate(const float fElapsedTime)
	{
		if( m_frameToApplyAudio3D == 0 )
		{
			if (m_initClass->useInnerRadius())
			{
				m_emitter.InnerRadius = 2.0f;
				m_emitter.InnerRadiusAngle = X3DAUDIO_PI/4.0f;
			}
			else
			{
				m_emitter.InnerRadius = 0.0f;
				m_emitter.InnerRadiusAngle = 0.0f;
			}

			if( fElapsedTime > 0 )
			{
				D3DXVECTOR3 lVelocity = ( m_initClass->m_listenerPos - m_initClass->m_listener.Position ) / fElapsedTime;
				m_initClass->m_listener.Position = m_initClass->m_listenerPos;
				m_initClass->m_listener.Velocity = lVelocity;

				D3DXVECTOR3 eVelocity = ( m_emitterPos - m_emitter.Position ) / fElapsedTime;
				m_emitter.Position = m_emitterPos;
				m_emitter.Velocity = eVelocity;
			}

			DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
				| X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
				| X3DAUDIO_CALCULATE_REVERB;
			if (m_initClass->useRedirectToLFE())
			{
				// On devices with an LFE channel, allow the mono source data
				// to be routed to the LFE destination channel.
				dwCalcFlags |= X3DAUDIO_CALCULATE_REDIRECT_TO_LFE;
			}

			X3DAudioCalculate( m_initClass->m_x3DInstance, &m_initClass->m_listener, &m_emitter, dwCalcFlags,
				&m_dspSettings );

			IXAudio2SourceVoice* voice = this->getSourceVoice();
			if( voice )
			{

				// Apply XAudio3D generated DSP settings to XAudio2
				voice->SetFrequencyRatio( m_dspSettings.DopplerFactor );
				voice->SetOutputMatrix( m_initClass->m_masteringVoice, m_nInputChannels, m_initClass->getChannels(),
					m_matrixCoefficients );

				//if (m_initClass->getSubmixVoice() == NULL)
				//LOG->logMsg(LT_ERROR, "submix voice is null");
				voice->SetOutputMatrix(m_initClass->getSubmixVoice(), 1, 1, &m_dspSettings.ReverbLevel);
				XAUDIO2_FILTER_PARAMETERS FilterParametersDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI/6.0f * m_dspSettings.LPFDirectCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
				voice->SetOutputFilterParameters(m_initClass->m_masteringVoice, &FilterParametersDirect);
				XAUDIO2_FILTER_PARAMETERS FilterParametersReverb = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI/6.0f * m_dspSettings.LPFReverbCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
				voice->SetOutputFilterParameters(m_initClass->getSubmixVoice(), &FilterParametersReverb);
			}
		}

		m_frameToApplyAudio3D++;
		m_frameToApplyAudio3D &= 1;
	}

	void Sound3D::setEmitterPos(const D3DXVECTOR3& emitterPos)
	{
		this->m_emitterPos = emitterPos;
	}

	const bool Sound3D::createSourceVoice(WAVEFORMATEX wfm)
	{
		if (this->m_sourceVoice != NULL)
		{
			this->m_sourceVoice->Stop(0);
			this->m_sourceVoice->DestroyVoice();
		}

		HRESULT hr;

		XAUDIO2_SEND_DESCRIPTOR sendDescriptors[2];
		sendDescriptors[0].Flags = XAUDIO2_SEND_USEFILTER; // LPF direct-path
		sendDescriptors[0].pOutputVoice = m_masteringVoice;
		sendDescriptors[1].Flags = XAUDIO2_SEND_USEFILTER; // LPF reverb-path -- omit for better performance at the cost of less realistic occlusion
		sendDescriptors[1].pOutputVoice = m_submixVoice;
		const XAUDIO2_VOICE_SENDS sendList = { 2, sendDescriptors };


		//Create the source voice
		if (FAILED(hr = this->m_XAudio2->CreateSourceVoice( &m_sourceVoice, &wfm, 0,
			2.0f, NULL, &sendList)))
		{
			LOG->logMsg(LT_ERROR, "Error %#X creating source voice", hr);
			return false;
		}

		return true;
	}

	const bool Sound3D::setEmitterEcho(const bool enabled, const float feedback, 
										const float wdQuote, const float delay)
	{
		if (enabled)
		{
			IUnknown * pXAPO;
			if (FAILED(CreateFX(__uuidof(FXEcho),&pXAPO)))
				return false;

			XAUDIO2_EFFECT_DESCRIPTOR descriptor;
			descriptor.InitialState = true;
			descriptor.OutputChannels = 1;
			descriptor.pEffect = pXAPO;

			XAUDIO2_EFFECT_CHAIN chain;
			chain.EffectCount = 1;
			chain.pEffectDescriptors = &descriptor;

			FXECHO_PARAMETERS echoParams;
			echoParams.Delay = delay;
			echoParams.WetDryMix = wdQuote;
			echoParams.Feedback = feedback;

			if (FAILED(m_sourceVoice->SetEffectChain(&chain)))
				return false;

			pXAPO->Release();

			if (FAILED(m_sourceVoice->SetEffectParameters( 0, &echoParams, sizeof( FXECHO_PARAMETERS ) ) ) )
				return false;
		}
		else
		{
			if (FAILED(m_sourceVoice->SetEffectChain(NULL)))
				return false;
		}

		return true;
	}

	const bool Sound3D::calculateEmitterEcho(const float echoDistance, const float emitterDistance,
												const float speedOfSound)
	{
		if ((echoDistance >= emitterDistance) && (speedOfSound > 0))
		{
			//TODO: calculate wet/dry mix, depending on distance between emitter and echo

			// calculate realistic delay of the echo:
			float delay = ((2*(echoDistance-emitterDistance)) / (speedOfSound)) * 1000;
			return this->setEmitterEcho(true, 0.5f, 0.6f, delay);
		}

		return false;
	}

	void Sound3D::setVolumeCurve(X3DAUDIO_DISTANCE_CURVE* const VolumeCurve)
	{
		if (!VolumeCurve)
		{
			LOG->logMsg(LT_ERROR, "Error Code: IP1002");
			return;
		}
		this->m_distCurv = *VolumeCurve;
		this->m_emitter.pVolumeCurve = &this->m_distCurv;
	}

} //namespace clearsky