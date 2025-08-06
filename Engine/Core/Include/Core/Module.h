#pragma once

#include <string>
#include <memory>

namespace Daisy {

class Module {
public:
    Module(const std::string& name) : m_name(name) {}
    virtual ~Module() = default;
    
    virtual bool Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Shutdown() = 0;
    
    const std::string& GetName() const { return m_name; }
    bool IsInitialized() const { return m_initialized; }
    
protected:
    std::string m_name;
    bool m_initialized = false;
};

template<typename T>
concept ModuleType = std::is_base_of_v<Module, T>;

}