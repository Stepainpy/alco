#ifndef ALCO_ARENA_H
#define ALCO_ARENA_H

#include <stddef.h>
#include <stdbool.h>

#ifndef ALCO_ARENA_DEF
#define ALCO_ARENA_DEF
#endif 

typedef struct {
    void* base;
    void* head;
    void* tail;
} alco_arena_t;

ALCO_ARENA_DEF bool alco_arena_init(
    alco_arena_t* arena, void* buffer, size_t buffer_size);
ALCO_ARENA_DEF void* alco_arena_alloc(alco_arena_t* arena, size_t size);
ALCO_ARENA_DEF void alco_arena_free_all(alco_arena_t* arena);

/**
 * alco_arena_free not provided, because you can't
 * free allocated memory, only all or nothing
 */

#endif // ALCO_ARENA_H

#ifdef ALCO_ARENA_IMPLEMENTATION

bool alco_arena_init(alco_arena_t* arena, void* buffer, size_t buffer_size) {
    if (!arena || !buffer)
        return false;
    
    arena->base = arena->head = buffer;
    arena->tail = (char*)buffer + buffer_size;

    return true;
}

void* alco_arena_alloc(alco_arena_t* arena, size_t size) {
    if ((char*)arena->head + size > (char*)arena->tail)
        return NULL;
    void* out = arena->head;
    arena->head = (char*)arena->head + size;
    return out;
}

void alco_arena_free_all(alco_arena_t* arena) {
    arena->head = arena->base;
}

#endif // ALCO_ARENA_IMPLEMENTATION