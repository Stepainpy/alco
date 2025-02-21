#ifndef ALCO_STACK_H
#define ALCO_STACK_H

#include <stddef.h>
#include <stdbool.h>

#ifndef ALCO_STACK_DEF
#define ALCO_STACK_DEF
#endif

typedef struct {
    void* base;
    void* head;
    void* tail;
} alco_stack_t;

ALCO_STACK_DEF bool alco_stack_init(
    alco_stack_t* stack, void* buffer, size_t buffer_size);
ALCO_STACK_DEF void* alco_stack_alloc(alco_stack_t* stack, size_t size);
ALCO_STACK_DEF void alco_stack_free(alco_stack_t* stack);
ALCO_STACK_DEF void alco_stack_free_all(alco_stack_t* stack);

#endif // ALCO_STACK_H

#ifdef ALCO_STACK_IMPLEMENTATION

bool alco_stack_init(alco_stack_t* stack, void* buffer, size_t buffer_size) {
    if (!stack || !buffer)
        return false;

    stack->base = stack->head = buffer;
    stack->tail = (char*)buffer + buffer_size;

    return true;
}

void* alco_stack_alloc(alco_stack_t* stack, size_t size) {
    if (!size) return NULL;
    if ((char*)stack->head + size + sizeof(void*) > (char*)stack->tail)
        return NULL;

    void* out = stack->head;
    stack->head = (char*)stack->head + size;
    *(void**)stack->head = out;
    stack->head = (char*)stack->head + sizeof(void*);

    return out;
}

void alco_stack_free(alco_stack_t* stack) {
    if (stack->head == stack->base) return;
    stack->head = (char*)stack->head - sizeof(void*);
    stack->head = *(void**)stack->head;
}

void alco_stack_free_all(alco_stack_t* stack) {
    stack->head = stack->base;
}

#endif // ALCO_STACK_IMPLEMENTATION