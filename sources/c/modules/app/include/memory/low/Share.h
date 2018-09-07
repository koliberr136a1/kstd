#include <memory/low/Type.h>

typedef struct Share {
    void* (*address)(struct Share* self);
    void (*flush)(struct Share* self, tsize size);
} Share;

Share* share_new(char* name, tsize size);
void share_free(Share* share);