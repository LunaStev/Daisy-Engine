#include "WorldStreamer.h"
#include "Core/Logger.h"

namespace Daisy {

WorldStreamer::WorldStreamer() : Module("WorldStreamer") {
}

bool WorldStreamer::Initialize() {
    DAISY_INFO("Initializing World Streamer");
    
    m_chunks.reserve(10000);
    
    m_initialized = true;
    DAISY_INFO("World Streamer initialized successfully");
    return true;
}

void WorldStreamer::Update(float deltaTime) {
    if (!m_initialized) return;
    
    m_streamingUpdateTimer += deltaTime;
    
    if (m_streamingUpdateTimer >= 0.1f) { // Update 10 times per second
        UpdateStreaming();
        
        if (m_settings.enablePredictiveStreaming) {
            PredictiveLoading();
        }
        
        CleanupUnusedChunks();
        m_streamingUpdateTimer = 0.0f;
    }
}

void WorldStreamer::Shutdown() {
    if (!m_initialized) return;
    
    DAISY_INFO("Shutting down World Streamer");
    
    m_chunks.clear();
    m_chunksToLoad.clear();
    m_chunksToUnload.clear();
    
    m_initialized = false;
    DAISY_INFO("World Streamer shut down successfully");
}

void WorldStreamer::SetObserverPosition(const Vector3& position) {
    m_lastObserverPosition = m_observerPosition;
    m_observerPosition = position;
}

void WorldStreamer::SetStreamingSettings(const StreamingSettings& settings) {
    m_settings = settings;
}

WorldChunk* WorldStreamer::GetChunk(const Vector3& worldPosition) {
    Vector3 chunkPos = WorldToChunkPosition(worldPosition);
    uint64_t key = ChunkPositionToKey(chunkPos);
    
    auto it = m_chunks.find(key);
    return it != m_chunks.end() ? it->second.get() : nullptr;
}

std::vector<WorldChunk*> WorldStreamer::GetLoadedChunks() {
    std::vector<WorldChunk*> loadedChunks;
    
    for (auto& [key, chunk] : m_chunks) {
        if (chunk->loaded) {
            loadedChunks.push_back(chunk.get());
        }
    }
    
    return loadedChunks;
}

void WorldStreamer::GenerateChunk(WorldChunk& chunk) {
    if (chunk.generated) return;
    
    // Generate procedural content for this chunk
    // This would involve calling render, physics, and AI modules
    
    chunk.generated = true;
    DAISY_DEBUG("Generated chunk at ({}, {}, {})", chunk.position.x, chunk.position.y, chunk.position.z);
}

void WorldStreamer::LoadChunk(const Vector3& chunkPosition) {
    uint64_t key = ChunkPositionToKey(chunkPosition);
    
    if (m_chunks.find(key) != m_chunks.end()) {
        return; // Already loaded or loading
    }
    
    if (m_currentLoadingJobs >= m_settings.maxConcurrentLoads) {
        m_chunksToLoad.push_back(chunkPosition);
        return;
    }
    
    auto chunk = std::make_unique<WorldChunk>();
    chunk->position = chunkPosition;
    chunk->loaded = false;
    
    GenerateChunk(*chunk);
    chunk->loaded = true;
    chunk->lastAccessTime = 0.0f; // Current time would be used here
    
    m_chunks[key] = std::move(chunk);
    m_currentLoadingJobs++;
    
    DAISY_DEBUG("Loaded chunk at ({}, {}, {})", chunkPosition.x, chunkPosition.y, chunkPosition.z);
}

void WorldStreamer::UnloadChunk(const Vector3& chunkPosition) {
    uint64_t key = ChunkPositionToKey(chunkPosition);
    auto it = m_chunks.find(key);
    
    if (it != m_chunks.end()) {
        DAISY_DEBUG("Unloaded chunk at ({}, {}, {})", chunkPosition.x, chunkPosition.y, chunkPosition.z);
        m_chunks.erase(it);
        m_currentLoadingJobs = std::max(0, m_currentLoadingJobs - 1);
    }
}

Vector3 WorldStreamer::WorldToChunkPosition(const Vector3& worldPos) {
    float chunkSize = 1000.0f; // Configurable chunk size
    return Vector3(
        std::floor(worldPos.x / chunkSize) * chunkSize,
        std::floor(worldPos.y / chunkSize) * chunkSize,
        std::floor(worldPos.z / chunkSize) * chunkSize
    );
}

uint64_t WorldStreamer::ChunkPositionToKey(const Vector3& chunkPos) {
    // Simple hash function for chunk position
    uint64_t x = static_cast<uint64_t>(chunkPos.x / 1000.0f) + 1000000;
    uint64_t y = static_cast<uint64_t>(chunkPos.y / 1000.0f) + 1000000;
    uint64_t z = static_cast<uint64_t>(chunkPos.z / 1000.0f) + 1000000;
    
    return (x << 32) | (y << 16) | z;
}

Vector3 WorldStreamer::KeyToChunkPosition(uint64_t key) {
    uint64_t x = ((key >> 32) & 0xFFFFFFFF) - 1000000;
    uint64_t y = ((key >> 16) & 0xFFFF) - 1000000;
    uint64_t z = (key & 0xFFFF) - 1000000;
    
    return Vector3(x * 1000.0f, y * 1000.0f, z * 1000.0f);
}

void WorldStreamer::UpdateStreaming() {
    // Load chunks within load radius
    int chunkRadius = static_cast<int>(m_settings.loadRadius / 1000.0f);
    
    for (int x = -chunkRadius; x <= chunkRadius; ++x) {
        for (int y = -chunkRadius; y <= chunkRadius; ++y) {
            for (int z = -chunkRadius; z <= chunkRadius; ++z) {
                Vector3 chunkOffset(x * 1000.0f, y * 1000.0f, z * 1000.0f);
                Vector3 chunkPos = WorldToChunkPosition(m_observerPosition + chunkOffset);
                
                float distance = (chunkPos - m_observerPosition).Length();
                
                if (distance <= m_settings.loadRadius) {
                    LoadChunk(chunkPos);
                }
            }
        }
    }
    
    // Unload chunks outside unload radius
    std::vector<uint64_t> chunksToUnload;
    
    for (auto& [key, chunk] : m_chunks) {
        float distance = (chunk->position - m_observerPosition).Length();
        
        if (distance > m_settings.unloadRadius) {
            chunksToUnload.push_back(key);
        }
    }
    
    for (uint64_t key : chunksToUnload) {
        Vector3 chunkPos = KeyToChunkPosition(key);
        UnloadChunk(chunkPos);
    }
}

void WorldStreamer::PredictiveLoading() {
    Vector3 velocity = m_observerPosition - m_lastObserverPosition;
    
    if (velocity.LengthSquared() > 0) {
        Vector3 predictedPosition = m_observerPosition + velocity.Normalized() * m_settings.predictionRadius;
        
        // Load chunks around predicted position
        Vector3 predictedChunk = WorldToChunkPosition(predictedPosition);
        LoadChunk(predictedChunk);
    }
}

void WorldStreamer::CleanupUnusedChunks() {
    // Remove chunks that haven't been accessed recently
    std::vector<uint64_t> chunksToCleanup;
    
    for (auto& [key, chunk] : m_chunks) {
        if (chunk->lastAccessTime > 300.0f) { // 5 minutes
            chunksToCleanup.push_back(key);
        }
    }
    
    for (uint64_t key : chunksToCleanup) {
        m_chunks.erase(key);
    }
}

}