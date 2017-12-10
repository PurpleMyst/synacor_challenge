#ifndef _STACK_H_
#define _STACK_H_
#include <stdint.h>

typedef struct sStack {
    uint16_t value;
    struct sStack *next;
} Stack;

Stack *make_stack();
void free_stack(Stack*);
void push_stack(Stack**, uint16_t);
uint16_t pop_stack(Stack**);

#endif
