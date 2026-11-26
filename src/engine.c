#include <SDL2/SDL.h>
#include <time.h>
#include "engine.h"
#include "definitions.h"
#include "rasterizer.h"
#include "camMath.h"
#include "modelMath.h"
#include "obj.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;
static uint32_t* colourBuffer = NULL;

static Uint64 lastTime = 0;
static Uint64 perfFreq = 0;

static vec3 camLocation = {1.5f , 1.0f, 5.0f};
static vec3 target = {0, 0, 0};
static vec3 worldUp = {0, 1, 0};

static vec3 scale = {1, 1, 1};
static vec3 rot = {0, 0, 0};
static vec3 position = {0, 0, 0};

static Mesh triangle;

static vec3 points[] = {
    {-0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},
    {-0.5f,  0.5f, -0.5f},
    {-0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f}
};

static int indices[][3] = {
    {4, 5, 6}, {4, 6, 7},
    {0, 2, 1}, {0, 3, 2},
    {0, 1, 5}, {0, 5, 4},
    {3, 6, 2}, {3, 7, 6},
    {1, 2, 6}, {1, 6, 5},
    {0, 4, 7}, {0, 7, 3}
};

bool init(void) {
    srand(time(NULL)); rand();

    if (!objLoadMesh("Untitled.obj", &triangle)) return false;

    for (int i = 0; i < triangle.numTris; i++) triangle.triColours[i] = randomColour();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init error: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Software 3D Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    colourBuffer = malloc(WIDTH * HEIGHT * sizeof(uint32_t));

    if (!window || !renderer || !texture || !colourBuffer) {
        printf("Error creating window or renderer: %s", SDL_GetError());
        return false;
    }

    perfFreq = SDL_GetPerformanceFrequency();
    lastTime = SDL_GetPerformanceCounter();

    return true;
}

void loop(void) {
    Uint64 now = SDL_GetPerformanceCounter();
    float dt = (float)((double)(now - lastTime)) / (double)perfFreq;
    lastTime = now;

    if (dt > 0.1f) dt = 0.1f;

    mat4 camToWorld;
    getCameraToWorld(camLocation, target, worldUp, camToWorld);

    //rot[Y] += glm_rad(0.5f);
    rot[X] -= glm_rad(90 * dt);

    buildModelMatrix(scale, rot, position, triangle.localToWorld);

    vec3 worldPoints[triangle.numVerts];
    for (int i = 0; i < triangle.numVerts; i++) {
        vec4 local4 = {
            triangle.vertices[i][X],
            triangle.vertices[i][Y],
            triangle.vertices[i][Z],
            1.0f
        };
        vec4 result;
        glm_mat4_mulv(triangle.localToWorld, local4, result);

        worldPoints[i][X] = result[X];
        worldPoints[i][Y] = result[Y];
        worldPoints[i][Z] = result[Z];
    }

    vec2 rasterPoints[triangle.numVerts];
    for (int i = 0; i < triangle.numVerts; i++) {
        worldPointToCam(camToWorld, worldPoints[i], rasterPoints[i]);

        rasterPoints[i][X] = ((rasterPoints[i][X] + 1.0f) * 0.5f) * WIDTH;
        rasterPoints[i][Y] = (1.0f - (rasterPoints[i][Y] * 0.5f + 0.5f)) * HEIGHT;
    }

    memset(colourBuffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));

    for (int i = 0; i < triangle.numTris; i++) {
        if (worldTriFacingCam(
            worldPoints[triangle.indices[i][0]], 
            worldPoints[triangle.indices[i][1]], 
            worldPoints[triangle.indices[i][2]],
            camLocation)) {
                drawTri(
                    rasterPoints[triangle.indices[i][0]],
                    rasterPoints[triangle.indices[i][1]],
                    rasterPoints[triangle.indices[i][2]],
                    triangle.triColours[i],
                    colourBuffer
                );
            }
    }

    SDL_UpdateTexture(texture, NULL, colourBuffer, WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void kill(void) {
    mesh_destroy(&triangle);
    free(colourBuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture);
    SDL_Quit();
}