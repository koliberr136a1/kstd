#ifdef __unix__

#include <low/concurrency/Lock.h>

#include <low/local/time/Time.h>
#include <pthread.h>
#include <stdint.h>

struct Lock_ {
    struct Lock this;
    pthread_mutex_t mutex;
};

// link methods
int lock_lock(struct Lock* self);
int lock_unlock(struct Lock* self);
int lock_trylock(struct Lock* self, int timeout);

int lock_lock(struct Lock* self) {
    struct Lock_* lock_ = self;

    // lock the pthread mutex
    pthread_mutex_lock(&(lock_->mutex));

    return 0;
}

int lock_unlock(struct Lock* self) {
    struct Lock_* lock_ = self;

    // unlock the pthread mutex
    pthread_mutex_unlock(&(lock_->mutex));

    return 0;
}

int lock_trylock(struct Lock* self, int timeout) {
    struct Lock_* lock_ = self;

    // get start time
    uint64_t time = time_micro();

    // try lock until timeout
    while (time_micro() - time < timeout * 1.0e3) {
        if (pthread_mutex_trylock(&(lock_->mutex)) == 0) {
            return 0;
        }
    }

    return -1;
}

struct Lock* lock_new() {
    struct Lock_* lock_ = memory_alloc(sizeof(struct Lock_));

    // init private methods
    lock_->this.lock = lock_lock;
    lock_->this.unlock = lock_unlock;
    lock_->this.trylock = lock_trylock;

    // init the core of lock (mutex)
    pthread_mutex_init(&(lock_->mutex), NULL);

    return lock_;
}

void lock_free(struct Lock* lock) {
    struct Lock_* lock_ = lock;

    // destry mutex
    pthread_mutex_destroy(&(lock_->mutex));

    memory_free(lock_);
}

#endif