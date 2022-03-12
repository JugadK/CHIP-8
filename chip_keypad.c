#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_stdinc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>



// Keyboard

// The CHIP-8 keyboard has 16 key with hexdecimal values

// By using SDL_GETKEYSTATE we can get an array of all currently pressed keys

// We can then map the original keyboard to osme new keyboard

void GetKeyPadState(int keyPadValues[]) {
  if (sizeof(*keyPadValues) < 16) {

  } else {

    printf("ENTER KEYPAD");

    // 0 out array

    memset(keyPadValues, 0, sizeof(*keyPadValues));

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Chip 8 Keyboard

    // 1 2 3 C
    // 4 5 6 D
    // 7 8 9 E
    // A 0 B F

    // Map to Modern Keyboard

    // 7 8 9 0
    // U I O P
    // J K L ;
    // M , . /

    if (state[SDLK_7]) {
      keyPadValues[0x1] = 1;
    }
    if (state[SDLK_8]) {
      keyPadValues[0x2] = 1;
    }
    if (state[SDLK_9]) {
      keyPadValues[0x3] = 1;
    }
    if (state[SDLK_0]) {
      keyPadValues[0xC] = 1;
    }
    if (state[SDLK_u]) {
      keyPadValues[0x4] = 1;
    }
    if (state[SDLK_i]) {
      keyPadValues[0x5] = 1;
    }
    if (state[SDLK_o]) {
      keyPadValues[0x6] = 1;
    }
    if (state[SDLK_p]) {
      keyPadValues[0xD] = 1;
    }
    if (state[SDLK_j]) {
      keyPadValues[0x7] = 1;
    }
    if (state[SDLK_k]) {
      keyPadValues[0x8] = 1;
    }
    if (state[SDLK_l]) {
      keyPadValues[0x9] = 1;
    }
    if (state[SDLK_SEMICOLON]) {
      keyPadValues[0xE] = 1;
    }
    if (state[SDLK_m]) {
      keyPadValues[0xA] = 1;
    }
    if (state[SDLK_COMMA]) {
      keyPadValues[0x0] = 1;
    }
    if (state[SDLK_PERIOD]) {
      keyPadValues[0xB] = 1;
    }
    if (state[SDLK_BACKSLASH]) {
      keyPadValues[0xF] = 1;
    }
  }
}
