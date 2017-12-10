#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

Stack *make_stack() {
    Stack *stack = malloc(sizeof(Stack));
    stack->value = 0;
    stack->next = NULL;
    return stack;
}

void free_stack(Stack *stack) {
    if (stack->next) free_stack(stack->next);
    free(stack);
}

void push_stack(Stack **stack, uint16_t value) {
    Stack *new_stack = make_stack();
    new_stack->value = value;
    new_stack->next = *stack;
    *stack = new_stack;
}

uint16_t pop_stack(Stack **stack) {
    uint16_t value = (*stack)->value;
    *stack = (*stack)->next;
    return value;
}
