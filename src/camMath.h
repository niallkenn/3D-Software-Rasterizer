#ifndef CAMM_H
#define CAMM_H

#include <cglm/cglm.h>
#include <stdbool.h>

void getCameraToWorld(vec3 camLocation, vec3 target, vec3 worldUp, vec4* matrix);
void worldPointToCam(mat4 camToWorld, vec3 point, vec2 dst);
bool worldTriFacingCam(vec3 p0, vec3 p1, vec3 p2, vec3 camLocation);

#endif // CAMM_H