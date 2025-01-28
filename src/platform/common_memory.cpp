// common_memory.cpp
#include <cstdint>
#include <platform/common_memory.h>
#include <platform/log.h>
#include <utils/algoritom.h>

namespace LunaVoxalEngine::Platform
{
static MemoryManager instance;

MemoryManager::MemoryManager()
    : pools(nullptr)
    , total_allocated(0)
    , total_used(0)
{
}

MemoryManager::~MemoryManager()
{
    // We need to clean up all the pools that have been allocated.
    // We start by keeping track of the current pool and the next pool in the list.
    // This is done so that we can deallocate the current pool without losing track of the next one.
    Pool *current = pools;
    while (current)
    {
        // Store the next pool in the list in case we need to free it later.
        Pool *next = current->next_pool;

        // Free the memory block for the current pool.
        // This memory was allocated in the create_pool function.
        os_free(current->first_block);

        // Free the memory for the current pool itself.
        // This memory was also allocated in the create_pool function.
        os_free(current);

        // Move on to the next pool in the list.
        current = next;
    }

    // We've finished cleaning up all the pools, so we can reset the first pool to nullptr.
    pools = nullptr;
}

MemoryManager &MemoryManager::get_instance()
{
    return instance;
}

// This function takes a pointer to a memory address and an alignment value.
// It returns a new pointer that is aligned to the specified alignment.
// The new pointer is calculated by adding the alignment minus one to the
// original pointer, and then using a bitwise AND with the negated alignment
// to clear the lower bits. This effectively rounds the address up to the
// next multiple of the alignment.
// The reason for adding alignment minus one is to ensure that any remainder
// when dividing by the alignment is accounted for, effectively rounding up.
// The bitwise AND with the negated alignment helps to clear the lower bits,
// making the result a multiple of the alignment.
void *MemoryManager::align_address(void *ptr, size_t alignment)
{
    // Calculate the aligned address by adding the alignment minus one
    // to the original address. This ensures that any remainder when
    // dividing by the alignment is accounted for, effectively
    // rounding up to the next multiple of the alignment.
    // The bitwise AND with the negated alignment helps to clear
    // the lower bits, making the result a multiple of the alignment.
    return reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) + (alignment - 1) & ~(alignment - 1));
}

// This function aligns the given size to the specified alignment.
// It returns the smallest size that is a multiple of the alignment
// and greater than or equal to the original size.
size_t MemoryManager::align_size(size_t size, size_t alignment)
{
    // Calculate the aligned size by adding the alignment minus one
    // to the original size. This ensures that any remainder when
    // dividing by the alignment is accounted for, effectively
    // rounding up to the next multiple of the alignment.
    // The bitwise AND with the negated alignment helps to clear
    // the lower bits, making the result a multiple of the alignment.
    return (size + alignment - 1) & ~(alignment - 1);
}

MemoryManager::Pool *MemoryManager::create_pool(size_t min_size)
{
    // Determine the pool size, which is the maximum of the requested minimum size and a predefined constant.
    size_t pool_size = Utils::max(min_size, POOL_SIZE);

    // Allocate memory for the entire pool structure, which includes the pool metadata and the memory blocks.
    Pool *pool = reinterpret_cast<Pool *>(os_malloc(sizeof(Pool) + pool_size));

    // If the allocation failed, return nullptr to indicate an error.
    if (!pool) 
        return nullptr;

    // Initialize the pool's total size with the calculated pool size.
    pool->total_size = pool_size;

    // Set the next pool pointer to nullptr, indicating that this is the last pool in the list for now.
    pool->next_pool = nullptr;

    // Initialize the first block in the pool to point to the memory immediately following the pool metadata.
    pool->first_block = reinterpret_cast<Block *>(pool + 1);

    // Set the size of the first block to the remaining memory after accounting for the block's metadata.
    pool->first_block->size = pool_size - sizeof(Block);

    // Mark the first block as unused initially.
    pool->first_block->used = false;

    // Set the pointer to the next block to nullptr as there are no more blocks yet.
    pool->first_block->next = nullptr;

    // Set the alignment of the block to the default alignment.
    pool->first_block->alignment = DEFAULT_ALIGN;

    // Update the total allocated memory size by adding the pool size.
    total_allocated += pool_size;

    // Return the pointer to the newly created pool.
    return pool;
}

