#ifndef _OPCODES_H
#define _OPCODES_H

// Predef
struct Chip8;

// Function pointer for opcodes
typedef void (*opcode_func_ptr)(Chip8*);

void Op_0xxx(Chip8*);
void Op_1NNN(Chip8*);
void Op_2NNN(Chip8*);
void Op_3XNN(Chip8*);
void Op_4XNN(Chip8*);
void Op_5XY0(Chip8*);
void Op_6XNN(Chip8*);
void Op_7XNN(Chip8*);
void Op_8xxx(Chip8*);
void Op_9XY0(Chip8*);
void Op_ANNN(Chip8*);
void Op_BNNN(Chip8*);
void Op_CXNN(Chip8*);
void Op_DXYN(Chip8*);
void Op_EXxx(Chip8*);
void Op_FXxx(Chip8*);

const opcode_func_ptr opcode_table[] = {
    Op_0xxx, // 0x0
    Op_1NNN, // 0x1
    Op_2NNN, // 0x2
    Op_3XNN, // 0x3
    Op_4XNN, // 0x4
    Op_5XY0, // 0x5
    Op_6XNN, // 0x6
    Op_7XNN, // 0x7
    Op_8xxx, // 0x8
    Op_9XY0, // 0x9
    Op_ANNN, // 0xA
    Op_BNNN, // 0xB
    Op_CXNN, // 0xC
    Op_DXYN, // 0xD
    Op_EXxx, // 0xE
    Op_FXxx, // 0xF
};

#endif