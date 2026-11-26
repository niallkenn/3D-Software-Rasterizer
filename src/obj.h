#ifndef OBJ_H
#define OBJ_H

#include <stdbool.h>
#include "rasterizer.h"

bool objLoadMesh(const char* path, Mesh* mesh);

#endif // OBJ_H