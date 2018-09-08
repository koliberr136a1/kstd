#include <memory/low/Type.h>

typedef struct LinkedList {
    int (*add)(struct LinkedList* self, void* item);
    int (*addto)(struct LinkedList* self, int position, void* item);
    void* (*put)(struct LinkedList* self, int position, void* item);
    void* (*remove)(struct LinkedList* self, int position);
    void* (*get)(struct LinkedList* self, int position);
    int (*indexof)(struct LinkedList* self, void* item);
    int (*size)(struct LinkedList* self);
} LinkedList;

typedef struct LinkedListIterator {
    int (*hasnext)(struct LinkedListIterator* self);
    void* (*next)(struct LinkedListIterator* self);
} LinkedListIterator;

LinkedList* linkedlist_new(int mode, int (*comperator)(void*, void*));
void linkedlist_free(LinkedList* linkedlist);

LinkedListIterator* linkedlistiterator_new(LinkedList* linkedlist);
void linkedlistiterator_free(LinkedListIterator* linkedlistiterator);