#include <low/Share.h>

#if defined(APP_ANDROID)

#include <low/ReentrantLock.h>
#include <low/Heap.h>
#include <low/String.h>

struct Share_ {
    // self public object
    Share self;

    // constructor data
    String* name;
    tsize size;
    tsize offset;

    // private data
    void* memory;
};

// vtable
Share_VTable* share_vtable;

// link methods
void* share_address(Share* self);
int share_connections(Share* self);
int share_flush(Share* self, tsize size);

// local methods
void* share_named_new(char* name, tsize size, tsize offset);
void share_named_free(void* memory, char* name, tsize size, tsize offset);
void* share_anonymous_new(tsize size, tsize offset);
void share_anonymous_free(void* memory);

// implement methods
void* share_named_new(char* name, tsize size, tsize offset){
    // android does not implement standard share memory
    return share_anonymous_new(size, offset);
}
void share_named_free(void* memory, char* name, tsize size, tsize offset){
    // android does not implement standard share memory
    share_anonymous_free(memory);
}
void* share_anonymous_new(tsize size, tsize offset){
    void* result = heap_alloc(sizeof(int) + size);

    // get connections address
    int* connections = result;

    // init connections
    *connections = 1;

    return result;
}
void share_anonymous_free(void* memory){
    heap_free(memory);
}

// vtable operators
void* share_address(Share* self){
    struct Share_* share_ = (struct Share_*)self;

    // get memory address
    void* result = (char*) share_->memory + sizeof(int);

    return result;
}
int share_connections(Share* self){
    struct Share_* share_ = (struct Share_*)self;

    // get connections address
    int result = *(int*) share_->memory;

    return result;
}
int share_flush(Share* self, tsize size){
    // android does not implement standard share memory

    return 0;
}

// object allocation and deallocation operators
void share_init() {
    // init vtable
    share_vtable = heap_alloc(sizeof(Share_VTable));
    share_vtable->address = share_address;
    share_vtable->connections = share_connections;
    share_vtable->flush = share_flush;
}
Share* share_new() {
    struct Share_* share_ = heap_alloc(sizeof(struct Share_));

    // set vtable
    share_->self.vtable = share_vtable;

    // set constructor data
    share_->name = NULL;
    share_->size = 0;
    share_->offset = 0;

    // set private data
    share_->memory = NULL;

    return (Share*)share_;
}
void share_free(Share* share) {
    struct Share_* share_ = (struct Share_*)share;

    // free private data
    if(share_->memory != NULL){
        if(share_->name != NULL){
            // try lock critical
            if (critical != NULL) {
                critical->vtable->lock(critical, UINT_64_MAX);
            }

            // free named memory
            share_named_free(share_->memory, share_->name->vtable->value(share_->name), share_->size, share_->offset);

            // try unlock critical
            if (critical != NULL) {
                critical->vtable->unlock(critical);
            }
        }else{
            // free anonymous memory
            share_anonymous_free(share_->memory);
        }
    }

    // free constructor data
    if(share_->name != NULL){
        string_free(share_->name);
    }

    // free self
    heap_free(share_);
}
Share* share_new_anonymous(tsize size, tsize offset){
    struct Share_* share_ = (struct Share_*)share_new();

    // set constructor data
    share_->size = size;
    share_->offset = offset;

    // set private data
    // new anonymous memory
    share_->memory = share_anonymous_new(share_->size, share_->offset);

    return (Share*)share_;
}
Share* share_new_named(char* name, tsize size, tsize offset){
    struct Share_* share_ = (struct Share_*)share_new();

    // set constructor data
    share_->name = string_new_printf("%s_sh", name);
    share_->size = size;
    share_->offset = offset;

    // set private data
    // try lock critical
    if (critical != NULL) {
        critical->vtable->lock(critical, UINT_64_MAX);
    }

    // new named memory
    share_->memory = share_named_new(share_->name->vtable->value(share_->name), share_->size, share_->offset);

    // try unlock critical
    if (critical != NULL) {
        critical->vtable->unlock(critical);
    }

    return (Share*)share_;
}

#endif
