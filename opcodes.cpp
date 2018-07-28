#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>

#include "opcodes.h"
#include "Chip8.h"
#include "Chip8Input.h"

#define OpCodeNotImpl(oc) printf("Opcode 0x%X not implemented\n", oc); exit(0);

// Define some shortcuts for extracting X and Y from the opcode
#define REG_X (chip8->opcode & 0x0F00) >> 8
#define REG_Y (chip8->opcode & 0x00F0) >> 4

void Op_0xxx(Chip8* chip8)
{
    // There are a number of 0___ opcodes
    switch(chip8->opcode)
    {
        case 0x00E0:
        // 0x00E0 - Display Clear
        memset(chip8->gfx, 0, sizeof(chip8->gfx));
        break;
        
        case 0x00EE:
        // 0x00EE - return from subroutine
        chip8->pc = chip8->stack[--chip8->sp];
        // Flatten the memory
        chip8->stack[chip8->sp] = 0;
        break;
        
        default:
        // 0x0NNN
        //OpCodeNotImpl(chip8->opcode);
        break;
    }
}

void Op_1NNN(Chip8* chip8)
{
    // 0x1NNN
    // Jumps to address NNN
    chip8->pc = chip8->opcode & 0x0FFF;
}

void Op_2NNN(Chip8* chip8)
{
    // Calls subroutine at NNN.
    // Push next pc onto stack
    chip8->stack[chip8->sp++] = chip8->pc;
    
    // Jump to the subroutine location
    chip8->pc = chip8->opcode & 0x0FFF;
}

void Op_3XNN(Chip8* chip8)
{
    /*
    Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
    */
    if(chip8->V[REG_X] == (chip8->opcode & 0x00FF))
    {
        // Skip the next instruction
        chip8->pc += 2;
    }
}

void Op_4XNN(Chip8* chip8)
{
    /*
    Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
    */
    if(chip8->V[REG_X] != (chip8->opcode & 0x00FF))
    {
        // Skip the next instruction
        chip8->pc += 2;
    }
}

void Op_5XY0(Chip8* chip8)
{
    /*
    Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
    */
    if(chip8->V[REG_X] == chip8->V[REG_Y])
    {
        // Skip the next instruction
        chip8->pc += 2;
    }
}

void Op_6XNN(Chip8* chip8)
{
    // Sets VX to NN
    chip8->V[REG_X] = (chip8->opcode & 0x00FF);
}

void Op_7XNN(Chip8* chip8)
{
    // Adds NN to VX. (Carry flag is not changed)
    chip8->V[REG_X] += (chip8->opcode & 0x00FF);
}

