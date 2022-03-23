#include "main.h"
#include "chip_keypad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Use unsigned chars to stop hex overflow when reading binary numbers
unsigned char memory[4096] = {0};
int display[64][32];
unsigned char registers[16];
unsigned char sound_register;
bool play_sound;
unsigned char delay_register;
bool delay_not_zero;
unsigned int program_counter = 0x200;
unsigned int stack[16];
unsigned int *stack_pointer;
unsigned int registerI;
unsigned int draw_counter;

// Stores value of the starting pixel in the draw
int starting_pixel;

// Some instruction set the program counter to a specific addr, overriding the
// normal increment
bool increment_program_counter = true;

int height_increment = 0;

// Every Opcode is a two byte value, a hex value is represented by 4 binary
// numbers or half a byte So that means that an opcode contains 4 hex numbers,
// these can then be decoded to figure out what instructions is being called

// Ex. 6xkk, the 6 is the first nibble and shows that this opcode will take
// value kk and place it in register x
unsigned int current_opcode;

// Each instruction is made of two bytes, to help decode them we store both
unsigned char first_opcode_byte;
unsigned char second_opcode_byte;

// Each nibble is 4 bits, or one number in the binary file in hex
// We can use these to decode what the instruction is
unsigned char first_opcode_nibble;
unsigned char second_opcode_nibble;
unsigned char third_opcode_nibble;
unsigned char fourth_opcode_nibble;

