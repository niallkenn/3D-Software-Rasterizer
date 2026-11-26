#include "obj.h"
#include "definitions.h"

static size_t trisInFace(char line[512]);

bool objLoadMesh(const char* path, Mesh* mesh) {
    if (!path || !mesh) return false; // check for invalid NULL pointers

    FILE* fptr = fopen(path, "r"); // open file in reading mode
    if (!fptr) return false;

    char line[512]; // line read buffer

    size_t numVerts = 0;
    size_t numTris = 0;

    while (fgets(line, sizeof(line), fptr)) {
        if (line[0] == 'v' && (line[1] == ' ' || line[1] == '\t')) { // v  entry in file, vertex
            numVerts++;
        } else if (line[0] == 'f' && (line[1] == ' ' || line[1] == '\t')) { // f entry in file, face
            numTris += trisInFace(line);
        }
    }

    if (numVerts == 0 || numTris == 0) {
        fclose(fptr);
        return false;
    }

    if (!mesh_init(numVerts, numTris, mesh)) {
        fclose(fptr);
        return false;
    }

    rewind(fptr); // pull back to start of file

    size_t vIndex = 0;
    size_t tIndex = 0;

    while (fgets(line, sizeof(line), fptr)) {
        // loads vertices
        if (line[0] == 'v' && (line[1] == ' ' || line[1] == '\t')) {
            float x, y, z;
            if (sscanf(line + 1, "%f %f %f", &x, &y, &z) == 3 && vIndex < numVerts) {
                mesh->vertices[vIndex][X] = x;
                mesh->vertices[vIndex][Y] = y;
                mesh->vertices[vIndex][Z] = z;
                vIndex++;
            }
        } else if (line[0] == 'f' && (line[1] == ' ' || line[1] == '\t')) {
            int faceVerts[128];
            int verts = 0;
            const char* p = line + 1;

            while (*p && verts < 128) {
                while (*p == ' ' || *p == '\t') p++;
                if (*p == '\0' || *p == '\n' || *p == '\r' || *p == '#') break;

                char* end;
                long index = strtol(p, &end, 10);
                if (index == 0 || end == p) break;

                faceVerts[verts++] = (int)(index - 1);

                p = end;
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') p++;
            }

            for (int i = 0; i < verts - 2 && tIndex < numTris; i++) {
                mesh->indices[tIndex][0] = faceVerts[0];
                mesh->indices[tIndex][1] = faceVerts[i + 1];
                mesh->indices[tIndex][2] = faceVerts[i + 2];
                tIndex++;
            }
        }
    }

    mesh->numTris = tIndex;
    mesh->numVerts = vIndex;

    fclose(fptr);
    return true;
}

static size_t trisInFace(char line[512]) {
    int vertInFace = 0;
    const char* p = line + 1;

    while (*p) {
        while (*p == ' ' || *p == '\t') p++; // skip whitespace
        if (*p == '\0' || *p == '\n' || *p == '\r' || *p == '#') break;

        char* end; // character after the end of the index number , / , "1/1"
        long idx = strtol(p, &end, 10);
        if (idx == 0 || end == p) break; // no 0 index, 1 indexing, check something was read

        vertInFace++; 
        p = end; // move to after the inx read

        while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') p++; // skip until next whitespace
    }

    return (vertInFace >= 3) ? (vertInFace - 2) : 0;
}