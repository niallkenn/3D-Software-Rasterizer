#include "modelMath.h"
#include "definitions.h"

void buildModelMatrix(vec3 scale, vec3 rotation, vec3 position, mat4 dest) {
    mat4 S, Rx, Ry, Rz, R, T, temp;

    glm_mat4_identity(S);
    S[0][0] = scale[X];
    S[1][1] = scale[Y];
    S[2][2] = scale[Z];

    buildRotX(rotation[X], Rx);
    buildRotY(rotation[Y], Ry);
    buildRotZ(rotation[Z], Rz);

    glm_mat4_mul(Ry, Rx, temp);
    glm_mat4_mul(Rz, temp, R);

    glm_mat4_identity(T);
    T[3][0] = position[X];
    T[3][1] = position[Y];
    T[3][2] = position[Z];  

    glm_mat4_mul(R, S, temp);
    glm_mat4_mul(T, temp, dest);
}

void buildRotX(float angle, mat4 dest) {
    float c = cosf(angle);
    float s = sinf(angle);

    glm_mat4_identity(dest);

    dest[1][1] = c; dest[1][2] = s;
    dest[2][1] = -s; dest[2][2] = c;
}

void buildRotZ(float angle, mat4 dest) {
    float c = cosf(angle);
    float s = sinf(angle);

    glm_mat4_identity(dest);

    dest[0][0] = c; dest[0][1] = s;
    dest[1][0] = -s; dest[1][1] = c;
}

void buildRotY(float angle, mat4 dest) {
    float c = cosf(angle);
    float s = sinf(angle);

    glm_mat4_identity(dest);

    dest[0][0] =  c;  dest[0][1] = 0.0f;  dest[0][2] = -s;
    dest[1][0] = 0.0f; dest[1][1] = 1.0f;  dest[1][2] = 0.0f;
    dest[2][0] =  s;  dest[2][1] = 0.0f;  dest[2][2] =  c;
}
