#pragma once

#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <queue>
#include <vector>

namespace Daisy {

class MemoryTracker {
public:
    static MemoryTracker& GetInstance();
    
    void RecordAllocation(void* ptr, size_t size, const char* file, int line);
    void RecordDeallocation(void* ptr);
    
    size_t GetTotalAllocated() const { return m_totalAllocated.load(); }
    size_t GetActiveAllocations() const { return m_activeAllocations.load(); }
    
    void PrintMemoryReport() const;
    
private:
    struct AllocationInfo {
        size_t size;
        const char* file;
        int line;
    };
    
    std::unordered_map<void*, AllocationInfo> m_allocations;
    mutable std::mutex m_mutex;
    std::atomic<size_t> m_totalAllocated{0};
    std::atomic<size_t> m_activeAllocations{0};
};

template<typename T>
class PoolAllocator {
public:
    explicit PoolAllocator(size_t poolSize = 1024) : m_poolSize(poolSize) {
        m_pool.reserve(m_poolSize);
        for (size_t i = 0; i < m_poolSize; ++i) {
            m_pool.emplace_back(std::make_unique<T>());
            m_available.push(m_pool.back().get());
        }
    }
    
    T* Acquire() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_available.empty()) {
            return new T();
        }
        
        T* obj = m_available.front();
        m_available.pop();
        return obj;
    }
    
    void Release(T* obj) {
        if (!obj) return;
        
        std::lock_guard<std::mutex> lock(m_mutex);
        
        bool isFromPool = false;
        for (const auto& poolObj : m_pool) {
            if (poolObj.get() == obj) {
                isFromPool = true;
                break;
            }
        }
        
        if (isFromPool) {
            m_available.push(obj);
        } else {
            delete obj;
        }
    }
    
private:
    std::vector<std::unique_ptr<T>> m_pool;
    std::queue<T*> m_available;
    std::mutex m_mutex;
    size_t m_poolSize;
};

template<typename T, typename... Args>
std::unique_ptr<T> MakeUnique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
std::shared_ptr<T> MakeShared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

}

#ifdef DAISY_DEBUG
    #define DAISY_NEW(type, ...) \
        ([&]() { \
            auto ptr = new type(__VA_ARGS__); \
            Daisy::MemoryTracker::GetInstance().RecordAllocation(ptr, sizeof(type), __FILE__, __LINE__); \
            return ptr; \
        })()
    
    #define DAISY_DELETE(ptr) \
        do { \
            if (ptr) { \
                Daisy::MemoryTracker::GetInstance().RecordDeallocation(ptr); \
                delete ptr; \
                ptr = nullptr; \
            } \
        } while(0)
#else
    #define DAISY_NEW(type, ...) new type(__VA_ARGS__)
    #define DAISY_DELETE(ptr) delete ptr; ptr = nullptr
#endif