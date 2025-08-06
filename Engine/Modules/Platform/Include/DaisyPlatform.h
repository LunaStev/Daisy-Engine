#pragma once

#include "Core/Module.h"
#include "Platform.h"
#include <memory>
#include <vector>

namespace Daisy {

class DaisyPlatform : public Module {
public:
    DaisyPlatform() : Module("DaisyPlatform") {}
    virtual ~DaisyPlatform() = default;
    
    bool Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    
    Window* CreateEngineWindow(const WindowProperties& props = WindowProperties());
    void DestroyEngineWindow(Window* window);
    
    Window* GetMainWindow() const { return m_mainWindow.get(); }
    
private:
    std::unique_ptr<Window> m_mainWindow;
    std::vector<std::unique_ptr<Window>> m_windows;
};

}