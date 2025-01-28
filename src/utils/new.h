#ifndef NEW_H
#define NEW_H
#include <platform/common_memory.h>
inline void *operator new(unsigned long size)
{
    return LunaVoxalEngine::Platform::getGlobalMemoryManager()->allocate(size, __FILE__, __LINE__);
}
inline void *operator new[](unsigned long size)
{
    return LunaVoxalEngine::Platform::getGlobalMemoryManager()->allocate(size, __FILE__, __LINE__);
}

inline void* operator new(unsigned long, void* ptr) { return ptr; }
inline void* operator new[](unsigned long, void* ptr) { return ptr; }

inline void operator delete(void *ptr)
{
    LunaVoxalEngine::Platform::getGlobalMemoryManager()->deallocate(ptr);
}
inline void operator delete[](void *ptr)
{
    LunaVoxalEngine::Platform::getGlobalMemoryManager()->deallocate(ptr);
}
inline void operator delete(void *ptr, unsigned long)
{
    LunaVoxalEngine::Platform::getGlobalMemoryManager()->deallocate(ptr);
}
inline void operator delete[](void *ptr , unsigned long)
{
    LunaVoxalEngine::Platform::getGlobalMemoryManager()->deallocate(ptr);
}
#endif