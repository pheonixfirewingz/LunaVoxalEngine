// common_memory.cpp
#include <platform/common_memory.h>
#include <utils/algoritom.h>
#include <platform/log.h>

namespace LunaVoxalEngine::Platform
{
static MemoryManager manager;

MemoryManager* getGlobalMemoryManager()
{
    return &manager;
}

struct MemoryManager::AllocationInfo
{
    void* ptr;
    unsigned long size;
    const char* file;
    int line;
    AllocationInfo* next;
};

struct MemoryManager::Block
{
    Block* next;
    unsigned long size;
    unsigned long bucket_index;
    AllocationInfo* info;
};

struct MemoryManager::Bucket
{
    Block* free_blocks;
    unsigned long block_size;
    unsigned long num_blocks;
    unsigned long max_blocks;
};

struct MemoryManager::Chunk
{
    Chunk* next;
    char* memory;
    unsigned long size;
    unsigned long used;
};

MemoryManager::MemoryManager()
    : chunks(nullptr)
    , allocations(nullptr)
    , totalAllocated(0)
    , peakAllocation(0)
    , allocationCount(0)
    , defragThreshold(1000)
    , allocsSinceDefrag(0)
{
    buckets = (Bucket*)os_calloc(NUM_BUCKETS, sizeof(Bucket));
    initBuckets();
}

MemoryManager::~MemoryManager()
{
    reportLeaks();

    while (allocations)
    {
        AllocationInfo* next = allocations->next;
        os_free(allocations);
        allocations = next;
    }

    while (chunks)
    {
        Chunk* next = chunks->next;
        os_free(chunks);
        chunks = next;
    }

    os_free(buckets);
}

void MemoryManager::initBuckets()
{
    unsigned long size = MIN_ALLOC;
    static const unsigned long CHUNK_SIZE = 64 * 1024; // 64KB chunks

    for (unsigned long i = 0; i < NUM_BUCKETS; i++)
    {
        buckets[i].block_size = size;
        buckets[i].free_blocks = nullptr;
        buckets[i].num_blocks = 0;
        buckets[i].max_blocks = CHUNK_SIZE / size;

        if (size < 256)
            size *= 2;
        else if (size < 1024)
            size += 256;
        else if (size < 4096)
            size += 1024;
        else
            size *= 2;
    }
}

unsigned long MemoryManager::getBucketIndex(unsigned long size)
{
    for (unsigned long i = 0; i < NUM_BUCKETS; i++)
    {
        if (size <= buckets[i].block_size)
            return i;
    }
    return NUM_BUCKETS - 1;
}

unsigned long MemoryManager::align(unsigned long n)
{
    static const unsigned long ALIGNMENT = 8;
    return (n + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

unsigned long MemoryManager::alignAddress(unsigned long address, unsigned long alignment)
{
    return (address + (alignment - 1)) & ~(alignment - 1);
}

void MemoryManager::allocateChunkToBucket(unsigned long bucket_index)
{
    static const unsigned long CHUNK_SIZE = 64 * 1024;
    unsigned long block_size = buckets[bucket_index].block_size;

    Chunk* chunk = (Chunk*)os_calloc(1, CHUNK_SIZE);
    if (!chunk)
        return;

    chunk->memory = (char*)(chunk + 1);
    chunk->size = CHUNK_SIZE - sizeof(Chunk);
    chunk->used = 0;
    chunk->next = chunks;
    chunks = chunk;

    char* curr = chunk->memory;
    char* end = curr + chunk->size;
    while (curr + block_size <= end)
    {
        Block* block = (Block*)curr;
        block->size = block_size;
        block->bucket_index = bucket_index;
        block->next = buckets[bucket_index].free_blocks;
        buckets[bucket_index].free_blocks = block;
        buckets[bucket_index].num_blocks++;
        curr += block_size;
    }
}

MemoryManager::Block* MemoryManager::allocateAlignedBlock(unsigned long size, unsigned long alignment, unsigned long bucket_index)
{
    Block* block = buckets[bucket_index].free_blocks;
    while (block)
    {
        unsigned long blockAddress = (unsigned long)(block + 1);
        unsigned long alignedAddress = alignAddress(blockAddress, alignment);
        unsigned long adjustment = alignedAddress - blockAddress;

        if (block->size >= size + adjustment)
        {
            buckets[bucket_index].free_blocks = block->next;
            buckets[bucket_index].num_blocks--;

            if (adjustment > 0)
            {
                Block* alignedBlock = (Block*)((char*)block + adjustment);
                alignedBlock->size = block->size - adjustment;
                alignedBlock->bucket_index = bucket_index;
                alignedBlock->info = nullptr;

                if (adjustment >= sizeof(Block) + MIN_ALLOC)
                {
                    block->size = adjustment;
                    block->next = buckets[bucket_index].free_blocks;
                    buckets[bucket_index].free_blocks = block;
                    buckets[bucket_index].num_blocks++;
                }
                else
                {
                    alignedBlock->next = buckets[bucket_index].free_blocks;
                    buckets[bucket_index].free_blocks = alignedBlock;
                }

                return alignedBlock;
            }

            return block;
        }
        block = block->next;
    }

    return nullptr;
}

void MemoryManager::trackAllocation(Block* block, const char* file, int line)
{
    AllocationInfo* info = (AllocationInfo*)os_calloc(1, sizeof(AllocationInfo));
    if (!info)
        return;

    info->ptr = (void*)(block + 1);
    info->size = block->size - sizeof(Block);
    info->file = file;
    info->line = line;
    info->next = allocations;
    allocations = info;
    block->info = info;

    totalAllocated += info->size;
    allocationCount++;
    if (totalAllocated > peakAllocation)
    {
        peakAllocation = totalAllocated;
    }
}

void MemoryManager::untrackAllocation(Block* block)
{
    AllocationInfo** pp = &allocations;
    while (*pp)
    {
        if ((*pp)->ptr == (void*)(block + 1))
        {
            AllocationInfo* info = *pp;
            *pp = info->next;
            totalAllocated -= info->size;
            allocationCount--;
            os_free(info);
            return;
        }
        pp = &(*pp)->next;
    }
}

void* MemoryManager::allocate(unsigned long size, const char* file, int line)
{
    allocsSinceDefrag++;
    if (allocsSinceDefrag >= defragThreshold)
    {
        defragment();
    }

    if (size == 0)
        return nullptr;

    size = align(size + sizeof(Block));
    unsigned long bucket_index = getBucketIndex(size);

    if (buckets[bucket_index].free_blocks)
    {
        Block* block = buckets[bucket_index].free_blocks;
        buckets[bucket_index].free_blocks = block->next;
        buckets[bucket_index].num_blocks--;
        trackAllocation(block, file, line);
        return (void*)(block + 1);
    }

    allocateChunkToBucket(bucket_index);

    if (buckets[bucket_index].free_blocks)
    {
        Block* block = buckets[bucket_index].free_blocks;
        buckets[bucket_index].free_blocks = block->next;
        buckets[bucket_index].num_blocks--;
        trackAllocation(block, file, line);
        return (void*)(block + 1);
    }

    return nullptr;
}

void* MemoryManager::allocateAligned(unsigned long size, unsigned long alignment, const char* file, int line)
{
    allocsSinceDefrag++;
    if (allocsSinceDefrag >= defragThreshold)
    {
        defragment();
    }

    if (size == 0)
    {
        return nullptr;
    }

    // Ensure alignment is a power of 2
    if (alignment == 0 || (alignment & (alignment - 1)) != 0)
    {
        Log::debug("Alignment must be a power of 2");
        return nullptr;
    }

    // Ensure alignment is at least as strict as the default alignment
    alignment = Utils::max(alignment, (unsigned long)8);

    // Calculate the total size needed including potential padding
    unsigned long totalSize = size + alignment + sizeof(Block);
    unsigned long bucket_index = getBucketIndex(totalSize);

    // Try to allocate from existing blocks
    Block* block = allocateAlignedBlock(totalSize, alignment, bucket_index);
    
    if (!block)
    {
        // Allocate a new chunk and try again
        allocateChunkToBucket(bucket_index);
        block = allocateAlignedBlock(totalSize, alignment, bucket_index);
        
        if (!block)
        {
            return nullptr;
        }
    }

    trackAllocation(block, file, line);
    return (void*)(block + 1);
}

void MemoryManager::deallocate(void* ptr)
{
    if (!ptr)
        return;
    Block* block = ((Block*)ptr) - 1;
    unsigned long bucket_index = block->bucket_index;
    untrackAllocation(block);

    Utils::memset(ptr, 0, block->size - sizeof(Block));

    if (buckets[bucket_index].num_blocks < buckets[bucket_index].max_blocks)
    {
        block->next = buckets[bucket_index].free_blocks;
        buckets[bucket_index].free_blocks = block;
        buckets[bucket_index].num_blocks++;
    }
}

void MemoryManager::defragment()
{
    allocsSinceDefrag = 0;
    return;
    //TODO: this basiclly soft locks the engine for an infinite amount of time for defragmentation this is not good
    unsigned long freeBlocks = 0;
    for (unsigned long i = 0; i < NUM_BUCKETS; i++)
    {
        freeBlocks += buckets[i].num_blocks;
    }

    Chunk* chunk = chunks;
    while (chunk)
    {
        compactChunk(chunk);
        chunk = chunk->next;
    }

    unsigned long finalFreeBlocks = 0;
    for (unsigned long i = 0; i < NUM_BUCKETS; i++)
    {
        finalFreeBlocks += buckets[i].num_blocks;
    }
    allocsSinceDefrag = 0;
    Log::debug("Defragmentation complete:");
    Log::debug("- Initial free blocks: %zu", freeBlocks);
    Log::debug("- Final free blocks: %zu", finalFreeBlocks);
    Log::debug("- Blocks consolidated: %zu", freeBlocks - finalFreeBlocks);
}

void MemoryManager::compactChunk(Chunk* chunk)
{
    char* start = chunk->memory;
    char* end = start + chunk->size;
    char* writePtr = start;
    
    Block* curr = (Block*)start;
    while ((char*)curr < end)
    {
        if (curr->info)
        {
            unsigned long blockSize = curr->size;
            if ((char*)curr != writePtr)
            {
                void* oldLocation = (void*)(curr + 1);
                void* newLocation = (void*)(((Block*)writePtr) + 1);
                
                Utils::memmove(writePtr, curr, blockSize);
                
                updatePointers(oldLocation, newLocation, blockSize - sizeof(Block));
            }
            writePtr += curr->size;
        }
        curr = (Block*)((char*)curr + curr->size);
    }
    
    if (writePtr < end)
    {
        Block* freeBlock = (Block*)writePtr;
        freeBlock->size = end - writePtr;
        freeBlock->info = nullptr;
        freeBlock->bucket_index = getBucketIndex(freeBlock->size);
        
        freeBlock->next = buckets[freeBlock->bucket_index].free_blocks;
        buckets[freeBlock->bucket_index].free_blocks = freeBlock;
        buckets[freeBlock->bucket_index].num_blocks++;
    }
}

void MemoryManager::updatePointers(void* oldBase, void* newBase, unsigned long size)
{
    for (AllocationInfo* curr = allocations; curr; curr = curr->next)
    {
        if (curr->ptr >= oldBase && curr->ptr < (char*)oldBase + size)
        {
            curr->ptr = (void*)((char*)curr->ptr - (char*)oldBase + (char*)newBase);
        }
    }
}

void MemoryManager::reportStats()
{
    Log::debug("Memory Manager Statistics:");
    Log::debug("-------------------------");
    Log::debug("Active allocations: %zu", allocationCount);
    Log::debug("Total allocated: %zu bytes", totalAllocated);
    Log::debug("Peak allocation: %zu bytes", peakAllocation);

    Log::debug("\nBucket Statistics:");
    for (unsigned long i = 0; i < NUM_BUCKETS; i++)
    {
        if (buckets[i].num_blocks > 0)
        {
            Log::debug("Bucket %zu (%zu bytes): %zu free blocks", 
                      i, 
                      buckets[i].block_size, 
                      buckets[i].num_blocks);
        }
    }

    // Add aligned allocation statistics
    Log::debug("Alignment Statistics:");
    Log::debug("Last defrag: %zu allocations ago", allocsSinceDefrag);
}

void MemoryManager::reportLeaks()
{
    if (!allocations)
    {
        Log::debug("\nNo memory leaks detected.\n");
        return;
    }

    Log::debug("Memory Leaks Detected:");
    Log::debug("---------------------");
    unsigned long totalLeaked = 0;
    unsigned long leakCount = 0;

    AllocationInfo* curr = allocations;
    while (curr)
    {
        Log::debug("Leak: %zu bytes at %p", curr->size, curr->ptr);
        Log::debug("  Allocated in %s, line %d", curr->file, curr->line);
        totalLeaked += curr->size;
        leakCount++;
        curr = curr->next;
    }

    Log::debug("Total: %zu leaks, %zu bytes", leakCount, totalLeaked);
}

} // namespace LunaVoxalEngine::Platform
