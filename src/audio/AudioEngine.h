#pragma once

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include "Types.h"

namespace omegarace {

// using the pimpl idiom
struct CAudioEngineImpl
{
    CAudioEngineImpl();
    ~CAudioEngineImpl();

    void Update();

    FMOD::Studio::System* mStudioSystem;
    FMOD::System* mSystem;

    int mNextChannelId;

    typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;
    typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
    typedef std::map<std::string, FMOD::Sound*> SoundMap;
    typedef std::map<int, FMOD::Channel*> ChannelMap;

    BankMap mBanks;
    EventMap mEvents;
    SoundMap mSounds;
    ChannelMap mChannels;
};

class AudioEngine
{
public:
    static void Init();
    static void Update();
    static void Shutdown();
    static int ErrorCheck(FMOD_RESULT result);

    static void LoadSound(const std::string& sSoundName, bool b3d = false, bool bLooping = false, bool bStream = false);
    static void UnLoadSound(const std::string& sSoundName);
    static int PlaySoundFile(const std::string& sSoundName, const Vector3f& vPosition = Vector3f{0,0,0}, float fVolumedB = 0.0f);
    static bool IsPlaying(int nChannelId);

    static void SetChannel3dPosition(int nChannelId, const Vector3f& vPosition);
    static void SetChannelVolume(int nChannelId, float fVolumedB);

    static void LoadBank(const std::string& sBankName, FMOD_STUDIO_LOAD_BANK_FLAGS pflags);

    static void LoadEvent(const std::string& sEventName);
    static void PlayEvent(const std::string& sEventName);
    static void StopEvent(const std::string& sEventName, bool bImmediate = false);
    static bool IsEventPlaying(const std::string& sEventName);
    static void GetEventParameter(const std::string& sEventName, const std::string& sEventParameter, float* fParameter);
    static void SetEventParameter(const std::string& sEventName, const std::string& sParameterName, float fValue);

    static void Set3dListenerAndOrientation(const Vector3f& vPos = Vector3f{0,0,0}, float fVolumedB = 0.0f);
    static void StopChannel(int nChannelId);
    void StopAllChannels();

    static float dbToVolume(float fdB);
    static float VolumeTodB(float fVolume);
    static FMOD_VECTOR VectorToFmod(const Vector3f& vPosition);
};

} // namespace omegarace
