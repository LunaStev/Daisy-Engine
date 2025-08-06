#pragma once

#include "Core/Module.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

namespace Daisy {

struct ScriptContext {
    std::unordered_map<std::string, float> floatVars;
    std::unordered_map<std::string, int> intVars;
    std::unordered_map<std::string, std::string> stringVars;
    std::unordered_map<std::string, bool> boolVars;
};

class Script {
public:
    Script(const std::string& name) : m_name(name) {}
    virtual ~Script() = default;
    
    virtual bool Load(const std::string& source) = 0;
    virtual bool Execute(ScriptContext& context) = 0;
    virtual void Reset() = 0;
    
    const std::string& GetName() const { return m_name; }
    bool IsLoaded() const { return m_loaded; }
    
protected:
    std::string m_name;
    bool m_loaded = false;
};

class DaisyScript : public Script {
public:
    DaisyScript(const std::string& name) : Script(name) {}
    
    bool Load(const std::string& source) override;
    bool Execute(ScriptContext& context) override;
    void Reset() override;
    
private:
    std::vector<std::string> m_instructions;
    int m_currentLine = 0;
};

class ScriptSystem : public Module {
public:
    ScriptSystem();
    virtual ~ScriptSystem() = default;
    
    bool Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    
    bool LoadScript(const std::string& name, const std::string& filepath);
    bool LoadScriptFromString(const std::string& name, const std::string& source);
    void UnloadScript(const std::string& name);
    
    bool ExecuteScript(const std::string& name, ScriptContext& context);
    void ExecuteAllScripts(ScriptContext& context);
    
    void RegisterFunction(const std::string& name, std::function<void(ScriptContext&)> func);
    void SetGlobalVariable(const std::string& name, const std::string& value);
    void SetGlobalVariable(const std::string& name, float value);
    void SetGlobalVariable(const std::string& name, int value);
    void SetGlobalVariable(const std::string& name, bool value);
    
    void EnableModSupport(bool enable) { m_modSupportEnabled = enable; }
    void SetScriptDirectory(const std::string& directory) { m_scriptDirectory = directory; }
    
    void TriggerEvent(const std::string& eventName, ScriptContext& context);
    void RegisterEventHandler(const std::string& eventName, const std::string& scriptName);
    
private:
    void LoadModScripts();
    void ExecuteEventHandlers(const std::string& eventName, ScriptContext& context);
    
    std::unordered_map<std::string, std::unique_ptr<Script>> m_scripts;
    std::unordered_map<std::string, std::function<void(ScriptContext&)>> m_functions;
    std::unordered_map<std::string, std::vector<std::string>> m_eventHandlers;
    
    ScriptContext m_globalContext;
    
    bool m_modSupportEnabled = true;
    std::string m_scriptDirectory = "Scripts/";
    
    float m_updateTimer = 0.0f;
    std::vector<std::string> m_scriptsToExecute;
};

}