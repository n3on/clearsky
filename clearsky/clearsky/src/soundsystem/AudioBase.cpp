#include "core/precompiled.h"
#include "soundsystem\AudioBase.h"

namespace clearsky
{

// Specify sound cone to add directionality to m_listener for artistic effect:
// m_emitters behind the m_listener are defined here to be more attenuated,
// have a lower LPF cutoff frequency,
// yet have a slightly higher reverb send level.

	static const X3DAUDIO_CONE m_listener_DirectionalCone = { X3DAUDIO_PI*5.0f/6.0f, X3DAUDIO_PI*11.0f/6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f };

	static const unsigned short OUTPUTCHANNELS = 8;

	XAUDIO2FX_REVERB_I3DL2_PARAMETERS PRESET_PARAMS[ NUM_PRESETS ] =
	{
		XAUDIO2FX_I3DL2_PRESET_FOREST,
		XAUDIO2FX_I3DL2_PRESET_DEFAULT,
		XAUDIO2FX_I3DL2_PRESET_GENERIC,
		XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,
		XAUDIO2FX_I3DL2_PRESET_ROOM,
		XAUDIO2FX_I3DL2_PRESET_BATHROOM,
		XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,
		XAUDIO2FX_I3DL2_PRESET_STONEROOM,
		XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,
		XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,
		XAUDIO2FX_I3DL2_PRESET_CAVE,
		XAUDIO2FX_I3DL2_PRESET_ARENA,
		XAUDIO2FX_I3DL2_PRESET_HANGAR,	//12
		XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY,
		XAUDIO2FX_I3DL2_PRESET_HALLWAY,
		XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,
		XAUDIO2FX_I3DL2_PRESET_ALLEY,
		XAUDIO2FX_I3DL2_PRESET_CITY,
		XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,
		XAUDIO2FX_I3DL2_PRESET_QUARRY,
		XAUDIO2FX_I3DL2_PRESET_PLAIN,
		XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,
		XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,
		XAUDIO2FX_I3DL2_PRESET_UNDERWATER,
		XAUDIO2FX_I3DL2_PRESET_SMALLROOM,
		XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,
		XAUDIO2FX_I3DL2_PRESET_LARGEROOM,
		XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,
		XAUDIO2FX_I3DL2_PRESET_LARGEHALL,
		XAUDIO2FX_I3DL2_PRESET_PLATE,
	};

/*****************************

	AudioBase:
		- required for Sound class derivates
		- basis for the Audio3D class

	******************************************/

AudioBase::AudioBase(void)
{
	m_XAudio2 = NULL;
	m_masteringVoice = NULL;

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	//LOG->logMsg(LT_STATUS, "Audio base class instance was created.");
}

AudioBase::~AudioBase(void)
{
	if (m_masteringVoice != NULL)
		m_masteringVoice->DestroyVoice();

	SAFE_RELEASE(m_XAudio2);

	CoUninitialize();

	//LOG->logMsg(LT_STATUS, "Audio base class instance was destroyed.");
}

bool AudioBase::initialiseAudio()
{
	if (m_masteringVoice != NULL)
		m_masteringVoice->DestroyVoice();

	SAFE_RELEASE(m_XAudio2);

	HRESULT hr;
	if (FAILED(hr = XAudio2Create( &m_XAudio2, 0)))
	{
		LOG->logMsg(LT_ERROR, "Error while initialising Audio Base class: %#X");
		return false;
	}

	if (FAILED(hr = m_XAudio2->CreateMasteringVoice(&m_masteringVoice)))
	{
		LOG->logMsg(LT_ERROR, "Audio base class: Error while creating master voice: %#X");
		return false;
	}

	return true;
}

void AudioBase::alterVolume(const float fltVolume)
{
	if (this->m_masteringVoice == NULL)
		return;

	m_masteringVoice->SetVolume(fltVolume);	//Playback device volume
}

void AudioBase::getVolume(float &fltVolume) const
{
	if (this->m_masteringVoice == NULL)
		return;

	m_masteringVoice->GetVolume(&fltVolume);
}


/*****************************

	Audio3D:
		- required for 3DSound class derivates
		- derived from AudioBase class

	******************************************/

Audio3D::Audio3D() : AudioBase()
{
	m_initialized = false;
	this->m_curveDistanceScaler = 10;
	//LOG->logMsg(LT_STATUS, "Audio3D instance was constructed.");
}


Audio3D::~Audio3D()
{
	if (m_submixVoice != NULL)
		m_submixVoice->DestroyVoice();

	SAFE_RELEASE(m_reverbEffect);

	//LOG->logMsg(LT_STATUS, "Audio3D instance was destroyed.");
}

bool Audio3D::initialiseAudio()
{
	if (m_masteringVoice != NULL)
		m_masteringVoice->DestroyVoice();

	SAFE_RELEASE(m_XAudio2);

	HRESULT hr;
	if (FAILED(hr = XAudio2Create( &m_XAudio2, 0)))
	{
		LOG->logMsg(LT_ERROR, "Error while creating XAudio2: %#X", hr);
		SAFE_RELEASE(m_XAudio2);
		return false;
	}

	if (FAILED(hr = m_XAudio2->CreateMasteringVoice(&m_masteringVoice)))
	{
		LOG->logMsg(LT_ERROR, "Audio3D: Error while creating master voice: %#X", hr);
		return false;
	}

	XAUDIO2_DEVICE_DETAILS details;
    if( FAILED( hr = m_XAudio2->GetDeviceDetails( 0, &details ) ) )
    {
        SAFE_RELEASE( m_XAudio2 );
		LOG->logMsg(LT_ERROR, "Failed to retrieve DeviceDetails: %#X", hr);
        return false;
    }

    if( details.OutputFormat.Format.nChannels > OUTPUTCHANNELS )
    {
        SAFE_RELEASE( m_XAudio2 );
		LOG->logMsg(LT_ERROR, "nChannels too large.");
        return false;
    }

    m_channelMask = details.OutputFormat.dwChannelMask;
    m_channels = details.OutputFormat.Format.nChannels;

	//Create reverb effect:

	 if( FAILED( hr = XAudio2CreateReverb( &m_reverbEffect, 0 ) ) )
    {
        SAFE_RELEASE( m_XAudio2 );
		LOG->logMsg(LT_ERROR, "Failed to create reverb: %#X", hr);
        return false;
    }


	//Create submix voice for mixing effects

	XAUDIO2_EFFECT_DESCRIPTOR effects[] = { { m_reverbEffect, TRUE, 1 } };
    XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };

