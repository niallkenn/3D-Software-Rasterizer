#include <SDL2/SDL.h>
#include <time.h>
#include "engine.h"
#include "definitions.h"
#include "rasterizer.h"
#include "camMath.h"
#include "modelMath.h"

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

static Mesh cube;

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

    mesh_init(sizeof(points) / sizeof(points[0]), sizeof(indices) / sizeof(indices[0]), &cube);

    // populate vertices
    for (int i = 0; i < cube.numVerts; i++) {
        cube.vertices[i][X] = points[i][X];
        cube.vertices[i][Y] = points[i][Y];
        cube.vertices[i][Z] = points[i][Z];
    }
    // populate indices
    for (int i = 0; i < cube.numTris; i++) {
        cube.indices[i][0] = indices[i][0];
        cube.indices[i][1] = indices[i][1];
        cube.indices[i][2] = indices[i][2];
    }

    for (int i = 0; i < cube.numTris; i++) cube.triColours[i] = randomColour();

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

    mat4 camToWorld;
    getCameraToWorld(camLocation, target, worldUp, camToWorld);

    //rot[Y] += glm_rad(0.5f);
    rot[X] -= glm_rad(90 * dt);

    buildModelMatrix(scale, rot, position, cube.localToWorld);

    vec3 worldPoints[cube.numVerts];
    for (int i = 0; i < cube.numVerts; i++) {
        vec4 local4 = {
            cube.vertices[i][X],
            cube.vertices[i][Y],
            cube.vertices[i][Z],
            1.0f
        };
        vec4 result;
        glm_mat4_mulv(cube.localToWorld, local4, result);

        worldPoints[i][X] = result[X];
        worldPoints[i][Y] = result[Y];
        worldPoints[i][Z] = result[Z];
    }

    vec2 rasterPoints[cube.numVerts];
    for (int i = 0; i < cube.numVerts; i++) {
        worldPointToCam(camToWorld, worldPoints[i], rasterPoints[i]);

        rasterPoints[i][X] = ((rasterPoints[i][X] + 1.0f) * 0.5f) * WIDTH;
        rasterPoints[i][Y] = (1.0f - (rasterPoints[i][Y] * 0.5f + 0.5f)) * HEIGHT;
    }

    memset(colourBuffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));

    for (int i = 0; i < cube.numTris; i++) {
        if (worldTriFacingCam(
            worldPoints[cube.indices[i][0]], 
            worldPoints[cube.indices[i][1]], 
            worldPoints[cube.indices[i][2]],
            camLocation)) {
                drawTri(
                    rasterPoints[cube.indices[i][0]],
                    rasterPoints[cube.indices[i][1]],
                    rasterPoints[cube.indices[i][2]],
                    cube.triColours[i],
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
    mesh_destroy(&cube);
    free(colourBuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture);
    SDL_Quit();
}