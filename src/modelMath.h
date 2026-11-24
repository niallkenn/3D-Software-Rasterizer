#ifndef MM_H
#define MM_H

#include <cglm/cglm.h>

void buildModelMatrix(vec3 scale, vec3 rotation, vec3 position, mat4 dest);

void buildRotX(float angle, mat4 dest);
void buildRotY(float angle, mat4 dest);
void buildRotZ(float angle, mat4 dest);

#endif // MM_H