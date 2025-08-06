#include "EditorCore/AssetManager.h"
#include "Core/Logger.h"
#include <filesystem>
#include <algorithm>

namespace DaisyEditor {

AssetManager::AssetManager() {
    // Initialize extension mapping
    m_extensionMap[".dscene"] = AssetType::Scene;
    m_extensionMap[".obj"] = AssetType::Mesh;
    m_extensionMap[".fbx"] = AssetType::Mesh;
    m_extensionMap[".gltf"] = AssetType::Mesh;
    m_extensionMap[".glb"] = AssetType::Mesh;
    
    m_extensionMap[".png"] = AssetType::Texture;
    m_extensionMap[".jpg"] = AssetType::Texture;
    m_extensionMap[".jpeg"] = AssetType::Texture;
    m_extensionMap[".tga"] = AssetType::Texture;
    m_extensionMap[".bmp"] = AssetType::Texture;
    m_extensionMap[".dds"] = AssetType::Texture;
    
    m_extensionMap[".dmat"] = AssetType::Material;
    
    m_extensionMap[".wav"] = AssetType::Sound;
    m_extensionMap[".mp3"] = AssetType::Sound;
    m_extensionMap[".ogg"] = AssetType::Sound;
    
    m_extensionMap[".ds"] = AssetType::Script;    // Daisy Script
    m_extensionMap[".lua"] = AssetType::Script;
    m_extensionMap[".js"] = AssetType::Script;
    
    m_extensionMap[".vert"] = AssetType::Shader;
    m_extensionMap[".frag"] = AssetType::Shader;
    m_extensionMap[".comp"] = AssetType::Shader;
    m_extensionMap[".hlsl"] = AssetType::Shader;
    m_extensionMap[".glsl"] = AssetType::Shader;
    
    m_extensionMap[".ttf"] = AssetType::Font;
    m_extensionMap[".otf"] = AssetType::Font;
}

bool AssetManager::Initialize(const std::string& assetsDirectory) {
    m_assetsDirectory = assetsDirectory;
    
    // Create assets directory if it doesn't exist
    try {
        if (!std::filesystem::exists(m_assetsDirectory)) {
            std::filesystem::create_directories(m_assetsDirectory);
            DAISY_INFO("Created assets directory: {}", m_assetsDirectory);
        }
        
        // Create default subdirectories
        std::vector<std::string> defaultDirs = {
            "Scenes", "Meshes", "Textures", "Materials", 
            "Sounds", "Scripts", "Shaders", "Fonts"
        };
        
        for (const std::string& dir : defaultDirs) {
            std::string fullPath = m_assetsDirectory + "/" + dir;
            if (!std::filesystem::exists(fullPath)) {
                std::filesystem::create_directories(fullPath);
            }
        }
        
    } catch (const std::exception& e) {
        DAISY_ERROR("Failed to create assets directory: {}", e.what());
        return false;
    }
    
    RefreshAssets();
    
    DAISY_INFO("Asset Manager initialized - Assets directory: {}", m_assetsDirectory);
    return true;
}

void AssetManager::Shutdown() {
    m_assets.clear();
    m_assetMap.clear();
    m_directories.clear();
}

void AssetManager::RefreshAssets() {
    m_assets.clear();
    m_assetMap.clear();
    m_directories.clear();
    
    ScanDirectory(m_assetsDirectory);
    
    DAISY_INFO("Refreshed assets - Found {} assets", m_assets.size());
}

void AssetManager::ScanDirectory(const std::string& directory) {
    try {
        if (!std::filesystem::exists(directory)) {
            return;
        }
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                ProcessFile(entry.path().string());
            } else if (entry.is_directory()) {
                m_directories.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        DAISY_ERROR("Failed to scan directory {}: {}", directory, e.what());
    }
}

std::vector<AssetInfo> AssetManager::GetAssetsByType(AssetType type) const {
    std::vector<AssetInfo> filtered;
    
    for (const AssetInfo& asset : m_assets) {
        if (asset.type == type) {
            filtered.push_back(asset);
        }
    }
    
    return filtered;
}

AssetInfo* AssetManager::FindAsset(const std::string& filepath) {
    auto it = m_assetMap.find(filepath);
    return it != m_assetMap.end() ? it->second : nullptr;
}

AssetType AssetManager::GetAssetTypeFromExtension(const std::string& extension) const {
    std::string lowerExt = extension;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
    
    auto it = m_extensionMap.find(lowerExt);
    return it != m_extensionMap.end() ? it->second : AssetType::Unknown;
}

std::string AssetManager::GetAssetTypeString(AssetType type) const {
    switch (type) {
        case AssetType::Scene:    return "Scene";
        case AssetType::Mesh:     return "Mesh";
        case AssetType::Texture:  return "Texture";
        case AssetType::Material: return "Material";
        case AssetType::Sound:    return "Sound";
        case AssetType::Script:   return "Script";
        case AssetType::Shader:   return "Shader";
        case AssetType::Font:     return "Font";
        default:                  return "Unknown";
    }
}

bool AssetManager::ImportAsset(const std::string& filepath) {
    if (!IsValidAssetFile(filepath)) {
        DAISY_WARNING("Invalid asset file: {}", filepath);
        return false;
    }
    
    ProcessFile(filepath);
    DAISY_INFO("Imported asset: {}", filepath);
    return true;
}

bool AssetManager::DeleteAsset(const std::string& filepath) {
    try {
        if (std::filesystem::exists(filepath)) {
            std::filesystem::remove(filepath);
            
            // Remove from our lists
            auto it = m_assetMap.find(filepath);
            if (it != m_assetMap.end()) {
                m_assets.erase(std::remove_if(m_assets.begin(), m_assets.end(),
                    [&filepath](const AssetInfo& asset) {
                        return asset.filepath == filepath;
                    }), m_assets.end());
                m_assetMap.erase(it);
            }
            
            DAISY_INFO("Deleted asset: {}", filepath);
            return true;
        }
    } catch (const std::exception& e) {
        DAISY_ERROR("Failed to delete asset {}: {}", filepath, e.what());
    }
    
    return false;
}

bool AssetManager::RenameAsset(const std::string& oldPath, const std::string& newPath) {
    try {
        if (std::filesystem::exists(oldPath)) {
            std::filesystem::rename(oldPath, newPath);
            
            // Update our records
            auto it = m_assetMap.find(oldPath);
            if (it != m_assetMap.end()) {
                AssetInfo* asset = it->second;
                asset->filepath = newPath;
                // Update name from new path
                size_t lastSlash = newPath.find_last_of("/\\");
                asset->name = (lastSlash != std::string::npos) ? newPath.substr(lastSlash + 1) : newPath;
                
                m_assetMap.erase(it);
                m_assetMap[newPath] = asset;
            }
            
            DAISY_INFO("Renamed asset: {} -> {}", oldPath, newPath);
            return true;
        }
    } catch (const std::exception& e) {
        DAISY_ERROR("Failed to rename asset {}: {}", oldPath, e.what());
    }
    
    return false;
}

bool AssetManager::MoveAsset(const std::string& sourcePath, const std::string& destPath) {
    return RenameAsset(sourcePath, destPath);
}

bool AssetManager::CreateDirectory(const std::string& path) {
    try {
        if (std::filesystem::create_directories(path)) {
            m_directories.push_back(path);
            DAISY_INFO("Created directory: {}", path);
            return true;
        }
    } catch (const std::exception& e) {
        DAISY_ERROR("Failed to create directory {}: {}", path, e.what());
    }
    
    return false;
}

bool AssetManager::DeleteDirectory(const std::string& path) {
    try {
        if (std::filesystem::remove_all(path)) {
            // Remove from directories list
            m_directories.erase(
                std::remove(m_directories.begin(), m_directories.end(), path),
                m_directories.end());
            
            // Remove all assets in this directory
            m_assets.erase(
                std::remove_if(m_assets.begin(), m_assets.end(),
                    [&path](const AssetInfo& asset) {
                        return asset.filepath.find(path) == 0;
                    }), m_assets.end());
            
            // Update asset map
            auto it = m_assetMap.begin();
            while (it != m_assetMap.end()) {
                if (it->first.find(path) == 0) {
                    it = m_assetMap.erase(it);
                } else {
                    ++it;
                }
            }
            
            DAISY_INFO("Deleted directory: {}", path);
            return true;
        }
    } catch (const std::exception& e) {
        DAISY_ERROR("Failed to delete directory {}: {}", path, e.what());
    }
    
    return false;
}

std::vector<std::string> AssetManager::GetDirectories() const {
    return m_directories;
}

void AssetManager::ProcessFile(const std::string& filepath) {
    if (!IsValidAssetFile(filepath)) {
        return;
    }
    
    std::string extension = std::filesystem::path(filepath).extension().string();
    AssetType type = GetAssetTypeFromExtension(extension);
    
    AssetInfo asset(filepath, type);
    UpdateAssetInfo(asset);
    
    m_assets.push_back(asset);
    m_assetMap[filepath] = &m_assets.back();
}

bool AssetManager::IsValidAssetFile(const std::string& filepath) const {
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return m_extensionMap.find(extension) != m_extensionMap.end();
}

void AssetManager::UpdateAssetInfo(AssetInfo& asset) {
    try {
        if (std::filesystem::exists(asset.filepath)) {
            asset.fileSize = std::filesystem::file_size(asset.filepath);
            
            auto writeTime = std::filesystem::last_write_time(asset.filepath);
            // Convert to string (simplified)
            asset.lastModified = "Recently"; // Would implement proper time formatting
        }
    } catch (const std::exception& e) {
        DAISY_WARNING("Failed to update asset info for {}: {}", asset.filepath, e.what());
    }
}

}