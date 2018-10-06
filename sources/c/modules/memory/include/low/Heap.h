#include <low/Type.h>

void* heap_alloc(tsize size);
void heap_copy(void* address, void* source, tsize size);
void heap_move(void* address, void* source, tsize size);
void heap_set(void* address, char source, tsize size);
void* heap_realloc(void* address, tsize size);
void heap_free(void* address);