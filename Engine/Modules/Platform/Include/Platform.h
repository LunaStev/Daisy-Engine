#pragma once

#include <string>

// Only include Windows headers on Windows
#if defined(DAISY_PLATFORM_WINDOWS) && defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #include <windowsx.h>
    
    // Undefine problematic Windows macros
    #ifdef CreateWindow
        #undef CreateWindow
    #endif
    #ifdef DestroyWindow
        #undef DestroyWindow
    #endif
#endif

namespace Daisy {
    
struct WindowProperties {
    std::string title = "Daisy Engine Window";
    int width = 1280;
    int height = 720;
    bool fullscreen = false;
    bool resizable = true;
    bool vsync = true;
};

class Window {
public:
    virtual ~Window() = default;
    
    virtual bool Initialize(const WindowProperties& props) = 0;
    virtual void Shutdown() = 0;
    virtual void Update() = 0;
    
    virtual bool ShouldClose() const = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;
    
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual void* GetNativeWindow() const = 0;
    
    static Window* Create(const WindowProperties& props = WindowProperties());
};

}