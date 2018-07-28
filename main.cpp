#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "Chip8.h"
#include "opcodes.h"

void LoadROM(Chip8* chip8, const char* file_name)
{
    // Load ROM into the memory (starting at 0x200)
    FILE* f = fopen(file_name, "rb");
    if(f)
    {
        uint32_t bytes_read = 0;
        // Read directly into memory
        while(!feof(f))
        {
            // Read into memory
            fread(&chip8->memory[0x200 + bytes_read], 1, 1, f);
            bytes_read++;
        }
        
        fclose(f);
    }
    else
    {
        // Failed to load
        printf("Failed to load ROM %s\n", file_name);
        exit(1);
    }
}

void C8Initialise(Chip8* chip8)
{
    // Setup default program counter
    chip8->pc = 0x200;
    
    chip8->opcode = 0;
    
    // Setup fonts
    for(int i = 0; i < 80; ++i)
        chip8->memory[i] = chip8_fontset[i];
}

void C8GetOpcode(Chip8* chip8)
{
    // Get opcode at current program counter location
    chip8->opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc+1];
}

void C8EmulateCycle(Chip8* chip8)
{
    // Get the current opcode
    C8GetOpcode(chip8);
    
    
    // Opcode is now in memory, decode
    opcode_table[chip8->opcode >> 12](chip8);
}

int main()
{
    Chip8 chip8 = {};
    C8Initialise(&chip8);
    LoadROM(&chip8, "./games/INVADERS");
    
    for(int i = 0; i<100; ++i)
    {
        C8EmulateCycle(&chip8);
    }
}

