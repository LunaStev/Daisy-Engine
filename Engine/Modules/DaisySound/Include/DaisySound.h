#pragma once

#include "Core/Module.h"
#include "Core/Math.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace Daisy {

struct AudioSource {
    Vector3 position{0, 0, 0};
    Vector3 velocity{0, 0, 0};
    float volume = 1.0f;
    float pitch = 1.0f;
    float range = 100.0f;
    bool looping = false;
    bool playing = false;
    bool is3D = true;
    uint32_t soundId = 0;
};

struct AudioListener {
    Vector3 position{0, 0, 0};
    Vector3 velocity{0, 0, 0};
    Vector3 forward{0, 0, -1};
    Vector3 up{0, 1, 0};
};

struct EnvironmentSettings {
    float reverbLevel = 0.2f;
    float dampening = 0.1f;
    float roomSize = 1.0f;
    bool spaceEnvironment = false;
};

class DaisySound : public Module {
public:
    DaisySound();
    virtual ~DaisySound() = default;
    
    bool Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    
    uint32_t LoadSound(const std::string& filepath);
    void UnloadSound(uint32_t soundId);
    
    uint32_t CreateAudioSource();
    void DestroyAudioSource(uint32_t sourceId);
    AudioSource* GetAudioSource(uint32_t sourceId);
    
    void PlaySound(uint32_t sourceId, uint32_t soundId);
    void StopSound(uint32_t sourceId);
    void PauseSound(uint32_t sourceId);
    
    void SetListener(const AudioListener& listener) { m_listener = listener; }
    AudioListener& GetListener() { return m_listener; }
    
    void SetEnvironment(const EnvironmentSettings& environment) { m_environment = environment; }
    void SetMasterVolume(float volume) { m_masterVolume = volume; }
    
    void EnableDopplerEffect(bool enable) { m_dopplerEnabled = enable; }
    void EnableVoiceChat(bool enable) { m_voiceChatEnabled = enable; }
    
private:
    void UpdateSpatialAudio();
    void UpdateDopplerEffect();
    void ProcessVoiceChat();
    float CalculateAttenuation(const Vector3& sourcePos, const Vector3& listenerPos, float range);
    
    std::unordered_map<uint32_t, std::vector<uint8_t>> m_sounds;
    std::unordered_map<uint32_t, std::unique_ptr<AudioSource>> m_audioSources;
    
    AudioListener m_listener;
    EnvironmentSettings m_environment;
    
    uint32_t m_nextSoundId = 1;
    uint32_t m_nextSourceId = 1;
    
    float m_masterVolume = 1.0f;
    bool m_dopplerEnabled = true;
    bool m_voiceChatEnabled = false;
};

}