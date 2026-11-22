#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <cglm/cglm.h>

#define WIDTH 800
#define HEIGHT 600
#define X 0
#define Y 1
#define Z 2

uint8_t pointInTriangle(vec2 p, vec2 t0, vec2 t1, vec2 t2) {
    vec2 edge;
    vec2 point;
    // t0 -> t1
    glm_vec2_sub(t1, t0, edge);
    glm_vec2_sub(p, t0, point);

    float temp0 = edge[X];
    edge[X] = -edge[Y];
    edge[Y] = temp0;

    float dot1 = glm_vec2_dot(edge, point);
    // t1 -> t2
    glm_vec2_sub(t2, t1, edge);
    glm_vec2_sub(p, t1, point);

    temp0 = edge[X];
    edge[X] = -edge[Y];
    edge[Y] = temp0;

    float dot2 = glm_vec2_dot(edge, point);

    if (dot1 >= 0 != dot2 >= 0) return 0;

    // t2 -> t0
    glm_vec2_sub(t0, t2, edge);
    glm_vec2_sub(p, t2, point);

    temp0 = edge[X];
    edge[X] = -edge[Y];
    edge[Y] = temp0;

    float dot3 = glm_vec2_dot(edge, point);

    if (dot1 >= 0 != dot3 >= 0) return 0;

    return 1;
}

void drawTri(vec2 p0, vec2 p1, vec2 p2, uint32_t colour, uint32_t* buffer) {
    float f_min_x = fminf(p0[X], fminf(p1[X], p2[X]));
    float f_min_y = fminf(p0[Y], fminf(p1[Y], p2[Y]));
    float f_max_x = fmaxf(p0[X], fmaxf(p1[X], p2[X]));
    float f_max_y = fmaxf(p0[Y], fmaxf(p1[Y], p2[Y]));

    int min_x = (int)floorf(f_min_x);
    int min_y = (int)floorf(f_min_y);
    int max_x = (int)ceilf(f_max_x);
    int max_y = (int)ceilf(f_max_y);

    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= WIDTH) max_x = WIDTH - 1;
    if (max_y >= HEIGHT) max_y = HEIGHT - 1;

    for (int y = min_y; y < max_y; y++) {
        for (int x = min_x; x < max_x; x++) {
            vec2 half = {x + 0.5f, y + 0.5f};
            if (pointInTriangle(half, p0, p1, p2) == 1)
                buffer[y * WIDTH + x] = colour;
        }
    }
}

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

        memset(colourBuffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));
        vec2 p0 = {30, 30};
        vec2 p1 = {3 * WIDTH / 4, HEIGHT / 4};
        vec2 p2 = {WIDTH / 2, HEIGHT / 2};
        drawTri(p0, p1, p2, 0xFFFF00F0, colourBuffer);

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