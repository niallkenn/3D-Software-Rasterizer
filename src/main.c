#include <stdbool.h>
#include <cglm/cglm.h>
#include <SDL2/SDL.h>
#include "engine.h"

int main(void) {
    if (!init()) {
        printf("failed to initialise.\n");
        return 1;
    }

    bool running = true;
    
    while (running) {
        SDL_Event e;

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        loop();
    }

    kill();
}