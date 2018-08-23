typedef struct Queue {
    int (*enqueue)(struct Queue* self, int front, void* item);
    void* (*dequeue)(struct Queue* self, int front, int timeout);
    int (*size)(struct Queue* self);
} Queue;

struct Queue* queue_new(int mode, int (*comperator)(void*, void*));
void queue_free(struct Queue* queue);