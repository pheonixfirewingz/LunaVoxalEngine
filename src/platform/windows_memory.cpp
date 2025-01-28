#include <platform/common_memory.h>
#include <utils/algoritom.h>

struct SYSTEM_INFO {
    union {
        unsigned long dwOemId;
        struct {
            unsigned short wProcessorArchitecture;
            unsigned short wReserved;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    unsigned long dwPageSize;
    void* lpMinimumApplicationAddress;
    void* lpMaximumApplicationAddress;
    unsigned long dwActiveProcessorMask;
    unsigned long dwNumberOfProcessors;
    unsigned long dwProcessorType;
    unsigned long dwAllocationGranularity;
    unsigned short wProcessorLevel;
    unsigned short wProcessorRevision;
};


extern "C" {
    __declspec(dllimport) void* __stdcall VirtualAlloc(void* lpAddress, size_t dwSize, unsigned long dwAllocationType, unsigned long dwProtect);
    __declspec(dllimport) bool __stdcall VirtualFree(void* lpAddress, size_t dwSize, unsigned long dwFreeType);
    __declspec(dllimport) void GetSystemInfo(SYSTEM_INFO* lpSystemInfo);
}

namespace LunaVoxalEngine::Platform
{
struct AllocationHeader {
    size_t size;
};

void* os_malloc(size_t size) {
    if (size == 0) {
        return nullptr;
    }
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    size = (size + si.dwPageSize - 1) & ~(si.dwPageSize - 1);
    void* raw_ptr = VirtualAlloc(nullptr, size + sizeof(AllocationHeader), MEM_COMMIT, PAGE_READWRITE);
    if (raw_ptr == nullptr) {
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
    VirtualFree(header, 0, MEM_RELEASE);
}
}

