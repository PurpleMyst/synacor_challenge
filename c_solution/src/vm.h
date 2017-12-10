#ifndef _VM_H_
#define _VM_H_
#include <stdint.h>

#define LIMIT(x) ((x) % 32768)
#define REGISTER(x) ((x) - 32768)
#define VALUE(x) (x >= 32768 ? (x < 32776 ? registers[REGISTER(x)] : LIMIT(x)) : x)

#define ADVANCE(r) if (!next_instruction(r)) goto cleanup
#define JMP(r, x) if(!move_to(r, x)) goto cleanup

enum Opcode {HALT, SET, PUSH, POP, EQ, GT, JMP, JT, JF, ADD, MULT, MOD, AND, OR, NOT, RMEM, WMEM, CALL, RET, OUT, IN, NOOP};
const uint16_t ARITY[] = {0, 2, 1, 1, 3, 3, 1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 2, 1, 0, 1, 1, 0};
const char *NAMES[] = {"halt", "set", "push", "pop", "eq", "gt", "jmp", "jt", "jf", "add", "mult", "mod", "and", "or", "not", "rmem", "wmem", "call", "ret", "out", "in", "noop"};

typedef struct sMemoryAddress {
    uint16_t pc;
    uint16_t value;
    struct sMemoryAddress *next;
    struct sMemoryAddress *prev;
} MemoryAddress;

void free_memory_address(MemoryAddress*);
void print_instruction(MemoryAddress*);
bool next_instruction(MemoryAddress**);
MemoryAddress *load_file(const char*);
uint16_t offset_value(MemoryAddress*, uint16_t);
bool move_to(MemoryAddress**, uint16_t);
uint16_t rmem(MemoryAddress*, uint16_t);
bool wmem(MemoryAddress*, uint16_t, uint16_t);

#endif