void Op_8xxx(Chip8* chip8)
{
    // There are a number of 8___ opcodes
    switch(chip8->opcode & 0xF00F)
    {
        case 0x8000:
        // 0x8XY0 - Sets VX to the value of VY.
        chip8->V[REG_X] = chip8->V[REG_Y];
        break;
        
        case 0x8001:
        // 0x8XY1 - Sets VX to VX | VY. (Bitwise OR operation).
        chip8->V[REG_X] = (chip8->V[REG_X] | chip8->V[REG_Y]);
        break;
        
        case 0x8002:
        // 0x8XY2 - Sets VX to VX and VY. (Bitwise AND operation).
        chip8->V[REG_X] = (chip8->V[REG_X] & chip8->V[REG_Y]);
        break;
        
        case 0x8003:
        // 0x8XY3 - Sets VX to VX xor VY.
        chip8->V[REG_X] = (chip8->V[REG_X] ^ chip8->V[REG_Y]);
        break;
        
        case 0x8004:
        // 0x8XY4 - Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
        if(chip8->V[REG_X] > (0xFF - chip8->V[REG_Y]))
        {
            // Set carry flag
            chip8->V[0xF] = 1;
        }
        else
        {
            // Unset carry flag
            chip8->V[0xF] = 0;
        }
        chip8->V[REG_X] += chip8->V[REG_Y];
        break;
        
        case 0x8005:
        // 0x8XY5
        // VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when
        // there isn't.
        if(chip8->V[REG_X] <= chip8->V[REG_Y])
        {
            // Underflow - Set borrow flag
            chip8->V[0xF] = 0;
        }
        else
        {
            // Unset borrow flag
            chip8->V[0xF] = 1;
        }
        chip8->V[REG_X] -= chip8->V[REG_Y];
        break;
        
        case 0x8006:
        // 0x8XY6
        // Shifts VY right by one and stores the result to VX (VY remains unchanged).
        // VF is set to the value of the least significant bit of VY before the
        // shift
        chip8->V[0xF] = chip8->V[REG_Y] & 0x01;
        chip8->V[REG_X] = chip8->V[REG_Y] >> 1;
        break;
        
        case 0x8007:
        // 0x8XY7
        // Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when
        // there isn't.
        if(chip8->V[REG_Y] < chip8->V[REG_X])
        {
            // Underflow - Set borrow flag to 0
            chip8->V[0xF] = 0;
        }
        else
        {
            // Unset borrow flag to 1
            chip8->V[0xF] = 1;
        }
        chip8->V[REG_X] = chip8->V[REG_Y] - chip8->V[REG_X];
        break;
        
        case 0x800E:
        // 0x8XYE
        // Shifts VY left by one and copies the result to VX. VF is set to the value
        // of the most significant bit of VY before the shift.
        chip8->V[0xF] = chip8->V[REG_Y] >> 7;
        
        assert(chip8->V[0xF] == 1 || chip8->V[0xF] == 0);
        
        chip8->V[REG_X] = chip8->V[REG_Y] = chip8->V[REG_Y] << 1;
        break;
        
        default:
        OpCodeNotImpl(chip8->opcode);
        break;
    }
}

void Op_9XY0(Chip8* chip8)
{
    /*
    Skip next instruction if Vx != Vy.
    */
    if(chip8->V[REG_X] != chip8->V[REG_Y])
    {
        chip8->pc += 2;
    }
}

void Op_ANNN(Chip8* chip8)
{
    // Sets I to the address NNN.
    chip8->I = chip8->opcode & 0x0FFF;
}

void Op_BNNN(Chip8* chip8)
{
    // Jumps to the address NNN plus V0.
    chip8->pc = (chip8->opcode & 0x0FFF) + chip8->V[0];
}

#include <cstdlib>

void Op_CXNN(Chip8* chip8)
{
    // Sets VX to the result of a bitwise and operation on a random number
    // (Typically: 0 to 255) AND NN.
    chip8->V[REG_X] = (chip8->opcode & 0x00FF) & (rand() % 255);
}

void Op_DXYN(Chip8* chip8)
{
    // DXYN
    /* 
Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a
height of N pixels. Each row of 8 pixels is read as bit-coded starting from
memory location I; I value doesn’t change after the execution of this
instruction. As described above, VF is set to 1 if any screen pixels are
flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t
happen
    */
    
    // Reset V[0xF]
    chip8->V[0xF] = 0;
    
    uint32_t VX = chip8->V[REG_X];
    uint32_t VY = chip8->V[REG_Y];
    uint32_t height = (chip8->opcode & 0x000F);
    
    // Setup loop
    for(uint32_t y=0; y<height; ++y)
    {
        // Get the row from memory
        uint8_t pixel_row = chip8->memory[chip8->I + y];
        for(uint32_t x=0; x<8; ++x)
        {
            // Graphics are drawn by XOR-ing the value of each pixel - if a value
            // changes from a 1 to a 0 V[0xF] is set to 1
            uint8_t old_pixel_value = chip8->gfx[((VY + y) * SCREEN_WIDTH) + (VX + x)] & 0x01;
            
            uint8_t new_pixel_value = ((pixel_row >> (7 - x)) & 0x01);
            
            assert(old_pixel_value == 0 || old_pixel_value == 1);
            assert(new_pixel_value == 0 || new_pixel_value == 1);
            
            uint8_t tempVal = old_pixel_value ^ new_pixel_value;
            
            chip8->gfx[((VY + y) * SCREEN_WIDTH) + (VX + x)] = tempVal == 1 ? 255 : 0;
            if(old_pixel_value == 1 && new_pixel_value == 1)
            {
                chip8->V[0xF] = 1;
            }
        }
    }
    
    chip8->draw_flag = true;
}

