#include "main.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <stdbool.h>
#include <stdio.h>

// Use unsigned chars to stop hex overflow when reading binary numbers
unsigned char memory[4096] = {0};
int display[64][32] = {0};
unsigned char registers[16];
unsigned char sound_register;
bool play_sound;
unsigned char delay_register;
bool delay_not_zero;
unsigned int program_counter = 0x200;
unsigned int stack[16];
int stack_pointer;

// Some instruction set the program counter to a specific addr, overriding the
// normal increment
bool increment_program_counter = true;

// Every Opcode is a two byte value, a hex value is represented by 4 binary
// numbers or half a byte So that means that an opcode contains 4 hex numbers,
// these can then be decoded to figure out what opcode is being called

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

  // Rom is loaded into memory starting from 0x200, or 512
  // The original CHIP-8 had its intrpretor in the first 512 bytes
  // So to emulate it we leave it empty and start at 0x200

  fread(&memory[0x200], sizeof(memory), 1, fp);

  while (!quit) {

    if (program_counter > 0x208) {
      break;
    }
    // Reset flags
    increment_program_counter = true;

    SDL_WaitEvent(&event);

    SDL_RenderSetScale(renderer, 8, 8);

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

    switch (first_opcode_nibble) {
    case 0x0:
      if (current_opcode == 0x00e0) {
        memset(display, 0, sizeof(display[0][0]) * 64 * 32);
      }
      break;
    case 0x1:
      program_counter = second_opcode_nibble * 0x100 + second_opcode_byte;
      increment_program_counter = false;
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
        }
        registers[second_opcode_nibble] =
            registers[second_opcode_nibble] - registers[third_opcode_nibble];
        break;
      }

    case 0xf:
      // Useful for debugging purposes, not an actual instruction
      if (current_opcode == 0xffff) {
        quit = true;
        break;
      }
    }

    print_debug_info();

    for (int i = 0; i < 64; i++) {
      for (int j = 0; j < 32; j++) {

        if (j % 2 == 0) {

          SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

          SDL_RenderDrawPoint(renderer, i, j);

        } else {

          SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

          SDL_RenderDrawPoint(renderer, i, j);
        }
      }
    }

    SDL_RenderPresent(renderer);

    // Checks if close button is pressed and exits the program
    switch (event.type) {
    case SDL_QUIT:
      print_debug_info();
      quit = true;
      break;
    }

    if (increment_program_counter) {
      program_counter = program_counter + 0x2;
    }
  }

  fclose(fp);
  //  SDL_Quit();

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

  for (int row = 0; row < 32; row++) {
    for (int col = 0; col < 64; col++) {

      printf("%i", display[row][col]);
    }

    printf("\n");
  }
}