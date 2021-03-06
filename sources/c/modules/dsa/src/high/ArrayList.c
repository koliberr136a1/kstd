#include <high/ArrayList.h>

#include <low/Heap.h>
#include <low/ReadWriteLock.h>

struct ArrayList_ {
    // self public object
    ArrayList self;

    // constructor data
    float factor;
    int (*comperator)(void*, void*);

    // private data
    int length;
    int cursor;
    void** array;
    ReadWriteLock* readwritelock;
};

// vtable
ArrayList_VTable* arraylist_vtable_normal;
ArrayList_VTable* arraylist_vtable_concurrent;

// link methods
int arraylist_add_normal(ArrayList* self, void* item);
int arraylist_addto_normal(ArrayList* self, int position, void* item);
void* arraylist_put_normal(ArrayList* self, int position, void* item);
void* arraylist_remove_normal(ArrayList* self, int position);
void* arraylist_get_normal(ArrayList* self, int position);
int arraylist_indexof_normal(ArrayList* self, void* item);
int arraylist_size_normal(ArrayList* self);

int arraylist_add_concurrent(ArrayList* self, void* item);
int arraylist_addto_concurrent(ArrayList* self, int position, void* item);
void* arraylist_put_concurrent(ArrayList* self, int position, void* item);
void* arraylist_remove_concurrent(ArrayList* self, int position);
void* arraylist_get_concurrent(ArrayList* self, int position);
int arraylist_indexof_concurrent(ArrayList* self, void* item);
int arraylist_size_concurrent(ArrayList* self);

// implement methods
// normal mode vtable operators
int arraylist_add_normal(ArrayList* self, void* item) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // normal add
    int result = arraylist_addto_normal(self, arraylist_->cursor, item);

    return result;
}
int arraylist_addto_normal(ArrayList* self, int position, void* item) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // check position is valid
    if (position < 0 || position > arraylist_->cursor) {
        return -1;
    }

    // check array have free space or not
    if (arraylist_->cursor >= arraylist_->length) {
        arraylist_->length = (int)(arraylist_->length * arraylist_->factor + 1);
        arraylist_->array = heap_realloc(arraylist_->array, arraylist_->length * sizeof(void*));
    }

    // move other items one block next
    for (int cursor = arraylist_->cursor - 1; cursor >= position; cursor--) {
        arraylist_->array[cursor + 1] = arraylist_->array[cursor];
    }

    // add item
    int result = position;
    arraylist_->array[position] = item;
    arraylist_->cursor++;

    return result;
}
void* arraylist_put_normal(ArrayList* self, int position, void* item) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // check position is valid
    if (position < 0 || position >= arraylist_->cursor) {
        return NULL;
    }

    // put item to position
    void* result = arraylist_->array[position];
    arraylist_->array[position] = item;

    return result;
}
void* arraylist_remove_normal(ArrayList* self, int position) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // check position is valid
    if (position < 0 || position >= arraylist_->cursor) {
        return NULL;
    }

    // get item at position
    void* result = arraylist_->array[position];

    // move other items on that item
    for (int cursor = position + 1; cursor < arraylist_->cursor; cursor++) {
        arraylist_->array[cursor - 1] = arraylist_->array[cursor];
    }

    // remove item
    arraylist_->cursor--;

    // check array have free space or not
    if (arraylist_->cursor < arraylist_->length / arraylist_->factor) {
        arraylist_->length = (int)(arraylist_->length / arraylist_->factor);
        arraylist_->length = arraylist_->length ? arraylist_->length : 1;
        arraylist_->array = heap_realloc(arraylist_->array, arraylist_->length * sizeof(void*));
    }

    return result;
}
void* arraylist_get_normal(ArrayList* self, int position) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // check position is valid
    if (position < 0 || position >= arraylist_->cursor) {
        return NULL;
    }

    // get item at position
    void* result = arraylist_->array[position];

    return result;
}
int arraylist_indexof_normal(ArrayList* self, void* item) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // search in items to find item
    for (int cursor = 0; cursor < arraylist_->cursor; cursor++) {
        // check comperator function is not NULL
        if (arraylist_->comperator != NULL) {
            if (arraylist_->comperator(item, arraylist_->array[cursor])) {
                return cursor;
            }
        } else {
            if (item == arraylist_->array[cursor]) {
                return cursor;
            }
        }
    }

    return -1;
}
int arraylist_size_normal(ArrayList* self) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // get current cursor position (size)
    int result = arraylist_->cursor;

    return result;
}

