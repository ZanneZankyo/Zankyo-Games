//---------------------------------------------------------------------------
#include "Engine/Audio/Audio.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

AudioSystem* AudioSystem::s_instance = new AudioSystem();
//---------------------------------------------------------------------------
// FMOD startup code based on "GETTING STARTED With FMOD Ex Programmerís API for Windows" document
//	from the FMOD programming API at http://www.fmod.org/download/
//
AudioSystem::AudioSystem()
	: m_fmodSystem( nullptr )
{
	InitializeFMOD();
}


//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
	ShutDownFMOD();
}


//---------------------------------------------------------------------------
void AudioSystem::InitializeFMOD()
{
	const int MAX_AUDIO_DEVICE_NAME_LEN = 256;
	FMOD_RESULT result;
	unsigned int fmodVersion;
	int numDrivers;
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS deviceCapabilities;
	char audioDeviceName[ MAX_AUDIO_DEVICE_NAME_LEN ];

	// Create a System object and initialize.
	result = FMOD::System_Create( &m_fmodSystem );
	ValidateResult( result );

	result = m_fmodSystem->getVersion( &fmodVersion );
	ValidateResult( result );

	if( fmodVersion < FMOD_VERSION )
	{
		DebuggerPrintf( "Engine/Audio SYSTEM ERROR!  Your FMOD .dll is of an older version (0x%08x == %d) than that the .lib used to compile this code (0x%08x == %d).\n", fmodVersion, fmodVersion, FMOD_VERSION, FMOD_VERSION );
	}

	result = m_fmodSystem->getNumDrivers( &numDrivers );
	ValidateResult( result );

	if( numDrivers == 0 )
	{
		result = m_fmodSystem->setOutput( FMOD_OUTPUTTYPE_NOSOUND );
		ValidateResult( result );
	}
	else
	{
		result = m_fmodSystem->getDriverCaps( 0, &deviceCapabilities, 0, &speakerMode );
		ValidateResult( result );

		// Set the user selected speaker mode.
		result = m_fmodSystem->setSpeakerMode( speakerMode );
		ValidateResult( result );

		if( deviceCapabilities & FMOD_CAPS_HARDWARE_EMULATED )
		{
			// The user has the 'Acceleration' slider set to off! This is really bad
			// for latency! You might want to warn the user about this.
			result = m_fmodSystem->setDSPBufferSize( 1024, 10 );
			ValidateResult( result );
		}

		result = m_fmodSystem->getDriverInfo( 0, audioDeviceName, MAX_AUDIO_DEVICE_NAME_LEN, 0 );
		ValidateResult( result );

		if( strstr( audioDeviceName, "SigmaTel" ) )
		{
			// Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
			// PCM floating point output seems to solve it.
			result = m_fmodSystem->setSoftwareFormat( 48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR );
			ValidateResult( result );
		}
	}

	result = m_fmodSystem->init( 100, FMOD_INIT_NORMAL, 0 );
	if( result == FMOD_ERR_OUTPUT_CREATEBUFFER )
	{
		// Ok, the speaker mode selected isn't supported by this sound card. Switch it
		// back to stereo...
		result = m_fmodSystem->setSpeakerMode( FMOD_SPEAKERMODE_STEREO );
		ValidateResult( result );

		// ... and re-init.
		result = m_fmodSystem->init( 100, FMOD_INIT_NORMAL, 0 );
		ValidateResult( result );
	}
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::ShutDownFMOD()
{
//	FMOD_RESULT result = FMOD_OK;
//	result = FMOD_System_Close( m_fmodSystem );
//	result = FMOD_System_Release( m_fmodSystem );
	m_fmodSystem = nullptr;
}


//---------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound( const std::string& soundFileName )
{
	std::map< std::string, SoundID >::iterator found = m_registeredSoundIDs.find( soundFileName );
	if( found != m_registeredSoundIDs.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		m_fmodSystem->createSound( soundFileName.c_str(), FMOD_DEFAULT, nullptr, &newSound );
		if( newSound )
		{
			SoundID newSoundID = m_registeredSounds.size();
			m_registeredSoundIDs[ soundFileName ] = newSoundID;
			m_registeredSounds.push_back( newSound );
			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}


//---------------------------------------------------------------------------
void AudioSystem::StopChannel( AudioChannelHandle channel )
{
	if( channel != nullptr )
	{
		FMOD::Channel* fmodChannel = (FMOD::Channel*) channel;
		fmodChannel->stop();
	}
}


//---------------------------------------------------------------------------
AudioChannelHandle AudioSystem::PlayAudio( SoundID soundID, float volumeLevel )
{
	unsigned int numSounds = m_registeredSounds.size();
	if( soundID < 0 || soundID >= numSounds )
		return nullptr;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if( !sound )
		return nullptr;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( FMOD_CHANNEL_FREE, sound, false, &channelAssignedToSound );
	if( channelAssignedToSound )
	{
		channelAssignedToSound->setVolume( volumeLevel );
	}

	return (AudioChannelHandle) channelAssignedToSound;
}

AudioChannelHandle AudioSystem::LoopAudio(SoundID soundID, float volumeLevel)
{
	unsigned int numSounds = m_registeredSounds.size();
	if (soundID < 0 || soundID >= numSounds)
		return nullptr;

	FMOD::Sound* sound = m_registeredSounds[soundID];
	if (!sound)
		return nullptr;

	FMOD::Channel* channelAssignedToSound = nullptr;
	sound->setMode(FMOD_LOOP_NORMAL);
	sound->setLoopCount(-1);
	m_fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &channelAssignedToSound);
	if (channelAssignedToSound)
	{
		channelAssignedToSound->setVolume(volumeLevel);
	}

	return (AudioChannelHandle)channelAssignedToSound;
}

AudioChannelHandle AudioSystem::LoopAudio(SoundID soundID, int loopBegin, int loopEnd, float volumeLevel)
{
	unsigned int numSounds = m_registeredSounds.size();
	if (soundID < 0 || soundID >= numSounds)
		return nullptr;

	FMOD::Sound* sound = m_registeredSounds[soundID];
	if (!sound)
		return nullptr;

	FMOD::Channel* channelAssignedToSound = nullptr;
	sound->setMode(FMOD_LOOP_NORMAL);
	sound->setLoopPoints(loopBegin, FMOD_TIMEUNIT_MS, loopEnd, FMOD_TIMEUNIT_MS);
	sound->setLoopCount(-1);
	m_fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &channelAssignedToSound);
	if (channelAssignedToSound)
	{
		channelAssignedToSound->setVolume(volumeLevel);
		//channelAssignedToSound->setLoopPoints(loopBegin, FMOD_TIMEUNIT_MS, loopEnd, FMOD_TIMEUNIT_MS);
	}

	return (AudioChannelHandle)channelAssignedToSound;
}


std::vector<float> AudioSystem::GetSpectrum(AudioChannelHandle channel, size_t numOfSegment)
{
	UNUSED(channel);
	std::vector<float> spectrum;
	spectrum.resize(numOfSegment, 0.f);
	m_fmodSystem->getSpectrum(&spectrum[0], numOfSegment, 0, FMOD_DSP_FFT_WINDOW_BLACKMAN);
	return spectrum;
}

bool AudioSystem::GetSpectrum(AudioChannelHandle channelHandle, std::vector<float>& spectrum)
{
	FMOD_CHANNEL* channel = (FMOD_CHANNEL*)channelHandle;
	FMOD_Channel_GetSpectrum(channel, &spectrum[0], spectrum.size(), 0, FMOD_DSP_FFT_WINDOW_MAX);
	//auto result1 = channel->getSpectrum(&spectrum[0], spectrum.size(), 0, FMOD_DSP_FFT_WINDOW_TRIANGLE);
	return true;
	//auto result2 = m_fmodSystem->getSpectrum(&spectrum[0], spectrum.size(), 0, FMOD_DSP_FFT_WINDOW_TRIANGLE);
	
}

//---------------------------------------------------------------------------
void AudioSystem::Update()
{
	FMOD_RESULT result = m_fmodSystem->update();
	ValidateResult( result );
}

AudioSystem * AudioSystem::GetInstance()
{
	return s_instance;
}


//---------------------------------------------------------------------------
void AudioSystem::ValidateResult( FMOD_RESULT result )
{
	GUARANTEE_OR_DIE( result == FMOD_OK, Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %d.\n", result ) );
}


