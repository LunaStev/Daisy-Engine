#include "Core/Memory.h"
#include "Core/Logger.h"
#include <iostream>

namespace Daisy {

MemoryTracker& MemoryTracker::GetInstance() {
    static MemoryTracker instance;
    return instance;
}

void MemoryTracker::RecordAllocation(void* ptr, size_t size, const char* file, int line) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_allocations[ptr] = {size, file, line};
    m_totalAllocated.fetch_add(size);
    m_activeAllocations.fetch_add(1);
}

void MemoryTracker::RecordDeallocation(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_allocations.find(ptr);
    if (it != m_allocations.end()) {
        m_totalAllocated.fetch_sub(it->second.size);
        m_activeAllocations.fetch_sub(1);
        m_allocations.erase(it);
    }
}

void MemoryTracker::PrintMemoryReport() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    size_t totalActive = 0;
    std::cout << "\n=== Memory Report ===\n";
    std::cout << "Active Allocations: " << m_activeAllocations.load() << "\n";
    std::cout << "Total Allocated: " << m_totalAllocated.load() << " bytes\n";
    
    if (!m_allocations.empty()) {
        std::cout << "\nLeak Details:\n";
        for (const auto& [ptr, info] : m_allocations) {
            std::cout << "  " << ptr << " - " << info.size << " bytes at " 
                     << info.file << ":" << info.line << "\n";
            totalActive += info.size;
        }
        std::cout << "Total leaked: " << totalActive << " bytes\n";
    }
    std::cout << "=====================\n\n";
}

}