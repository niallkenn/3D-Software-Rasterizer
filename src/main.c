#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600

int main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init error: %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Software 3D Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    if (!window | !renderer | !texture) {
        printf("Error creating window or renderer: %s", SDL_GetError());
        return 1;
    }

    bool running = true;

    uint32_t* colourBuffer = malloc(WIDTH * HEIGHT * sizeof(uint32_t));

    for (size_t i = 0; i < WIDTH * HEIGHT; i++) {
        colourBuffer[i] = 0xFFFF0000;
    }

    uint32_t colour = 0;
    while (running) {
        SDL_Event e;

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        colour += 100;
        for (size_t i = 0; i < WIDTH * HEIGHT; i++) {
            colourBuffer[i] = colour | 0xFFF00000;
        }
        SDL_UpdateTexture(texture, NULL, colourBuffer, WIDTH * sizeof(uint32_t));

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    free(colourBuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}