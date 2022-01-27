#include <SDL2/SDL.h>
#include <stdbool.h>

int main() {

  int memory[4096];
  int display[64][32];
  int registers[12];

  SDL_Renderer *renderer = NULL;
  SDL_Event event;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *win = SDL_CreateWindow("Hello world", SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, 512, 256, 0);

  renderer = SDL_CreateRenderer(win, -1, 0);
  bool quit = false;

  while (!quit) {

    SDL_WaitEvent(&event);

    SDL_RenderSetScale(renderer, 8, 8);

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

    SDL_RenderPresent(renderer);

    // Checks if close button is pressed and exits the program
    switch (event.type) {
    case SDL_QUIT:
      quit = true;
      break;
    }
  }

  SDL_Quit();

  return 0;
}