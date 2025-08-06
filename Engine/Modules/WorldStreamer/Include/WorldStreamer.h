#pragma once

#include "Core/Module.h"
#include "Core/Math.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace Daisy {

struct WorldChunk {
    Vector3 position;
    uint32_t size = 1000;
    bool loaded = false;
    bool generated = false;
    std::vector<uint32_t> renderObjects;
    std::vector<uint32_t> physicsObjects;
    std::vector<uint32_t> aiAgents;
    float lastAccessTime = 0.0f;
};

struct StreamingSettings {
    float loadRadius = 5000.0f;
    float unloadRadius = 8000.0f;
    float predictionRadius = 10000.0f;
    int maxConcurrentLoads = 4;
    bool enablePredictiveStreaming = true;
    bool enableServerSideStreaming = true;
};

class WorldStreamer : public Module {
public:
    WorldStreamer();
    virtual ~WorldStreamer() = default;
    
    bool Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    
    void SetObserverPosition(const Vector3& position);
    void SetStreamingSettings(const StreamingSettings& settings);
    
    WorldChunk* GetChunk(const Vector3& worldPosition);
    std::vector<WorldChunk*> GetLoadedChunks();
    
    void GenerateChunk(WorldChunk& chunk);
    void LoadChunk(const Vector3& chunkPosition);
    void UnloadChunk(const Vector3& chunkPosition);
    
    void EnableInfiniteWorld(bool enable) { m_infiniteWorldEnabled = enable; }
    void SetWorldScale(double scale) { m_worldScale = scale; }
    
private:
    Vector3 WorldToChunkPosition(const Vector3& worldPos);
    uint64_t ChunkPositionToKey(const Vector3& chunkPos);
    Vector3 KeyToChunkPosition(uint64_t key);
    
    void UpdateStreaming();
    void PredictiveLoading();
    void CleanupUnusedChunks();
    
    std::unordered_map<uint64_t, std::unique_ptr<WorldChunk>> m_chunks;
    Vector3 m_observerPosition{0, 0, 0};
    Vector3 m_lastObserverPosition{0, 0, 0};
    
    StreamingSettings m_settings;
    
    bool m_infiniteWorldEnabled = true;
    double m_worldScale = 1e12; // Observable universe scale
    
    float m_streamingUpdateTimer = 0.0f;
    std::vector<Vector3> m_chunksToLoad;
    std::vector<Vector3> m_chunksToUnload;
    
    int m_currentLoadingJobs = 0;
};

}