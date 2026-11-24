#ifndef RAS_H
#define RAS_H

#include <cglm/cglm.h>

typedef struct {
    mat4 localToWorld; // local -> world transform, rotation, scale, translation

    vec3* vertices; // local space positions
    size_t numVerts; // count

    int (*indices)[3]; // triangle index triples
    size_t numTris; // count of triangles

    uint32_t* triColours; // colour for each triangle
} Mesh;

void mesh_init(size_t numVerts, size_t numTris, Mesh* mesh);

void mesh_destroy(Mesh* mesh);

// checks if a point p is in triangle t
uint8_t pointInTriangle(vec2 p, vec2 t0, vec2 t1, vec2 t2);
// draws triangle to buffer
void drawTri(vec2 p0, vec2 p1, vec2 p2, uint32_t colour, uint32_t* buffer);
// loops through points to draw convex polygon with triangles
void drawConvexPolygon(vec2* points, uint32_t* colours, int numPoints, uint32_t* buffer);
// generates random opaque colour
uint32_t randomColour(void);
#endif // RAS_H