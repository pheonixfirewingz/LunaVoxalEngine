#ifndef THREAD_ABSTRACTION_H
#define THREAD_ABSTRACTION_H
#include <utils/cdef.h>
namespace LunaVoxelEngine
{
namespace Platform
{
// Error handling
enum class ThreadError
{
    THREAD_SUCCESS,
    THREAD_ERROR_CREATE,
    THREAD_ERROR_WAIT,
    THREAD_ERROR_TIMEOUT
};

// Thread creation flags
enum ThreadFlags
{
    THREAD_FLAG_NONE = 0,
    THREAD_FLAG_DETACHED = 1,
    THREAD_FLAG_REALTIME = 2
};

// Thread priority levels
enum class ThreadPriority
{
    THREAD_PRIORITY_LOWEST,
    THREAD_PRIORITY_LOW,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_HIGH,
    THREAD_PRIORITY_HIGHEST,
    THREAD_PRIORITY_REALTIME
};
struct thread_handle;
struct mutex_handle;
struct rwlock_handle;
struct condition_handle;
struct barrier_handle;
struct spinlock_handle;

// Thread function type
typedef size_t (*thread_func)(void *param);

// Core thread operations
thread_handle *thread_create(thread_func func, void *arg, int flags);
ThreadError thread_destroy(thread_handle *handle);
ThreadError thread_wait(thread_handle *handle, size_t timeout_ms);
ThreadError thread_set_priority(thread_handle *handle, ThreadPriority priority);
size_t thread_get_id();
void thread_yield();
void thread_sleep(size_t ms);

// Mutex operations
mutex_handle *mutex_create();
void mutex_destroy(mutex_handle *handle);
ThreadError mutex_lock(mutex_handle *handle, size_t timeout_ms);
void mutex_unlock(mutex_handle *handle);

// Read-Write Lock operations
rwlock_handle *rwlock_create();
void rwlock_destroy(rwlock_handle *handle);
ThreadError rwlock_read_lock(rwlock_handle *handle, size_t timeout_ms);
ThreadError rwlock_write_lock(rwlock_handle *handle, size_t timeout_ms);
void rwlock_read_unlock(rwlock_handle *handle);
void rwlock_write_unlock(rwlock_handle *handle);

// Condition Variable operations
condition_handle *condition_create();
void condition_destroy(condition_handle *handle);
ThreadError condition_wait(condition_handle *handle, mutex_handle *mutex, size_t timeout_ms);
void condition_signal(condition_handle *handle);
void condition_broadcast(condition_handle *handle);

// Barrier synchronization
barrier_handle *barrier_create(size_t thread_count);
void barrier_destroy(barrier_handle *handle);
ThreadError barrier_wait(barrier_handle *handle);

// Spinlock operations
spinlock_handle *spinlock_create();
void spinlock_destroy(spinlock_handle *handle);
void spinlock_lock(spinlock_handle *handle);
void spinlock_unlock(spinlock_handle *handle);

// Thread Wrapper
class [[nodiscard]] Thread final
{
  public:
    Thread(thread_func func, void *arg, int flags = THREAD_FLAG_NONE)
        : handle(thread_create(func, arg, flags))
    {
    }

    ~Thread()
    {
        if (handle)
            thread_destroy(handle);
    }

    ThreadError wait(size_t timeout_ms = 0xFFFFFFFF)
    {
        return thread_wait(handle, timeout_ms);
    }

    ThreadError set_priority(ThreadPriority priority)
    {
        return thread_set_priority(handle, priority);
    }

    static size_t get_id()
    {
        return thread_get_id();
    }

    static void yield()
    {
        thread_yield();
    }

    static void sleep(size_t ms)
    {
        thread_sleep(ms);
    }

  private:
    thread_handle *handle;
};

// Mutex Wrapper
class [[nodiscard]] Mutex final
{
  public:
    Mutex()
        : handle(mutex_create())
    {
    }

    ~Mutex()
    {
        if (handle)
            mutex_destroy(handle);
    }

    ThreadError lock(size_t timeout_ms = 0xFFFFFFFF)
    {
        return mutex_lock(handle, timeout_ms);
    }

    ThreadError try_lock()
    {
        return mutex_lock(handle, 0);
    }

    void unlock()
    {
        mutex_unlock(handle);
    }

    ThreadError Lock(size_t timeout_ms = 0xFFFFFFFF)
    {
        return lock(timeout_ms);
    }
    ThreadError TryLock()
    {
        return try_lock();
    }
    void Unlock()
    {
        unlock();
    }

