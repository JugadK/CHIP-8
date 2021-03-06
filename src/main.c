#include "main.h"
#include "chip_keypad.h"
#include "stack.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

// Use unsigned chars to stop hex overflow when reading binary numbers
unsigned char memory[4096] = {0};
int display[64][32];
unsigned char registers[16];
unsigned char sound_register;
bool play_sound;
unsigned char delay_register;
bool delay_not_zero;
unsigned int program_counter = 0x200;
struct stack chip8_stack;
unsigned int registerI;
unsigned int draw_counter;
unsigned int render_counter;

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

unsigned int x_position_draw;
unsigned int y_position_draw;

int main(int argc, char **argv) {

  int keyPadValues[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  const Uint8 *state = SDL_GetKeyboardState(NULL);

  chip8_stack = createStack(16);

  SDL_Renderer *renderer = NULL;
  SDL_Event event;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *win = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, 512, 256, 0);

  renderer = SDL_CreateRenderer(win, -1, 0);
  bool quit = false;

  FILE *fp;

  // Load ROM file

  fp = fopen("flightrunner.ch8", "rb"); // read mode

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

  // CHIP-8 Display was 64 x 32, this is really small in modern displays so I increase it all by 8 to make it
  // look more readable

  SDL_RenderSetLogicalSize(renderer, 64, 32);

  SDL_RenderSetScale(renderer, 8, 8);

  // Instead of drawing every opcode we can instead draw every 60th instruction
  int draw_counter = 0;

  render_counter = 0;

  bool collision_flag = false;

  // Rom is loaded into memory starting from 0x200, or 512
  // The original CHIP-8 had its intrpretor in the first 512 bytes
  // So to emulate it we leave it empty and start at 0x200

  fread(&memory[0x200], sizeof(memory), 1, fp);

  while (!quit) {

    // Clock rate 500 HZ
    SDL_Delay(2);

    GetKeyPadState(keyPadValues);

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        print_debug_info();
        quit = true;
        break;
      }
    }


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // Show all the has been done behind the scenes
    SDL_RenderPresent(renderer);

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
    // code for the first nibble, since with that we can correctly figure out
    // the correct instruction for most of the instructions, after that we
    // then branch into core switch statements depending on waht the opcode id

    switch (first_opcode_nibble) {

    case 0x0:

      switch (second_opcode_byte) {

      case 0xE0:
        memset(display, 0, sizeof(display[0][0]) * 64 * 32);
        break;
        
      case 0xEE:
        program_counter = pop(&chip8_stack);
        break;
      }
      break;
    case 0x1:
      program_counter = second_opcode_nibble * 0x100 + second_opcode_byte;
      increment_program_counter = false;

      break;
    case 0x2:

      push(&chip8_stack, program_counter);

      program_counter = current_opcode % 0x1000;

      increment_program_counter = false;

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
        registers[second_opcode_nibble] = registers[third_opcode_nibble];
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
      registers[second_opcode_nibble] =
          (rand() % (255 + 1)) & second_opcode_byte;
      break;
    case 0xD:

      draw_counter = 0;
      // If any of the pixels are drawn over
      collision_flag = false;

      for (unsigned int i = registerI; i < registerI + fourth_opcode_nibble;
           i++) {

        // Sprite Building Blocks
        // Each Sprite in CHIP-8 is 5x8 and each line is represented by a
        // certain hex value which is then turned into some combination of
        // on and off pixels

        // 0xF0 ****
        // 0xE0 ***
        // 0xD0 ** *
        // 0xC0 **
        // 0xB0 * **
        // 0xA0 * *
        // 0x90 *  *
        // 0x80 *
        // 0x70  ***
        // 0x60  **
        // 0x50  * *
        // 0x40  *
        // 0x30   **
        // 0x20   *
        // 0x10     *
        // 0x00

        x_position_draw = registers[second_opcode_nibble];
        y_position_draw = registers[third_opcode_nibble];

        int previous_display[4] = {0, 0, 0, 0};

        previous_display[0] =
            display[x_position_draw][y_position_draw + draw_counter];
        previous_display[1] =
            display[x_position_draw + 1][y_position_draw + draw_counter];
        previous_display[2] =
            display[x_position_draw + 2][y_position_draw + draw_counter];
        previous_display[3] =
            display[x_position_draw + 3][y_position_draw + draw_counter];

        display[x_position_draw][y_position_draw + draw_counter] =
            display[x_position_draw][y_position_draw + draw_counter] ^
            (((memory[i] & 0b10000000) == 0)
                 ? 0
                 : 1); // Bit mask to check if the bit with 1 is 0
        display[x_position_draw + 1][y_position_draw + draw_counter] =
            display[x_position_draw + 1][y_position_draw + draw_counter] ^
            (((memory[i] & 0b01000000) == 0) ? 0 : 1);
        display[x_position_draw + +2][y_position_draw + draw_counter] =
            display[x_position_draw + 2][y_position_draw + draw_counter] ^
            (((memory[i] & 0b00100000) == 0) ? 0 : 1);
        display[x_position_draw + 3][y_position_draw + draw_counter] =
            display[x_position_draw + 3][y_position_draw + draw_counter] ^
            (((memory[i] & 0b00010000) == 0) ? 0 : 1);

        if (collision_flag == false) {
          for (int j = 0; j < 4; j++) {
            if (previous_display[j] == 1 &&
                display[x_position_draw + j][y_position_draw + draw_counter] ==
                    0) {

              collision_flag = true;
              break;
            }
          }
        }

        draw_counter++;
      }

      if (collision_flag) {
        registers[0xF] = 1;
      } else {
        registers[0xF] = 0;
      }

      break;

    case 0xE:

      switch (second_opcode_byte) {
      case 0x9E:

        if (keyPadValues[registers[second_opcode_nibble]] == 1) {
          program_counter = program_counter + 2;
        }
        break;

      case 0xA1:

        if (keyPadValues[registers[second_opcode_nibble]] == 0) {
          program_counter = program_counter + 2;
        }
      };
      break;

    case 0xF:

      switch (second_opcode_byte) {

      case 0x0A:

        while (true) {
          int keyPressed = WaitForKeyPress();

          if (keyPressed != 0xFF) {

            registers[second_opcode_nibble] = keyPressed;

            break;
          }
        }
        break;

      case 0x07:
        registers[second_opcode_nibble] = delay_register;
        break;
      case 0x15:
        delay_register = registers[second_opcode_nibble];
        break;
      case 0x18:
        sound_register = registers[second_opcode_nibble];
        break;

      case 0x1E:
        registerI = registerI + registers[second_opcode_nibble];
        break;

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
        break;
      case 0x33:
        memory[registerI] = (registers[second_opcode_nibble] / 100) % 10;
        memory[registerI + 1] = (registers[second_opcode_nibble] / 10) % 10;
        memory[registerI + 2] = (registers[second_opcode_nibble] % 10);

        break;
      case 0x55:

        for (int i = 0; i < second_opcode_nibble + 1; i++) {

          memory[registerI + i] = registers[i];
        }

        break;

      case 0x65:

        for (int i = 0; i < second_opcode_nibble + 1; i++) {

          registers[i] = memory[registerI + i];
        }

        break;
      }
    }

    render_counter++;

    // Every 8th clock cycle we render the display which gives us a 60hz refreah rate

    if (render_counter > 8) {
      for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 32; j++) {

          if (display[i][j] == 0) {

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

            SDL_RenderDrawPoint(renderer, i, j);

          } else {

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            SDL_RenderDrawPoint(renderer, i, j);
          }
        }
      }

      if (delay_register > 0) {

        delay_register--;
      }

      if (sound_register > 0) {
        sound_register--;
      }

      render_counter = 0;
    }
    // SDL_RenderPresent(renderer);

    // Checks if close button is pressed and exits the program

    if (increment_program_counter) {
      program_counter = program_counter + 0x2;
    }
  }

  free(chip8_stack.stack_pointer);
  fclose(fp);
  SDL_Quit();

  return 0;
}

void print_debug_info() {

  printf("current opcode %x\n", current_opcode);
  printf("opcode first byte %x\n", first_opcode_byte);
  printf("opcode second byte %x\n", second_opcode_byte);
  printf("opcode first nibble %x\n", first_opcode_nibble);
  printf("opcode second nibble %x\n", second_opcode_nibble);
  printf("opcode third nibble %x\n", third_opcode_nibble);
  printf("opcode fourth nibble %x\n", fourth_opcode_nibble);
  printf("\nregister I %x \n", registerI);
  printf("stack pointer %x \n", *chip8_stack.stack_pointer);

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