MemoryManager::Block *MemoryManager::find_block(size_t size, size_t alignment) noexcept
{
    // Find a block of memory that is free and large enough to satisfy
    // the allocation request.
    for (Pool *pool = pools; pool; pool = pool->next_pool)
    {
        // Iterate through all the blocks in the pool.
        Block *block = pool->first_block;
        while (block)
        {
            // Calculate the address of the aligned block.
            void *aligned_addr = align_address(block + 1, alignment);

            // Calculate the offset between the aligned address and the
            // start of the block. This is the amount of memory that will
            // be wasted if we use this block.
            auto const offset = reinterpret_cast<char *>(aligned_addr) - reinterpret_cast<char *>(block + 1);

            // Check if the block is large enough to satisfy the allocation
            // request, and if the block is not already used.
            if (block->size >= size + offset && !block->used)
            {
                // If the block is large enough, return it.
                return block;
            }

            // Move on to the next block in the linked list.
            block = block->next;
        }
    }
    // If no free block was found, return nullptr.
    return nullptr;
}

void MemoryManager::coalesce()
{
    // Iterate through all the pools.
    for (Pool *pool = pools; pool; pool = pool->next_pool)
    {
        // Iterate through all the blocks in the pool.
        Block *block = pool->first_block;
        while (block && block->next)
        {
            // If the current block is used or the next block is used, we can't coalesce them.
            if (block->used || block->next->used)
            {
                // Just move on to the next block.
                block = block->next;
            }
            else
            {
                // We can coalesce them, so add the size of the next block to the current block.
                block->size += sizeof(Block) + block->next->size;

                // Then, make the next block of the current block point to the block after the next block.
                block->next = block->next->next;
            }
        }
    }
}

void *MemoryManager::allocate(size_t size, size_t alignment)
{
    // If the size of the allocation is 0, just return nullptr.
    if (size == 0)
        return nullptr;

    // Calculate the size of the allocation, taking into account the alignment.
    size = align_size(size, alignment);

    // Lock the mutex to ensure that only one thread can access the pool at any given time.
    Platform::GuardLock lock(pool_mutex);

    // Search the pools for a free block that is large enough to satisfy the allocation.
    Block *block = find_block(size, alignment);

    // If no free block was found, we need to create a new pool.
    if (!block)
    {
        // Create a new pool that is large enough to satisfy the allocation.
        Pool *new_pool = create_pool(size + sizeof(Block));

        // If the new pool couldn't be created, return nullptr.
        if (!new_pool)
            return nullptr;

        // Add the new pool to the front of the list of pools.
        new_pool->next_pool = pools;
        pools = new_pool;

        // Set the block to the first block in the new pool.
        block = new_pool->first_block;
    }

    // Calculate the address of the aligned block.
    void *aligned_addr = align_address(block + 1, alignment);

    // Calculate the offset between the aligned address and the start of the block.
    size_t offset = (char *)aligned_addr - (char *)(block + 1);

    // Mark the block as used.
    block->used = true;

    // Store the alignment in the block header.
    block->alignment = alignment;

    // Update the total amount of used memory.
    total_used += size;

    // Return the aligned address.
    return aligned_addr;
}

void MemoryManager::deallocate(void *ptr)
{
    // This function is called when someone wants to deallocate some memory.
    // This memory is represented by a pointer 'ptr'.
    if (!ptr)
        return;

    // We subtract 1 from the pointer to get to the address of the block that
    // contains the information about the size of the block.
    Block *block = ((Block *)ptr) - 1;

    {
        // We lock the mutex to ensure that only one thread can access the pool
        // at any given time.
        Platform::GuardLock lock(pool_mutex);

        // We mark the block as unused.
        block->used = false;

        // We subtract the size of the block from the total amount of used memory.
        total_used -= block->size;
    }

    // We call coalesce to merge the deallocated block with any adjacent free
    // blocks.
    coalesce();
}

// Static operator new/delete implementations
void *MemoryManager::operator_new(size_t size)
{
    return get_instance().allocate(size);
}

void *MemoryManager::operator_new_aligned(size_t size, size_t alignment)
{
    return get_instance().allocate(size, alignment);
}

void *MemoryManager::operator_new_array(size_t size)
{
    size_t *ptr = (size_t *)get_instance().allocate(size + sizeof(size_t));
    if (ptr)
    {
        *ptr = size;
        return (ptr + 1);
    }
    return nullptr;
}

void MemoryManager::operator_delete(void *ptr) noexcept
{
    if (ptr)
    {
        get_instance().deallocate(ptr);
    }
}

void MemoryManager::operator_delete_array(void *ptr) noexcept
{
    if (ptr)
    {
        size_t *size_ptr = ((size_t *)ptr) - 1;
        get_instance().deallocate(size_ptr);
    }
}

} // namespace LunaVoxalEngine::Platform
