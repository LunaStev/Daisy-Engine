#pragma once

#include "Platform.h"
#include "Core/Logger.h"
#include <string>

#if defined(DAISY_PLATFORM_WINDOWS) && defined(_WIN32)

namespace Daisy {

class WindowsWindow : public Window {
public:
    WindowsWindow() = default;
    virtual ~WindowsWindow();
    
    bool Initialize(const WindowProperties& props) override;
    void Shutdown() override;
    void Update() override;
    
    bool ShouldClose() const override { return m_shouldClose; }
    void SetVSync(bool enabled) override { m_data.vsync = enabled; }
    bool IsVSync() const override { return m_data.vsync; }
    
    int GetWidth() const override { return m_data.width; }
    int GetHeight() const override { return m_data.height; }
    void* GetNativeWindow() const override { return m_hwnd; }
    
    HWND GetHWND() const { return m_hwnd; }
    
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    struct WindowData {
        std::string title;
        int width, height;
        bool vsync;
    };
    
    HWND m_hwnd = nullptr;
    HINSTANCE m_hinstance = nullptr;
    WindowData m_data;
    bool m_shouldClose = false;
    
    static bool s_classRegistered;
};

}

#endif