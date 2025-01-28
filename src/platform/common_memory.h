// common_memory.h
#ifndef MEMORY_H
#define MEMORY_H

namespace LunaVoxalEngine::Platform
{
class MemoryManager
{
  private:
    static const unsigned long NUM_BUCKETS = 32;
    static const unsigned long MIN_ALLOC = 16;

    struct AllocationInfo;
    struct Block;
    struct Bucket;
    struct Chunk;

    Chunk* chunks;
    Bucket* buckets;
    AllocationInfo* allocations;
    unsigned long totalAllocated;
    unsigned long peakAllocation;
    unsigned long allocationCount;
    unsigned long defragThreshold;
    unsigned long allocsSinceDefrag;

    void initBuckets();
    unsigned long getBucketIndex(unsigned long size);
    unsigned long align(unsigned long n);
    void allocateChunkToBucket(unsigned long bucket_index);
    void trackAllocation(Block* block, const char* file, int line);
    void untrackAllocation(Block* block);
    void compactChunk(Chunk* chunk);
    void updatePointers(void* oldBase, void* newBase, unsigned long size);
    unsigned long alignAddress(unsigned long address, unsigned long alignment);
    Block* allocateAlignedBlock(unsigned long size, unsigned long alignment, unsigned long bucket_index);

  public:
    MemoryManager();
    ~MemoryManager();

    // Delete copy constructor and assignment operator
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    void* allocate(unsigned long size, const char* file = "unknown", int line = 0);
    void* allocateAligned(unsigned long size, unsigned long alignment, const char* file = "unknown", int line = 0);
    void deallocate(void* ptr);
    void reportStats();
    void reportLeaks();
    void defragment();
};

MemoryManager* getGlobalMemoryManager();
void* os_calloc(unsigned long num, unsigned long size);
void os_free(void* ptr);
} // namespace LunaVoxalEngine::Platform

#endif