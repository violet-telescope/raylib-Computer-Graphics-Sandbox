#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include "raylib.h"
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    OBJ_CIRCLE,
    OBJ_SQUARE,
    OBJ_RECTANGLE,
    OBJ_WAVEFRONT_OBJ,
    OBJ_FRACTAL,
    OBJ_TEXT
} ObjectType;

typedef struct {
    float pitch, yaw, roll;
} Rotation;

typedef enum {
	NOTHING             = 0,
	FLIP_IMAGE          = 1, // flip vertically
	CENTER_IMAGE        = 2, // center the image at 0,0,0
	CENTER_FLIP_IMAGE   = 3, // center the image at 0,0,0 and flip
	BOTTOM_IMAGE        = 4, // center the bottom of the image at 0,0,0
	BOTTOM_FLIP_IMAGE   = 5, // center the bottom of the image at 0,0,0 and flip
	TOP_IMAGE           = 6, // center the top of the image at 0,0,0
	TOP_FLIP_IMAGE      = 7  // center the top of the image at 0,0,0 and flip
} ImageJustify;

typedef enum {
	XYZ = 0,
	XZY = 1,
	YXZ = 2,
	YZX = 3,
	ZXY = 4,
	ZYX = 5
} VertexExchange;

typedef struct {
    Vector3 vector;
    Color color;
} Vertex;

// TODO: reimplement with Vertex struct instead of two separate vertices and vertexColors arrays
typedef struct {
    Vector3 *vertices;
    int numVertices;
    Color *vertexColors;
    Vector3 *faces;
    int numFaces;
} MeshObject;

typedef struct {
    // type-agnostic fields
    char name[64];
    ObjectType type;
    Color color;
    Vector3 position;
    Rotation rotation;
    int isVisible;

    // type-specific fields
    float diameter;                     // CIRCLE
    float width;                        // SQUARE, RECTANGLE
    float height;                       // RECTANGLE

    // WAVEFRONT_OBJ
    char filename[64];          
    float objScale;    
    VertexExchange vertexExchange;      // swap vertex order for other coord systems (Z up, Y up, etc.)
    ImageJustify justification;         // adjust vertex positions so object is centered on origin
    MeshObject *objMesh;

    // FRACTAL
    char fractalType[64];       
    int iterations;
    bool isSeeded;                      // 1 = seeded, 0 = random
    int seed;
    Vector3 scale;
    Vertex *vertices;
    Vertex baseVertices[4];
    Vector3 baseIndex[2];
    MeshObject *fractalMesh;
    // color_125...1000

    // TEXT
    char fontname[64];
    char fontfamily[64];
    int fontsize;
    char **textLines;                   // array of arrays for each line
    int textLineCount;                  // size of array of arrays
} Object;

typedef struct {
    char name[64];
    Camera3D camera;
} cameraWithName;

#endif