// concurrent mode vtable operators
int arraylist_add_concurrent(ArrayList* self, void* item) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // concurrent writelock
    arraylist_->readwritelock->vtable->write_lock(arraylist_->readwritelock, UINT_64_MAX);

    // normal add
    int result = arraylist_add_normal(self, item);

    // concurrent writeunlock
    arraylist_->readwritelock->vtable->write_unlock(arraylist_->readwritelock);

    return result;
}
int arraylist_addto_concurrent(ArrayList* self, int position, void* item) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // check position is valid
    if (position < 0 || position > arraylist_->cursor) {
        return -1;
    }

    // concurrent writelock
    arraylist_->readwritelock->vtable->write_lock(arraylist_->readwritelock, UINT_64_MAX);

    // normal addto
    int result = arraylist_addto_normal(self, position, item);

    // concurrent writeunlock
    arraylist_->readwritelock->vtable->write_unlock(arraylist_->readwritelock);

    return result;
}
void* arraylist_put_concurrent(ArrayList* self, int position, void* item) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // check position is valid
    if (position < 0 || position >= arraylist_->cursor) {
        return NULL;
    }

    // concurrent writelock
    arraylist_->readwritelock->vtable->write_lock(arraylist_->readwritelock, UINT_64_MAX);

    // normal put
    void* result = arraylist_put_normal(self, position, item);

    // concurrent writeunlock
    arraylist_->readwritelock->vtable->write_unlock(arraylist_->readwritelock);

    return result;
}
void* arraylist_remove_concurrent(ArrayList* self, int position) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // check position is valid
    if (position < 0 || position >= arraylist_->cursor) {
        return NULL;
    }

    // concurrent writelock
    arraylist_->readwritelock->vtable->write_lock(arraylist_->readwritelock, UINT_64_MAX);

    // normal remove
    void* result = arraylist_remove_normal(self, position);

    // concurrent writeunlock
    arraylist_->readwritelock->vtable->write_unlock(arraylist_->readwritelock);

    return result;
}
void* arraylist_get_concurrent(ArrayList* self, int position) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // check position is valid
    if (position < 0 || position >= arraylist_->cursor) {
        return NULL;
    }

    // concurrent readlock
    arraylist_->readwritelock->vtable->read_lock(arraylist_->readwritelock, UINT_64_MAX);

    // normal get
    void* result = arraylist_get_normal(self, position);

    // concurrent readunlock
    arraylist_->readwritelock->vtable->read_unlock(arraylist_->readwritelock);

    return result;
}
int arraylist_indexof_concurrent(ArrayList* self, void* item) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // concurrent readlock
    arraylist_->readwritelock->vtable->read_lock(arraylist_->readwritelock, UINT_64_MAX);

    // normal indexof
    int result = arraylist_indexof_normal(self, item);

    // concurrent readunlock
    arraylist_->readwritelock->vtable->read_unlock(arraylist_->readwritelock);

    return result;
}
int arraylist_size_concurrent(ArrayList* self) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)self;

    // concurrent readlock
    arraylist_->readwritelock->vtable->read_lock(arraylist_->readwritelock, UINT_64_MAX);

    // normal size
    int result = arraylist_size_normal(self);

    // concurrent readunlock
    arraylist_->readwritelock->vtable->read_unlock(arraylist_->readwritelock);

    return result;
}

// object allocation and deallocation operators
void arraylist_init() {
    // init normal vtable
    arraylist_vtable_normal = heap_alloc(sizeof(ArrayList_VTable));
    arraylist_vtable_normal->add = arraylist_add_normal;
    arraylist_vtable_normal->addto = arraylist_addto_normal;
    arraylist_vtable_normal->put = arraylist_put_normal;
    arraylist_vtable_normal->remove = arraylist_remove_normal;
    arraylist_vtable_normal->get = arraylist_get_normal;
    arraylist_vtable_normal->indexof = arraylist_indexof_normal;
    arraylist_vtable_normal->size = arraylist_size_normal;

    // init concurrent vtable
    arraylist_vtable_concurrent = heap_alloc(sizeof(ArrayList_VTable));
    arraylist_vtable_concurrent->add = arraylist_add_concurrent;
    arraylist_vtable_concurrent->addto = arraylist_addto_concurrent;
    arraylist_vtable_concurrent->put = arraylist_put_concurrent;
    arraylist_vtable_concurrent->remove = arraylist_remove_concurrent;
    arraylist_vtable_concurrent->get = arraylist_get_concurrent;
    arraylist_vtable_concurrent->indexof = arraylist_indexof_concurrent;
    arraylist_vtable_concurrent->size = arraylist_size_concurrent;
}
ArrayList* arraylist_new(int mode) {
    struct ArrayList_* arraylist_ = heap_alloc(sizeof(struct ArrayList_));

    // set vtable
    switch (mode) {
        case 0:
            arraylist_->self.vtable = arraylist_vtable_normal;
            break;
        case 1:
            arraylist_->self.vtable = arraylist_vtable_concurrent;
            break;
    }

    // set constructor data
    arraylist_->factor = 0;
    arraylist_->comperator = NULL;

    // set private data
    arraylist_->length = 0;
    arraylist_->cursor = 0;
    arraylist_->array = NULL;
    arraylist_->readwritelock = NULL;

    return (ArrayList*)arraylist_;
}
void arraylist_free(ArrayList* arraylist) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)arraylist;

    // free private data
    if (arraylist_->array != NULL) {
        heap_free(arraylist_->array);
    }
    if (arraylist_->readwritelock != NULL) {
        readwritelock_free(arraylist_->readwritelock);
    }

    // free self
    heap_free(arraylist_);
}
ArrayList* arraylist_new_object(int mode, float factor, int (*comperator)(void*, void*)) {
    struct ArrayList_* arraylist_ = (struct ArrayList_*)arraylist_new(mode);

    // set constructor data
    arraylist_->factor = factor;
    arraylist_->comperator = comperator;

    // set private data
    arraylist_->length = 1;
    arraylist_->cursor = 0;
    arraylist_->array = heap_alloc(arraylist_->length * sizeof(void*));
    if (mode == 1) {
        arraylist_->readwritelock = readwritelock_new_anonymous();
    }

    return (ArrayList*)arraylist_;
}