int main(int argc, char **argv) {

  int keyPadValues[16];

  const Uint8 *state = SDL_GetKeyboardState(NULL);

  stack_pointer = &stack[0];

  SDL_Renderer *renderer = NULL;
  SDL_Event event;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *win = SDL_CreateWindow("Hello world", SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, 512, 256, 0);

  renderer = SDL_CreateRenderer(win, -1, 0);
  bool quit = false;

  FILE *fp;

  fp = fopen("hex_test.bin", "rb"); // read mode

  if (fp == NULL) {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  // Load hex sprites into memory, these are dedicated sprites any CHIP-8
  // program can use

  // 0 sprite

  memory[0x0] = 0xF0;
  memory[0x1] = 0x90;
  memory[0x2] = 0x90;
  memory[0x3] = 0x90;
  memory[0x4] = 0xF0;

  // 1 Sprite

  memory[0x5] = 0x20;
  memory[0x6] = 0x60;
  memory[0x7] = 0x20;
  memory[0x8] = 0x20;
  memory[0x9] = 0x70;

  // 2 Sprite

  memory[0xA] = 0xF0;
  memory[0xB] = 0x10;
  memory[0xC] = 0xF0;
  memory[0xD] = 0x80;
  memory[0xE] = 0xF0;

  // 3 Sprite

  memory[0xF] = 0xF0;
  memory[0x10] = 0x10;
  memory[0x11] = 0xF0;
  memory[0x12] = 0x10;
  memory[0x13] = 0xF0;

  // 4 Sprite

  memory[0x14] = 0x90;
  memory[0x15] = 0x90;
  memory[0x16] = 0xF0;
  memory[0x17] = 0x10;
  memory[0x18] = 0x10;

  // 5 Sprite

  memory[0x19] = 0xF0;
  memory[0x1A] = 0x80;
  memory[0x1B] = 0xF0;
  memory[0x1C] = 0x10;
  memory[0x1D] = 0xF0;

  // 6 Sprite

  memory[0x1E] = 0xF0;
  memory[0x20] = 0x80;
  memory[0x21] = 0xF0;
  memory[0x22] = 0x90;
  memory[0x23] = 0xF0;

  // 7 Sprite

  memory[0x24] = 0xF0;
  memory[0x25] = 0x10;
  memory[0x26] = 0x20;
  memory[0x27] = 0x40;
  memory[0x28] = 0x40;

  // 8 Sprite

  memory[0x29] = 0xF0;
  memory[0x2A] = 0x90;
  memory[0x2B] = 0xF0;
  memory[0x2C] = 0x90;
  memory[0x2D] = 0xF0;

  // 9 Sprite

  memory[0x2E] = 0xF0;
  memory[0x2F] = 0x90;
  memory[0x30] = 0xF0;
  memory[0x31] = 0x10;
  memory[0x32] = 0xF0;

  // A Sprite

  memory[0x33] = 0xF0;
  memory[0x34] = 0x90;
  memory[0x35] = 0xF0;
  memory[0x36] = 0x90;
  memory[0x37] = 0x90;

  // B Sprite

  memory[0x38] = 0xE0;
  memory[0x39] = 0x90;
  memory[0x3A] = 0xE0;
  memory[0x3B] = 0x90;
  memory[0x3C] = 0xE0;

  // C Sprite

  memory[0x3D] = 0xF0;
  memory[0x3E] = 0x80;
  memory[0x3F] = 0x80;
  memory[0x40] = 0x80;
  memory[0x41] = 0xF0;

  // D Sprite

  memory[0x42] = 0xE0;
  memory[0x43] = 0x90;
  memory[0x44] = 0x90;
  memory[0x45] = 0x90;
  memory[0x46] = 0xE0;

  // E Sprite

  memory[0x47] = 0xF0;
  memory[0x48] = 0x80;
  memory[0x49] = 0xF0;
  memory[0x4A] = 0x80;
  memory[0x4B] = 0xF0;

  // F Sprite

  memory[0x4C] = 0xF0;
  memory[0x4D] = 0x80;
  memory[0x4E] = 0xF0;
  memory[0x4F] = 0x80;
  memory[0x50] = 0x80;

  // Rom is loaded into memory starting from 0x200, or 512
  // The original CHIP-8 had its intrpretor in the first 512 bytes
  // So to emulate it we leave it empty and start at 0x200

  fread(&memory[0x200], sizeof(memory), 1, fp);

  SDL_WaitEvent(&event);

  SDL_RenderSetScale(renderer, 8, 8);

  while (!quit) {

    print_debug_info();

    // if (program_counter > 0x208) {
    //  break;
    //}
    // Reset flags
    increment_program_counter = true;

    first_opcode_byte = memory[program_counter];
    second_opcode_byte = memory[program_counter + 1];

    current_opcode = (first_opcode_byte * 0x100) + second_opcode_byte;

    // Decode opcode

    // nibble = 4 bits/half byte/one hex value

    first_opcode_nibble = current_opcode / 0x1000;
    second_opcode_nibble = (current_opcode / 0x100) % 0x10;
    third_opcode_nibble = (current_opcode / 0x10) % 0x10;
    fourth_opcode_nibble = current_opcode % 0x10;

    // To make reduce the amount of if statements, I decided to use a switch
    // code for the first nibble, since with that we can correclty figure out
    // the correct instruction for most of the instructions, after that we then
    // branch into if statements if neccesary

    //    printf("%x\n", current_opcode);
    //
    //    printf("%x\n", first_opcode_byte);
    //    printf("%x\n", second_opcode_byte);

    printf("%u \n", program_counter);

    if (current_opcode == 0xFFFF) {
      break;
    }

    switch (first_opcode_nibble) {

    case 0x0:
      if (current_opcode == 0x00e0) {
        memset(display, 0, sizeof(display[0][0]) * 64 * 32);
      }
      break;
    case 0x1:
      program_counter = second_opcode_nibble * 0x100 + second_opcode_byte;
      increment_program_counter = false;
      printf("%u gsdfgsdfg", program_counter);

      break;
    case 0x2:
      stack_pointer++;
      break;
    case 0x3:
      if (registers[second_opcode_nibble] == second_opcode_byte) {
        program_counter = program_counter + 2;
      }
      break;
    case 0x4:
      if (registers[second_opcode_nibble] != second_opcode_byte) {
        program_counter = program_counter + 2;
      }
      break;
    case 0x5:
      if (registers[second_opcode_nibble] == registers[third_opcode_nibble]) {
        program_counter = program_counter + 2;
      }
      break;
    case 0x6:
      registers[second_opcode_nibble] = second_opcode_byte;
      break;
    case 0x7:
      registers[second_opcode_nibble] =
          registers[second_opcode_nibble] + second_opcode_byte;
      break;
    case 0x8:

      switch (fourth_opcode_nibble) {

      case 0x0:
        registers[third_opcode_nibble] = registers[second_opcode_nibble];
        break;
      case 0x1:
        registers[second_opcode_nibble] =
            registers[second_opcode_nibble] | registers[third_opcode_nibble];
        break;
      case 0x2:
        registers[second_opcode_nibble] =
            registers[second_opcode_nibble] & registers[third_opcode_nibble];
        break;
      case 0x3:
        registers[second_opcode_nibble] =
            registers[second_opcode_nibble] ^ registers[third_opcode_nibble];
        break;
      case 0x4:
        registers[second_opcode_nibble] =
            registers[second_opcode_nibble] + registers[third_opcode_nibble];
        if (registers[second_opcode_nibble] > 255) {
          registers[0xF] = 1;
        }
        break;
      case 0x5:
        if (registers[second_opcode_nibble] > registers[third_opcode_nibble]) {
          registers[0xF] = 1;
        } else {
          registers[0xF] = 0;
        }
        registers[second_opcode_nibble] =
            registers[second_opcode_nibble] - registers[third_opcode_nibble];
        break;
      case 0x6:
        if (registers[second_opcode_nibble] % 0xF == 1) {
          registers[0xF] = 1;
        } else {
          registers[0xF] = 0;
        }
        registers[second_opcode_nibble] = registers[second_opcode_nibble] / 2;
        break;
      case 0x7:
        if (registers[third_opcode_nibble] > registers[second_opcode_nibble]) {
          registers[0xF] = 1;
        } else {
          registers[0xF] = 0;
        }
        registers[second_opcode_nibble] =
            registers[third_opcode_nibble] - registers[second_opcode_nibble];
        break;
      case 0xE:
        if (registers[second_opcode_nibble] % 0xF == 1) {
          registers[0xF] = 1;
        } else {
          registers[0xF] = 0;
        }
        registers[second_opcode_nibble] = registers[second_opcode_nibble] * 2;
        break;
      }

      break;

    case 0x9:
      if (registers[second_opcode_nibble] != registers[third_opcode_nibble]) {
        program_counter = program_counter + 2;
      }
      break;
    case 0xA:
      registerI = current_opcode % 0x1000;
      break;
    case 0xB:
      program_counter = program_counter + (current_opcode % 0xFFFF);
      break;
    case 0xC:
      registers[third_opcode_nibble] =
          rand() % (255 + 1) & registers[second_opcode_byte];
      break;
    case 0xD:

      draw_counter = 0;

      for (int i = registerI; i < registerI + fourth_opcode_nibble; i++) {

        printf("\ni %u register I %x  nibble %x memory %x %x\n", i, registerI,
               fourth_opcode_nibble, memory[i], draw_counter);

        // Sprite Building Blocks
        // Each Sprite in CHIP-8 is 5x8 and each line is represented by a
        // certain hex value which is then turned into some combination of
        // on and off pixels

        // 0xF0 ****
        // 0xE0 ***
        // 0x90 *  *
        // 0x10    *
        // 0x80 *
        // 0x20   *
        // 0x40  *
        // 0x70  ***

        switch (memory[i]) {
        case 0xF0:

          display[third_opcode_nibble][second_opcode_nibble + draw_counter] =
              display[third_opcode_nibble]
                     [second_opcode_nibble + draw_counter] ^
              1;
          display[third_opcode_nibble + 1]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 1]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          display[third_opcode_nibble + +2]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 2]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          display[third_opcode_nibble + 3]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 3]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          break;

        case 0xE0:
          display[third_opcode_nibble][second_opcode_nibble + draw_counter] =
              display[third_opcode_nibble]
                     [second_opcode_nibble + draw_counter] ^
              1;
          display[third_opcode_nibble + 1]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 1]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          display[third_opcode_nibble + 2]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 2]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          break;
        case 0x90:
          display[third_opcode_nibble][second_opcode_nibble + draw_counter] =
              display[third_opcode_nibble]
                     [second_opcode_nibble + draw_counter] ^
              1;
          display[third_opcode_nibble + 3]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 3]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          break;
        case 0x10:
          display[third_opcode_nibble + 3]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 3]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          break;
        case 0x20:
          display[third_opcode_nibble + 2]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 2]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          break;
        case 0x40:
          display[third_opcode_nibble + 1]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 1]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          break;

        case 0x80:
          display[third_opcode_nibble][second_opcode_nibble + draw_counter] =
              display[third_opcode_nibble]
                     [second_opcode_nibble + draw_counter] ^
              1;
          break;
        case 0x70:
          display[third_opcode_nibble + 1]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 1]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          display[third_opcode_nibble + 2]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 2]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          display[third_opcode_nibble + 3]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 3]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          break;
        case 0x60:
          display[third_opcode_nibble + 1]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 1]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          display[third_opcode_nibble + 2]
                 [second_opcode_nibble + draw_counter] =
                     display[third_opcode_nibble + 2]
                            [second_opcode_nibble + draw_counter] ^
                     1;
          break;
        }

        draw_counter++;
      }

      break;

    case 0xE:

      switch (second_opcode_byte) {
      case 0x9E:

        GetKeyPadState(*keyPadValues);

        if (keyPadValues[second_opcode_nibble] == 1) {

          program_counter = program_counter + 2;
        }
        break;

      case 0xA1:

        GetKeyPadState(*keyPadValues);

        if (keyPadValues[second_opcode_nibble] == 0) {

          program_counter = program_counter + 2;
        }
      }
      break;

    case 0xf:

      switch (second_opcode_byte) {

      case 0x0A:
        registers[second_opcode_byte] = 0x1;

      case 0x07:
        registers[second_opcode_nibble] = delay_register;
        break;
      case 0x15:
        delay_register = registers[second_opcode_nibble];
        break;
      case 0x18:
        sound_register = registers[second_opcode_nibble];

      case 0x1E:
        registerI = registerI + registers[second_opcode_nibble];

      case 0x29:
        switch (second_opcode_nibble) {
        case 0x0:
          registerI = 0x0;
          break;
        case 0x1:
          registerI = 0x5;
          break;
        case 0x2:
          registerI = 0xA;
          break;
        case 0x3:
          registerI = 0xF;
          break;
        case 0x4:
          registerI = 0x14;
          break;
        case 0x5:
          registerI = 0x19;
          break;
        case 0x6:
          registerI = 0x1E;
          break;
        case 0x7:
          registerI = 0x24;
          break;
        case 0x8:
          registerI = 0x29;
          break;
        case 0x9:
          registerI = 0x2E;
          break;
        case 0xA:
          registerI = 0x33;
          break;
        case 0xB:
          registerI = 0x38;
          break;
        case 0xC:
          registerI = 0x3D;
          break;
        case 0xD:
          registerI = 0x42;
          break;
        case 0xE:
          registerI = 0x47;
          break;
        case 0xF:
          registerI = 0x4C;
          break;
        }
      }
    }

    for (int i = 0; i < 64; i++) {
      for (int j = 0; j < 32; j++) {

        if (display[i][j] == 0) {

          SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

          SDL_RenderDrawPoint(renderer, i, j);

        } else {

          printf("%u", display[i][j]);

          SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

          SDL_RenderDrawPoint(renderer, i, j);
        }
      }
    }

    // SDL_RenderPresent(renderer);

    // Checks if close button is pressed and exits the program
    switch (event.type) {
    case SDL_QUIT:
      print_debug_info();
    // quit = true;
    // break;
    }

    if (increment_program_counter) {
      program_counter = program_counter + 0x2;
    }

    sleep(1);
  }

  fclose(fp);
  //SDL_Quit();

  return 0;
}

void print_debug_info() {

  printf("\n");
  printf("current opcode %x\n", current_opcode);
  printf("opcode first byte %x\n", first_opcode_byte);
  printf("opcode second byte %x\n", second_opcode_byte);
  printf("opcode first nibble %x\n", first_opcode_nibble);
  printf("opcode second nibble %x\n", second_opcode_nibble);
  printf("opcode third nibble %x\n", third_opcode_nibble);
  printf("opcode fourth nibble %x\n", fourth_opcode_nibble);

  for (int current_register = 0; current_register < 16; current_register++) {

    printf("register%x: %x\n", current_register, registers[current_register]);
  }

  for (int col = 0; col < 32; col++) {
    for (int row = 0; row < 64; row++) {

      printf("%i", display[row][col]);
    }

    printf("\n");
  }
}