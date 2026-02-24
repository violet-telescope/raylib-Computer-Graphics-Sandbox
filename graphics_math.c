#include "graphics_math.h"

/* OpenGL-style matrix reference
    m0  m4  m8  m12
    m1  m5  m9  m13
    m2  m6  m10 m14
    m3  m7  m11 m15
*/

const float toRadians = (float)(PI / 180.0f);
const float toDegrees = (float)(180.0f / PI);

Matrix MXMultiply(Matrix a, Matrix b)
{
    Matrix c = {0};
    
    c.m0  = a.m0*b.m0  + a.m4*b.m1  + a.m8*b.m2   + a.m12*b.m3;
    c.m1  = a.m1*b.m0  + a.m5*b.m1  + a.m9*b.m2   + a.m13*b.m3;
    c.m2  = a.m2*b.m0  + a.m6*b.m1  + a.m10*b.m2  + a.m14*b.m3;
    c.m3  = a.m3*b.m0  + a.m7*b.m1  + a.m11*b.m2  + a.m15*b.m3;

    c.m4  = a.m0*b.m4  + a.m4*b.m5  + a.m8*b.m6   + a.m12*b.m7;
    c.m5  = a.m1*b.m4  + a.m5*b.m5  + a.m9*b.m6   + a.m13*b.m7;
    c.m6  = a.m2*b.m4  + a.m6*b.m5  + a.m10*b.m6  + a.m14*b.m7;
    c.m7  = a.m3*b.m4  + a.m7*b.m5  + a.m11*b.m6  + a.m15*b.m7;

    c.m8  = a.m0*b.m8  + a.m4*b.m9  + a.m8*b.m10  + a.m12*b.m11;
    c.m9  = a.m1*b.m8  + a.m5*b.m9  + a.m9*b.m10  + a.m13*b.m11;
    c.m10 = a.m2*b.m8  + a.m6*b.m9  + a.m10*b.m10 + a.m14*b.m11;
    c.m11 = a.m3*b.m8  + a.m7*b.m9  + a.m11*b.m10 + a.m15*b.m11;

    c.m12 = a.m0*b.m12 + a.m4*b.m13 + a.m8*b.m14  + a.m12*b.m15;
    c.m13 = a.m1*b.m12 + a.m5*b.m13 + a.m9*b.m14  + a.m13*b.m15;
    c.m14 = a.m2*b.m12 + a.m6*b.m13 + a.m10*b.m14 + a.m14*b.m15;
    c.m15 = a.m3*b.m12 + a.m7*b.m13 + a.m11*b.m14 + a.m15*b.m15;

    return c;
}


Matrix MXTranslationMatrix(float x, float y, float z)
{
    Matrix m = {0};

    m.m0 = 1;
    m.m5 = 1;
    m.m10 = 1;
    m.m15 = 1;
    m.m12 = x;
    m.m13 = y;
    m.m14 = z;

    return m;
}

Matrix MXRotationX(float angle)
{
    Matrix m = {0};
    m.m0  = 1;
    m.m5  = cosf(angle);
    m.m6  = sinf(angle);
    m.m9  = -sinf(angle);
    m.m10 = cosf(angle);
    m.m15 = 1;
    return m;
}

Matrix MXRotationY(float angle) // Y has yaw in raylib
{
    Matrix m = {0};

    m.m0 = cosf(angle);
    m.m2 = sinf(angle);
    m.m5 = 1;
    m.m8 = -sinf(angle);
    m.m10 = cosf(angle);
    m.m15 = 1;

    return m;
}

Matrix MXRotationZ(float angle) // Z has roll in raylib
{
    Matrix m = {0};

    m.m0 = cosf(angle);
    m.m1 = -sinf(angle);
    m.m4 = sinf(angle);
    m.m5 = cosf(angle);
    m.m10 = 1;
    m.m15 = 1;

    return m;
}

Matrix MXTranspose(Matrix m)
{
    Matrix result = {0};
    float *M = (float *)&m;
    float *R = (float *)&result;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            R[j*4 + i] = M[i*4 + j];
    return result;
}

Matrix MXPerspective(float fovy, float aspectRatio, float near, float far)
{
    Matrix m = {0};

    float scaleY = 1.0f / tanf(fovy / 2.0f);
    float scaleX = scaleY / aspectRatio;
    float A = -(far + near) / (far - near);
    float B = -(2.0f * far * near) / (far - near);

    m.m0 = scaleX;
    m.m5 = scaleY;
    m.m10 = A;
    m.m11 = -1;
    m.m14 = B;

    return m;
}

Vector4 Vector4Scaling(Vector4 v, float scalar)
{
    return (Vector4){ v.x * scalar, v.y * scalar, v.z * scalar, v.w };
}

Vector4 Vector4Transform(Vector4 v, Matrix m)
{
    Vector4 result;
    
    result.x = (v.x * m.m0) + (v.y * m.m4) + (v.z * m.m8) + (v.w * m.m12);
    result.y = (v.x * m.m1) + (v.y * m.m5) + (v.z * m.m9) + (v.w * m.m13);
    result.z = (v.x * m.m2) + (v.y * m.m6) + (v.z * m.m10) + (v.w * m.m14);
    result.w = (v.x * m.m3) + (v.y * m.m7) + (v.z * m.m11) + (v.w * m.m15);

    return result;
}

Vector4 Vector4PerspectiveDivide(Vector4 v)
{
    if (v.w == 0.0f) return v;

    Vector4 result;

    result.x = v.x / v.w;
    result.y = v.y / v.w;
    result.z = v.z / v.w;
    result.w = 1.0f;

    return result;
}

// scale the viewport from NDC
Vector2 ViewportTransform(Vector4 v, int screenWidth, int screenHeight)
{
    Vector2 result;

    result.x = (v.x + 1) / 2.0f * screenWidth;
    result.y = (1 - v.y) / 2.0f * screenHeight;

    return result;
}