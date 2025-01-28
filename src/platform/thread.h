#ifndef THREAD_ABSTRACTION_H
#define THREAD_ABSTRACTION_H
namespace LunaVoxalEngine::Platform
{
// Error handling
enum ThreadError {
    THREAD_SUCCESS = 0,
    THREAD_ERROR_CREATE,
    THREAD_ERROR_WAIT,
    THREAD_ERROR_TIMEOUT
};

// Opaque type definitions
typedef struct thread_handle thread_handle;
typedef struct mutex_handle mutex_handle;
typedef struct rwlock_handle rwlock_handle;
typedef struct condition_handle condition_handle;
typedef struct barrier_handle barrier_handle;
typedef struct spinlock_handle spinlock_handle;

// Thread function type
typedef unsigned int (*thread_func)(void* param);

// Thread creation flags
enum ThreadFlags {
    THREAD_FLAG_NONE = 0,
    THREAD_FLAG_DETACHED = 1,
    THREAD_FLAG_REALTIME = 2
};

// Thread priority levels
enum ThreadPriority {
    THREAD_PRIORITY_LOWEST,
    THREAD_PRIORITY_LOW,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_HIGH,
    THREAD_PRIORITY_HIGHEST,
    THREAD_PRIORITY_REALTIME
};

// Core thread operations
thread_handle* thread_create(thread_func func, void* arg, int flags);
ThreadError thread_destroy(thread_handle* handle);
ThreadError thread_wait(thread_handle* handle, unsigned int timeout_ms);
ThreadError thread_set_priority(thread_handle* handle, ThreadPriority priority);
unsigned int thread_get_id();
void thread_yield();
void thread_sleep(unsigned int ms);

// Mutex operations
mutex_handle* mutex_create();
void mutex_destroy(mutex_handle* handle);
ThreadError mutex_lock(mutex_handle* handle, unsigned int timeout_ms);
void mutex_unlock(mutex_handle* handle);

// Read-Write Lock operations
rwlock_handle* rwlock_create();
void rwlock_destroy(rwlock_handle* handle);
ThreadError rwlock_read_lock(rwlock_handle* handle, unsigned int timeout_ms);
ThreadError rwlock_write_lock(rwlock_handle* handle, unsigned int timeout_ms);
void rwlock_read_unlock(rwlock_handle* handle);
void rwlock_write_unlock(rwlock_handle* handle);

// Condition Variable operations
condition_handle* condition_create();
void condition_destroy(condition_handle* handle);
ThreadError condition_wait(condition_handle* handle, mutex_handle* mutex, unsigned int timeout_ms);
void condition_signal(condition_handle* handle);
void condition_broadcast(condition_handle* handle);

// Barrier synchronization
barrier_handle* barrier_create(unsigned int thread_count);
void barrier_destroy(barrier_handle* handle);
ThreadError barrier_wait(barrier_handle* handle);

// Spinlock operations
spinlock_handle* spinlock_create();
void spinlock_destroy(spinlock_handle* handle);
void spinlock_lock(spinlock_handle* handle); 
void spinlock_unlock(spinlock_handle* handle);

// Thread Wrapper
class Thread final
{
public:
    Thread(thread_func func, void* arg, int flags = THREAD_FLAG_NONE)
        : handle(thread_create(func, arg, flags)) {}

    ~Thread() { if (handle) thread_destroy(handle); }

    ThreadError wait(unsigned int timeout_ms = 0xFFFFFFFF) { return thread_wait(handle, timeout_ms); }

    ThreadError set_priority(ThreadPriority priority) { return thread_set_priority(handle, priority); }

    static unsigned int get_id() { return thread_get_id(); }

    static void yield() { thread_yield(); }

    static void sleep(unsigned int ms) { thread_sleep(ms); }

private:
    thread_handle* handle;
};

// Mutex Wrapper
class ConditionVariable;
class Mutex final
{
public:
    Mutex() : handle(mutex_create()) {}

