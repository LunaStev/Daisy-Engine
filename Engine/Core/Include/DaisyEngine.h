#pragma once

#include "Core/Engine.h"
#include "Core/Module.h"
#include "Core/Logger.h"
#include "Core/Memory.h"
#include "Core/Math.h"

namespace Daisy {

class DaisyEngine {
public:
    static DaisyEngine& GetInstance();
    
    bool Initialize();
    void Run();
    void Shutdown();
    
    template<typename T>
    T* GetModule() { return m_engine->GetModule<T>(); }
    
    Engine* GetEngine() { return m_engine.get(); }
    
private:
    DaisyEngine() = default;
    ~DaisyEngine() = default;
    
    std::unique_ptr<Engine> m_engine;
    bool m_initialized = false;
};

}

#define DAISY_ENGINE Daisy::DaisyEngine::GetInstance()