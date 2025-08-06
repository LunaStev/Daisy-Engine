#pragma once

#include "Platform.h"
#include "Core/Logger.h"
#include <string>

#if !defined(DAISY_PLATFORM_WINDOWS) || !defined(_WIN32)

namespace Daisy {

class DummyWindow : public Window {
public:
    DummyWindow() = default;
    virtual ~DummyWindow() = default;
    
    bool Initialize(const WindowProperties& props) override {
        m_data.title = props.title;
        m_data.width = props.width;
        m_data.height = props.height;
        m_data.vsync = props.vsync;
        
        DAISY_INFO("Created dummy window: {} ({}x{})", m_data.title, m_data.width, m_data.height);
        return true;
    }
    
    void Shutdown() override {
        DAISY_INFO("Shut down dummy window");
    }
    
    void Update() override {
        // No-op for dummy window
    }
    
    bool ShouldClose() const override { return false; } // Never close in dummy mode
    void SetVSync(bool enabled) override { m_data.vsync = enabled; }
    bool IsVSync() const override { return m_data.vsync; }
    
    int GetWidth() const override { return m_data.width; }
    int GetHeight() const override { return m_data.height; }
    void* GetNativeWindow() const override { return nullptr; }
    
private:
    struct WindowData {
        std::string title;
        int width, height;
        bool vsync;
    };
    
    WindowData m_data;
};

}

#endif