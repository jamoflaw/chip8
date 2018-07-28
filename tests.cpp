#include "Chip8.h"

#include <cassert>
#include <cstdlib>

void CheckC8Structures(Chip8* input, Chip8* expected)
{
    // Opcode
    assert(input->opcode == expected->opcode);
    
    // Memory state
    for(uint16_t b=0; b<MEMSIZE; ++b)
    {
        assert(input->memory[b] == expected->memory[b]);
    }
    
    // Registers
    for(uint8_t r=0; r<REGISTERCOUNT; ++r)
    {
        assert(input->V[r] == expected->V[r]);
    }
    
    // Register I
    assert(input->I == expected->I);
    
    // Program counter
    assert(input->pc == expected->pc);
    
    // GFX Memory
    for(uint16_t i=0; i<(SCREEN_WIDTH * SCREEN_HEIGHT); ++i)
    {
        assert(input->gfx[i] == expected->gfx[i]);
    }
    
    // Timers
    assert(input->delay_timer == expected->delay_timer);
    assert(input->sound_timer == expected->sound_timer);
    
    // Skip clock start time as this is dynamic
    
    // Stack
    for(uint16_t s=0; s<16; ++s)
    {
        assert(input->stack[s] == expected->stack[s]);
    }
    
    // Stack Ptr
    assert(input->sp == expected->sp);
    
    // Dont check keys
    // Dont check keymap
    
    // Draw flag
    assert(input->draw_flag == expected->draw_flag);
    
    // Dont check screen ptr
}

void Test(const char* opcode, Chip8 input, Chip8 expected)
{
    printf("Testing %s...", opcode);
    
    // Emulate CPU
    C8EmulateCycle(&input);
    
    // Decrement counters
    C8DecrementCounters(&input);
    
    // Check against expected
    CheckC8Structures(&input, &expected);
    
    printf("PASS\n");
}

Chip8 SetupTestC8(uint16_t opcode)
{
    // Setup input C8 Struct
    Chip8 chip8 = {};
    C8Initialise(&chip8);
    
    // Place the opcode at 0x200
    chip8.memory[0x200] = opcode >> 8;
    chip8.memory[0x201] = opcode & 0x00FF;
    
    chip8.opcode = opcode;
    
    return chip8;
}

void Test_0x00E0()
{
    Chip8 input = SetupTestC8(0x00E0);
    
    // This should clear the video RAM so we need to add some random bytes to the GFX
    for(int x=0; x<(SCREEN_WIDTH * SCREEN_HEIGHT); ++x)
    {
        input.gfx[x] = rand() % 255;
    }
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x00E0);
    expected.pc +=2;
    
    Test("0x00E0", input, expected);
}

void Test_0x00EE()
{
    Chip8 input = SetupTestC8(0x00EE);
    input.stack[0] = 0x210;
    input.sp = 1;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x00EE);
    expected.sp = 0;
    expected.stack[0] = 0;
    expected.pc = 0x210;
    
    Test("0x00EE", input, expected);
}

void Test_0x1NNN()
{
    Chip8 input = SetupTestC8(0x1123);
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x1123);
    expected.pc = 0x0123;
    Test("0x1NNN", input, expected);
}

void Test_0x2NNN()
{
    Chip8 input = SetupTestC8(0x2123);
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x2123);
    expected.pc = 0x0123;
    expected.stack[0] = 0x202;
    expected.sp = 1;
    
    Test("0x2NNN", input, expected);
}

void Test_0x3XNN_NotEqual()
{
    Chip8 input = SetupTestC8(0x3466);
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x3466);
    expected.pc += 2;
    
    Test("0x3XNN NOT EQUAL", input, expected);
}

void Test_0x3XNN_Equal()
{
    Chip8 input = SetupTestC8(0x3466);
    input.V[4] = 0x66;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x3466);
    expected.V[4] = 0x66;
    expected.pc += 4;
    
    Test("0x3XNN EQUAL", input, expected);
}

void Test_0x4XNN_NotEqual()
{
    Chip8 input = SetupTestC8(0x4466);
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x4466);
    expected.pc += 4;
    
    Test("0x4XNN NOT EQUAL", input, expected);
}

