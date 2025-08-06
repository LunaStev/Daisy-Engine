#include "Core/Engine.h"
#include "Core/Logger.h"
#include <chrono>

namespace Daisy {

Engine::Engine() {
    m_lastFrameTime = std::chrono::high_resolution_clock::now();
}

Engine::~Engine() {
    if (m_initialized) {
        Shutdown();
    }
}

bool Engine::Initialize() {
    if (m_initialized) {
        DAISY_WARNING("Engine already initialized");
        return true;
    }
    
    DAISY_INFO("Initializing Daisy Engine...");
    
    for (auto& typeIndex : m_moduleOrder) {
        auto& module = m_modules[typeIndex];
        DAISY_INFO("Initializing module: {}", module->GetName());
        
        if (!module->Initialize()) {
            DAISY_ERROR("Failed to initialize module: {}", module->GetName());
            return false;
        }
    }
    
    m_initialized = true;
    m_running = true;
    
    DAISY_INFO("Daisy Engine initialized successfully");
    return true;
}

void Engine::Update() {
    if (!m_initialized || !m_running) {
        return;
    }
    
    CalculateDeltaTime();
    
    for (auto& typeIndex : m_moduleOrder) {
        auto& module = m_modules[typeIndex];
        if (module->IsInitialized()) {
            module->Update(m_deltaTime);
        }
    }
}

void Engine::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    DAISY_INFO("Shutting down Daisy Engine...");
    
    for (auto it = m_moduleOrder.rbegin(); it != m_moduleOrder.rend(); ++it) {
        auto& module = m_modules[*it];
        if (module->IsInitialized()) {
            DAISY_INFO("Shutting down module: {}", module->GetName());
            module->Shutdown();
        }
    }
    
    m_modules.clear();
    m_moduleOrder.clear();
    m_initialized = false;
    m_running = false;
    
    DAISY_INFO("Daisy Engine shut down successfully");
}

void Engine::CalculateDeltaTime() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastFrameTime);
    m_deltaTime = duration.count() / 1000000.0f;
    m_lastFrameTime = currentTime;
}

}