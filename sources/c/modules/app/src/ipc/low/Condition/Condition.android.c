#include <ipc/low/Condition.h>

#if defined(APP_ANDROID)

#include <dsa/low/String.h>
#include <ipc/low/Mutex.h>
#include <local/low/Time.h>
#include <memory/low/Heap.h>
#include <pthread.h>

struct Condition_ {
    struct Condition self;
    void* memory;
    String* name;
};

// link methods
int condition_wait(struct Condition* self, uint_64 timeout);
int condition_signal(struct Condition* self, int count);

// local methods
void* condition_anonymous_new();
void condition_anonymous_free(void* memory);
void* condition_named_new(char* name);
void condition_named_free(void* memory, char* name);

void* condition_anonymous_new() {
    // alocate mutex
    void* result = heap_alloc(sizeof(pthread_mutex_t) + sizeof(pthread_cond_t));

    // get mutex and cond address
    pthread_mutex_t* mutex = result;
    pthread_mutex_t* cond = result + sizeof(pthread_mutex_t);

    // init mutex
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setpshared(&mattr, 0);
    pthread_mutex_init(mutex, &mattr);
    pthread_mutexattr_destroy(&mattr);

    // init cond
    pthread_condattr_t cattr;
    pthread_condattr_init(&cattr);
    pthread_condattr_setpshared(&cattr, 0);
    pthread_cond_init(cond, &cattr);
    pthread_condattr_destroy(&cattr);

    return result;
}
void condition_anonymous_free(void* memory) {
    // get mutex and cond address
    pthread_mutex_t* mutex = memory;
    pthread_cond_t* cond = memory + sizeof(pthread_mutex_t);

    // destroy internal mutex
    pthread_mutex_destroy(mutex);
    pthread_cond_destroy(cond);

    heap_free(memory);
}
void* condition_named_new(char* name) {
    return NULL;
}
void condition_named_free(void* memory, char* name) {}

// implement methods
int condition_wait(struct Condition* self, uint_64 timeout) {
    struct Condition_* condition_ = (struct Condition_*)self;

    // get mutex and cond address
    pthread_mutex_t* mutex = condition_->memory;
    pthread_cond_t* cond = condition_->memory + sizeof(pthread_mutex_t);

    // aquire the pthread mutex
    pthread_mutex_lock(mutex);

    // wait the pthread cond
    int result = -1;
    if (timeout == UINT_64_MAX) {
        // infinity
        if (pthread_cond_wait(cond, mutex) == 0) {
            result = 0;
        }
    } else {
        // timed
        struct timeval time_now;
        struct timespec time_out;
        gettimeofday(&time_now, NULL);
        time_out.tv_sec = time_now.tv_sec;
        time_out.tv_nsec = time_now.tv_usec * 1000;
        time_out.tv_sec += timeout / 1000;
        time_out.tv_nsec += (timeout % 1000) * 1000000;
        if (pthread_cond_timedwait(cond, mutex, &time_out) == 0) {
            result = 0;
        }
    }

    // release the pthread mutex
    pthread_mutex_unlock(mutex);

    return result;
}
int condition_signal(struct Condition* self, int count) {
    struct Condition_* condition_ = (struct Condition_*)self;

    // get mutex and cond address
    pthread_mutex_t* mutex = condition_->memory;
    pthread_cond_t* cond = condition_->memory + sizeof(pthread_mutex_t);

    // aquire the pthread mutex
    pthread_mutex_lock(mutex);

    // signal the pthread cond
    int result = -1;
    if (count == INT_MAX) {
        // broadcast
        if (pthread_cond_broadcast(cond) == 0) {
            result = 0;
        }
    } else {
        // signal
        for (int cursor = 0; cursor < count; cursor++) {
            pthread_cond_signal(cond);
        }
        result = 0;
    }

    // release the pthread mutex
    pthread_mutex_unlock(mutex);

    return result;
}

Condition* condition_new(char* name) {
    struct Condition_* condition_ = heap_alloc(sizeof(struct Condition_));

    // init private methods
    condition_->self.wait = condition_wait;
    condition_->self.signal = condition_signal;

    if (name == NULL) {
        condition_->name = NULL;

        // create internal mutex
        condition_->memory = mutex_anonymous_new();
    } else {
        heap_free(condition_);
        condition_ = NULL;
    }

    return (Condition*)condition_;
}
void condition_free(Condition* condition) {
    struct Condition_* condition_ = (struct Condition_*)condition;

    if (condition_->name == NULL) {
        // destroy internal mutex and cond
        condition_anonymous_free(condition_->memory);
    }

    heap_free(condition_);
}

#endif