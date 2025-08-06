#include "DaisySound.h"
#include "Core/Logger.h"
#include <algorithm>

namespace Daisy {

DaisySound::DaisySound() : Module("DaisySound") {
}

bool DaisySound::Initialize() {
    DAISY_INFO("Initializing Daisy Sound Engine");
    
    m_sounds.reserve(1000);
    m_audioSources.reserve(1000);
    
    m_initialized = true;
    DAISY_INFO("Daisy Sound Engine initialized successfully");
    return true;
}

void DaisySound::Update(float deltaTime) {
    if (!m_initialized) return;
    
    UpdateSpatialAudio();
    if (m_dopplerEnabled) {
        UpdateDopplerEffect();
    }
    if (m_voiceChatEnabled) {
        ProcessVoiceChat();
    }
}

void DaisySound::Shutdown() {
    if (!m_initialized) return;
    
    DAISY_INFO("Shutting down Daisy Sound Engine");
    
    for (auto& [id, source] : m_audioSources) {
        if (source->playing) {
            StopSound(id);
        }
    }
    
    m_sounds.clear();
    m_audioSources.clear();
    
    m_initialized = false;
    DAISY_INFO("Daisy Sound Engine shut down successfully");
}

uint32_t DaisySound::LoadSound(const std::string& filepath) {
    uint32_t id = m_nextSoundId++;
    
    // Sound loading implementation would be here
    std::vector<uint8_t> soundData;
    m_sounds[id] = std::move(soundData);
    
    return id;
}

void DaisySound::UnloadSound(uint32_t soundId) {
    m_sounds.erase(soundId);
}

uint32_t DaisySound::CreateAudioSource() {
    uint32_t id = m_nextSourceId++;
    m_audioSources[id] = std::make_unique<AudioSource>();
    return id;
}

void DaisySound::DestroyAudioSource(uint32_t sourceId) {
    auto it = m_audioSources.find(sourceId);
    if (it != m_audioSources.end()) {
        if (it->second->playing) {
            StopSound(sourceId);
        }
        m_audioSources.erase(it);
    }
}

AudioSource* DaisySound::GetAudioSource(uint32_t sourceId) {
    auto it = m_audioSources.find(sourceId);
    return it != m_audioSources.end() ? it->second.get() : nullptr;
}

void DaisySound::PlaySound(uint32_t sourceId, uint32_t soundId) {
    auto* source = GetAudioSource(sourceId);
    if (source && m_sounds.find(soundId) != m_sounds.end()) {
        source->soundId = soundId;
        source->playing = true;
        // Audio playback implementation would be here
    }
}

void DaisySound::StopSound(uint32_t sourceId) {
    auto* source = GetAudioSource(sourceId);
    if (source) {
        source->playing = false;
        // Stop audio playback implementation would be here
    }
}

void DaisySound::PauseSound(uint32_t sourceId) {
    auto* source = GetAudioSource(sourceId);
    if (source && source->playing) {
        // Pause audio playback implementation would be here
    }
}

void DaisySound::UpdateSpatialAudio() {
    for (auto& [id, source] : m_audioSources) {
        if (!source->playing || !source->is3D) continue;
        
        float attenuation = CalculateAttenuation(source->position, m_listener.position, source->range);
        float finalVolume = source->volume * attenuation * m_masterVolume;
        
        // Apply 3D positioning to audio source
    }
}

void DaisySound::UpdateDopplerEffect() {
    for (auto& [id, source] : m_audioSources) {
        if (!source->playing || !source->is3D) continue;
        
        Vector3 relativeVelocity = source->velocity - m_listener.velocity;
        Vector3 direction = (source->position - m_listener.position).Normalized();
        float velocityComponent = relativeVelocity.Dot(direction);
        
        const float speedOfSound = 343.0f; // m/s
        float dopplerFactor = speedOfSound / (speedOfSound - velocityComponent);
        
        float adjustedPitch = source->pitch * dopplerFactor;
        // Apply pitch adjustment to audio source
    }
}

void DaisySound::ProcessVoiceChat() {
    // Voice chat processing implementation would be here
}

float DaisySound::CalculateAttenuation(const Vector3& sourcePos, const Vector3& listenerPos, float range) {
    float distance = (sourcePos - listenerPos).Length();
    if (distance >= range) return 0.0f;
    
    // Linear attenuation
    return 1.0f - (distance / range);
}

}