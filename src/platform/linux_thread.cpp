#include <platform/thread.h>

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>
namespace LunaVoxalEngine::Platform::Thread
{
struct thread_handle
{
    pthread_t thread;
};

struct condition_handle
{
    pthread_cond_t cond;
};

struct mutex_handle
{
    pthread_mutex_t mutex;
};

struct rwlock_handle
{
    pthread_rwlock_t rwlock;
};

struct barrier_handle {
    pthread_barrier_t barrier;
};

struct spinlock_handle {
    pthread_spinlock_t spinlock;
};

thread_handle *thread_create(thread_func func, void *arg, int flags)
{
    thread_handle *th = new thread_handle();
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (flags & THREAD_FLAG_DETACHED)
    {
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    }

    if (flags & THREAD_FLAG_REALTIME)
    {
        struct sched_param param;
        param.sched_priority = sched_get_priority_max(SCHED_FIFO);
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        pthread_attr_setschedparam(&attr, &param);
        pthread_create(&th->thread, &attr, reinterpret_cast<void *(*)(void *)>(func), arg);
        pthread_attr_destroy(&attr);
    }
    return th;
}
ThreadError thread_destroy(thread_handle *handle)
{
    if (!handle)
        return THREAD_ERROR_CREATE;
    pthread_detach(handle->thread);
    delete handle;
    return THREAD_SUCCESS;
}
ThreadError thread_wait(thread_handle *handle, unsigned int timeout_ms)
{
    if (!handle)
        return THREAD_ERROR_CREATE;
    return pthread_join(handle->thread, nullptr) == 0 ? THREAD_SUCCESS : THREAD_ERROR_WAIT;
}
ThreadError thread_set_priority(thread_handle *handle, ThreadPriority priority)
{
    struct sched_param param;
    int policy;
    switch (priority)
    {
    case THREAD_PRIORITY_LOWEST: {
        policy = SCHED_OTHER;
        param.sched_priority = sched_get_priority_min(SCHED_OTHER);
        break;
    }
    case THREAD_PRIORITY_LOW: {
        policy = SCHED_OTHER;
        param.sched_priority = (sched_get_priority_min(SCHED_OTHER) + sched_get_priority_max(SCHED_OTHER)) / 2;
        break;
    }
    case THREAD_PRIORITY_NORMAL: {
        policy = SCHED_OTHER;
        param.sched_priority = 0;
        break;
    }
    case THREAD_PRIORITY_HIGH: {
        policy = SCHED_RR;
        param.sched_priority = sched_get_priority_max(SCHED_RR) / 2;
        break;
    }
    case THREAD_PRIORITY_HIGHEST: {
        policy = SCHED_RR;
        param.sched_priority = sched_get_priority_max(SCHED_RR);
        break;
    }
    case THREAD_PRIORITY_REALTIME: {
        policy = SCHED_FIFO;
        param.sched_priority = sched_get_priority_max(SCHED_FIFO);
        break;
    }
    default:
        return THREAD_ERROR_CREATE;
    }
    return pthread_setschedparam(handle->thread, policy, &param) == 0 ? THREAD_SUCCESS : THREAD_ERROR_CREATE;
}
unsigned int thread_get_id()
{
    return (unsigned int)pthread_self();
}
void thread_yield()
{
    sched_yield();
}
void thread_sleep(unsigned int ms)
{
    usleep(ms * 1000);
}
// Mutex operations

mutex_handle *mutex_create()
{
    mutex_handle *handle = new mutex_handle();
    pthread_mutex_init(&handle->mutex, nullptr);
    return handle;
}

void mutex_destroy(mutex_handle *handle)
{
    if (handle)
        pthread_mutex_destroy(&handle->mutex);
}

ThreadError mutex_lock(mutex_handle *handle, unsigned int timeout_ms)
{
    if (!handle)
        return THREAD_ERROR_CREATE;

    if (timeout_ms == 0)
    {
        return pthread_mutex_lock(&handle->mutex) == 0 ? THREAD_SUCCESS : THREAD_ERROR_WAIT;
    }
    else
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;

        // Normalize nanoseconds
        if (ts.tv_nsec >= 1000000000)
            ts.tv_sec++;
        ts.tv_nsec -= 1000000000;

        int result = pthread_mutex_timedlock(&handle->mutex, &ts);

        switch (result)
        {
        case 0:
            return THREAD_SUCCESS;
        case ETIMEDOUT:
            return THREAD_ERROR_TIMEOUT;
        default:
            return THREAD_ERROR_WAIT;
        }
    }
}

void mutex_unlock(mutex_handle *handle)
{
    if (handle)
        pthread_mutex_unlock(&handle->mutex);
}

// Read-Write Lock operations
rwlock_handle *rwlock_create()
{
    rwlock_handle *handle = new rwlock_handle();
    pthread_rwlock_init(&handle->rwlock, nullptr);
    return handle;
}

void rwlock_destroy(rwlock_handle *handle)
{
    if (handle)
    {
        pthread_rwlock_destroy(&handle->rwlock);
        delete handle;
    }
}

