#include "DaisyPlatform.h"
#include "Core/Logger.h"
#include <algorithm>

#if defined(DAISY_PLATFORM_WINDOWS) && defined(_WIN32)
    #include "WindowsWindow.h"
#else
    #include "DummyWindow.h"
#endif

namespace Daisy {

Window* Window::Create(const WindowProperties& props) {
#if defined(DAISY_PLATFORM_WINDOWS) && defined(_WIN32)
    return new WindowsWindow();
#else
    return new DummyWindow();
#endif
}

bool DaisyPlatform::Initialize() {
    DAISY_INFO("Initializing Platform module...");
    
    // Create main window
    WindowProperties props;
    props.title = "Daisy Engine";
    props.width = 1280;
    props.height = 720;
    
    m_mainWindow.reset(Window::Create(props));
    if (!m_mainWindow) {
        DAISY_ERROR("Failed to create main window");
        return false;
    }
    
    if (!m_mainWindow->Initialize(props)) {
        DAISY_ERROR("Failed to initialize main window");
        m_mainWindow.reset();
        return false;
    }
    
    DAISY_INFO("Platform module initialized successfully");
    return true;
}

void DaisyPlatform::Update(float deltaTime) {
    // Update main window
    if (m_mainWindow) {
        m_mainWindow->Update();
    }
    
    // Update other windows
    for (auto& window : m_windows) {
        if (window) {
            window->Update();
        }
    }
}

void DaisyPlatform::Shutdown() {
    DAISY_INFO("Shutting down Platform module...");
    
    m_windows.clear();
    m_mainWindow.reset();
    
    DAISY_INFO("Platform module shut down successfully");
}

Window* DaisyPlatform::CreateEngineWindow(const WindowProperties& props) {
    auto window = std::unique_ptr<Window>(Window::Create(props));
    if (!window || !window->Initialize(props)) {
        DAISY_ERROR("Failed to create window: {}", props.title);
        return nullptr;
    }
    
    Window* windowPtr = window.get();
    m_windows.push_back(std::move(window));
    
    DAISY_INFO("Created window: {}", props.title);
    return windowPtr;
}

void DaisyPlatform::DestroyEngineWindow(Window* window) {
    auto it = std::find_if(m_windows.begin(), m_windows.end(),
        [window](const std::unique_ptr<Window>& ptr) {
            return ptr.get() == window;
        });
    
    if (it != m_windows.end()) {
        m_windows.erase(it);
        DAISY_INFO("Destroyed window");
    }
}

}