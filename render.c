#include "render.h"
#include "graphics_math.h"

#define SCENE_SCALE 1.0f // scales size of objects in world

extern int CONTROLS_WIDTH; // pixel size of controls UI

// TODO: implement personal line drawing using raylib's `DrawPixel` and Bresenham's line algorithm
void RenderDrawLine(Vector2 a, Vector2 b, Color color)
{

}

// TODO: update draw calls with new line drawing once implemented
void DrawAxes(Matrix mvp, int screenWidth, int screenHeight)
{
    Vector4 origin = { 0.0f, 0.0f, 0.0f, 1.0f };
    Vector4 xAxis  = { 1000.0f, 0.0f, 0.0f, 1.0f };
    Vector4 yAxis  = { 0.0f, 1000.0f, 0.0f, 1.0f };
    Vector4 zAxis  = { 0.0f, 0.0f, 1000.0f, 1.0f };

    int xVisible  = xAxis.w > 0.0f;
    int yVisible  = yAxis.w > 0.0f;
    int zVisible  = zAxis.w > 0.0f;

    origin = Vector4Transform(origin, mvp);
    xAxis  = Vector4Transform(xAxis,  mvp);
    yAxis  = Vector4Transform(yAxis,  mvp);
    zAxis  = Vector4Transform(zAxis,  mvp);

    origin = Vector4PerspectiveDivide(origin);
    xAxis  = Vector4PerspectiveDivide(xAxis);
    yAxis  = Vector4PerspectiveDivide(yAxis);
    zAxis  = Vector4PerspectiveDivide(zAxis);

    Vector2 originScreen = ViewportTransform(origin, screenWidth, screenHeight);
    Vector2 xScreen      = ViewportTransform(xAxis,  screenWidth, screenHeight);
    Vector2 yScreen      = ViewportTransform(yAxis,  screenWidth, screenHeight);
    Vector2 zScreen      = ViewportTransform(zAxis,  screenWidth, screenHeight);

    DrawCircle((int)originScreen.x, (int)originScreen.y, 5, WHITE);

    if (xVisible) DrawLineV(originScreen, xScreen, RED);
    if (yVisible) DrawLineV(originScreen, yScreen, GREEN);
    if (zVisible) DrawLineV(originScreen, zScreen, BLUE);

}

Vector4 *GetObjectVertices(Object *object, int *vertexCount)
{
    switch (object->type) {
        case OBJ_CIRCLE:
        {
            const int segments = 32;
            *vertexCount = segments;
            Vector4 *vertices = calloc(segments, sizeof(Vector4));

            for (int i = 0; i < segments; i++)
            {
                float angle = (2.0f * PI * i) / segments;
                float x = (object->diameter / 2.0f) * cosf(angle);
                float y = (object->diameter / 2.0f) * sinf(angle);
                vertices[i] = Vector4Scaling((Vector4){x, y, 0.0f, 1.0f}, SCENE_SCALE);
            }

            return vertices;
        }
        case OBJ_SQUARE:
        {
            *vertexCount = 4;
            Vector4 *vertices = calloc(4, sizeof(Vector4));

            vertices[0] = Vector4Scaling((Vector4){ -object->width/2, object->width/2, 0.0f, 1.0f}, SCENE_SCALE);
            vertices[1] = Vector4Scaling((Vector4){ object->width/2, object->width/2, 0.0f, 1.0f}, SCENE_SCALE);
            vertices[2] = Vector4Scaling((Vector4){ -object->width/2, -object->width/2, 0.0f, 1.0f}, SCENE_SCALE);
            vertices[3] = Vector4Scaling((Vector4){ object->width/2, -object->width/2, 0.0f, 1.0f}, SCENE_SCALE);

            return vertices;
        }
        case OBJ_RECTANGLE:
        {
            *vertexCount = 4;
            Vector4 *vertices = calloc(4, sizeof(Vector4));

            vertices[0] = Vector4Scaling((Vector4){ -object->width/2, object->height/2, 0.0f, 1.0f}, SCENE_SCALE);
            vertices[1] = Vector4Scaling((Vector4){ object->width/2, object->height/2, 0.0f, 1.0f}, SCENE_SCALE);
            vertices[2] = Vector4Scaling((Vector4){ -object->width/2, -object->height/2, 0.0f, 1.0f}, SCENE_SCALE);
            vertices[3] = Vector4Scaling((Vector4){ object->width/2, -object->height/2, 0.0f, 1.0f}, SCENE_SCALE);

            return vertices;
        }
        case OBJ_TEXT:
            // TODO: implement text rendering (hard!)
            break;
        default:
            *vertexCount = 0;
            return NULL;
    }
    // following two lines not necessary once all cases return
    *vertexCount = 0;
    return NULL;
}

