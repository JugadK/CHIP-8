#include "main.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

unsigned char memory[4096];
int display[64][32];
unsigned char registers[16];
unsigned char sound_register;
bool play_sound;
unsigned char delay_register;
bool delay_not_zero;
int program_counter;
int stack[16];
int stack_pointer;

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

  // Use unsigned chars to stop hex overflow when reading binary numbers
  FILE *fp;

  fp = fopen("hex_test.bin", "rb"); // read mode

  if (fp == NULL) {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  // Rom is loaded into memory starting from 0x200, or 512
  // The original CHIP-8 had its intrpretor in the first 512 bytes
  // So to emulate it we leave it empty and start at 0x200
  fread(&memory[512], sizeof(memory), 1, fp);

  int current_address = 512;
  bool read_rom = false;

  while (!quit) {

    SDL_WaitEvent(&event);

    SDL_RenderSetScale(renderer, 8, 8);

    if (current_address > 514 && !read_rom) {

      read_rom = false;

    } else {

      first_opcode_byte = memory[current_address];
      second_opcode_byte = memory[current_address + 1];

      current_opcode =
          (memory[current_address] * 0x100) + memory[current_address + 1];

      // Decode opcode

      first_opcode_nibble = current_opcode / 0x1000;
      second_opcode_nibble = (current_opcode / 0x100) % 0x10;
      third_opcode_nibble = (current_opcode / 0x10) % 0x10;
      fourth_opcode_nibble = current_opcode % 0x10;

      if (first_opcode_nibble == 0x6) {

        registers[second_opcode_nibble] = second_opcode_byte;
      }

      print_debug_info();
      quit = true;
      break;

      current_address = current_address + 2;
    }

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
}
