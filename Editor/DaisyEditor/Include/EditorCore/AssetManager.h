#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace DaisyEditor {

enum class AssetType {
    Unknown,
    Scene,
    Mesh,
    Texture,
    Material,
    Sound,
    Script,
    Shader,
    Font
};

struct AssetInfo {
    std::string filepath;
    std::string name;
    AssetType type;
    size_t fileSize = 0;
    std::string lastModified;
    bool loaded = false;
    
    AssetInfo() = default;
    AssetInfo(const std::string& path, AssetType t) : filepath(path), type(t) {
        // Extract name from filepath
        size_t lastSlash = path.find_last_of("/\\");
        name = (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;
    }
};

class AssetManager {
public:
    AssetManager();
    ~AssetManager() = default;
    
    bool Initialize(const std::string& assetsDirectory = "Assets");
    void Shutdown();
    
    void RefreshAssets();
    void ScanDirectory(const std::string& directory);
    
    const std::vector<AssetInfo>& GetAllAssets() const { return m_assets; }
    std::vector<AssetInfo> GetAssetsByType(AssetType type) const;
    AssetInfo* FindAsset(const std::string& filepath);
    
    AssetType GetAssetTypeFromExtension(const std::string& extension) const;
    std::string GetAssetTypeString(AssetType type) const;
    
    // Asset operations
    bool ImportAsset(const std::string& filepath);
    bool DeleteAsset(const std::string& filepath);
    bool RenameAsset(const std::string& oldPath, const std::string& newPath);
    bool MoveAsset(const std::string& sourcePath, const std::string& destPath);
    
    // Directory operations
    bool CreateDirectory(const std::string& path);
    bool DeleteDirectory(const std::string& path);
    std::vector<std::string> GetDirectories() const;
    
    const std::string& GetAssetsDirectory() const { return m_assetsDirectory; }
    
private:
    void ProcessFile(const std::string& filepath);
    bool IsValidAssetFile(const std::string& filepath) const;
    void UpdateAssetInfo(AssetInfo& asset);
    
    std::string m_assetsDirectory;
    std::vector<AssetInfo> m_assets;
    std::unordered_map<std::string, AssetInfo*> m_assetMap;
    std::vector<std::string> m_directories;
    
    std::unordered_map<std::string, AssetType> m_extensionMap;
};

}