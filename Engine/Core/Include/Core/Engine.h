#pragma once

#include "Module.h"
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <vector>
#include <chrono>

namespace Daisy {

class Engine {
public:
    Engine();
    ~Engine();
    
    bool Initialize();
    void Update();
    void Shutdown();
    
    template<ModuleType T, typename... Args>
    T* RegisterModule(Args&&... args) {
        auto module = std::make_unique<T>(std::forward<Args>(args)...);
        T* modulePtr = module.get();
        
        std::type_index typeIndex(typeid(T));
        m_modules[typeIndex] = std::move(module);
        m_moduleOrder.push_back(typeIndex);
        
        return modulePtr;
    }
    
    template<ModuleType T>
    T* GetModule() {
        std::type_index typeIndex(typeid(T));
        auto it = m_modules.find(typeIndex);
        if (it != m_modules.end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }
    
    float GetDeltaTime() const { return m_deltaTime; }
    bool IsRunning() const { return m_running; }
    void Stop() { m_running = false; }
    void RequestShutdown() { m_running = false; }
    
private:
    void CalculateDeltaTime();
    
    std::unordered_map<std::type_index, std::unique_ptr<Module>> m_modules;
    std::vector<std::type_index> m_moduleOrder;
    
    bool m_initialized = false;
    bool m_running = false;
    float m_deltaTime = 0.0f;
    
    std::chrono::high_resolution_clock::time_point m_lastFrameTime;
};

}