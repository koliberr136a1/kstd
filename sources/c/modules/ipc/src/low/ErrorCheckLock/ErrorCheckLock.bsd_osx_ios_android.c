#include <low/ErrorCheckLock.h>

#if defined(APP_BSD) || defined(APP_OSX) || defined(APP_IOS) || defined(APP_ANDROID)

#include <low/Heap.h>
#include <low/ReentrantLock.h>
#include <low/Share.h>
#include <low/String.h>
#include <pthread.h>
#include <sys/time.h>
#include <zconf.h>

struct ErrorCheckLock_ {
    // self public object
    ErrorCheckLock self;

    // constructor data

    // private data
    Share* share;
};

struct ErrorCheckLock_Memory {
    pthread_mutex_t mutex;
};

// vtable
ErrorCheckLock_VTable* errorchecklock_vtable;

// link methods
int errorchecklock_lock(ErrorCheckLock* self, uint_64 timeout);
int errorchecklock_unlock(ErrorCheckLock* self);

// implement methods
// vtable operators
int errorchecklock_lock(ErrorCheckLock* self, uint_64 timeout) {
    struct ErrorCheckLock_* errorchecklock_ = (struct ErrorCheckLock_*)self;

    // get memory address
    struct ErrorCheckLock_Memory* memory = errorchecklock_->share->vtable->address(errorchecklock_->share);

    // lock the pthread mutex
    if (timeout == UINT_64_MAX) {
        // infinity
        if (pthread_mutex_lock(&(memory->mutex)) == 0) {
            return 0;
        }
    } else if (timeout > 0) {
        // timed

        // get time_out
        struct timeval time_start, time_now;
        gettimeofday(&time_start, NULL);

        // timed lock
        do{
            // try lock
            if(pthread_mutex_trylock(&(memory->mutex)) == 0){
                return 0;
            }

            // sleep 10 ms
            usleep(10);

            // get now time
            gettimeofday(&time_now, NULL);
        }while(((time_now.tv_sec + time_now.tv_usec / 1000) - (time_start.tv_sec + time_start.tv_usec / 1000)) <= timeout);
    } else {
        // try
        if (pthread_mutex_trylock(&(memory->mutex)) == 0) {
            return 0;
        }
    }

    return -1;
}
int errorchecklock_unlock(ErrorCheckLock* self) {
    struct ErrorCheckLock_* errorchecklock_ = (struct ErrorCheckLock_*)self;

    // get memory address
    struct ErrorCheckLock_Memory* memory = errorchecklock_->share->vtable->address(errorchecklock_->share);

    // unlock the pthread mutex
    if (pthread_mutex_unlock(&(memory->mutex)) == 0) {
        return 0;
    }

    return -1;
}

// object allocation and deallocation operators
void errorchecklock_init() {
    // init vtable
    errorchecklock_vtable = heap_alloc(sizeof(ErrorCheckLock_VTable));
    errorchecklock_vtable->lock = errorchecklock_lock;
    errorchecklock_vtable->unlock = errorchecklock_unlock;
}
ErrorCheckLock* errorchecklock_new() {
    struct ErrorCheckLock_* errorchecklock_ = heap_alloc(sizeof(struct ErrorCheckLock_));

    // set vtable
    errorchecklock_->self.vtable = errorchecklock_vtable;

    // set constructor data

    // set private data
    errorchecklock_->share = NULL;

    return (ErrorCheckLock*)errorchecklock_;
}
void errorchecklock_free(ErrorCheckLock* errorchecklock) {
    struct ErrorCheckLock_* errorchecklock_ = (struct ErrorCheckLock_*)errorchecklock;

    // free private data
    if (errorchecklock_->share != NULL) {
        // if share connections is 1, destroy
        if (errorchecklock_->share->vtable->connections(errorchecklock_->share) <= 1) {
            struct ErrorCheckLock_Memory* memory = errorchecklock_->share->vtable->address(errorchecklock_->share);

            // destroy
            pthread_mutex_destroy(&(memory->mutex));
        }

        share_free(errorchecklock_->share);
    }

    // free constructor data

    // free self
    heap_free(errorchecklock_);
}
ErrorCheckLock* errorchecklock_new_anonymous(){
    struct ErrorCheckLock_* errorchecklock_ = (struct ErrorCheckLock_*)errorchecklock_new();

    // set constructor data

    // set private data
    // open errorcheck lock
    errorchecklock_->share = share_new_anonymous(sizeof(struct ErrorCheckLock_Memory), 0);

    // if share connections is 1, init share
    if (errorchecklock_->share->vtable->connections(errorchecklock_->share) <= 1) {
        // get memory address
        struct ErrorCheckLock_Memory* memory = errorchecklock_->share->vtable->address(errorchecklock_->share);

        // init mutex
        pthread_mutexattr_t mattr;
        pthread_mutexattr_init(&mattr);
        pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_PRIVATE);
        pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&(memory->mutex), &mattr);
        pthread_mutexattr_destroy(&mattr);
    }

    return (ErrorCheckLock*)errorchecklock_;
}
ErrorCheckLock* errorchecklock_new_named(char* name){
    struct ErrorCheckLock_* errorchecklock_ = (struct ErrorCheckLock_*)errorchecklock_new();

    // set constructor data

    // set private data
    // try lock critical
    if (critical != NULL) {
        critical->vtable->lock(critical, UINT_64_MAX);
    }

    // open share errorcheck lock
    String* errorchecklock_name = string_new_printf("%s_el", name);
    errorchecklock_->share = share_new_named(errorchecklock_name->vtable->value(errorchecklock_name), sizeof(struct ErrorCheckLock_Memory), 0);
    string_free(errorchecklock_name);

    // if share connections is 1, init share
    if (errorchecklock_->share->vtable->connections(errorchecklock_->share) <= 1) {
        // get memory address
        struct ErrorCheckLock_Memory* memory = errorchecklock_->share->vtable->address(errorchecklock_->share);

        // init mutex
        pthread_mutexattr_t mattr;
        pthread_mutexattr_init(&mattr);
        pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
        pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&(memory->mutex), &mattr);
        pthread_mutexattr_destroy(&mattr);
    }

    // try unlock critical
    if (critical != NULL) {
        critical->vtable->unlock(critical);
    }

    return (ErrorCheckLock*)errorchecklock_;
}

#endif
