#include "DaisyEditor.h"
#include "EditorUI/EditorWindow.h"
#include "Core/Logger.h"
#include "DaisyPhysics.h"
#include "DaisyRender.h"
#include "DaisySound.h"
#include "DaisyAI.h"
#include "DaisyNet.h"
#include "WorldStreamer.h"
#include "ScriptSystem.h"

// ImGui includes - these would be proper ImGui headers
#include <iostream>
#include <thread>
#include <chrono>

namespace DaisyEditor {

DaisyEditor& DaisyEditor::GetInstance() {
    static DaisyEditor instance;
    return instance;
}

bool DaisyEditor::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    DAISY_INFO("Initializing Daisy Editor...");
    
    // Initialize the engine
    if (!DAISY_ENGINE.Initialize()) {
        DAISY_ERROR("Failed to initialize Daisy Engine");
        return false;
    }
    
    m_engine = DAISY_ENGINE.GetEngine();
    if (!m_engine) {
        DAISY_ERROR("Failed to get Engine module");
        return false;
    }
    
    // Register engine modules
    auto* physics = m_engine->RegisterModule<Daisy::DaisyPhysics>();
    auto* renderer = m_engine->RegisterModule<Daisy::DaisyRender>();
    auto* sound = m_engine->RegisterModule<Daisy::DaisySound>();
    auto* ai = m_engine->RegisterModule<Daisy::DaisyAI>();
    auto* network = m_engine->RegisterModule<Daisy::DaisyNet>();
    auto* worldStreamer = m_engine->RegisterModule<Daisy::WorldStreamer>();
    auto* scriptSystem = m_engine->RegisterModule<Daisy::ScriptSystem>();
    
    // Configure renderer for editor
    if (renderer) {
        renderer->SetViewport(0, 0, m_viewportWidth, m_viewportHeight);
        renderer->SetClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        renderer->EnableInfiniteWorld(true);
    }
    
    // Initialize editor components
    m_currentScene = std::make_unique<Scene>("Untitled Scene");
    m_assetManager = std::make_unique<AssetManager>();
    m_editorCamera = std::make_unique<EditorCamera>();
    
    if (!m_assetManager->Initialize()) {
        DAISY_WARNING("Failed to initialize Asset Manager");
    }
    
    // Initialize UI
    InitializeUI();
    
    m_initialized = true;
    DAISY_INFO("Daisy Editor initialized successfully");
    return true;
}

void DaisyEditor::Run() {
    if (!m_initialized) {
        DAISY_ERROR("Editor not initialized");
        return;
    }
    
    DAISY_INFO("Starting Daisy Editor main loop...");
    m_running = true;
    
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (m_running && m_engine->IsRunning()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        HandleInput();
        Update(deltaTime);
        Render();
        
        // Update engine
        m_engine->Update();
        
        // Cap frame rate to ~60 FPS for editor
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    DAISY_INFO("Daisy Editor main loop ended");
}

void DaisyEditor::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    DAISY_INFO("Shutting down Daisy Editor...");
    
    m_running = false;
    
    // Shutdown editor components
    m_windows.clear();
    m_editorCamera.reset();
    m_assetManager.reset();
    m_currentScene.reset();
    
    // Shutdown engine
    DAISY_ENGINE.Shutdown();
    
    m_initialized = false;
    DAISY_INFO("Daisy Editor shut down successfully");
}

void DaisyEditor::NewScene() {
    m_currentScene = std::make_unique<Scene>("New Scene");
    m_currentScenePath.clear();
    DAISY_INFO("Created new scene");
}

void DaisyEditor::OpenScene(const std::string& filepath) {
    auto newScene = std::make_unique<Scene>();
    if (newScene->LoadFromFile(filepath)) {
        m_currentScene = std::move(newScene);
        m_currentScenePath = filepath;
        DAISY_INFO("Opened scene: {}", filepath);
    } else {
        DAISY_ERROR("Failed to open scene: {}", filepath);
    }
}

void DaisyEditor::SaveScene(const std::string& filepath) {
    std::string savePath = filepath.empty() ? m_currentScenePath : filepath;
    
    if (savePath.empty()) {
        savePath = "Assets/Scenes/" + m_currentScene->GetName() + ".dscene";
    }
    
    if (m_currentScene->SaveToFile(savePath)) {
        m_currentScenePath = savePath;
        DAISY_INFO("Saved scene: {}", savePath);
    } else {
        DAISY_ERROR("Failed to save scene: {}", savePath);
    }
}

void DaisyEditor::SetViewportSize(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
    
    m_editorCamera->SetAspectRatio(static_cast<float>(width) / height);
    
    auto* renderer = m_engine->GetModule<Daisy::DaisyRender>();
    if (renderer) {
        renderer->SetViewport(0, 0, width, height);
    }
}

void DaisyEditor::SetPlayMode(bool play) {
    if (m_playMode == play) return;
    
    m_playMode = play;
    
    if (m_playMode) {
        DAISY_INFO("Entering play mode");
        // Save current scene state for restoration
    } else {
        DAISY_INFO("Exiting play mode");
        // Restore scene state
    }
}

void DaisyEditor::InitializeUI() {
    // Create editor windows
    m_windows.push_back(std::make_unique<MenuBarWindow>());
    m_windows.push_back(std::make_unique<ViewportWindow>());
    m_windows.push_back(std::make_unique<HierarchyWindow>());
    m_windows.push_back(std::make_unique<InspectorWindow>());
    m_windows.push_back(std::make_unique<AssetBrowserWindow>());
    m_windows.push_back(std::make_unique<ConsoleWindow>());
    
    DAISY_INFO("Editor UI initialized");
}

void DaisyEditor::Update(float deltaTime) {
    // Update editor camera
    m_editorCamera->Update(deltaTime);
    
    // Update editor windows
    for (auto& window : m_windows) {
        window->Update(deltaTime);
    }
    
    // Update renderer camera if not in play mode
    if (!m_playMode) {
        auto* renderer = m_engine->GetModule<Daisy::DaisyRender>();
        if (renderer) {
            Daisy::Camera camera = renderer->GetCamera();
            camera.position = m_editorCamera->GetPosition();
            camera.fov = m_editorCamera->GetFOV();
            camera.nearPlane = m_editorCamera->GetNearPlane();
            camera.farPlane = m_editorCamera->GetFarPlane();
            camera.aspect = m_editorCamera->GetAspectRatio();
            renderer->SetCamera(camera);
        }
    }
}

void DaisyEditor::Render() {
    // Begin ImGui frame (placeholder - would be actual ImGui calls)
    
    // Render editor windows
    for (auto& window : m_windows) {
        if (window->IsVisible()) {
            window->Render();
        }
    }
    
    // End ImGui frame and render
    
    // For now, just print a status message periodically
    static int frameCount = 0;
    if (++frameCount % 300 == 0) { // Every ~5 seconds at 60fps
        DAISY_INFO("Editor running - Frame {}", frameCount);
    }
}

void DaisyEditor::HandleInput() {
    // Handle editor-specific input
    m_editorCamera->HandleInput(m_engine->GetDeltaTime());
    
    // Handle keyboard shortcuts
    // (This would be implemented with actual input handling)
    
    // Example shortcuts:
    // Ctrl+N - New Scene
    // Ctrl+O - Open Scene  
    // Ctrl+S - Save Scene
    // Space - Toggle Play Mode
    // F - Focus on selected object
}

}