#include <low/Type.h>

// vtable
typedef struct Thread_VTable {
    int (*start)(struct Thread* self, int (*function)(void*), void* arg);
    int (*join)(struct Thread* self);
    int (*id)(struct Thread* self);
    int (*stop)(struct Thread* self);
} Thread_VTable;

// vtable + private data problem solve
typedef struct Thread {
    Thread_VTable* vtable;
} Thread;

// init vtable
void thread_init();

// new raw thread
Thread* thread_new();

// free raw thread
void thread_free(Thread* thread);

// new thread
Thread* thread_new_object(tsize stack);

// local thread methods
<<<<<<< HEAD
int thread_id();
=======
int thread_self();
>>>>>>> a37be36ed975eeb630021237dedce3819a9faeac
