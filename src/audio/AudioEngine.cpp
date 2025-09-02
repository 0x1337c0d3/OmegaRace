#include "AudioEngine.h"
#include "Window.h"
#include "Logger.h"
#include <thread>
#include <chrono>

namespace omegarace {

CAudioEngineImpl* mImplementation = nullptr;

CAudioEngineImpl::CAudioEngineImpl()
{
    std::cout << "CAudioEngineImpl constructor called" << std::endl;
    
    mStudioSystem = nullptr;
    std::cout << "Creating FMOD Studio System..." << std::endl;
    FMOD_RESULT result = FMOD::Studio::System::create(&mStudioSystem);
    std::cout << "FMOD::Studio::System::create() returned: " << result << std::endl;
    AudioEngine::ErrorCheck(result);
    
    std::cout << "Initializing FMOD Studio System..." << std::endl;
    result = mStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, nullptr);
    std::cout << "mStudioSystem->initialize() returned: " << result << std::endl;
    AudioEngine::ErrorCheck(result);

    mSystem = nullptr;
    std::cout << "Getting FMOD Core System..." << std::endl;
    result = mStudioSystem->getCoreSystem(&mSystem);
    std::cout << "mStudioSystem->getCoreSystem() returned: " << result << std::endl;
    AudioEngine::ErrorCheck(result);
    
    std::cout << "CAudioEngineImpl constructor completed successfully" << std::endl;
}

CAudioEngineImpl::~CAudioEngineImpl()
{
    AudioEngine::ErrorCheck(mStudioSystem->unloadAll());
    AudioEngine::ErrorCheck(mStudioSystem->release());
}

void CAudioEngineImpl::Update()
{
    // check the current sound to see if any sounds are stopped
    std::vector<ChannelMap::iterator> vStoppedChannels;
    for(auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
    {
        bool bIsPlaying = false;
        it->second->isPlaying(&bIsPlaying);
        if (!bIsPlaying)
        {
            vStoppedChannels.push_back(it);
        }
    }

    // erase the stopped sound from our checking
    for (auto& it : vStoppedChannels)
    {
        mChannels.erase(it);
    }

    // update the fmod system object
    AudioEngine::ErrorCheck(mStudioSystem->update());
}

void AudioEngine::Init()
{
    Logger::Info("Initializing audio engine");
    std::cout << "AudioEngine::Init() called" << std::endl;
    
    std::cout << "Creating CAudioEngineImpl..." << std::endl;
    mImplementation = new CAudioEngineImpl;
    std::cout << "CAudioEngineImpl created successfully, mImplementation = " << mImplementation << std::endl;
}

void AudioEngine::Update()
{
    mImplementation->Update();
}

void AudioEngine::Shutdown()
{
    Logger::Info("Shutting down audio engine");
    delete mImplementation;
}

int AudioEngine::ErrorCheck(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        // Convert FMOD_RESULT to string representation
        std::string errorStr = "FMOD Error Code: " + std::to_string(result);
        Logger::Error("FMOD Error", errorStr.c_str());
        throw std::runtime_error("FMOD error");
    }

    return 0;
}