void Test_0x4XNN_Equal()
{
    Chip8 input = SetupTestC8(0x4466);
    input.V[4] = 0x66;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x4466);
    expected.V[4] = 0x66;
    expected.pc += 2;
    
    Test("0x4XNN EQUAL", input, expected);
}

void Test_0x5XY0_NotEqual()
{
    Chip8 input = SetupTestC8(0x5450);
    input.V[4] = 0x23;
    input.V[5] = 0x13;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x5450);
    expected.pc += 2;
    expected.V[4] = 0x23;
    expected.V[5] = 0x13;
    
    Test("0x5XY0 NOT EQUAL", input, expected);
}

void Test_0x5XY0_Equal()
{
    Chip8 input = SetupTestC8(0x5450);
    input.V[4] = 0x23;
    input.V[5] = 0x23;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x5450);
    expected.pc += 4;
    expected.V[4] = 0x23;
    expected.V[5] = 0x23;
    
    Test("0x5XY0 EQUAL", input, expected);
}

void Test_0x6XNN()
{
    Chip8 input = SetupTestC8(0x67FF);
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x67FF);
    expected.pc += 2;
    expected.V[7] = 0xFF;
    
    Test("0x6XNN", input, expected);
}

void Test_0x7XNN_1()
{
    Chip8 input = SetupTestC8(0x7923);
    input.V[9] = 0x07;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x7923);
    expected.pc += 2;
    expected.V[9] = (0x07 + 0x23);
    
    Test("0x7XNN No Overflow", input, expected);
}

void Test_0x7XNN_2()
{
    Chip8 input = SetupTestC8(0x7904);
    input.V[9] = 255;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x7904);
    expected.pc += 2;
    expected.V[9] = 0x03;
    
    Test("0x7XNN Overflow", input, expected);
}

void Test_0x8XY0()
{
    Chip8 input = SetupTestC8(0x8340);
    input.V[3] = 0xFF;
    input.V[4] = 0xEF;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x8340);
    expected.pc += 2;
    expected.V[3] = 0xEF;
    expected.V[4] = 0xEF;
    
    Test("0x8XY0", input, expected);
}

void Test_0x8XY1()
{
    Chip8 input = SetupTestC8(0x8891);
    input.V[8] = 0xF4;
    input.V[9] = 0xE5;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x8891);
    expected.pc += 2;
    expected.V[8] = 0xF5;
    expected.V[9] = 0xE5;
    
    Test("0x8XY1", input, expected);
}

void Test_0x8XY2()
{
    Chip8 input = SetupTestC8(0x8892);
    input.V[8] = 0xF4;
    input.V[9] = 0xE5;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x8892);
    expected.pc += 2;
    expected.V[8] = 0xE4;
    expected.V[9] = 0xE5;
    
    Test("0x8XY2", input, expected);
}

void Test_0x8XY3()
{
    Chip8 input = SetupTestC8(0x8893);
    input.V[8] = 0xF4;
    input.V[9] = 0xE5;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x8893);
    expected.pc += 2;
    expected.V[8] = 0x11;
    expected.V[9] = 0xE5;
    
    Test("0x8XY3", input, expected);
}

void Test_0x8XY4_NoCarry()
{
    Chip8 input = SetupTestC8(0x8894);
    input.V[8] = 0x04;
    input.V[9] = 0x03;
    input.V[0xF] = 5; // Set to something non 0 or 1 to begin with for testing
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x8894);
    expected.pc += 2;
    expected.V[8] = 0x07;
    expected.V[9] = 0x03;
    expected.V[0xF] = 0;
    
    Test("0x8XY4 No Carry", input, expected);
}

void Test_0x8XY4_Carry()
{
    Chip8 input = SetupTestC8(0x8894);
    input.V[8] = 0xFE;
    input.V[9] = 0x05;
    input.V[0xF] = 5; // Set to something non 0 or 1 to begin with for testing
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x8894);
    expected.pc += 2;
    expected.V[8] = 0x03;
    expected.V[9] = 0x05;
    expected.V[0xF] = 1;
    
    Test("0x8XY4 Carry", input, expected);
}

