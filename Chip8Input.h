#ifndef _CHIP8INPUT_H
#define _CHIP8INPUT_H

#include <stdint.h>

// Predef
struct Chip8;

enum Chip8Keypad
{
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    MAX_KEYS
};

void C8SetupInput(Chip8* chip8);
void C8GetInput(Chip8* chip8);
uint32_t C8GetKeyMap(Chip8* chip8, Chip8Keypad key);
void C8SetKeyMap(Chip8* chip8, Chip8Keypad key, uint32_t key_code);

#endif

