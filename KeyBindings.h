#ifndef _KEYDEFINITIONS_H
#define _KEYDEFINITIONS_H

// Ensure our XMacro is unbound to begin with
#undef KEYBINDING

////////////
// Keyboard

// Definition of keys
enum KEYS
{
#define KEYBINDING(key, desc) key,
#include "keys.def"
#undef KEYBINDING
    // Always last
    NUM_KEYS,
};

// Key Name
static char const * KEY_NAME[] = {
#define KEYBINDING(key, desc) #key,
#include "keys.def"
#undef KEYBINDING
};

// Key Descriptions
static char const * KEY_DESC[] = {
#define KEYBINDING(key, desc) #desc,
#include "keys.def"
#undef KEYBINDING
};

// End Keyboard
////////////////

#endif
