#include <platform/common_memory.h>
#include <utils/algoritom.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
namespace LunaVoxalEngine::Platform
{
struct AllocationHeader {
    size_t size;
};

void* os_malloc(size_t size) {
    if (size == 0) {
        return nullptr;
    }
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) {
        return nullptr;
    }
    size = (size + page_size - 1) & ~(page_size - 1);
    void* raw_ptr = reinterpret_cast<void*>(syscall(SYS_mmap, nullptr, size + sizeof(AllocationHeader),
                                                    PROT_READ | PROT_WRITE,
                                                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (raw_ptr == MAP_FAILED) {
        return nullptr;
    }
    auto* header = static_cast<AllocationHeader*>(raw_ptr);
    header->size = size;
   return static_cast<void*>(header + 1);
}


void os_free(void* ptr) {
    if (ptr == nullptr) {
        return;
    }
    auto* header = reinterpret_cast<AllocationHeader*>(ptr) - 1;
    syscall(SYS_munmap, header, header->size + sizeof(AllocationHeader));
}
}