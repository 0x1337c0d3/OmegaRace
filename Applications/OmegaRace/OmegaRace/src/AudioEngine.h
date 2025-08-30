#pragma once

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include "vmath.h"

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

    void LoadSound(const std::string& sSoundName, bool b3d = false, bool bLooping = false, bool bStream = false);
    void UnLoadSound(const std::string& sSoundName);
    int PlaySoundFile(const std::string& sSoundName, const Vector3f& vPosition = Vector3f{0,0,0}, float fVolumedB = 0.0f);
    bool IsPlaying(int nChannelId);

    void SetChannel3dPosition(int nChannelId, const Vector3f& vPosition);
    void SetChannelVolume(int nChannelId, float fVolumedB);

    void LoadBank(const std::string& sBankName, FMOD_STUDIO_LOAD_BANK_FLAGS pflags);

    void LoadEvent(const std::string& sEventName);
    void PlayEvent(const std::string& sEventName);
    void StopEvent(const std::string& sEventName, bool bImmediate = false);
    bool IsEventPlaying(const std::string& sEventName);
    void GetEventParameter(const std::string& sEventName, const std::string& sEventParameter, float* fParameter);
    void SetEventParameter(const std::string& sEventName, const std::string& sParameterName, float fValue);

    void Set3dListenerAndOrientation(const Vector3f& vPos = Vector3f{0,0,0}, float fVolumedB = 0.0f);
    void StopChannel(int nChannelId);
    void StopAllChannels();

    float dbToVolume(float fdB);
    float VolumeTodB(float fVolume);
    FMOD_VECTOR VectorToFmod(const Vector3f& vPosition);
};

} // namespace omegarace
