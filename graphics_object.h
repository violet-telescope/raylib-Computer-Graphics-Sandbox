#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include "raylib.h"
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum { // there may be more object types implemented in the orignal, make sure to go back and check
    OBJ_CIRCLE,
    OBJ_SQUARE,
    OBJ_RECTANGLE,
    OBJ_WAVEFRONT_OBJ,
    OBJ_TEXT
} ObjectType;

typedef struct {
    float pitch, yaw, roll;
} Rotation;

typedef struct {
    ObjectType type;
    char name[64];
    Vector3 position;
    Rotation rotation;
    Color color;
    int isVisible;

    // type-specific fields
    float diameter;             // CIRCLE
    float width;                // SQUARE, RECTANGLE
    float height;               // RECTANGLE
    char filename[64];          // WAVEFRONT_OBJ
    float scale;                // WAVEFRONT_OBJ
    char vertex_exchange[4];    // WAVEFRONT_OBJ -- ???
    char justification[64];     // WAVEFRONT_OBJ -- ???
    char fontname[64];          // TEXT
    char fontfamily[64];        // TEXT
    int fontsize;               // TEXT
    char **textLines;           // TEXT -- array of arrays for each line
    int textLineCount;          // TEXT -- size of array of arrays
} Object;

typedef struct {
    char name[64];
    Camera3D camera;
} cameraWithName;

#endif