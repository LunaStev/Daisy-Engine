#pragma once

#include "DaisyEngine.h"
#include "EditorUI/EditorWindow.h"
#include "EditorCore/EditorCamera.h"
#include "EditorCore/Scene.h"
#include "EditorCore/AssetManager.h"
#include <memory>
#include <vector>

namespace DaisyEditor {

class DaisyEditor {
public:
    static DaisyEditor& GetInstance();
    
    bool Initialize();
    void Run();
    void Shutdown();
    
    Daisy::Engine* GetEngine() { return m_engine; }
    Scene* GetCurrentScene() { return m_currentScene.get(); }
    AssetManager* GetAssetManager() { return m_assetManager.get(); }
    EditorCamera* GetEditorCamera() { return m_editorCamera.get(); }
    
    void NewScene();
    void OpenScene(const std::string& filepath);
    void SaveScene(const std::string& filepath = "");
    
    void SetViewportSize(int width, int height);
    bool IsPlayMode() const { return m_playMode; }
    void SetPlayMode(bool play);
    
private:
    DaisyEditor() = default;
    ~DaisyEditor() = default;
    
    void InitializeUI();
    void Update(float deltaTime);
    void Render();
    void HandleInput();
    
    Daisy::Engine* m_engine = nullptr;
    
    std::unique_ptr<Scene> m_currentScene;
    std::unique_ptr<AssetManager> m_assetManager;
    std::unique_ptr<EditorCamera> m_editorCamera;
    
    std::vector<std::unique_ptr<EditorWindow>> m_windows;
    
    bool m_initialized = false;
    bool m_running = false;
    bool m_playMode = false;
    
    int m_viewportWidth = 1280;
    int m_viewportHeight = 720;
    
    std::string m_currentScenePath;
};

}

#define DAISY_EDITOR DaisyEditor::DaisyEditor::GetInstance()