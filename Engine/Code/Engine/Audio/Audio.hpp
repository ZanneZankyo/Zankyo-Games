#pragma once

//---------------------------------------------------------------------------
#pragma comment( lib, "ThirdParty/fmod/fmodex_vc" ) // Link in the fmodex_vc.lib static library


//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>
#include "ThirdParty/fmod/fmod.hpp"


//---------------------------------------------------------------------------
typedef unsigned int SoundID;
typedef void* AudioChannelHandle;
const unsigned int MISSING_SOUND_ID = 0xffffffff;


/////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();

	SoundID CreateOrGetSound( const std::string& soundFileName );
	AudioChannelHandle PlayAudio( SoundID soundID, float volumeLevel=1.f );
	AudioChannelHandle LoopAudio( SoundID soundID, float volumeLevel = 1.f );
	AudioChannelHandle LoopAudio(SoundID soundID, int loopBegin, int loopEnd, float volumeLevel = 1.f);
	std::vector<float> GetSpectrum(AudioChannelHandle channel, size_t numOfSegment);
	bool GetSpectrum(AudioChannelHandle channel, std::vector<float>& spectrum);
	void StopChannel( AudioChannelHandle channel );
	void Update();

	static AudioSystem* GetInstance();

protected:
	void InitializeFMOD();
	void ShutDownFMOD();
	void ValidateResult( FMOD_RESULT result );

protected:
	FMOD::System*						m_fmodSystem;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;

private:
	static AudioSystem* s_instance;
};

