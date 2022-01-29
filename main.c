#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

int main() {

  unsigned char memory[4096];
  int display[64][32];
  int registers[12];

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


  while (!quit) {

    SDL_WaitEvent(&event);

    SDL_RenderSetScale(renderer, 8, 8);

    if(current_address > 520) {
      quit = true;
      break;
    }

    printf("%x%x", memory[current_address], memory[current_address+1]);


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
      quit = true;
      break;
    }

    current_address++;
  }

  fclose(fp);
  SDL_Quit();

  return 0;
}