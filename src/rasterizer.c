#include "rasterizer.h"
#include "definitions.h"

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
    if (numPoints == 3) {
        drawTri(points[0], points[1], points[2], colours[0], buffer);
        return;
    }
    
    for (int i = 0; i < numPoints - 1; i++) {
        drawTri(points[0], points[i], points[i + 1], colours[i], buffer);
    }
}

uint32_t randomColour(void) {
    uint32_t colour = 0xFF000000;

    colour |= (rand() % (0xFF + 1)) << 8; //  green
    colour |= (rand() % (0xFF + 1)) << 16; // red
    colour |= (rand() % (0xFF + 1)); //       blue

    return colour;
}

bool mesh_init(size_t numVerts, size_t numTris, Mesh* mesh) {
    if (!mesh) return false;

    glm_mat4_identity(mesh->localToWorld);

    mesh->vertices = malloc(sizeof(*mesh->vertices) * numVerts);
    mesh->indices = malloc(sizeof(*mesh->indices) * numTris);
    mesh->triColours = malloc(sizeof(*mesh->triColours) * numTris);

    if (!mesh->vertices || !mesh->indices || !mesh->triColours) {
        mesh_destroy(mesh);
        return false;
    }

    mesh->numVerts = numVerts;
    mesh->numTris = numTris;

    return true;
}

void mesh_destroy(Mesh* mesh) {
    free(mesh->vertices);
    free(mesh->indices);
    free(mesh->triColours);

    mesh->vertices = NULL;
    mesh->indices = NULL;
    mesh->triColours = NULL;

    mesh->numTris = 0;
    mesh->numVerts = 0;
}