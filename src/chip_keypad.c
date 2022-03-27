#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Keyboard

// The CHIP-8 keyboard has 16 key with hexdecimal values

// By using SDL_GETKEYSTATE we can get an array of all currently pressed keys

// We can then map the original keyboard to osme new keyboard

void GetKeyPadState(int *keyPadValues) {

  //uprintf("ENTER KEYPAD\n\n\n\n\n\n\n\n\n\n");

  // 0 out array array size is fixed to 16

  memset(keyPadValues, 0, 64);


  const Uint8 *state = SDL_GetKeyboardState(NULL);

  SDL_PumpEvents();

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

  if (state[SDL_SCANCODE_7]) {
    keyPadValues[0x1] = 1;
  }
  if (state[SDL_SCANCODE_8]) {
    keyPadValues[0x2] = 1;
  }
  if (state[SDL_SCANCODE_9]) {
    keyPadValues[0x3] = 1;
  }
  if (state[SDL_SCANCODE_0]) {
    keyPadValues[0xC] = 1;
  }
  if (state[SDL_SCANCODE_U]) {
    keyPadValues[0x4] = 1;
  }
  if (state[SDL_SCANCODE_I]) {
    keyPadValues[0x5] = 1;
  }
  if (state[SDL_SCANCODE_O]) {
    keyPadValues[0x6] = 1;
  }
  if (state[SDL_SCANCODE_P]) {
    keyPadValues[0xD] = 1;
  }
  if (state[SDL_SCANCODE_J]) {
    keyPadValues[0x7] = 1;
  }
  if (state[SDL_SCANCODE_K]) {
    keyPadValues[0x8] = 1;
  }
  if (state[SDL_SCANCODE_L]) {
    keyPadValues[0x9] = 1;
  }
  if (state[SDL_SCANCODE_SEMICOLON]) {
    keyPadValues[0xE] = 1;
  }
  if (state[SDL_SCANCODE_M]) {
    keyPadValues[0xA] = 1;
  }
  if (state[SDL_SCANCODE_COMMA]) {
    keyPadValues[0x0] = 1;
  }
  if (state[SDL_SCANCODE_PERIOD]) {
    keyPadValues[0xB] = 1;
  }
  if (state[SDL_SCANCODE_BACKSLASH]) {
    keyPadValues[0xF] = 1;
  }

  //printf("%x", state[SDL_SCANCODE_8]);
  //printf("%x", state[SDL_SCANCODE_9]);
  //printf("%x", state[SDL_SCANCODE_0]);
  //printf("%x", state[SDL_SCANCODE_U]);
  //printf("%x", state[SDL_SCANCODE_I]);
  //printf("%x", state[SDL_SCANCODE_O]);
  //printf("%x", state[SDL_SCANCODE_P]);
  //printf("%x", state[SDL_SCANCODE_J]);
  //printf("%x", state[SDL_SCANCODE_K]);
  //printf("%x", state[SDL_SCANCODE_L]);
  //printf("%x", state[SDL_SCANCODE_SEMICOLON]);
  //printf("%x", state[SDL_SCANCODE_M]);
  //printf("%x", state[SDL_SCANCODE_COMMA]);
  //printf("%x", state[SDL_SCANCODE_PERIOD]);
  //printf("%x", state[SDL_SCANCODE_BACKSLASH]);
}

int WaitForKeyPress() {

  const Uint8 *state = SDL_GetKeyboardState(NULL);

  SDL_PumpEvents();

  if (state[SDL_SCANCODE_7]) {
    return 0x1;
  }
  if (state[SDL_SCANCODE_8]) {
    return 0x2;
  }
  if (state[SDL_SCANCODE_9]) {
    return 0x3;
  }
  if (state[SDL_SCANCODE_0]) {
    return 0xC;
  }
  if (state[SDL_SCANCODE_U]) {
    return 0x4;
  }
  if (state[SDL_SCANCODE_I]) {
    return 0x5;
  }
  if (state[SDL_SCANCODE_O]) {
    return 0x6;
  }
  if (state[SDL_SCANCODE_P]) {
    return 0xD;
  }
  if (state[SDL_SCANCODE_J]) {
    return 0x7;
  }
  if (state[SDL_SCANCODE_K]) {
    return 0x8;
  }
  if (state[SDL_SCANCODE_L]) {
    return 0x9;
  }
  if (state[SDL_SCANCODE_SEMICOLON]) {
    return 0xE;
  }
  if (state[SDL_SCANCODE_M]) {
    return 0xA;
  }
  if (state[SDL_SCANCODE_COMMA]) {
    return 0x0;
  }
  if (state[SDL_SCANCODE_PERIOD]) {
    return 0xB;
  }
  if (state[SDL_SCANCODE_BACKSLASH]) {
    return 0xF;
  }

  return 0xFF;
}