void Test_0x8XY5_Borrow()
{
    Chip8 input = SetupTestC8(0x8895);
    input.V[8] = 0x01;
    input.V[9] = 0x05;
    input.V[0xF] = 5; // Set to something non 0 or 1 to begin with for testing
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x8895);
    expected.pc += 2;
    expected.V[8] = 0xFC;
    expected.V[9] = 0x05;
    expected.V[0xF] = 0;
    
    Test("0x8XY5 Borrow", input, expected);
}

void Test_0x8XY5_NoBorrow()
{
    Chip8 input = SetupTestC8(0x8895);
    input.V[8] = 0x05;
    input.V[9] = 0x03;
    input.V[0xF] = 5; // Set to something non 0 or 1 to begin with for testing
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x8895);
    expected.pc += 2;
    expected.V[8] = 0x02;
    expected.V[9] = 0x03;
    expected.V[0xF] = 1;
    
    Test("0x8XY5 No Borrow", input, expected);
}

void Test_0x8XY6_Odd()
{
    Chip8 input = SetupTestC8(0x8896);
    input.V[8] = 0x05;
    input.V[9] = 0x03;
    input.V[0xF] = 5; // Set to something non 0 or 1 to begin with for testing
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x8896);
    expected.pc += 2;
    expected.V[8] = 0x01;
    expected.V[9] = 0x03;
    expected.V[0xF] = 1;
    
    Test("0x8XY6 Odd", input, expected);
}


void Test_0x8XY6_Even()
{
    Chip8 input = SetupTestC8(0x8896);
    input.V[8] = 0x05;
    input.V[9] = 0x04;
    input.V[0xF] = 5; // Set to something non 0 or 1 to begin with for testing
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x8896);
    expected.pc += 2;
    expected.V[8] = 0x02;
    expected.V[9] = 0x04;
    expected.V[0xF] = 0;
    
    Test("0x8XY6 Even", input, expected);
}

void Test_0x8XY7_NoBorrow()
{
    Chip8 input = SetupTestC8(0x8897);
    input.V[8] = 25;
    input.V[9] = 65;
    input.V[0xF] = 5; // Set to something non 0 or 1 to begin with for testing
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x8897);
    expected.pc += 2;
    expected.V[8] = 40;
    expected.V[9] = 65;
    expected.V[0xF] = 1;
    
    Test("0x8XY7 No Borrow", input, expected);
}

void Test_0x8XY7_Borrow()
{
    Chip8 input = SetupTestC8(0x8897);
    input.V[8] = 0x05;
    input.V[9] = 0x04;
    input.V[0xF] = 5; // Set to something non 0 or 1 to begin with for testing
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x8897);
    expected.pc += 2;
    expected.V[8] = 0xFF;
    expected.V[9] = 0x04;
    expected.V[0xF] = 0;
    
    Test("0x8XY7 Borrow", input, expected);
}

void Test_0x8XYE_MSB0()
{
    Chip8 input = SetupTestC8(0x889E);
    input.V[8] = 0x03;
    input.V[9] = 0x03;
    input.V[0xF] = 5; // Set to something non 0 or 1 to begin with for testing
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x889E);
    expected.pc += 2;
    expected.V[8] = 0x06;
    expected.V[9] = 0x06;
    expected.V[0xF] = 0;
    
    Test("0x8XYE MSB0", input, expected);
}

void Test_0x8XYE_MSB1()
{
    Chip8 input = SetupTestC8(0x889E);
    input.V[8] = 0xFF;
    input.V[9] = 0xFE;
    input.V[0xF] = 5; // Set to something non 0 or 1 to begin with for testing
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x889E);
    expected.pc += 2;
    expected.V[8] = 0xFC;
    expected.V[9] = 0xFC;
    expected.V[0xF] = 1;
    
    Test("0x8XYE MSB1", input, expected);
}

void Test_0x9XY0_Equal()
{
    Chip8 input = SetupTestC8(0x9890);
    input.V[8] = 0xFE;
    input.V[9] = 0xFE;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x9890);
    expected.pc += 2;
    expected.V[8] = 0xFE;
    expected.V[9] = 0xFE;
    
    Test("0x9XY0 Equal", input, expected);
}

