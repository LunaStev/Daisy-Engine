#include "ScriptSystem.h"
#include "Core/Logger.h"
#include <fstream>
#include <sstream>

namespace Daisy {

bool DaisyScript::Load(const std::string& source) {
    m_instructions.clear();
    m_currentLine = 0;
    
    std::istringstream iss(source);
    std::string line;
    
    while (std::getline(iss, line)) {
        if (!line.empty() && line[0] != '#') { // Skip comments
            m_instructions.push_back(line);
        }
    }
    
    m_loaded = !m_instructions.empty();
    return m_loaded;
}

bool DaisyScript::Execute(ScriptContext& context) {
    if (!m_loaded) return false;
    
    for (const auto& instruction : m_instructions) {
        // Simple instruction parsing (placeholder)
        if (instruction.find("set ") == 0) {
            // Parse variable assignment
            // set variableName value
        } else if (instruction.find("if ") == 0) {
            // Parse conditional statement
        } else if (instruction.find("call ") == 0) {
            // Parse function call
        }
    }
    
    return true;
}

void DaisyScript::Reset() {
    m_currentLine = 0;
}

ScriptSystem::ScriptSystem() : Module("ScriptSystem") {
}

bool ScriptSystem::Initialize() {
    DAISY_INFO("Initializing Script System");
    
    // Register built-in functions
    RegisterFunction("log", [](ScriptContext& ctx) {
        // Logging function implementation
    });
    
    RegisterFunction("wait", [](ScriptContext& ctx) {
        // Wait function implementation
    });
    
    if (m_modSupportEnabled) {
        LoadModScripts();
    }
    
    m_initialized = true;
    DAISY_INFO("Script System initialized successfully");
    return true;
}

void ScriptSystem::Update(float deltaTime) {
    if (!m_initialized) return;
    
    m_updateTimer += deltaTime;
    
    // Execute scripts that need regular updates
    if (m_updateTimer >= 0.016f) { // ~60 FPS
        for (const std::string& scriptName : m_scriptsToExecute) {
            ExecuteScript(scriptName, m_globalContext);
        }
        m_updateTimer = 0.0f;
    }
}

void ScriptSystem::Shutdown() {
    if (!m_initialized) return;
    
    DAISY_INFO("Shutting down Script System");
    
    m_scripts.clear();
    m_functions.clear();
    m_eventHandlers.clear();
    m_scriptsToExecute.clear();
    
    m_initialized = false;
    DAISY_INFO("Script System shut down successfully");
}

bool ScriptSystem::LoadScript(const std::string& name, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        DAISY_ERROR("Failed to open script file: {}", filepath);
        return false;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    return LoadScriptFromString(name, source);
}

bool ScriptSystem::LoadScriptFromString(const std::string& name, const std::string& source) {
    auto script = std::make_unique<DaisyScript>(name);
    
    if (!script->Load(source)) {
        DAISY_ERROR("Failed to load script: {}", name);
        return false;
    }
    
    m_scripts[name] = std::move(script);
    DAISY_INFO("Loaded script: {}", name);
    return true;
}

void ScriptSystem::UnloadScript(const std::string& name) {
    auto it = m_scripts.find(name);
    if (it != m_scripts.end()) {
        m_scripts.erase(it);
        DAISY_INFO("Unloaded script: {}", name);
    }
}

bool ScriptSystem::ExecuteScript(const std::string& name, ScriptContext& context) {
    auto it = m_scripts.find(name);
    if (it == m_scripts.end()) {
        DAISY_WARNING("Script not found: {}", name);
        return false;
    }
    
    return it->second->Execute(context);
}

void ScriptSystem::ExecuteAllScripts(ScriptContext& context) {
    for (auto& [name, script] : m_scripts) {
        if (script->IsLoaded()) {
            script->Execute(context);
        }
    }
}

void ScriptSystem::RegisterFunction(const std::string& name, std::function<void(ScriptContext&)> func) {
    m_functions[name] = func;
    DAISY_DEBUG("Registered script function: {}", name);
}

void ScriptSystem::SetGlobalVariable(const std::string& name, const std::string& value) {
    m_globalContext.stringVars[name] = value;
}

void ScriptSystem::SetGlobalVariable(const std::string& name, float value) {
    m_globalContext.floatVars[name] = value;
}

void ScriptSystem::SetGlobalVariable(const std::string& name, int value) {
    m_globalContext.intVars[name] = value;
}

void ScriptSystem::SetGlobalVariable(const std::string& name, bool value) {
    m_globalContext.boolVars[name] = value;
}

void ScriptSystem::TriggerEvent(const std::string& eventName, ScriptContext& context) {
    ExecuteEventHandlers(eventName, context);
}

void ScriptSystem::RegisterEventHandler(const std::string& eventName, const std::string& scriptName) {
    m_eventHandlers[eventName].push_back(scriptName);
    DAISY_DEBUG("Registered event handler for '{}': {}", eventName, scriptName);
}

void ScriptSystem::LoadModScripts() {
    // Load scripts from mod directories
    // Implementation would scan mod directories and load scripts
    DAISY_INFO("Loading mod scripts from: {}", m_scriptDirectory);
}

void ScriptSystem::ExecuteEventHandlers(const std::string& eventName, ScriptContext& context) {
    auto it = m_eventHandlers.find(eventName);
    if (it != m_eventHandlers.end()) {
        for (const std::string& scriptName : it->second) {
            ExecuteScript(scriptName, context);
        }
    }
}

}