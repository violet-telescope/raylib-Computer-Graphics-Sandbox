#include "render.h"
#include "graphics_math.h"

#define SCENE_SCALE 1.0f        // scales size of everything in world
#define WAVEFORM_OBJ_SCALE 6.0f // scales size of OBJ_WAVEFORM_OBJs

extern int CONTROLS_WIDTH; // pixel size of controls UI

void RenderDrawLine(Vector2 a, Vector2 b, Color color)
{
    int x0 = (int)a.x;
    int y0 = (int)a.y;
    int x1 = (int)b.x;
    int y1 = (int)b.y;

    if (abs(x1 - x0) > abs(y1 - y0))
    {
        RenderDrawLineH(x0, y0, x1, y1, color);
    }
    else
    {
        RenderDrawLineV(x0, y0, x1, y1, color);
    }
}

void RenderDrawLineH(int x0, int y0, int x1, int y1, Color color)
{
    // if this line points left, swap the coordinates to make it point right
    if (x0 > x1)
    {
        int tmp = x0;
        x0 = x1;
        x1 = tmp;

        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    // does this line have positive or negative slope?
    int dir = (dy < 0) ? -1 : 1;
    dy *= dir;

    if (dx != 0)
    {
        int y = y0;
        int p = (2 * dy) - dx;
        for (int i = 0; i < (dx + 1); i++)
        {
            DrawPixel(x0 + i, y, color);
            if (p >= 0)
            {
                y += dir;
                p = p - (2 * dx);
            }
            p = p + (2 * dy);
        }
    }
}

void RenderDrawLineV(int x0, int y0, int x1, int y1, Color color)
{
    // if this line points left, swap the coordinates to make it point right
    if (y0 > y1)
    {
        int tmp = x0;
        x0 = x1;
        x1 = tmp;

        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    // does this line have positive or negative slope?
    int dir = (dx < 0) ? -1 : 1;
    dx *= dir;

    if (dy != 0)
    {
        int x = x0;
        int p = (2 * dx) - dy;
        for (int i = 0; i < (dy + 1); i++)
        {
            DrawPixel(x, y0 + i, color);
            if (p >= 0)
            {
                x += dir;
                p = p - (2 * dy);
            }
            p = p + (2 * dx);
        }
    }
}

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

    if (xVisible) RenderDrawLine(originScreen, xScreen, RED);
    if (yVisible) RenderDrawLine(originScreen, yScreen, GREEN);
    if (zVisible) RenderDrawLine(originScreen, zScreen, BLUE);

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
        case OBJ_WAVEFRONT_OBJ:
        {
            *vertexCount = object->objMesh->numVertices;
            Vector4 *vertices = calloc(*vertexCount, sizeof(Vector4));

            for (int i = 0; i < *vertexCount; i++)
            {
                float x = object->objMesh->vertices[i].x;
                float y = object->objMesh->vertices[i].y;
                float z = object->objMesh->vertices[i].z;
                vertices[i] = Vector4Scaling((Vector4){x, y, z, 1.0f}, SCENE_SCALE * WAVEFORM_OBJ_SCALE);
            }

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

void DrawQuadrilateral(Vector2 *vertices, Color color)
{
    RenderDrawLine(vertices[0], vertices[1], color);
    RenderDrawLine(vertices[1], vertices[3], color);
    RenderDrawLine(vertices[3], vertices[2], color);
    RenderDrawLine(vertices[2], vertices[0], color);
}

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
        Matrix rotation = MXMultiply(MXRotationZ(object->rotation.pitch), MXMultiply(MXRotationY(object->rotation.yaw), MXRotationX(object->rotation.roll)));
        // M = T * R * S (we have no scaling matrix S because we scale on creation of T)
        Matrix model = MXMultiply(translation, rotation);
        // MVP = P * V * M
        Matrix modelViewProjection = MXMultiply(projection, MXMultiply(view, model));

        int vertexCount = 0;
        Vector4 *vertices = GetObjectVertices(object, &vertexCount);
        if (!vertices) continue;
        Vector2 *screenSpaceVertices = calloc(vertexCount, sizeof(Vector2));

        bool behindCamera = false;
        for (int j = 0; j < vertexCount; j++)
        {
            Vector4 vertex = vertices[j];
            // vertex_clip-space = P * V * M * vertex
            vertex = Vector4Transform(vertex, modelViewProjection); 
            if (vertex.w <= 0.0f)
            {
                behindCamera = true;
                break;
            }
            // vertex_NDC = { vertex_clip-space.x / vertex_clip-space.w,
            //                vertex_clip-space.y / vertex_clip-space.w,
            //                vertex_clip-space.z / vertex_clip-space.w,
            //                vertex_clip-space.w = 1.0                  }
            vertex = Vector4PerspectiveDivide(vertex);
            screenSpaceVertices[j] = ViewportTransform(vertex, trueScreenWidth, GetScreenHeight());
        }
        
        if (behindCamera)
        {
            free(vertices);
            free(screenSpaceVertices);
            continue;
        }            

        switch (object->type) {
            case OBJ_CIRCLE:
            {
                for (int j = 0; j < vertexCount - 1; j++)
                {
                    RenderDrawLine(screenSpaceVertices[j], screenSpaceVertices[j+1], object->color);
                }
                RenderDrawLine(screenSpaceVertices[vertexCount-1], screenSpaceVertices[0], object->color);
                break;
            }
            case OBJ_SQUARE:
            case OBJ_RECTANGLE:
            {
                DrawQuadrilateral(screenSpaceVertices, object->color);
                break;
            }
            case OBJ_WAVEFRONT_OBJ:
            {
                for (int i = 0; i < object->objMesh->numFaces; i++)
                {
                    /*
                    RenderDrawLine(screenSpaceVertices[(int)object->objMesh->faces[i].x-1],
                        screenSpaceVertices[(int)object->objMesh->faces[i].y-1], object->color);
                    RenderDrawLine(screenSpaceVertices[(int)object->objMesh->faces[i].y-1],
                        screenSpaceVertices[(int)object->objMesh->faces[i].z-1], object->color);
                    RenderDrawLine(screenSpaceVertices[(int)object->objMesh->faces[i].z-1],
                        screenSpaceVertices[(int)object->objMesh->faces[i].x-1], object->color);
                    */
                    DrawLineV(screenSpaceVertices[(int)object->objMesh->faces[i].x-1],
                        screenSpaceVertices[(int)object->objMesh->faces[i].y-1], object->color);
                    DrawLineV(screenSpaceVertices[(int)object->objMesh->faces[i].y-1],
                        screenSpaceVertices[(int)object->objMesh->faces[i].z-1], object->color);
                    DrawLineV(screenSpaceVertices[(int)object->objMesh->faces[i].z-1],
                        screenSpaceVertices[(int)object->objMesh->faces[i].x-1], object->color);
                }
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