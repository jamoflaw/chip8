#ifndef _CHIP8_H
#define _CHIP8_H

#include <stdint.h>

#include "Screen.h"
#include "Chip8Input.h"

#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<Clock> Clock_Time;
typedef std::chrono::duration<int64_t, std::nano> PerfNano_Counter;

// The Chip-8's memory size
#define MEMSIZE 4096

enum GPRegisters
{
    V0,
    V1,
    V2,
    V3,
    V4,
    V5,
    V6,
    V7,
    V8,
    V9,
    V10,
    V11,
    V12,
    V13,
    V14,
    V15,
    REGISTERCOUNT,
};

const unsigned char chip8_fontset[] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

struct Chip8
{
    uint16_t opcode;
    uint8_t memory[MEMSIZE];
    
    // Registers
    uint8_t V[REGISTERCOUNT]; // GP Registers
    uint16_t I; // Index register
    uint16_t pc; // Program counter
    uint8_t gfx[SCREEN_WIDTH * SCREEN_HEIGHT];
    
    // Timers
    uint8_t delay_timer;
    uint8_t sound_timer;
    
    // System clock
    Clock_Time start_time;
    
    uint16_t stack[16];
    uint16_t sp;
    
    // Gamepad
    uint8_t keys[16];
    
    // Create a keymap to mapt Chip8 Keys to keyboard keys
    uint32_t keymap[MAX_KEYS];
    
    // Do we need to update the texture
    bool draw_flag;
    
    // Ptr to configured screen object
    Screen* screen;
};

inline void DumpRegisters(Chip8* chip8)
{
    printf("Register Dump\n");
    printf("-------------\n");
    printf("Register V0\t\t%X\n", chip8->V[0x0]);
    printf("Register V1\t\t%X\n", chip8->V[0x1]);
    printf("Register V2\t\t%X\n", chip8->V[0x2]);
    printf("Register V3\t\t%X\n", chip8->V[0x3]);
    printf("Register V4\t\t%X\n", chip8->V[0x4]);
    printf("Register V5\t\t%X\n", chip8->V[0x5]);
    printf("Register V6\t\t%X\n", chip8->V[0x6]);
    printf("Register V7\t\t%X\n", chip8->V[0x7]);
    printf("Register V8\t\t%X\n", chip8->V[0x8]);
    printf("Register V9\t\t%X\n", chip8->V[0x9]);
    printf("Register VA\t\t%X\n", chip8->V[0xA]);
    printf("Register VB\t\t%X\n", chip8->V[0xB]);
    printf("Register VC\t\t%X\n", chip8->V[0xC]);
    printf("Register VD\t\t%X\n", chip8->V[0xD]);
    printf("Register VE\t\t%X\n", chip8->V[0xE]);
    printf("Register VF\t\t%X\n", chip8->V[0xF]);
    
    printf("Register I\t\t%X\n", chip8->I);
    printf("Program Counter\t\t%X\n", chip8->pc);
}

// Functions
void C8Initialise(Chip8*);
void C8SetupInput(Chip8*);
void C8GetInput(Chip8*);
void C8EmulateCycle(Chip8*);
void C8DecrementCounters(Chip8*);

#endif