#ifndef GRAPHICS_MATH_H
#define GRAPHICS_MATH_H

#include "raylib.h"
#include <math.h>

Matrix MXMultiply(Matrix A, Matrix B);
Matrix MXTranslationMatrix(float x, float y, float z);
Matrix MXRotationX(float angle);
Matrix MXRotationY(float angle);
Matrix MXRotationZ(float angle);
Matrix MXTranspose(Matrix m);
Matrix MXPerspective(float fovy, float aspectRatio, float minCameraDistance, float maxCameraDistance);

Vector4 Vector4Scaling(Vector4 v, float scalar);
Vector4 Vector4Transform(Vector4 v, Matrix m);
Vector4 Vector4PerspectiveDivide(Vector4 v);
Vector2 ViewportTransform(Vector4 v, int screenWidth, int screenHeight);

#endif