void Test_0x9XY0_NotEqual()
{
    Chip8 input = SetupTestC8(0x9890);
    input.V[8] = 0xFE;
    input.V[9] = 0x0E;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0x9890);
    expected.pc += 4;
    expected.V[8] = 0xFE;
    expected.V[9] = 0x0E;
    
    Test("0x9XY0 Not Equal", input, expected);
}

void Test_0xANNN()
{
    Chip8 input = SetupTestC8(0xA123);
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xA123);
    expected.pc += 2;
    expected.I = 0x123;
    
    Test("0xANNN", input, expected);
}

void Test_0xBNNN()
{
    Chip8 input = SetupTestC8(0xB123);
    input.V[0] = 0x03;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xB123);
    expected.V[0] = 0x03;
    expected.pc = 0x123 + 0x03;
    
    Test("0xBNNN", input, expected);
}

void Test_0xEX9E_Pressed()
{
    Chip8 input = SetupTestC8(0xE39E);
    input.V[3] = 6;
    input.keys[6] = 1;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xE39E);
    expected.V[3] = 6;
    expected.keys[6] = 1;
    expected.pc +=4;
    
    Test("0xE39E Pressed", input, expected);
}

void Test_0xEX9E_NotPressed()
{
    Chip8 input = SetupTestC8(0xE39E);
    input.V[3] = 6;
    input.keys[6] = 0;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xE39E);
    expected.V[3] = 6;
    expected.keys[6] = 0;
    expected.pc +=2;
    
    Test("0xE39E Not Pressed", input, expected);
}

void Test_0xEXA1_Pressed()
{
    Chip8 input = SetupTestC8(0xE3A1);
    input.V[3] = 7;
    input.keys[7] = 1;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xE3A1);
    expected.V[3] = 7;
    expected.keys[7] = 1;
    expected.pc +=2;
    
    Test("0xE3A1 Pressed", input, expected);
}

void Test_0xEXA1_NotPressed()
{
    Chip8 input = SetupTestC8(0xE3A1);
    input.V[3] = 7;
    input.keys[7] = 0;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xE3A1);
    expected.V[3] = 7;
    expected.keys[7] = 0;
    expected.pc +=4;
    
    Test("0xE3A1 Not Pressed", input, expected);
}

void Test_0xFX07()
{
    Chip8 input = SetupTestC8(0xF607);
    input.V[6] = 25;
    input.delay_timer = 45;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xF607);
    expected.V[6] = 45;
    expected.delay_timer = 44; // Always the first cylce will decrement
    expected.pc +=2;
    
    Test("0xFX07", input, expected);
}

void Test_0xFX15()
{
    Chip8 input = SetupTestC8(0xF615);
    input.V[6] = 25;
    input.delay_timer = 45;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xF615);
    expected.V[6] = 25;
    expected.delay_timer = 24; // Always the first cycle will decrement
    expected.pc +=2;
    
    Test("0xFX15", input, expected);
}

void Test_0xFX18()
{
    Chip8 input = SetupTestC8(0xF618);
    input.V[6] = 25;
    input.sound_timer = 45;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xF618);
    expected.V[6] = 25;
    expected.sound_timer = 24; // Always the first cycle will decrement
    expected.pc +=2;
    
    Test("0xFX18", input, expected);
}


void Test_0xFX1E()
{
    Chip8 input = SetupTestC8(0xF61E);
    input.V[6] = 25;
    input.I = 45;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xF61E);
    expected.V[6] = 25;
    expected.I = 70;
    expected.pc +=2;
    
    Test("0xFX1E", input, expected);
}

void Test_0xFX29()
{
    Chip8 input = SetupTestC8(0xF629);
    input.V[6] = 0x01;
    input.I = 2;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xF629);
    expected.V[6] = 0x01;
    expected.I = 5;
    expected.pc +=2;
    
    Test("0xFX29", input, expected);
}

