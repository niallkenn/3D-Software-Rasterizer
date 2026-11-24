#include "camMath.h"
#include "definitions.h"

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

bool worldTriFacingCam(vec3 p0, vec3 p1, vec3 p2, vec3 camLocation) {
    vec3 normal;
    vec3 toCam;
    vec3 ab;
    vec3 ac;
    
    glm_vec3_sub(p0, p1, ab);
    glm_vec3_sub(p0, p2, ac);

    glm_vec3_cross(ab, ac, normal);
    glm_vec3_sub(camLocation, p0, toCam);

    return glm_vec3_dot(normal, toCam) > 0;
}