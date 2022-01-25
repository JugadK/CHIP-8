#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <stdio.h>

int main() {
  SDL_Renderer *renderer = NULL;

  bool quit = false;
  SDL_Event event;

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *win = SDL_CreateWindow("Hello world", SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, 640, 480, 0);

  SDL_RenderPresent(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderDrawLine(renderer, 0, 0, 42, 42);
 
  SDL_RenderDrawPoint(renderer, 2, 2);

  renderer = SDL_CreateRenderer(win, -1, 0);

  while (!quit) {
    SDL_WaitEvent(&event);

    // Render something
    SDL_RenderSetLogicalSize(renderer, 640, 480);

     SDL_RenderSetScale(renderer, 8.0f, 8.0f);

    // Set colour of renderer
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    // Clear the screen to the set colour
    SDL_RenderClear(renderer);

    // Show all the has been done behind the scenes
    SDL_RenderPresent(renderer);

    switch (event.type) {
    case SDL_QUIT:
      quit = true;
      break;
    }
  }

  SDL_Quit();

  return 0;

  return 0;
}