    if( FAILED( hr = m_XAudio2->CreateSubmixVoice( &m_submixVoice, 1,
                                                               details.OutputFormat.Format.nSamplesPerSec, 0, 0,
                                                               NULL, &effectChain ) ) )
    {
        SAFE_RELEASE( m_XAudio2 );
        SAFE_RELEASE( m_reverbEffect );
		LOG->logMsg(LT_ERROR, "Failed to create submix voice: %#X", hr);
        return false;
    }

	
	// Set default FX params
    XAUDIO2FX_REVERB_PARAMETERS native;
    ReverbConvertI3DL2ToNative( &PRESET_PARAMS[0], &native );
    m_submixVoice->SetEffectParameters( 0, &native, sizeof( native ) );

	const float SPEEDOFSOUND = X3DAUDIO_SPEED_OF_SOUND;

    X3DAudioInitialize( details.OutputFormat.dwChannelMask, SPEEDOFSOUND, m_x3DInstance );

    m_useInnerRadius = true;
	m_listenerPos = D3DXVECTOR3( 0, 0, 0 );
	m_useRedirectToLFE = ((details.OutputFormat.dwChannelMask & SPEAKER_LOW_FREQUENCY) != 0);

	//
    // Setup 3D audio structs
    //
    m_listener.Position = m_listenerPos;
    m_listener.OrientFront = D3DXVECTOR3( 0, 1, 0 );
    m_listener.OrientTop = D3DXVECTOR3( 0, 1, 0 );
    m_listener.pCone = (X3DAUDIO_CONE*)&m_listener_DirectionalCone;

	//LOG->logMsg(LT_STATUS, "Audio3D: initialized.");

	this->m_curveDistanceScaler = 60.0f;

	m_initialized = true;

	return true;
}

IXAudio2SubmixVoice* Audio3D::getSubmixVoice()
{
	return this->m_submixVoice;
}

HRESULT Audio3D::setReverb( int nReverb )
{
    if( !isInitialized() )
        return S_FALSE;

    if( nReverb < 0 || nReverb >= NUM_PRESETS )
        return E_FAIL;

	if(m_submixVoice)
    {
        XAUDIO2FX_REVERB_PARAMETERS native;
        ReverbConvertI3DL2ToNative( &PRESET_PARAMS[ nReverb ], &native );
        m_submixVoice->SetEffectParameters( 0, &native, sizeof( native ) );
		LOG->logMsg(LT_STATUS, "Audio3D: Reverb has been set.");
    }


    return S_OK;
}

void Audio3D::setListenerPosition(const D3DXVECTOR3& listenerPos)
{
	this->m_listenerPos = listenerPos;
}

void Audio3D::setUseInnerRadius(bool useIt)
{
	this->m_useInnerRadius = useIt;
	//LOG->logMsg(LT_STATUS, "Audio3D: inner radius usage changed.");
}

void Audio3D::setListenerCone(const X3DAUDIO_CONE& listenerCone)
{
	this->m_Cone = listenerCone;
	m_listener.pCone = &this->m_Cone;
	//LOG->logMsg(LT_STATUS, "Audio3D: ListenerCone was changed.");
}

bool Audio3D::useListenerCone() const
{
	return (this->m_listener.pCone != NULL);
}

bool Audio3D::useInnerRadius() const
{
	return this->m_useInnerRadius;
}

const X3DAUDIO_VECTOR& Audio3D::getListenerPosition() const
{
	return this->m_listener.Position;
}

const unsigned int Audio3D::getChannels() const
{
	return this->m_channels;
}

const float Audio3D::getCurveDistanceScaler() const
{
	return this->m_curveDistanceScaler;
}

bool Audio3D::isInitialized() const
{
	return this->m_initialized;
}

bool  Audio3D::useRedirectToLFE() const
{
	return this->m_useRedirectToLFE;
}

void Audio3D::setOrientFront(const D3DXVECTOR3& orientFront)
{
	this->m_listener.OrientFront = orientFront;
}

void Audio3D::setCurveDistanceScaler(const float cds)
{
	this->m_curveDistanceScaler = cds;
}

}