void Op_EXxx(Chip8* chip8)
{
    // There are a number of EX__ opcodes
    switch(chip8->opcode & 0xF0FF)
    {
        case 0xE09E:
        /* 0xEX9E
    Skips the next instruction if the key stored in VX is pressed. (Usually the
    next instruction is a jump to skip a code block)
        */
        if(chip8->keys[chip8->V[REG_X]] != 0)
        {
            // Key down, skip an extra instruction
            chip8->pc +=2;
        }
        break;
        
        case 0xE0A1:
        /*
        Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
        */
        if(chip8->keys[chip8->V[REG_X]] == 0)
        {
            // Key not down, skip an extra instruction
            chip8->pc +=2;
        }
        break;
        
        default:
        OpCodeNotImpl(chip8->opcode);
    }
}

#include <unistd.h>
void Op_FXxx(Chip8* chip8)
{
    // There are a number of FX__ opcodes
    switch(chip8->opcode & 0xF0FF)
    {
        case 0xF007:
        /* 
    0xFX07
    Sets VX to the value of the delay timer.
        */
        chip8->V[REG_X] = chip8->delay_timer;
        break;
        
        case 0xF00A:
        // 0xFX0A
        // A key press is awaited, and then stored in VX. (Blocking Operation. All
        // instruction halted until next key event)
        while(true)
        {
            // Make sure we poll so the input state is refreshed
            glfwPollEvents();
            
            C8GetInput(chip8);
            
            bool key_changed = false;
            for(int i=0; i<MAX_KEYS; ++i)
            {
                if(chip8->keys[i] != 0)
                {
                    chip8->V[REG_X] = i;
                    key_changed = true;
                    break;
                }
            }
            
            if(key_changed)
            {
                break;
            }
            
            sleep(1);
        }
        break;
        
        case 0xF015:
        // 0xFX15
        // Sets the delay timer to VX.
        chip8->delay_timer = chip8->V[REG_X];
        break;
        
        case 0xF018:
        // 0xFX15
        // Sets the sound timer to VX.
        chip8->sound_timer = chip8->V[REG_X];
        break;
        
        case 0xF01E:
        // 0xFX1E - Adds VX to I
        if(((uint32_t)chip8->I + (uint32_t)chip8->V[REG_X]) > 0xFFF)
        {
            chip8->V[0xF] = 1;
        }
        else
        {
            chip8->V[0xF] = 0;
        }
        
        chip8->I += chip8->V[REG_X];
        break;
        
        case 0xF029:
        /*
    0xFX29
    
    Sets I to the location of the sprite for the character in VX. Characters
        0-F (in hexadecimal) are represented by a 4x5 font.
        */
        chip8->I = chip8->V[(REG_X)] * 5;
        break;
        
        case 0xF033:
        // 0xFX33
        /*
        Stores the binary-coded decimal representation of VX, with the most
    significant of three digits at the address in I, the middle digit at I plus
    1, and the least significant digit at I plus 2. (In other words, take the
    decimal representation of VX, place the hundreds digit in memory at location
    in I, the tens digit at location I+1, and the ones digit at location I+2.)
        */
        chip8->memory[chip8->I] = chip8->V[REG_X] / 100;
        chip8->memory[chip8->I + 1] = (chip8->V[REG_X] / 10) % 10;
        chip8->memory[chip8->I + 2] = (chip8->V[REG_X] % 100) % 10;
        break;
        
        case 0xF055:
        /*
        Stores V0 to VX (including VX) in memory starting at address I. The offset
    from I is increased by 1 for each value written, but I itself is left
    unmodified.
        */
        for(int v=0; v<= REG_X; ++v)
        {
            chip8->memory[chip8->I + v] = chip8->V[v];
        }
        break;
        
        case 0xF065:
        /*
        Fills V0 to VX (including VX) with values from memory starting at address I.
    The offset from I is increased by 1 for each value written, but I itself is
    left unmodified.
        */
        for(int v=0; v<= REG_X; ++v)
        {
            chip8->V[v] = chip8->memory[chip8->I + v];
        }
        break;
        
        default:
        OpCodeNotImpl(chip8->opcode);
        break;
    }
}
