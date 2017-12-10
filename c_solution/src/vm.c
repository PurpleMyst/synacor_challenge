#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"
#include "vm.h"
#include "stack.h"

void free_memory_address(MemoryAddress *addr) {
    if (addr->next) free_memory_address(addr->next);
    free(addr);
}

void print_instruction(MemoryAddress *addr) {
    fprintf(stderr, "%d: ", addr->pc);
    fprintf(stderr, "%s", NAMES[addr->value]);
    MemoryAddress *seek = addr->next;
    for (uint16_t i = 0; i < ARITY[addr->value]; ++i) {
        fprintf(stderr, " %d", seek->value);
        seek = seek->next;
    }
    fprintf(stderr, "\n");
}

bool next_instruction(MemoryAddress **addr) {
    if ((*addr)->value < 22) {
        uint16_t arity = ARITY[(*addr)->value];
        for (uint16_t i = 0; i <= arity; ++i) {
            if ((*addr)->next) {
                *addr = (*addr)->next;
            } else {
                return false;
            }
        }
        return true;
    } else {
        while ((*addr)->value >= 22) {
            if ((*addr)->next) {
                *addr = (*addr)->next;
            } else {
                return false;
            }
        }
        return true;
    }
}

MemoryAddress *load_file(const char *filename) {
    FILE *fobj = fopen(filename, "rb");
    uint16_t pc = 0;
    MemoryAddress *root = malloc(sizeof(MemoryAddress));
    root->prev = NULL;
    MemoryAddress *node = root;
    while (true) {
        int b1 = fgetc(fobj);
        int b2 = fgetc(fobj);
        if (b1 == EOF || b2 == EOF) break;
        uint16_t value = merge(b1, b2);

        MemoryAddress *next = malloc(sizeof(MemoryAddress));

        node->pc = pc;
        node->value = value;
        node->next = next;
        node->next->prev = node;
        node = node->next;
        pc += 1;
    }
    node->next = NULL;
    fclose(fobj);
    return root;
}

uint16_t offset_value(MemoryAddress *head, uint16_t offset) {
    for (uint16_t i = 0; i < offset; ++i) {
        if (head->next) head = head->next;
        else return 0;
    }
    return head->value;
}

bool move_to(MemoryAddress **head, uint16_t pc) {
    while ((*head)->pc != pc) {
        if ((*head)->pc > pc) {
            if ((*head)->prev) *head = (*head)->prev;
            else return false;
        } else {
            if ((*head)->next) *head = (*head)->next;
            else return false;
        }
    }
    return true;
}

uint16_t rmem(MemoryAddress *head, uint16_t pc) {
    while (head->pc != pc) {
        if (head->pc > pc) {
            if (head->prev) head = head->prev;
            else return 0;
        } else {
            if (head->next) head = head->next;
            else return 0;
        }
    }
    return head->value;
}

bool wmem(MemoryAddress *head, uint16_t pc, uint16_t value) {
    while (head->pc != pc) {
        if (head->pc > pc) {
            if (head->prev) head = head->prev;
            else return false;
        } else {
            if (head->next) head = head->next;
            else return false;
        }
    }
    head->value = value;
    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "USAGE: vm FILENAME\n");
        return 1;
    }

    MemoryAddress *root = load_file(argv[1]);
    MemoryAddress *conductor = root;
    Stack *stack = make_stack();

    uint16_t registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    while (true) {
        // print_instruction(conductor);
        uint16_t a = offset_value(conductor, 1);
        uint16_t b = offset_value(conductor, 2);
        uint16_t c = offset_value(conductor, 3);
        switch (conductor->value) {
            case NOOP:
                ADVANCE(&conductor);
                break;
            case PUSH:
                push_stack(&stack, VALUE(a));
                ADVANCE(&conductor);
                break;
            case POP:
                registers[REGISTER(a)] = pop_stack(&stack);
                ADVANCE(&conductor);
                break;
            case SET:
                registers[REGISTER(a)] = VALUE(b);
                ADVANCE(&conductor);
                break;
            case EQ:
                registers[REGISTER(a)] = VALUE(b) == VALUE(c);
                ADVANCE(&conductor);
                break;
            case GT:
                registers[REGISTER(a)] = VALUE(b) > VALUE(c);
                ADVANCE(&conductor);
                break;
            case JMP:
                JMP(&conductor, VALUE(a));
                break;
            case JT:
                if (VALUE(a) != 0) {
                    JMP(&conductor, VALUE(b));
                } else {
                    ADVANCE(&conductor);
                }
                break;
            case JF:
                if (VALUE(a) == 0) {
                    JMP(&conductor, VALUE(b));
                } else {
                    ADVANCE(&conductor);
                }
                break;
            case ADD:
                registers[REGISTER(a)] = LIMIT(VALUE(b) + VALUE(c));
                ADVANCE(&conductor);
                break;
            case MULT:
                registers[REGISTER(a)] = LIMIT(VALUE(b) * VALUE(c));
                ADVANCE(&conductor);
                break;
            case MOD:
                registers[REGISTER(a)] = LIMIT(VALUE(b) % VALUE(c));
                ADVANCE(&conductor);
                break;
            case AND:
                registers[REGISTER(a)] = LIMIT(VALUE(b) & VALUE(c));
                ADVANCE(&conductor);
                break;
            case OR:
                registers[REGISTER(a)] = LIMIT(VALUE(b) | VALUE(c));
                ADVANCE(&conductor);
                break;
            case NOT:
                registers[REGISTER(a)] = LIMIT((uint16_t) ~VALUE(b));
                ADVANCE(&conductor);
                break;
            case RMEM:
                registers[REGISTER(a)] = rmem(conductor, VALUE(b));
                ADVANCE(&conductor);
                break;
            case WMEM:
                wmem(conductor, VALUE(a), VALUE(b));
                ADVANCE(&conductor);
                break;
            case CALL:
                push_stack(&stack, conductor->pc + 2);
                JMP(&conductor, VALUE(a));
                break;
            case RET:
                JMP(&conductor, pop_stack(&stack));
                break;
            case OUT:
                printf("%c", VALUE(a));
                ADVANCE(&conductor);
                break;
            case IN:
                registers[REGISTER(a)] = getchar();
                ADVANCE(&conductor);
                break;
            case HALT:
            default:
                goto cleanup;
        }
    }

    cleanup:
    free_memory_address(root);
    free_stack(stack);
}
