#pragma once

#include <string>
#include <memory>
#include <vector>

// Forward declarations for ImGui (will be included in implementation)
struct ImGuiContext;

namespace DaisyEditor {

class EditorWindow {
public:
    EditorWindow(const std::string& title, bool visible = true);
    virtual ~EditorWindow() = default;
    
    virtual void Render() = 0;
    virtual void Update(float deltaTime) {}
    
    const std::string& GetTitle() const { return m_title; }
    void SetTitle(const std::string& title) { m_title = title; }
    
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }
    
    bool IsFocused() const { return m_focused; }
    
protected:
    std::string m_title;
    bool m_visible;
    bool m_focused = false;
    
    // Helper methods for common UI operations
    void BeginWindow();
    void EndWindow();
    bool IsWindowFocused();
};

// Specific editor window types
class ViewportWindow : public EditorWindow {
public:
    ViewportWindow();
    void Render() override;
    void Update(float deltaTime) override;
    
private:
    void RenderViewport();
    void HandleViewportInput();
    
    bool m_viewportFocused = false;
    bool m_viewportHovered = false;
    int m_viewportWidth = 1280;
    int m_viewportHeight = 720;
};

class HierarchyWindow : public EditorWindow {
public:
    HierarchyWindow();
    void Render() override;
    
private:
    void RenderEntityHierarchy();
    void RenderEntityNode(class Entity* entity);
    
    Entity* m_selectedEntity = nullptr;
};

class InspectorWindow : public EditorWindow {
public:
    InspectorWindow();
    void Render() override;
    
    void SetSelectedEntity(Entity* entity) { m_selectedEntity = entity; }
    
private:
    void RenderEntityInspector(Entity* entity);
    void RenderTransformComponent(Entity* entity);
    void RenderAddComponentMenu(Entity* entity);
    
    Entity* m_selectedEntity = nullptr;
};

class AssetBrowserWindow : public EditorWindow {
public:
    AssetBrowserWindow();
    void Render() override;
    
private:
    void RenderDirectoryTree();
    void RenderAssetGrid();
    void RenderAssetItem(const struct AssetInfo& asset);
    void HandleAssetDoubleClick(const AssetInfo& asset);
    
    std::string m_currentDirectory;
    std::string m_selectedAsset;
    float m_thumbnailSize = 64.0f;
};

class ConsoleWindow : public EditorWindow {
public:
    ConsoleWindow();
    void Render() override;
    
    void AddLog(const std::string& message, int level = 0);
    void Clear();
    
private:
    struct LogEntry {
        std::string message;
        int level; // 0=info, 1=warning, 2=error
        std::string timestamp;
    };
    
    std::vector<LogEntry> m_logs;
    bool m_autoScroll = true;
    bool m_showInfo = true;
    bool m_showWarning = true;
    bool m_showError = true;
};

class MenuBarWindow : public EditorWindow {
public:
    MenuBarWindow();
    void Render() override;
    
private:
    void RenderFileMenu();
    void RenderEditMenu();
    void RenderViewMenu();
    void RenderHelpMenu();
};

}