#include "EditorCore/Scene.h"
#include "Core/Logger.h"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace DaisyEditor {

Daisy::Matrix4 Transform::GetMatrix() const {
    Daisy::Matrix4 translation = Daisy::Matrix4::Translation(position);
    Daisy::Matrix4 rot = rotation.ToMatrix();
    Daisy::Matrix4 scl = Daisy::Matrix4::Scale(scale);
    
    return translation * rot * scl;
}

void Entity::SetParent(Entity* parent) {
    if (m_parent == parent) return;
    
    // Remove from old parent
    if (m_parent) {
        m_parent->RemoveChild(this);
    }
    
    // Set new parent
    m_parent = parent;
    
    // Add to new parent
    if (m_parent) {
        m_parent->AddChild(this);
    }
}

void Entity::AddChild(Entity* child) {
    if (!child || child == this) return;
    
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it == m_children.end()) {
        m_children.push_back(child);
        child->m_parent = this;
    }
}

void Entity::RemoveChild(Entity* child) {
    if (!child) return;
    
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        (*it)->m_parent = nullptr;
        m_children.erase(it);
    }
}

Scene::Scene(const std::string& name) : m_name(name) {
    DAISY_INFO("Created scene: {}", name);
}

Scene::~Scene() {
    Clear();
}

Entity* Scene::CreateEntity(const std::string& name) {
    auto entity = std::make_unique<Entity>(m_nextEntityId++, name);
    Entity* entityPtr = entity.get();
    
    m_entityMap[entityPtr->GetId()] = entityPtr;
    m_entities.push_back(std::move(entity));
    
    DAISY_DEBUG("Created entity '{}' with ID {}", name, entityPtr->GetId());
    return entityPtr;
}

void Scene::DestroyEntity(uint32_t entityId) {
    auto it = std::find_if(m_entities.begin(), m_entities.end(),
        [entityId](const std::unique_ptr<Entity>& entity) {
            return entity->GetId() == entityId;
        });
    
    if (it != m_entities.end()) {
        Entity* entity = it->get();
        
        // Remove from parent
        if (entity->GetParent()) {
            entity->GetParent()->RemoveChild(entity);
        }
        
        // Destroy children (or reparent them)
        std::vector<Entity*> children = entity->GetChildren();
        for (Entity* child : children) {
            child->SetParent(nullptr); // Orphan the children (could also destroy them)
        }
        
        m_entityMap.erase(entityId);
        m_entities.erase(it);
        
        DAISY_DEBUG("Destroyed entity with ID {}", entityId);
    }
}

Entity* Scene::GetEntity(uint32_t entityId) {
    auto it = m_entityMap.find(entityId);
    return it != m_entityMap.end() ? it->second : nullptr;
}

Entity* Scene::FindEntityByName(const std::string& name) {
    auto it = std::find_if(m_entities.begin(), m_entities.end(),
        [&name](const std::unique_ptr<Entity>& entity) {
            return entity->GetName() == name;
        });
    
    return it != m_entities.end() ? it->get() : nullptr;
}

std::vector<Entity*> Scene::GetRootEntities() const {
    std::vector<Entity*> rootEntities;
    
    for (const auto& entity : m_entities) {
        if (!entity->GetParent()) {
            rootEntities.push_back(entity.get());
        }
    }
    
    return rootEntities;
}

bool Scene::SaveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        DAISY_ERROR("Failed to open file for writing: {}", filepath);
        return false;
    }
    
    // Simple JSON-like format for scene serialization
    file << "{\n";
    file << "  \"name\": \"" << m_name << "\",\n";
    file << "  \"entities\": [\n";
    
    for (size_t i = 0; i < m_entities.size(); ++i) {
        const Entity* entity = m_entities[i].get();
        const Transform& transform = entity->GetTransform();
        
        file << "    {\n";
        file << "      \"id\": " << entity->GetId() << ",\n";
        file << "      \"name\": \"" << entity->GetName() << "\",\n";
        file << "      \"visible\": " << (entity->IsVisible() ? "true" : "false") << ",\n";
        file << "      \"parent\": " << (entity->GetParent() ? entity->GetParent()->GetId() : 0) << ",\n";
        file << "      \"transform\": {\n";
        file << "        \"position\": [" << transform.position.x << ", " << transform.position.y << ", " << transform.position.z << "],\n";
        file << "        \"rotation\": [" << transform.rotation.x << ", " << transform.rotation.y << ", " << transform.rotation.z << ", " << transform.rotation.w << "],\n";
        file << "        \"scale\": [" << transform.scale.x << ", " << transform.scale.y << ", " << transform.scale.z << "]\n";
        file << "      }\n";
        file << "    }";
        
        if (i < m_entities.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    DAISY_INFO("Saved scene to: {}", filepath);
    return true;
}

bool Scene::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        DAISY_ERROR("Failed to open file for reading: {}", filepath);
        return false;
    }
    
    // Clear current scene
    Clear();
    
    // Simple parsing (in a real implementation, would use JSON library)
    std::string line;
    bool inEntities = false;
    
    while (std::getline(file, line)) {
        // Very basic parsing - would need proper JSON parser
        if (line.find("\"name\"") != std::string::npos) {
            size_t start = line.find("\"", line.find(":")) + 1;
            size_t end = line.find("\"", start);
            if (start != std::string::npos && end != std::string::npos) {
                m_name = line.substr(start, end - start);
            }
        }
        
        // More parsing would be implemented here...
        // For now, just create a placeholder entity
    }
    
    // Create a default entity since we don't have full parsing
    CreateEntity("Default Entity");
    
    DAISY_INFO("Loaded scene from: {}", filepath);
    return true;
}

void Scene::Clear() {
    m_entities.clear();
    m_entityMap.clear();
    m_nextEntityId = 1;
    DAISY_INFO("Cleared scene");
}

}