void AudioEngine::LoadSound(const std::string &sSoundName, bool b3d, bool bLooping, bool bStream)
{
    std::cout << "LoadSound: Starting to load sound: " << sSoundName << std::endl;
    
    // Check if mImplementation is valid
    if (!mImplementation) {
        std::cout << "LoadSound: ERROR - mImplementation is null!" << std::endl;
        return;
    }
    std::cout << "LoadSound: mImplementation is valid" << std::endl;
    
    // Check if mSystem is valid
    if (!mImplementation->mSystem) {
        std::cout << "LoadSound: ERROR - mImplementation->mSystem is null!" << std::endl;
        return;
    }
    std::cout << "LoadSound: mSystem is valid" << std::endl;
    
    // check if the sound is loaded
    auto tFoundIt = mImplementation->mSounds.find(sSoundName);
    if (tFoundIt != mImplementation->mSounds.end()) {
        std::cout << "LoadSound: Sound already loaded: " << sSoundName << std::endl;
        return;
    }

    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3d ? FMOD_3D : FMOD_2D;
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    // load the sound
    FMOD::Sound* sound = nullptr;
    std::string path = Window::dataPath() + "/audio/" + sSoundName + ".wav";
    std::cout << "LoadSound: Attempting to load from path: " << path << std::endl;
    
    // Add debug info before the critical call
    std::cout << "LoadSound: About to call mSystem->createSound() for: " << sSoundName << std::endl;
    std::cout << "LoadSound: Total sounds loaded so far: " << mImplementation->mSounds.size() << std::endl;
    
    FMOD_RESULT result = mImplementation->mSystem->createSound(path.c_str(), eMode, nullptr, &sound);
    std::cout << "LoadSound: createSound returned result: " << result << std::endl;
    
    AudioEngine::ErrorCheck(result);
    if (sound)
    {
        mImplementation->mSounds[sSoundName] = sound;
        std::cout << "LoadSound: Successfully loaded sound: " << sSoundName << std::endl;
        
        // Add small delay to prevent FMOD internal memory corruption
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } else {
        std::cout << "LoadSound: Failed to load sound: " << sSoundName << std::endl;
    }
}

void AudioEngine::UnLoadSound(const std::string &sSoundName)
{
    // check if the sound is loaded
    auto foundIt = mImplementation->mSounds.find(sSoundName);
    if (foundIt == mImplementation->mSounds.end())
        return;

    // unload the sound
    AudioEngine::ErrorCheck(foundIt->second->release());
    mImplementation->mSounds.erase(foundIt);
}

int AudioEngine::PlaySoundFile(const std::string &sSoundName, const Vector3f &vPosition, float fVolumedB)
{
    // check if the sound is loaded
    int channelId = mImplementation->mNextChannelId++;
    auto foundIt = mImplementation->mSounds.find(sSoundName);

    // the sound is not loaded, so we load it
    if (foundIt == mImplementation->mSounds.end())
    {
        LoadSound(sSoundName, true);
        foundIt = mImplementation->mSounds.find(sSoundName);
        if (foundIt == mImplementation->mSounds.end())
        {
            return channelId;
        }
    }

    // play the sound in a new created channel
    FMOD::Channel* channel = nullptr;
    AudioEngine::ErrorCheck(mImplementation->mSystem->playSound(foundIt->second, nullptr, true, &channel));
    if (channel)
    {
        FMOD_MODE currMode;
        foundIt->second->getMode(&currMode);

        // if the sound is in 3d space, we set its 3d attributes in fmod
        if (currMode & FMOD_3D)
        {
            FMOD_VECTOR fmodVector = VectorToFmod(vPosition);
            AudioEngine::ErrorCheck(channel->set3DAttributes(&fmodVector, nullptr));
        }
        AudioEngine::ErrorCheck(channel->setVolume(dbToVolume(fVolumedB)));
        AudioEngine::ErrorCheck(channel->setPaused(false));

        mImplementation->mChannels[channelId] = channel;
    }

    return channelId;
}

void AudioEngine::SetChannel3dPosition(int nChannelId, const Vector3f &vPosition)
{
    auto foundIt = mImplementation->mChannels.find(nChannelId);
    if (foundIt == mImplementation->mChannels.end())
        return;

    FMOD_VECTOR position = VectorToFmod(vPosition);
    AudioEngine::ErrorCheck(foundIt->second->set3DAttributes(&position, nullptr));
}

void AudioEngine::SetChannelVolume(int nChannelId, float fVolumedB)
{
    auto foundIt = mImplementation->mChannels.find(nChannelId);
    if (foundIt == mImplementation->mChannels.end())
        return;

    AudioEngine::ErrorCheck(foundIt->second->setVolume(fVolumedB));
}

