#ifndef NEW_H
#define NEW_H
#include <platform/common_memory.h>
// Global operator overloads
inline void *operator new(size_t size)
{
    return LunaVoxelEngine::Platform::MemoryManager::operator_new(size);
}

inline void *operator new[](size_t size)
{
    return LunaVoxelEngine::Platform::MemoryManager::operator_new_array(size);
}

inline void *operator new(size_t size, std::align_val_t alignment)
{
    return LunaVoxelEngine::Platform::MemoryManager::operator_new_aligned(size, static_cast<size_t>(alignment));
}

inline void *operator new[](size_t size, std::align_val_t alignment)
{
    size_t align = static_cast<size_t>(alignment);
    void *ptr = LunaVoxelEngine::Platform::MemoryManager::operator_new_aligned(size + sizeof(size_t), align);
    *(size_t *)ptr = size;
    return (char *)ptr + sizeof(size_t);
}

inline void operator delete(void *ptr) noexcept
{
    LunaVoxelEngine::Platform::MemoryManager::operator_delete(ptr);
}

inline void operator delete[](void *ptr) noexcept
{
    LunaVoxelEngine::Platform::MemoryManager::operator_delete_array(ptr);
}

inline void operator delete(void *ptr, size_t) noexcept
{
    LunaVoxelEngine::Platform::MemoryManager::operator_delete(ptr);
}

inline void operator delete[](void *ptr, size_t) noexcept
{
    LunaVoxelEngine::Platform::MemoryManager::operator_delete_array(ptr);
}

inline void operator delete(void *ptr, std::align_val_t) noexcept
{
    LunaVoxelEngine::Platform::MemoryManager::operator_delete(ptr);
}

inline void operator delete[](void *ptr, std::align_val_t) noexcept
{
    LunaVoxelEngine::Platform::MemoryManager::operator_delete_array(ptr);
}

// Placement new operators
inline void *operator new(size_t, void *ptr) noexcept
{
    return ptr;
}
inline void *operator new[](size_t, void *ptr) noexcept
{
    return ptr;
}
#endif