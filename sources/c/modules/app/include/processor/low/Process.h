#include <memory/low/Type.h>

typedef struct Process {
    int (*start)(struct Process* self, int (*function)(void*), void* arg);
    int (*join)(struct Process* self);
    int (*id)(struct Process* self);
    int (*stop)(struct Process* self);
} Process;

// init vtable
void process_init();

// new raw process
Process* process_new();

// free raw thread
void process_free(Process* process);

// new process
Process* process_new_object();

// get self id or parent id
int process_self();
int process_parent();