// TODO: update draw calls with new line drawing once implemented
void RenderScene(Object *objects, int numObjects, Camera3D camera, bool drawAxes)
{
    int trueScreenWidth = GetScreenWidth() - CONTROLS_WIDTH;
    float aspectRatio = (float)trueScreenWidth / (float)GetScreenHeight();
    Matrix view = GetCameraMatrix(camera);
    Matrix projection = MXPerspective(camera.fovy * (PI / 180.0f), aspectRatio, 0.1f, 5000.0f);

    for (int i = 0; i < numObjects; i++)
    {
        Object *object = &objects[i];

        Matrix translation = MXTranslationMatrix(
            object->position.x * SCENE_SCALE,
            object->position.y * SCENE_SCALE,
            object->position.z * SCENE_SCALE
        );

        // R = Z * Y * X 
        Matrix rotation = MXMultiply(MXRotationZ(object->rotation.roll), MXMultiply(MXRotationY(object->rotation.yaw), MXRotationX(object->rotation.pitch)));
        // M = T * R * S (we have no scaling matrix S because we scale on creation of T)
        Matrix model = MXMultiply(translation, rotation);
        // MVP = P * V * M
        Matrix modelViewProjection = MXMultiply(projection, MXMultiply(view, model));

        int vertexCount = 0;
        Vector4 *vertices = GetObjectVertices(object, &vertexCount);
        if (!vertices) continue;
        Vector2 *screenSpaceVertices = calloc(vertexCount, sizeof(Vector2));

        for (int j = 0; j < vertexCount; j++)
        {
            Vector4 vertex = vertices[j];
            // vertex_clip-space = P * V * M * vertex
            vertex = Vector4Transform(vertex, modelViewProjection); 
            // vertex_NDC = { vertex_clip-space.x / vertex_clip-space.w,
            //                vertex_clip-space.y / vertex_clip-space.w,
            //                vertex_clip-space.z / vertex_clip-space.w,
            //                vertex_clip-space.w = 1.0                  }
            vertex = Vector4PerspectiveDivide(vertex);
            screenSpaceVertices[j] = ViewportTransform(vertex, trueScreenWidth, GetScreenHeight());
        }

        switch (object->type) {
            case OBJ_CIRCLE:
            {
                for (int j = 0; j < vertexCount - 1; j++)
                {
                    DrawLineV(screenSpaceVertices[j], screenSpaceVertices[j+1], object->color);
                }
                DrawLineV(screenSpaceVertices[vertexCount-1], screenSpaceVertices[0], object->color);
                break;
            }
            case OBJ_SQUARE:
            case OBJ_RECTANGLE:
            {
                DrawLineV(screenSpaceVertices[0], screenSpaceVertices[1], object->color);
                DrawLineV(screenSpaceVertices[1], screenSpaceVertices[3], object->color);
                DrawLineV(screenSpaceVertices[3], screenSpaceVertices[2], object->color);
                DrawLineV(screenSpaceVertices[2], screenSpaceVertices[0], object->color);
                break;
            }
            case OBJ_TEXT: // TODO: implement text rendering (hard!)
                break;
            default:
                break;
        }

        free(vertices);
        free(screenSpaceVertices);
    }
    if (drawAxes)
    {
        Matrix viewProjection = MXMultiply(projection, view);
        DrawAxes(viewProjection, trueScreenWidth, GetScreenHeight());
    }

}