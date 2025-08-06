#pragma once

#include "Core/Math.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace DaisyEditor {

struct Transform {
    Daisy::Vector3 position{0, 0, 0};
    Daisy::Quaternion rotation;
    Daisy::Vector3 scale{1, 1, 1};
    
    Daisy::Matrix4 GetMatrix() const;
};

class Entity {
public:
    Entity(uint32_t id, const std::string& name) : m_id(id), m_name(name) {}
    
    uint32_t GetId() const { return m_id; }
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    Transform& GetTransform() { return m_transform; }
    const Transform& GetTransform() const { return m_transform; }
    
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }
    
    Entity* GetParent() const { return m_parent; }
    void SetParent(Entity* parent);
    
    const std::vector<Entity*>& GetChildren() const { return m_children; }
    void AddChild(Entity* child);
    void RemoveChild(Entity* child);
    
    // Component system placeholder
    template<typename T>
    T* AddComponent() { return nullptr; } // Will be implemented later
    
    template<typename T>
    T* GetComponent() { return nullptr; } // Will be implemented later
    
private:
    uint32_t m_id;
    std::string m_name;
    Transform m_transform;
    bool m_visible = true;
    
    Entity* m_parent = nullptr;
    std::vector<Entity*> m_children;
};

class Scene {
public:
    Scene(const std::string& name = "New Scene");
    ~Scene();
    
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    Entity* CreateEntity(const std::string& name = "Entity");
    void DestroyEntity(uint32_t entityId);
    Entity* GetEntity(uint32_t entityId);
    Entity* FindEntityByName(const std::string& name);
    
    const std::vector<std::unique_ptr<Entity>>& GetEntities() const { return m_entities; }
    std::vector<Entity*> GetRootEntities() const;
    
    bool SaveToFile(const std::string& filepath) const;
    bool LoadFromFile(const std::string& filepath);
    
    void Clear();
    
private:
    std::string m_name;
    std::vector<std::unique_ptr<Entity>> m_entities;
    std::unordered_map<uint32_t, Entity*> m_entityMap;
    uint32_t m_nextEntityId = 1;
};

}