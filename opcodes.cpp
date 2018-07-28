#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "opcodes.h"
#include "Chip8.h"

#define OpCodeNotImpl(oc) printf("Opcode 0x%X not implemented\n", oc); exit(0);

void Op_0xxx(Chip8* chip8)
{
    // There are a number of 0___ opcodes
    switch(chip8->opcode)
    {
        case 0x00E0:
        // 0x00E0 - Display Clear
        printf("Opcode: 0x%X\n", chip8->opcode);
        
        memset(chip8->gfx, 0, sizeof(chip8->gfx));
        break;
        
        case 0x00EE:
        // 0x00EE - return from subroutine
        printf("Opcode: 0x%X\n", chip8->opcode);
        
        chip8->pc = chip8->stack[--chip8->sp];
        chip8->stack[chip8->sp] = 0;
        break;
        
        default:
        // 0x0NNN
        printf("Opcode: 0x%X\n", chip8->opcode);
        
        OpCodeNotImpl(chip8->opcode);
        break;
    }
}

void Op_1NNN(Chip8* chip8)
{
    // Jumps to address NNN
    printf("Opcode: 0x%X\n", chip8->opcode);
    
    // 1NNN
    chip8->pc = chip8->opcode & 0x0FFF;
}

void Op_2NNN(Chip8* chip8)
{
    OpCodeNotImpl(chip8->opcode);
}

void Op_3XNN(Chip8* chip8)
{
    OpCodeNotImpl(chip8->opcode);
}

void Op_4XNN(Chip8* chip8)
{
    OpCodeNotImpl(chip8->opcode);
}

void Op_5XY0(Chip8* chip8)
{
    OpCodeNotImpl(chip8->opcode);
}

void Op_6XNN(Chip8* chip8)
{
    // Sets VX to NN
    printf("Opcode: 0x%X\n", chip8->opcode);
    
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->opcode & 0x00FF;
    chip8->pc += 2;
}

void Op_7XNN(Chip8* chip8)
{
    // Adds NN to VX. (Carry flag is not changed)
    printf("Opcode: 0x%X\n", chip8->opcode);
    
    chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->opcode & 0x00FF;
    chip8->pc += 2;
}

void Op_8xxx(Chip8* chip8)
{
    OpCodeNotImpl(chip8->opcode);
}

void Op_9XY0(Chip8* chip8)
{
    OpCodeNotImpl(chip8->opcode);
}

void Op_ANNN(Chip8* chip8)
{
    // Sets I to the address NNN.
    printf("Opcode: 0x%X\n", chip8->opcode);
    
    chip8->I = chip8->opcode & 0x0FFF;
    chip8->pc += 2;
}

void Op_BNNN(Chip8* chip8)
{
    OpCodeNotImpl(chip8->opcode);
}

void Op_CXNN(Chip8* chip8)
{
    OpCodeNotImpl(chip8->opcode);
}

void Op_DXYN(Chip8* chip8)
{
    printf("Opcode: 0x%X\n", chip8->opcode);
    printf("TODO\n");
    // DXYN
    /* 
Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a
height of N pixels. Each row of 8 pixels is read as bit-coded starting from
memory location I; I value doesn’t change after the execution of this
instruction. As described above, VF is set to 1 if any screen pixels are
flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t
happen
    */
    chip8->pc +=2;
}

void Op_EXxx(Chip8* chip8)
{
    OpCodeNotImpl(chip8->opcode);
}

void Op_FXxx(Chip8* chip8)
{
    OpCodeNotImpl(chip8->opcode);
}
