#ifndef COMMON_MEMORY_H
#define COMMON_MEMORY_H
#include <utils/cdef.h>

namespace LunaVoxelEngine
{
namespace Platform
{
class MemoryManager
{
  private:
    struct Block
    {
        size_t size;
        bool used;
        Block *next;
        size_t alignment;
    };

    struct Pool
    {
        size_t total_size;
        Block *first_block;
        Pool *next_pool;
    };

    static const size_t MIN_ALLOC = 64;
    static const size_t POOL_SIZE = 8192;
    static const size_t DEFAULT_ALIGN = sizeof(void *);

    Pool *pools;
    size_t total_allocated;
    size_t total_used;
    // Prevent copying
    MemoryManager(const MemoryManager &) = delete;
    MemoryManager &operator=(const MemoryManager &) = delete;

    // Internal allocation helpers
    void *align_address(void *ptr, size_t alignment);
    size_t align_size(size_t size, size_t alignment);
    Pool *create_pool(size_t min_size);
    Block *find_block(size_t size, size_t alignment) noexcept;
    void coalesce();

  public:
    MemoryManager();
    ~MemoryManager();
    static MemoryManager &get_instance();

    void *allocate(size_t size, size_t alignment = DEFAULT_ALIGN);
    size_t get_allocated_size(void *ptr);
    void deallocate(void *ptr);

    // Memory usage statistics
    size_t get_total_allocated() const
    {
        return total_allocated;
    }
    size_t get_total_used() const
    {
        return total_used;
    }
    float get_fragmentation() const
    {
        return total_allocated > 0 ? 1.0f - ((float)total_used / total_allocated) : 0.0f;
    }

    // Operator new/delete interface
    static void *operator_new(size_t size);
    static void *operator_new_aligned(size_t size, size_t alignment);
    static void *operator_new_array(size_t size);
    static void operator_delete(void *ptr) noexcept;
    static void operator_delete_array(void *ptr) noexcept;
    template<typename T>
    friend class ScopedHeap;
};
template<typename T>
class [[nodiscard]] ScopedHeap final
{
  public:
    ScopedHeap(size_t count)
    {
        ptr = static_cast<T*>(MemoryManager::get_instance().operator_new_array(sizeof(T) * count)); 
    }
    ~ScopedHeap()
    {
        MemoryManager::get_instance().operator_delete_array(ptr);
    }
    ScopedHeap(const ScopedHeap &) = delete;
    ScopedHeap &operator=(const ScopedHeap &) = delete;

    T *operator&() const
    {
        return ptr;
    }

    T &operator[](size_t index)
    {
        return ptr[index];
    }

  private:
    T* ptr;
};
void * os_malloc(size_t size);
void os_free(void *ptr);
} // namespace Platform
} // namespace LunaVoxelEngine
#endif