// banks are what stores all the sounds and informations for each FMOD event
void AudioEngine::LoadBank(const std::string &sBankName, FMOD_STUDIO_LOAD_BANK_FLAGS pflags)
{
    // check if the bank is loaded
    auto foundIt = mImplementation->mBanks.find(sBankName);
    if (foundIt != mImplementation->mBanks.end())
        return;

    // load the bank
    FMOD::Studio::Bank* bank;
    std::string path = Window::dataPath() + "/audio/" + sBankName;
    AudioEngine::ErrorCheck(mImplementation->mStudioSystem->loadBankFile(
            path.c_str(), pflags, &bank));
    if (bank)
    {
        mImplementation->mBanks[sBankName] = bank;
    }
}

// FMOD events have a description and an instance
//   the description is the information and the instance is what actually plays the sound
// FIXME: Only works with the GUID for now
void AudioEngine::LoadEvent(const std::string &sEventName)
{
    // check if the event is loaded
    auto foundIt = mImplementation->mEvents.find(sEventName);
    if (foundIt != mImplementation->mEvents.end())
        return;

    FMOD::Studio::EventDescription* eventDescription = nullptr;
    AudioEngine::ErrorCheck(mImplementation->mStudioSystem->getEvent(sEventName.c_str(), &eventDescription));

    if (eventDescription)
    {
        FMOD::Studio::EventInstance* eventInstance = nullptr;
        AudioEngine::ErrorCheck(eventDescription->createInstance(&eventInstance));
        if (eventInstance)
        {
            mImplementation->mEvents[sEventName] = eventInstance;
        }
    }
}

// FIXME: Only works with the GUID for now
void AudioEngine::PlayEvent(const std::string &sEventName)
{
    // check if the event is loaded
    auto foundIt = mImplementation->mEvents.find(sEventName);

    // the sound is not loaded, so we load it
    if (foundIt == mImplementation->mEvents.end())
    {
        LoadEvent(sEventName);
        foundIt = mImplementation->mEvents.find(sEventName);
        if (foundIt == mImplementation->mEvents.end())
            return;
    }

    // play the event
    foundIt->second->start();
}

void AudioEngine::StopEvent(const std::string &sEventName, bool bImmediate)
{
    // check if the event is loaded
    auto foundIt = mImplementation->mEvents.find(sEventName);
    if (foundIt == mImplementation->mEvents.end())
        return;

    // stop the event
    FMOD_STUDIO_STOP_MODE mode;
    mode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    AudioEngine::ErrorCheck(foundIt->second->stop(mode));
}

bool AudioEngine::IsEventPlaying(const std::string &sEventName)
{
    // check is the event is loaded
    auto foundIt = mImplementation->mEvents.find(sEventName);
    if (foundIt == mImplementation->mEvents.end())
        return false;

    FMOD_STUDIO_PLAYBACK_STATE* state = nullptr;
    if (foundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
    {
        return true;
    }
    return false;
}

void AudioEngine::GetEventParameter(const std::string &sEventName, const std::string &sEventParameter, float *fParameter)
{
    // check if the event is loaded
    auto foundIt = mImplementation->mEvents.find(sEventName);
    if (foundIt == mImplementation->mEvents.end())
        return;

    AudioEngine::ErrorCheck(foundIt->second->getParameterByName(sEventName.c_str(), fParameter, nullptr));
}

void AudioEngine::SetEventParameter(const std::string &sEventName, const std::string &sParameterName, float fValue)
{
    auto foundIt = mImplementation->mEvents.find(sEventName);
    if (foundIt == mImplementation->mEvents.end())
        return;

    AudioEngine::ErrorCheck(foundIt->second->setParameterByName(sEventName.c_str(), fValue));
}

void AudioEngine::StopChannel(int nChannelId) {
    auto foundIt = mImplementation->mChannels.find(nChannelId);
    if (foundIt == mImplementation->mChannels.end())
        return;

    foundIt->second->stop();
}

void AudioEngine::StopAllChannels() {
}

float AudioEngine::dbToVolume(float fdB)
{
    return powf(10.0f, 0.05f * fdB);
}

float AudioEngine::VolumeTodB(float fVolume)
{
    return 20.0f * log10f(fVolume);
}

FMOD_VECTOR AudioEngine::VectorToFmod(const Vector3f &vPosition)
{
    return FMOD_VECTOR{
            vPosition.x,
            vPosition.y,
            vPosition.z
    };
}

} // namespace omegarace