ThreadError rwlock_read_lock(rwlock_handle *handle, unsigned int timeout_ms)
{
    if (!handle)
        return THREAD_ERROR_CREATE;

    if (timeout_ms == 0)
    {
        return pthread_rwlock_rdlock(&handle->rwlock) == 0 ? THREAD_SUCCESS : THREAD_ERROR_WAIT;
    }
    else
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;

        // Normalize nanoseconds
        if (ts.tv_nsec >= 1000000000)
        {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }

        int result = pthread_rwlock_timedrdlock(&handle->rwlock, &ts);

        switch (result)
        {
        case 0:
            return THREAD_SUCCESS;
        case ETIMEDOUT:
            return THREAD_ERROR_TIMEOUT;
        default:
            return THREAD_ERROR_WAIT;
        }
    }
}

ThreadError rwlock_write_lock(rwlock_handle *handle, unsigned int timeout_ms)
{
    if (!handle)
        return THREAD_ERROR_CREATE;

    if (timeout_ms == 0)
    {
        return pthread_rwlock_wrlock(&handle->rwlock) == 0 ? THREAD_SUCCESS : THREAD_ERROR_WAIT;
    }
    else
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;

        // Normalize nanoseconds
        if (ts.tv_nsec >= 1000000000)
        {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }

        int result = pthread_rwlock_timedwrlock(&handle->rwlock, &ts);

        switch (result)
        {
        case 0:
            return THREAD_SUCCESS;
        case ETIMEDOUT:
            return THREAD_ERROR_TIMEOUT;
        default:
            return THREAD_ERROR_WAIT;
        }
    }
}

void rwlock_read_unlock(rwlock_handle *handle)
{
    if (handle)
    {
        pthread_rwlock_unlock(&handle->rwlock);
    }
}

void rwlock_write_unlock(rwlock_handle *handle)
{
    if (handle)
    {
        pthread_rwlock_unlock(&handle->rwlock);
    }
}

// Condition Variable operations
condition_handle *condition_create()
{
    condition_handle *handle = new condition_handle();
    pthread_cond_init(&handle->cond, nullptr);
    return handle;
}

void condition_destroy(condition_handle *handle)
{
    if (handle)
    {
        pthread_cond_destroy(&handle->cond);
        delete handle;
    }
}

ThreadError condition_wait(condition_handle *handle, mutex_handle *mutex, unsigned int timeout_ms)
{
    if (!handle || !mutex)
        return THREAD_ERROR_CREATE;

    if (timeout_ms == 0)
    {
        // Infinite wait
        return pthread_cond_wait(&handle->cond, &mutex->mutex) == 0 ? THREAD_SUCCESS : THREAD_ERROR_WAIT;
    }
    else
    {
        // Timed wait
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;

        // Normalize nanoseconds
        if (ts.tv_nsec >= 1000000000)
        {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }

        int result = pthread_cond_timedwait(&handle->cond, &mutex->mutex, &ts);

        switch (result)
        {
        case 0:
            return THREAD_SUCCESS;
        case ETIMEDOUT:
            return THREAD_ERROR_TIMEOUT;
        default:
            return THREAD_ERROR_WAIT;
        }
    }
}

void condition_signal(condition_handle *handle)
{
    if (handle)
    {
        pthread_cond_signal(&handle->cond);
    }
}

void condition_broadcast(condition_handle *handle)
{
    if (handle)
    {
        pthread_cond_broadcast(&handle->cond);
    }
}

// Barrier synchronization
barrier_handle* barrier_create(unsigned int thread_count) {
    barrier_handle* handle = new barrier_handle;
    if (pthread_barrier_init(&handle->barrier, nullptr, thread_count) != 0) {
        delete handle;
        return nullptr;
    }
    return handle;
}

void barrier_destroy(barrier_handle* handle) {
    if (handle) {
        pthread_barrier_destroy(&handle->barrier);
        delete handle;
    }
}

ThreadError barrier_wait(barrier_handle* handle) {
    if (!handle) return THREAD_ERROR_WAIT;
    int result = pthread_barrier_wait(&handle->barrier);
    return (result == 0 || result == PTHREAD_BARRIER_SERIAL_THREAD) ? THREAD_SUCCESS : THREAD_ERROR_WAIT;
}


// Spinlock operations
spinlock_handle* spinlock_create() {
    spinlock_handle* handle = new spinlock_handle;
    if (pthread_spin_init(&handle->spinlock, PTHREAD_PROCESS_PRIVATE) != 0) {
        delete handle;
        return nullptr;
    }
    return handle;
}

void spinlock_destroy(spinlock_handle* handle) {
    if (handle) {
        pthread_spin_destroy(&handle->spinlock);
        delete handle;
    }
}

void spinlock_lock(spinlock_handle* handle) {
    if (handle) {
        pthread_spin_lock(&handle->spinlock);
    }
}

void spinlock_unlock(spinlock_handle* handle) {
    if (handle) {
        pthread_spin_unlock(&handle->spinlock);
    }
}

} // namespace LunaVoxalEngine::Platform
