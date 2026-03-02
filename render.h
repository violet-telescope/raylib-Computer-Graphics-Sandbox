#ifndef RENDER_H
#define RENDER_H

#include <stdlib.h>
#include "raylib.h"
#include "graphics_object.h"

void RenderDrawLine(Vector2 v1, Vector2 v2, Color color);
void RenderDrawLineH(int x0, int y0, int x1, int y1, Color color);
void RenderDrawLineV(int x0, int y0, int x1, int y1, Color color);
void DrawAxes(Matrix mvp, int screenWidth, int screenHeight);
void RenderDrawCircle(Vector2 *vertices, int vertexCount, Color color);
void RenderDrawQuadrilateral(Vector2 *vertices, Color color);
void RenderDrawWaveformObj(MeshObject *meshObj, Vector2 *vertices, Color color);
Vector4 *GetObjectVertices(Object *object, int *vertexCount);
void RenderScene(Object *objects, int numObjects, Camera3D camera, bool drawAxes);

#endif