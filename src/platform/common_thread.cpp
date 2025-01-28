#include <platform/thread.h>
#include <platform/log.h>
#ifdef ON_LINUX
using namespace LunaVoxalEngine::Platform;

static Mutex guard_mutex;

extern "C" void __cxa_guard_acquire(void* guard_var) {
    ThreadError result = guard_mutex.lock();
    if (result != ThreadError::THREAD_SUCCESS)
        LunaVoxalEngine::Log::error("Error acquiring guard mutex!");
}

extern "C" void __cxa_guard_release(void* guard_var) {
    guard_mutex.unlock();
}

extern "C" void __cxa_guard_abort(void* guard_var) {
    guard_mutex.unlock();
}
#endif