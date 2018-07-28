#include "Chip8Input.h"
#include "Chip8.h"

void C8SetupInput(Chip8* chip8)
{
    C8SetKeyMap(chip8, KEY_0, GLFW_KEY_0);
    C8SetKeyMap(chip8, KEY_1, GLFW_KEY_1);
    C8SetKeyMap(chip8, KEY_2, GLFW_KEY_2);
    C8SetKeyMap(chip8, KEY_3, GLFW_KEY_3);
    C8SetKeyMap(chip8, KEY_4, GLFW_KEY_4);
    C8SetKeyMap(chip8, KEY_5, GLFW_KEY_5);
    C8SetKeyMap(chip8, KEY_6, GLFW_KEY_6);
    C8SetKeyMap(chip8, KEY_7, GLFW_KEY_7);
    C8SetKeyMap(chip8, KEY_8, GLFW_KEY_8);
    C8SetKeyMap(chip8, KEY_9, GLFW_KEY_9);
    C8SetKeyMap(chip8, KEY_A, GLFW_KEY_A);
    C8SetKeyMap(chip8, KEY_B, GLFW_KEY_B);
    C8SetKeyMap(chip8, KEY_C, GLFW_KEY_C);
    C8SetKeyMap(chip8, KEY_D, GLFW_KEY_D);
    C8SetKeyMap(chip8, KEY_E, GLFW_KEY_E);
    C8SetKeyMap(chip8, KEY_F, GLFW_KEY_F);
}

void C8GetInput(Chip8* chip8)
{
    // Loop over the key array and get the current input state
    for(int key=0; key<MAX_KEYS; ++key)
    {
        if (glfwGetKey(chip8->screen->window, chip8->keymap[key]) == GLFW_PRESS)
        {
            // Set the key to 1
            chip8->keys[key] = 1;
        }
        else
        {
            chip8->keys[key] = 0;
        }
    }
}

void C8SetKeyMap(Chip8* chip8, Chip8Keypad key, uint32_t key_code)
{
    chip8->keymap[key] = key_code;
}

uint32_t C8GetKeyMap(Chip8* chip8, Chip8Keypad key)
{
    return chip8->keymap[key];
}