void Test_0xFX33()
{
    Chip8 input = SetupTestC8(0xF633);
    input.V[6] = 123;
    input.I = 0x250;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xF633);
    expected.V[6] = 123;
    expected.I = 0x250;
    expected.pc +=2;
    
    expected.memory[0x250] = 1;
    expected.memory[0x251] = 2;
    expected.memory[0x252] = 3;
    
    Test("0xFX33", input, expected);
}

void Test_0xFX55()
{
    Chip8 input = SetupTestC8(0xF655);
    input.V[0] = 1;
    input.V[1] = 2;
    input.V[2] = 3;
    input.V[3] = 4;
    input.V[4] = 5;
    input.V[5] = 6;
    input.V[6] = 7;
    input.I = 0x250;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xF655);
    expected.V[0] = 1;
    expected.V[1] = 2;
    expected.V[2] = 3;
    expected.V[3] = 4;
    expected.V[4] = 5;
    expected.V[5] = 6;
    expected.V[6] = 7;
    expected.I = 0x250;
    expected.pc +=2;
    
    expected.memory[0x250] = 1;
    expected.memory[0x251] = 2;
    expected.memory[0x252] = 3;
    expected.memory[0x253] = 4;
    expected.memory[0x254] = 5;
    expected.memory[0x255] = 6;
    expected.memory[0x256] = 7;
    
    Test("0xFX55", input, expected);
}

void Test_0xFX65()
{
    Chip8 input = SetupTestC8(0xF665);
    input.I = 0x250;
    
    input.memory[0x250] = 1;
    input.memory[0x251] = 2;
    input.memory[0x252] = 3;
    input.memory[0x253] = 4;
    input.memory[0x254] = 5;
    input.memory[0x255] = 6;
    input.memory[0x256] = 7;
    
    // Setup the expected result
    Chip8 expected = SetupTestC8(0xF665);
    expected.V[0] = 1;
    expected.V[1] = 2;
    expected.V[2] = 3;
    expected.V[3] = 4;
    expected.V[4] = 5;
    expected.V[5] = 6;
    expected.V[6] = 7;
    
    expected.memory[0x250] = 1;
    expected.memory[0x251] = 2;
    expected.memory[0x252] = 3;
    expected.memory[0x253] = 4;
    expected.memory[0x254] = 5;
    expected.memory[0x255] = 6;
    expected.memory[0x256] = 7;
    
    expected.I = 0x250;
    expected.pc +=2;
    
    Test("0xFX65", input, expected);
}

void TestAll()
{
    // Perform some tests based on the opcodes
    Test_0x00E0();
    Test_0x00EE();
    Test_0x1NNN();
    Test_0x2NNN();
    Test_0x3XNN_NotEqual();
    Test_0x3XNN_Equal();
    Test_0x4XNN_NotEqual();
    Test_0x4XNN_Equal();
    Test_0x5XY0_NotEqual();
    Test_0x5XY0_Equal();
    Test_0x6XNN();
    Test_0x7XNN_1(); // No Overflow
    Test_0x7XNN_2(); // Overflow
    Test_0x8XY0();
    Test_0x8XY1();
    Test_0x8XY2();
    Test_0x8XY3();
    Test_0x8XY4_Carry();
    Test_0x8XY4_NoCarry();
    Test_0x8XY5_NoBorrow();
    Test_0x8XY5_Borrow();
    Test_0x8XY6_Odd();
    Test_0x8XY6_Even();
    Test_0x8XY7_Borrow();
    Test_0x8XY7_NoBorrow();
    Test_0x8XYE_MSB0();
    Test_0x8XYE_MSB1();
    Test_0x9XY0_NotEqual();
    Test_0x9XY0_Equal();
    Test_0xANNN();
    Test_0xBNNN();
    // Can't test 0xCXNN
    // TODO DXYN
    Test_0xEX9E_Pressed();
    Test_0xEX9E_NotPressed();
    Test_0xEXA1_Pressed();
    Test_0xEXA1_NotPressed();
    Test_0xFX07();
    // TODO FX0A
    Test_0xFX15();
    Test_0xFX18();
    Test_0xFX1E();
    Test_0xFX29();
    Test_0xFX33();
    Test_0xFX55();
    Test_0xFX65();
    
    //exit(0);
}
