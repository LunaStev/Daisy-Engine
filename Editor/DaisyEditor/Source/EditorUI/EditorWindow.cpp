#include "EditorUI/EditorWindow.h"
#include "DaisyEditor.h"
#include "EditorCore/Scene.h"
#include "EditorCore/AssetManager.h"
#include "Core/Logger.h"
#include <iostream>

namespace DaisyEditor {

// Base EditorWindow
EditorWindow::EditorWindow(const std::string& title, bool visible)
    : m_title(title), m_visible(visible) {
}

void EditorWindow::BeginWindow() {
    // ImGui::Begin(m_title.c_str(), &m_visible);
    // m_focused = ImGui::IsWindowFocused();
    m_focused = false; // Placeholder
}

void EditorWindow::EndWindow() {
    // ImGui::End();
}

bool EditorWindow::IsWindowFocused() {
    // return ImGui::IsWindowFocused();
    return m_focused;
}

// ViewportWindow
ViewportWindow::ViewportWindow() : EditorWindow("Viewport") {
}

void ViewportWindow::Render() {
    BeginWindow();
    
    RenderViewport();
    HandleViewportInput();
    
    EndWindow();
}

void ViewportWindow::Update(float deltaTime) {
    // Update viewport-specific logic
}

void ViewportWindow::RenderViewport() {
    // For now, just display viewport info as text
    std::cout << "Viewport Window - Size: " << m_viewportWidth << "x" << m_viewportHeight << std::endl;
    
    // In a real implementation, this would:
    // 1. Get the render texture from the engine
    // 2. Display it in an ImGui::Image
    // 3. Handle viewport resizing
    // 4. Display viewport controls (wireframe, lighting, etc.)
}

void ViewportWindow::HandleViewportInput() {
    // Handle viewport-specific input
    // Mouse picking, gizmos, etc.
}

// HierarchyWindow
HierarchyWindow::HierarchyWindow() : EditorWindow("Scene Hierarchy") {
}

void HierarchyWindow::Render() {
    BeginWindow();
    
    RenderEntityHierarchy();
    
    EndWindow();
}

void HierarchyWindow::RenderEntityHierarchy() {
    Scene* scene = DAISY_EDITOR.GetCurrentScene();
    if (!scene) {
        std::cout << "No scene loaded" << std::endl;
        return;
    }
    
    // Display scene name
    std::cout << "Scene: " << scene->GetName() << std::endl;
    
    // Render root entities
    std::vector<Entity*> rootEntities = scene->GetRootEntities();
    for (Entity* entity : rootEntities) {
        RenderEntityNode(entity);
    }
    
    // Right-click context menu would go here
    // Create Entity, Delete Entity, etc.
}

void HierarchyWindow::RenderEntityNode(Entity* entity) {
    if (!entity) return;
    
    // Display entity name with indentation based on hierarchy depth
    std::cout << "  - " << entity->GetName() << " (ID: " << entity->GetId() << ")" << std::endl;
    
    // Render children
    for (Entity* child : entity->GetChildren()) {
        RenderEntityNode(child);
    }
    
    // In ImGui, this would be:
    // bool nodeOpen = ImGui::TreeNode(entity->GetName().c_str());
    // if (ImGui::IsItemClicked()) m_selectedEntity = entity;
    // if (nodeOpen) {
    //     for (Entity* child : entity->GetChildren()) {
    //         RenderEntityNode(child);
    //     }
    //     ImGui::TreePop();
    // }
}

// InspectorWindow
InspectorWindow::InspectorWindow() : EditorWindow("Inspector") {
}

void InspectorWindow::Render() {
    BeginWindow();
    
    if (m_selectedEntity) {
        RenderEntityInspector(m_selectedEntity);
    } else {
        std::cout << "No entity selected" << std::endl;
    }
    
    EndWindow();
}

void InspectorWindow::RenderEntityInspector(Entity* entity) {
    if (!entity) return;
    
    std::cout << "Inspecting: " << entity->GetName() << std::endl;
    
    // Entity name field
    std::cout << "Name: " << entity->GetName() << std::endl;
    
    // Transform component
    RenderTransformComponent(entity);
    
    // Add Component button
    RenderAddComponentMenu(entity);
    
    // Other components would be rendered here
}

void InspectorWindow::RenderTransformComponent(Entity* entity) {
    Transform& transform = entity->GetTransform();
    
    std::cout << "Transform:" << std::endl;
    std::cout << "  Position: (" << transform.position.x << ", " 
              << transform.position.y << ", " << transform.position.z << ")" << std::endl;
    std::cout << "  Rotation: (" << transform.rotation.x << ", " 
              << transform.rotation.y << ", " << transform.rotation.z 
              << ", " << transform.rotation.w << ")" << std::endl;
    std::cout << "  Scale: (" << transform.scale.x << ", " 
              << transform.scale.y << ", " << transform.scale.z << ")" << std::endl;
    
    // In ImGui, this would be DragFloat3 controls for each vector
}

void InspectorWindow::RenderAddComponentMenu(Entity* entity) {
    std::cout << "Add Component: [Mesh Renderer] [Collider] [AI Agent]" << std::endl;
    
    // This would be a combo box or button menu in ImGui
}

// AssetBrowserWindow
AssetBrowserWindow::AssetBrowserWindow() 
    : EditorWindow("Asset Browser"), m_currentDirectory("Assets") {
}

void AssetBrowserWindow::Render() {
    BeginWindow();
    
    RenderDirectoryTree();
    RenderAssetGrid();
    
    EndWindow();
}

void AssetBrowserWindow::RenderDirectoryTree() {
    AssetManager* assetManager = DAISY_EDITOR.GetAssetManager();
    if (!assetManager) return;
    
    std::cout << "Directory Tree:" << std::endl;
    
    std::vector<std::string> directories = assetManager->GetDirectories();
    for (const std::string& dir : directories) {
        std::cout << "  [Folder] " << dir << std::endl;
    }
}

void AssetBrowserWindow::RenderAssetGrid() {
    AssetManager* assetManager = DAISY_EDITOR.GetAssetManager();
    if (!assetManager) return;
    
    std::cout << "Assets in " << m_currentDirectory << ":" << std::endl;
    
    const std::vector<AssetInfo>& assets = assetManager->GetAllAssets();
    for (const AssetInfo& asset : assets) {
        if (asset.filepath.find(m_currentDirectory) == 0) {
            RenderAssetItem(asset);
        }
    }
}

void AssetBrowserWindow::RenderAssetItem(const AssetInfo& asset) {
    std::cout << "  [" << DAISY_EDITOR.GetAssetManager()->GetAssetTypeString(asset.type) 
              << "] " << asset.name << std::endl;
    
    // In ImGui, this would be:
    // - Thumbnail image
    // - Asset name
    // - Click handling
    // - Drag & drop support
}

void AssetBrowserWindow::HandleAssetDoubleClick(const AssetInfo& asset) {
    if (asset.type == AssetType::Scene) {
        DAISY_EDITOR.OpenScene(asset.filepath);
    }
    // Handle other asset types...
}

// ConsoleWindow
ConsoleWindow::ConsoleWindow() : EditorWindow("Console") {
    // Add some sample logs
    AddLog("Daisy Editor started", 0);
    AddLog("Engine initialized successfully", 0);
}

void ConsoleWindow::Render() {
    BeginWindow();
    
    // Filter buttons
    std::cout << "Filters: ";
    std::cout << "[Info: " << (m_showInfo ? "ON" : "OFF") << "] ";
    std::cout << "[Warning: " << (m_showWarning ? "ON" : "OFF") << "] ";
    std::cout << "[Error: " << (m_showError ? "ON" : "OFF") << "] ";
    std::cout << "[Clear]" << std::endl;
    
    // Log entries
    std::cout << "Console Output:" << std::endl;
    for (const LogEntry& entry : m_logs) {
        bool shouldShow = (entry.level == 0 && m_showInfo) ||
                         (entry.level == 1 && m_showWarning) ||
                         (entry.level == 2 && m_showError);
        
        if (shouldShow) {
            std::string levelStr = (entry.level == 0) ? "INFO" :
                                 (entry.level == 1) ? "WARN" : "ERROR";
            std::cout << "[" << levelStr << "] " << entry.message << std::endl;
        }
    }
    
    EndWindow();
}

void ConsoleWindow::AddLog(const std::string& message, int level) {
    LogEntry entry;
    entry.message = message;
    entry.level = level;
    entry.timestamp = "00:00:00"; // Would use actual timestamp
    
    m_logs.push_back(entry);
    
    // Keep log size manageable
    if (m_logs.size() > 1000) {
        m_logs.erase(m_logs.begin());
    }
}

void ConsoleWindow::Clear() {
    m_logs.clear();
}

// MenuBarWindow
MenuBarWindow::MenuBarWindow() : EditorWindow("MenuBar") {
}

void MenuBarWindow::Render() {
    // Main menu bar
    std::cout << "=== DAISY EDITOR MENU ===" << std::endl;
    std::cout << "[File] [Edit] [View] [Help]" << std::endl;
    
    // In ImGui, this would be:
    // if (ImGui::BeginMainMenuBar()) {
    //     RenderFileMenu();
    //     RenderEditMenu();
    //     RenderViewMenu();
    //     RenderHelpMenu();
    //     ImGui::EndMainMenuBar();
    // }
}

void MenuBarWindow::RenderFileMenu() {
    std::cout << "File Menu: New Scene, Open Scene, Save Scene, Exit" << std::endl;
}

void MenuBarWindow::RenderEditMenu() {
    std::cout << "Edit Menu: Undo, Redo, Cut, Copy, Paste" << std::endl;
}

void MenuBarWindow::RenderViewMenu() {
    std::cout << "View Menu: Toggle Windows, Reset Layout" << std::endl;
}

void MenuBarWindow::RenderHelpMenu() {
    std::cout << "Help Menu: About, Documentation" << std::endl;
}

}