  private:
    mutex_handle *handle;
    friend class ConditionVariable;
};

// Scoped Mutex Wrapper
class [[nodiscard]] ScopeLock final
{
  public:
    explicit ScopeLock(Mutex &mutex, size_t timeout_ms = 0xFFFFFFFF)
        : mutex_ref(mutex)
        , is_locked(false)
    {
        if (mutex_ref.lock(timeout_ms) == ThreadError::THREAD_SUCCESS)
        {
            is_locked = true;
        }
    }

    ~ScopeLock()
    {
        if (is_locked)
        {
            mutex_ref.unlock();
        }
    }

    // Prevent copy semantics
    ScopeLock(const ScopeLock &) = delete;
    ScopeLock &operator=(const ScopeLock &) = delete;

    // Allow move semantics
    ScopeLock(ScopeLock &&other) noexcept
        : mutex_ref(other.mutex_ref)
        , is_locked(other.is_locked)
    {
        other.is_locked = false;
    }

    ScopeLock &operator=(ScopeLock &&other) noexcept
    {
        if (this != &other)
        {
            if (is_locked)
            {
                mutex_ref.unlock();
            }

            mutex_ref = other.mutex_ref;
            is_locked = other.is_locked;
            other.is_locked = false;
        }
        return *this;
    }

    bool owns_lock() const noexcept
    {
        return is_locked;
    }

  private:
    Mutex &mutex_ref;
    bool is_locked;
};

// GuardLock Wrapper
class [[nodiscard]] GuardLock final
{
  public:
    explicit GuardLock(Mutex &mutex)
        : mutex_ref(mutex)
    {
        mutex_ref.lock();
    }

    ~GuardLock()
    {
        mutex_ref.unlock();
    }

    // Prevent copy semantics
    GuardLock(const GuardLock &) = delete;
    GuardLock &operator=(const GuardLock &) = delete;

    // Prevent move semantics
    GuardLock(GuardLock &&) = delete;
    GuardLock &operator=(GuardLock &&) = delete;

  private:
    Mutex &mutex_ref;
};

// Read-Write Lock Wrapper
class RWLock final
{
  public:
    RWLock()
        : handle(rwlock_create())
    {
    }

    ~RWLock()
    {
        if (handle)
            rwlock_destroy(handle);
    }

    ThreadError read_lock(size_t timeout_ms = 0xFFFFFFFF)
    {
        return rwlock_read_lock(handle, timeout_ms);
    }

    ThreadError write_lock(size_t timeout_ms = 0xFFFFFFFF)
    {
        return rwlock_write_lock(handle, timeout_ms);
    }

    void read_unlock()
    {
        rwlock_read_unlock(handle);
    }

    void write_unlock()
    {
        rwlock_write_unlock(handle);
    }

  private:
    rwlock_handle *handle;
};

// Condition Variable Wrapper
class [[nodiscard]] ConditionVariable final
{
  public:
    ConditionVariable()
        : handle(condition_create())
    {
    }

    ~ConditionVariable()
    {
        if (handle)
            condition_destroy(handle);
    }

    ThreadError wait(Mutex *mutex, size_t timeout_ms = 0xFFFFFFFF)
    {
        return condition_wait(handle, mutex->handle, timeout_ms);
    }

    void signal()
    {
        condition_signal(handle);
    }

    void broadcast()
    {
        condition_broadcast(handle);
    }
    friend class Mutex;

  private:
    condition_handle *handle;
};

// Barrier Wrapper
class [[nodiscard]] Barrier final
{
  public:
    explicit Barrier(size_t thread_count)
        : handle(barrier_create(thread_count))
    {
    }

    ~Barrier()
    {
        if (handle)
            barrier_destroy(handle);
    }

    ThreadError wait()
    {
        return barrier_wait(handle);
    }

  private:
    barrier_handle *handle;
};

// Spinlock Wrapper
class [[nodiscard]] Spinlock final
{
  public:
    Spinlock()
        : handle(spinlock_create())
    {
    }

    ~Spinlock()
    {
        if (handle)
            spinlock_destroy(handle);
    }

    void lock()
    {
        spinlock_lock(handle);
    }

    void unlock()
    {
        spinlock_unlock(handle);
    }

  private:
    spinlock_handle *handle;
};
} // namespace Platform
} // namespace LunaVoxelEngine
#endif // THREAD_ABSTRACTION_H
