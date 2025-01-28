#include <platform/common_memory.h>
#include <utils/algoritom.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include "common_memory.h"
namespace LunaVoxalEngine::Platform
{
struct AllocationHeader {
    unsigned long total_size;
};

void* os_calloc(unsigned long num, unsigned long size) {
    if (num == 0 || size == 0) {
        return nullptr;
    }

    unsigned long total_size = num * size;
    if (num != 0 && total_size / num != size) {
        return nullptr;
    }

    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) {
        return nullptr;
    }

    total_size = (total_size + page_size - 1) & ~(page_size - 1);

    void* raw_ptr = reinterpret_cast<void*>(syscall(SYS_mmap, nullptr, total_size + sizeof(AllocationHeader),
                                                    PROT_READ | PROT_WRITE,
                                                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (raw_ptr == MAP_FAILED) {
        return nullptr;
    }

    auto* header = static_cast<AllocationHeader*>(raw_ptr);
    header->total_size = total_size;

    void* user_memory = static_cast<void*>(header + 1);
    Utils::memset(user_memory, 0, total_size);

    return user_memory;
}


void os_free(void* ptr) {
    if (ptr == nullptr) {
        return;
    }

    // Get the header by moving back from the user pointer
    auto* header = reinterpret_cast<AllocationHeader*>(ptr) - 1;

    // Free the memory region using the munmap syscall
    syscall(SYS_munmap, header, header->total_size + sizeof(AllocationHeader));
}
}