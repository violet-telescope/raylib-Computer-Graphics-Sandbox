#ifndef LOADER_H
#define LOADER_H

#include "graphics_object.h"
#include "graphics_math.h"
#include "cJSON.h"
#include <stdio.h>

VertexExchange GetVertexExchange(char* exchange);
ImageJustify GetJustification(char* justification);
Color ColorFromString(const char *str);
MeshObject *OBJFileToMesh(char* filename);
Object *LoadObjectsFromJSON(cJSON *jsonObjects, int *numObjects, cJSON *jsonSetUp);
cameraWithName *LoadCamerasFromJSON(cJSON *jsonCameras, int *numCameras);
void *LoadSceneFromJSON(const char *filename, Object **objects, int *numObjects, cameraWithName **cameras, int *numCameras);

#endif