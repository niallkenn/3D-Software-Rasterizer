#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <cglm/cglm.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600
#define X 0
#define Y 1
#define Z 2
#define FOV 60

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

void drawConvexPolygon(vec2* points, uint32_t* colours, int numPoints, uint32_t* buffer) {
    for (int i = 0; i < numPoints - 1; i++) {
        drawTri(points[0], points[i], points[i + 1], colours[i], buffer);
    }
}

void getCameraToWorld(vec3 camLocation, vec3 target, vec3 worldUp, vec4* matrix) {
    // bottom row 0 0 0 1
    matrix[0][3] = 0;
    matrix[1][3] = 0;
    matrix[2][3] = 0;
    matrix[3][3] = 1;

    vec3 camZ; // cam - target
    glm_vec3_sub(camLocation, target, camZ);
    glm_vec3_normalize(camZ);

    vec3 camR;
    glm_vec3_cross(camZ, worldUp, camR);
    glm_vec3_normalize(camR);

    vec3 camU;
    glm_vec3_cross(camR, camZ, camU);
    glm_vec3_normalize(camU);

    for (int i = 0; i < 3; i++) {
        matrix[0][i] = camR[i];
        
        matrix[1][i] = camU[i];

        matrix[2][i] = camZ[i];

        matrix[3][i] = camLocation[i];
    }
}

void worldPointToCam(mat4 camToWorld, vec3 point, vec2 dst) {
    mat4 worldToCam;
    glm_mat4_inv(camToWorld, worldToCam);

    vec4 point4 = {point[X], point[Y], point[Z], 1};

    vec4 transformed4;
    glm_mat4_mulv(worldToCam, point4, transformed4);

    float aspect = (float)WIDTH / (float)HEIGHT;
    float tanHalfFov = tanf(glm_rad(FOV * 0.5f));
    dst[X] = (transformed4[X] / -transformed4[Z]) / (tanHalfFov * aspect);
    dst[Y] = (transformed4[Y] / -transformed4[Z]) / tanHalfFov;
}

int main(void) {
    srand(time(NULL));

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

    vec3 points[] = {
    {-0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},
    {-0.5f,  0.5f, -0.5f},
    {-0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f}
};

    int indices[12][3] = {
        {4, 5, 6}, {4, 6, 7}, // back
        {0, 1, 2}, {0, 2, 3}, // front
        {0, 1, 5}, {0, 5, 4}, // bottom
        {2, 3, 6}, {3, 6, 7}, // top
        {1, 5, 6}, {5, 6, 2}, // right
        {1, 4, 7}, {1, 7, 3} // left
    };

    int numPoints = sizeof(points) / sizeof(points[0]);

    vec3 camLocation = {3, 3, 3};
    vec3 target = {0, 0, 0};
    vec3 worldUp = {0, 1, 0};

    mat4 camToWorld;
    float xa = -3;
    float za = 0;
    bool right = true;
    while (running) {
        
        if (right) {
            xa += 0.03f;
        } else {
            xa -= 0.03f;
        }
        
        if (xa <= -3 || xa >= 3) right = !right;
        za = sqrt(xa * 4 + 7 - pow(xa, 2));
        
        camLocation[X] = xa;
        camLocation[Z] = za;
        getCameraToWorld(camLocation, target, worldUp, camToWorld);

        SDL_Event e;

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        memset(colourBuffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));

        vec2 rasterPoints[numPoints];

        for (int i = 0; i < numPoints; i++) {
            worldPointToCam(camToWorld, points[i], rasterPoints[i]);

            rasterPoints[i][X] = ((rasterPoints[i][X] + 1.0f) * 0.5f) * WIDTH;
            rasterPoints[i][Y] = (1.0f - (rasterPoints[i][Y] * 0.5f + 0.5f)) * HEIGHT;
        }

        for (int i = 0; i < 12; i++) {
            drawTri(
                rasterPoints[indices[i][0]],
                rasterPoints[indices[i][1]],
                rasterPoints[indices[i][2]],
                0xFF00FFF0,
                colourBuffer
            );
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