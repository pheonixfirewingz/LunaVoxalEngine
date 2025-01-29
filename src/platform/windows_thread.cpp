#include <platform/thread.h>
#include <utils/new.h>
#include <process.h>
#include <windows.h>

namespace LunaVoxelEngine
{
namespace Platform
{

// Thread handle definition
struct thread_handle
{
    HANDLE handle;
    unsigned int id;
};

// Mutex handle definition
struct mutex_handle
{
    CRITICAL_SECTION cs;
};

// Read-Write Lock handle definition
struct rwlock_handle
{
    SRWLOCK lock;
};

// Condition variable handle definition
struct condition_handle
{
    CONDITION_VARIABLE cv;
};

// Barrier handle definition
struct barrier_handle
{
    size_t count;
    size_t waiting;
    HANDLE event;
    CRITICAL_SECTION cs;
};

// Spinlock handle definition
struct spinlock_handle
{
    volatile LONG lock;
};

// Thread entry wrapper
static unsigned __stdcall thread_entry(void *param)
{
    return reinterpret_cast<thread_func>(param)(nullptr);
}

// Thread creation
thread_handle *thread_create(thread_func func, void *arg, int flags)
{
    thread_handle *handle = new thread_handle();
    handle->handle = reinterpret_cast<HANDLE>(
        _beginthreadex(nullptr, 0, thread_entry, reinterpret_cast<void *>(func), 0, &handle->id));
    if (!handle->handle)
    {
        delete handle;
        return nullptr;
    }
    return handle;
}

// Thread destruction
ThreadError thread_destroy(thread_handle *handle)
{
    if (!handle)
        return ThreadError::THREAD_ERROR_CREATE;
    CloseHandle(handle->handle);
    delete handle;
    return ThreadError::THREAD_SUCCESS;
}

// Thread waiting
ThreadError thread_wait(thread_handle *handle, size_t timeout_ms)
{
    DWORD result = WaitForSingleObject(handle->handle, static_cast<DWORD>(timeout_ms));
    return (result == WAIT_OBJECT_0) ? ThreadError::THREAD_SUCCESS : ThreadError::THREAD_ERROR_TIMEOUT;
}

// Thread priority
ThreadError thread_set_priority(thread_handle *handle, ThreadPriority priority)
{
    static const int priorities[] = {THREAD_PRIORITY_LOWEST,  THREAD_PRIORITY_BELOW_NORMAL,
                                     THREAD_PRIORITY_NORMAL,  THREAD_PRIORITY_ABOVE_NORMAL,
                                     THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL};
    return SetThreadPriority(handle->handle, priorities[static_cast<size_t>(priority)])
               ? ThreadError::THREAD_SUCCESS
               : ThreadError::THREAD_ERROR_CREATE;
}

// Thread utilities
size_t thread_get_id()
{
    return GetCurrentThreadId();
}
void thread_yield()
{
    SwitchToThread();
}
void thread_sleep(size_t ms)
{
    Sleep(static_cast<DWORD>(ms));
}

// Mutex operations
mutex_handle *mutex_create()
{
    mutex_handle *handle = new mutex_handle();
    InitializeCriticalSection(&handle->cs);
    return handle;
}

void mutex_destroy(mutex_handle *handle)
{
    DeleteCriticalSection(&handle->cs);
    delete handle;
}

ThreadError mutex_lock(mutex_handle *handle, size_t timeout_ms)
{
    return TryEnterCriticalSection(&handle->cs) ? ThreadError::THREAD_SUCCESS : ThreadError::THREAD_ERROR_TIMEOUT;
}

void mutex_unlock(mutex_handle *handle)
{
    LeaveCriticalSection(&handle->cs);
}

// Read-Write Lock operations
rwlock_handle *rwlock_create()
{
    rwlock_handle *handle = new rwlock_handle();
    InitializeSRWLock(&handle->lock);
    return handle;
}

void rwlock_destroy(rwlock_handle *handle)
{
    delete handle;
}

ThreadError rwlock_read_lock(rwlock_handle *handle, size_t timeout_ms)
{
    AcquireSRWLockShared(&handle->lock);
    return ThreadError::THREAD_SUCCESS;
}

ThreadError rwlock_write_lock(rwlock_handle *handle, size_t timeout_ms)
{
    AcquireSRWLockExclusive(&handle->lock);
    return ThreadError::THREAD_SUCCESS;
}

void rwlock_read_unlock(rwlock_handle *handle)
{
    ReleaseSRWLockShared(&handle->lock);
}
void rwlock_write_unlock(rwlock_handle *handle)
{
    if (handle)
    {
        if (TryAcquireSRWLockExclusive(&handle->lock))
        {
            ReleaseSRWLockExclusive(&handle->lock);
        }
    }
}

// Condition Variable operations
condition_handle *condition_create()
{
    condition_handle *handle = new condition_handle();
    InitializeConditionVariable(&handle->cv);
    return handle;
}

void condition_destroy(condition_handle *handle)
{
    delete handle;
}

ThreadError condition_wait(condition_handle *handle, mutex_handle *mutex, size_t timeout_ms)
{
    return SleepConditionVariableCS(&handle->cv, &mutex->cs, static_cast<DWORD>(timeout_ms))
               ? ThreadError::THREAD_SUCCESS
               : ThreadError::THREAD_ERROR_TIMEOUT;
}

void condition_signal(condition_handle *handle)
{
    WakeConditionVariable(&handle->cv);
}
void condition_broadcast(condition_handle *handle)
{
    WakeAllConditionVariable(&handle->cv);
}

// Barrier synchronization
barrier_handle *barrier_create(size_t thread_count)
{
    barrier_handle *handle = new barrier_handle{thread_count, 0};
    handle->event = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    InitializeCriticalSection(&handle->cs);
    return handle;
}

void barrier_destroy(barrier_handle *handle)
{
    CloseHandle(handle->event);
    DeleteCriticalSection(&handle->cs);
    delete handle;
}

ThreadError barrier_wait(barrier_handle *handle)
{
    EnterCriticalSection(&handle->cs);
    handle->waiting++;
    if (handle->waiting == handle->count)
    {
        SetEvent(handle->event);
    }
    LeaveCriticalSection(&handle->cs);
    WaitForSingleObject(handle->event, INFINITE);
    return ThreadError::THREAD_SUCCESS;
}

// Spinlock operations
spinlock_handle *spinlock_create()
{
    spinlock_handle *handle = new spinlock_handle();
    handle->lock = 0;
    return handle;
}

void spinlock_destroy(spinlock_handle *handle)
{
    delete handle;
}

void spinlock_lock(spinlock_handle *handle)
{
    while (InterlockedCompareExchange(&handle->lock, 1, 0))
    {
        YieldProcessor();
    }
}

void spinlock_unlock(spinlock_handle *handle)
{
    InterlockedExchange(&handle->lock, 0);
}

} // namespace Platform
} // namespace LunaVoxelEngine
