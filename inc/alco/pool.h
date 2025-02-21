#ifndef ALCO_POOL_H
#define ALCO_POOL_H

#include <stddef.h>
#include <stdbool.h>

#ifndef ALCO_POOL_DEF
#define ALCO_POOL_DEF
#endif

typedef struct {
    void* head;          // pointer to free node
    void* chunks;        // base pointer to chunks
    size_t chunk_size;   // size of one chunk
    size_t chunk_count;  // count of chunks
} alco_pool_t;

ALCO_POOL_DEF bool alco_pool_init(
    alco_pool_t* pool, size_t sizeof_type,
    void* buffer, size_t buffer_size);
ALCO_POOL_DEF void* alco_pool_alloc(alco_pool_t* pool);
ALCO_POOL_DEF void alco_pool_free(alco_pool_t* pool, void* ptr);
ALCO_POOL_DEF void alco_pool_free_all(alco_pool_t* pool);

#endif // ALCO_POOL_H

#ifdef ALCO_POOL_IMPLEMENTATION

bool alco_pool_init(alco_pool_t* pool, size_t sizeof_type, void* buffer, size_t buffer_size) {
    if (!pool || !buffer)
        return false;

    const size_t chunk_size  =
          sizeof(void*) > sizeof_type
        ? sizeof(void*) : sizeof_type;
    const size_t chunk_count = buffer_size / chunk_size;

    pool->chunks = buffer;
    pool->head = chunk_count ? buffer : NULL;
    pool->chunk_size  = chunk_size;
    pool->chunk_count = chunk_count;

    alco_pool_free_all(pool);

    return true;
}

void* alco_pool_alloc(alco_pool_t* pool) {
    void* out = pool->head;
    if (out) pool->head = *(void**)out;
    return out;
}

void alco_pool_free(alco_pool_t* pool, void* ptr) {
    if (!ptr || ptr < pool->chunks) return;
    size_t len = (char*)ptr - (char*)pool->chunks;
    if (len > pool->chunk_size * pool->chunk_count) return;
    if (len % pool->chunk_size != 0) return;
    
    *(void**)ptr = pool->head;
    pool->head = ptr;
}

void alco_pool_free_all(alco_pool_t* pool) {
    char* chunk = (char*)pool->chunks;
    for (size_t i = 0; pool->chunk_count > 0 && i < pool->chunk_count - 1; i++) {
        char* next_chunk = chunk + pool->chunk_size;
        *(void**)chunk = next_chunk;
        chunk = next_chunk;
    }
    if (pool->chunk_count)
        *(void**)chunk = NULL;
}

#endif // ALCO_POOL_IMPLEMENTATION