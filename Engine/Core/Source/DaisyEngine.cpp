#include "DaisyEngine.h"
#include "Core/Logger.h"

namespace Daisy {

DaisyEngine& DaisyEngine::GetInstance() {
    static DaisyEngine instance;
    return instance;
}

bool DaisyEngine::Initialize() {
    if (m_initialized) {
        DAISY_WARNING("DaisyEngine already initialized");
        return true;
    }
    
    DAISY_LOG.Initialize();
    DAISY_INFO("Starting Daisy Engine initialization...");
    
    m_engine = std::make_unique<Engine>();
    
    if (!m_engine->Initialize()) {
        DAISY_ERROR("Failed to initialize engine core");
        return false;
    }
    
    m_initialized = true;
    DAISY_INFO("DaisyEngine initialized successfully");
    return true;
}

void DaisyEngine::Run() {
    if (!m_initialized) {
        DAISY_ERROR("DaisyEngine not initialized. Call Initialize() first.");
        return;
    }
    
    DAISY_INFO("Starting main engine loop...");
    
    while (m_engine->IsRunning()) {
        m_engine->Update();
    }
    
    DAISY_INFO("Engine loop ended");
}

void DaisyEngine::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    DAISY_INFO("Shutting down DaisyEngine...");
    
    if (m_engine) {
        m_engine->Shutdown();
        m_engine.reset();
    }
    
    m_initialized = false;
    DAISY_INFO("DaisyEngine shutdown complete");
}

}