    ~Mutex() { if (handle) mutex_destroy(handle); }

    ThreadError lock(unsigned int timeout_ms = 0xFFFFFFFF) { return mutex_lock(handle, timeout_ms); }

    ThreadError try_lock() { return mutex_lock(handle, 0); }

    void unlock() { mutex_unlock(handle); }

    ThreadError Lock(unsigned int timeout_ms = 0xFFFFFFFF) { return lock(timeout_ms); }
    ThreadError TryLock() { return try_lock(); }
    void Unlock() { unlock(); }

private:
    mutex_handle* handle;
    friend class ConditionVariable;
};

// Scoped Mutex Wrapper
class ScopeLock final
{
public:
    explicit ScopeLock(Mutex& mutex, unsigned int timeout_ms = 0xFFFFFFFF)
        : mutex_ref(mutex), is_locked(false)
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
    ScopeLock(const ScopeLock&) = delete;
    ScopeLock& operator=(const ScopeLock&) = delete;

    // Allow move semantics
    ScopeLock(ScopeLock&& other) noexcept
        : mutex_ref(other.mutex_ref), is_locked(other.is_locked)
    {
        other.is_locked = false;
    }

    ScopeLock& operator=(ScopeLock&& other) noexcept
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

    bool owns_lock() const noexcept { return is_locked; }

private:
    Mutex& mutex_ref;
    bool is_locked;
};

//GaurdLock Wrapper
class GuardLock final
{
public:
    explicit GuardLock(Mutex& mutex)
        : mutex_ref(mutex)
    {
        mutex_ref.lock();
    }

    ~GuardLock()
    {
        mutex_ref.unlock();
    }

    // Prevent copy semantics
    GuardLock(const GuardLock&) = delete;
    GuardLock& operator=(const GuardLock&) = delete;

    // Prevent move semantics
    GuardLock(GuardLock&&) = delete;
    GuardLock& operator=(GuardLock&&) = delete;

private:
    Mutex& mutex_ref;
};


// Read-Write Lock Wrapper
class RWLock final
{
public:
    RWLock() : handle(rwlock_create()) {}

    ~RWLock() { if (handle) rwlock_destroy(handle); }

    ThreadError read_lock(unsigned int timeout_ms = 0xFFFFFFFF) { return rwlock_read_lock(handle, timeout_ms); }

    ThreadError write_lock(unsigned int timeout_ms = 0xFFFFFFFF) { return rwlock_write_lock(handle, timeout_ms); }

    void read_unlock() { rwlock_read_unlock(handle); }

    void write_unlock() { rwlock_write_unlock(handle); }

private:
    rwlock_handle* handle;
};

// Condition Variable Wrapper
class ConditionVariable final
{
public:
    ConditionVariable() : handle(condition_create()) {}

    ~ConditionVariable() { if (handle) condition_destroy(handle); }

    ThreadError wait(Mutex* mutex, unsigned int timeout_ms = 0xFFFFFFFF) { return condition_wait(handle, mutex->handle, timeout_ms); }

    void signal() { condition_signal(handle); }

    void broadcast() { condition_broadcast(handle); }
    friend class Mutex;
private:
    condition_handle* handle;
};

// Barrier Wrapper
class Barrier final
{
public:
    explicit Barrier(unsigned int thread_count) : handle(barrier_create(thread_count)) {}

    ~Barrier() { if (handle) barrier_destroy(handle); }

    ThreadError wait() { return barrier_wait(handle); }

private:
    barrier_handle* handle;
};

// Spinlock Wrapper
class Spinlock final
{
public:
    Spinlock() : handle(spinlock_create()) {}

    ~Spinlock() { if (handle) spinlock_destroy(handle); }

    void lock() { spinlock_lock(handle); }

    void unlock() { spinlock_unlock(handle); }

private:
    spinlock_handle* handle;
};
}
#endif // THREAD_ABSTRACTION_H