#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <map>
#include <stack>

enum Opcode {
    HALT,
    SET,
    PUSH,
    POP,
    EQ,
    GT,
    JMP,
    JT,
    JF,
    ADD,
    MULT,
    MOD,
    AND,
    OR,
    NOT,
    RMEM,
    WMEM,
    CALL,
    RET,
    OUT,
    IN,
    NOOP
};

uint16_t get_number(std::istream &stream) {
    return stream.get() | (stream.get() << 8);
}

std::vector<uint16_t> load_stream(std::istream &stream) {
    std::vector<uint16_t> program;
    while (!stream.eof()) program.push_back(get_number(stream));
    return program;
}


struct VirtualMachine {
    // std::map<uint16_t, uint16_t> memory{};
    std::array<uint16_t, 8> registers{{0}};
    std::stack<uint16_t> stack{};

    uint16_t get_value(const uint16_t n) {
        if (n < 32768) return n;
        else return registers.at(n - 32768);
    }

    uint16_t get_register(const uint16_t n) {
        if (n < 32768 || n >= 32776) throw n;
        return registers.at(n - 32768);
    }

    void set_register(const uint16_t n, const uint16_t v) {
        registers.at(n - 32768) = v % 32768;
    }

    void execute_program(std::vector<uint16_t> program) {
        uint16_t pc = 0;
        uint16_t ps = program.size();

        while (pc < ps) {
            uint16_t opcode = program.at(pc);
            uint16_t a = 0, b = 0, c = 0;
            if (pc < ps - 1) a = program.at(pc + 1);
            if (pc < ps - 2) b = program.at(pc + 2);
            if (pc < ps - 3) c = program.at(pc + 3);
            if (pc == 358) std::cerr << opcode << ' ' << a << ' ' << b << ' ' << c << std::endl;
            switch(opcode) {
                case HALT:
                    return;
                case SET:
                    set_register(a, get_value(b));
                    pc += 3;
                    break;
                case PUSH:
                    stack.push(get_value(a));
                    pc += 2;
                    break;
                case EQ:
                    set_register(a, get_value(b) == get_value(c) ? 1 : 0);
                    pc += 4;
                    break;
                case GT:
                    set_register(a, get_value(b) > get_value(c) ? 1 : 0);
                    pc += 4;
                    break;
                case POP:
                    set_register(a, stack.top());
                    stack.pop();
                    pc += 2;
                    break;
                case JMP:
                    // std::cout << "Jumping to " << a << std::endl;
                    pc = get_value(a);
                    break;
                case JT:
                    if (get_value(a) != 0) pc = get_value(b);
                    else pc += 3;
                    break;
                case JF:
                    if (get_value(a) == 0) pc = get_value(b);
                    else pc += 3;
                    break;
                case ADD:
                    set_register(a, get_value(b) + get_value(c));
                    pc += 4;
                    break;
                case MULT:
                    set_register(a, get_value(b) * get_value(c));
                    pc += 4;
                    break;
                case MOD:
                    set_register(a, get_value(b) % get_value(c));
                    pc += 4;
                    break;
                case AND:
                    set_register(a, get_value(b) & get_value(c));
                    pc += 4;
                    break;
                case OR:
                    set_register(a, get_value(b) | get_value(c));
                    pc += 4;
                    break;
                case NOT:
                    set_register(a, ~get_value(b));
                    pc += 3;
                    break;
                case RMEM:
                    set_register(a, program.at(get_value(b)));
                    pc += 3;
                    break;
                case WMEM:
                    program.at(get_value(a)) = get_value(b);
                    pc += 3;
                    break;
                case CALL:
                    stack.push(pc + 2);
                    pc = get_value(a);
                    break;
                case RET:
                    pc = stack.top();
                    stack.pop();
                    break;
                case OUT:
                    std::cout << (char) get_value(a);
                    pc += 2;
                    break;
                case IN:
                    set_register(a, std::cin.get());
                    pc += 2;
                    break;
                case NOOP:
                    pc += 1;
                    break;
                default:
                    std::cerr << "Unknown: " << opcode << std::endl;
                    return;
            }
        }
    }
};

int main(int argc, char *argv[]) {
    std::fstream stream{argc< 2 ? "challenge.bin" : argv[1]};
    VirtualMachine{}.execute_program(load_stream(stream));
}
