#define _POSIX_C_SOURCE 200809L

#include "loader.h"
#include "graphics_object.h"

extern float toRadians;

VertexExchange GetVertexExchange(char* exchange)
{
	if (exchange == "XZY") return XZY;
	if (exchange == "YXZ") return YXZ;
	if (exchange == "YZX") return YZX;
	if (exchange == "ZXY") return ZXY;
	if (exchange == "ZYX") return ZYX;
	return XYZ;
}

ImageJustify GetJustification(char* justification)
{
	if (justification == "FLIP_IMAGE")          return FLIP_IMAGE;
	if (justification == "CENTER_IMAGE")        return CENTER_IMAGE;
	if (justification == "CENTER_FLIP_IMAGE")   return CENTER_FLIP_IMAGE;
	if (justification == "BOTTOM_IMAGE")        return BOTTOM_IMAGE;
	if (justification == "BOTTOM_FLIP_IMAGE")   return BOTTOM_FLIP_IMAGE;
	if (justification == "TOP_IMAGE")           return TOP_IMAGE;
	if (justification == "TOP_FLIP_IMAGE")      return TOP_FLIP_IMAGE;
	return NOTHING;
}

Color ColorFromString(const char *str)
{
    if (strcmp(str, "LIGHTGRAY")   == 0) return LIGHTGRAY;
    if (strcmp(str, "GRAY")        == 0) return GRAY;
    if (strcmp(str, "DARKGRAY")    == 0) return DARKGRAY;
    if (strcmp(str, "YELLOW")      == 0) return YELLOW;
    if (strcmp(str, "GOLD")        == 0) return GOLD;
    if (strcmp(str, "ORANGE")      == 0) return ORANGE;
    if (strcmp(str, "PINK")        == 0) return PINK;
    if (strcmp(str, "RED")         == 0) return RED;
    if (strcmp(str, "MAROON")      == 0) return MAROON;
    if (strcmp(str, "GREEN")       == 0) return GREEN;
    if (strcmp(str, "LIME")        == 0) return LIME;
    if (strcmp(str, "DARKGREEN")   == 0) return DARKGREEN;
    if (strcmp(str, "SKYBLUE")     == 0) return SKYBLUE;
    if (strcmp(str, "BLUE")        == 0) return BLUE;
    if (strcmp(str, "DARKBLUE")    == 0) return DARKBLUE;
    if (strcmp(str, "PURPLE")      == 0) return PURPLE;
    if (strcmp(str, "VIOLET")      == 0) return VIOLET;
    if (strcmp(str, "DARKPURPLE")  == 0) return DARKPURPLE;
    if (strcmp(str, "BEIGE")       == 0) return BEIGE;
    if (strcmp(str, "BROWN")       == 0) return BROWN;
    if (strcmp(str, "DARKBROWN")   == 0) return DARKBROWN;

    if (strcmp(str, "WHITE")       == 0) return RAYWHITE;
    if (strcmp(str, "BLACK")       == 0) return BLACK;
    if (strcmp(str, "BLANK")       == 0) return BLANK; // {0, 0, 0, 0}
    if (strcmp(str, "MAGENTA")     == 0) return MAGENTA;
    if (strcmp(str, "DARKMAGENTA") == 0) return (Color){139, 0, 139, 255}; // non-raylib color
    if (strcmp(str, "RAYWHITE")    == 0) return RAYWHITE;
    // add more as needed
    return MAGENTA; // unless magenta *is* the color, this should make missing colors clear
}

MeshObject *OBJFileToMesh(char* filename)
{
    int numVertices = 0;
    int numFaces = 0;

    FILE *file;
    file = fopen(filename, "r");
    char line[1024];

    if (file == NULL)
    {
        printf("%s failed to open.\n", filename);
        fclose(file);
        return NULL;
    }
    else
    {
        printf("Opened %s\n", filename);
        while (fgets(line, sizeof(line), file) != NULL)
        {
            if (line[0] == 'v' && line[1] == ' ') numVertices++;
            if (line[0] == 'f') numFaces++; 
        }
        rewind(file);

        MeshObject *newMesh = calloc(1, sizeof(MeshObject));
        newMesh->vertices = calloc(numVertices, sizeof(Vector3));
        newMesh->faces = calloc(numFaces * 2, sizeof(Vector3)); // * 2 for quads

        numVertices = 0;
        numFaces = 0;

        while (fgets(line, sizeof(line), file) != NULL) // while(getline(&line, &size, file) != -1)
        {
            line[strcspn(line, "\n")] = '\0'; // remove newline

            // split line into parts
            char **split = calloc(64, sizeof(char *));
            int numSplits = 0;

            char *token = strtok(line, " \t\n");
            while (token != NULL)
            {
                split[numSplits++] = token;
                token = strtok(NULL, " \t\n");
            }
            split[numSplits] = NULL;

            if (split[0] == NULL)
            {
                free(split);
                continue;
            }

            if (strcmp(split[0], "v") == 0)        // vector data
            {
                Vector3 newVector = { strtof(split[1], NULL), strtof(split[2], NULL), strtof(split[3], NULL) };
                newMesh->vertices[numVertices] = newVector;
                numVertices++;
            }
            else if (strcmp(split[0], "f") == 0)   // face data
            {
                float *faceVertices = calloc(numSplits-1, sizeof(float));
                for (int i = 1; i < numSplits; i++)
                {
                    char *slash = strchr(split[i], '/');
                    int length = slash - split[i];
                    char token[64];
                    strncpy(token, split[i], length);
                    token[length] = '\0';

                    faceVertices[i-1] = strtof(token, NULL);
                }
                
                if (numSplits == 4)         // 3 vertices, 1 triangle (1 face)
                {
                    Vector3 newFace = { faceVertices[0], faceVertices[1], faceVertices[2] };
                    newMesh->faces[numFaces] = newFace;
                    numFaces++;
                }
                else if (numSplits == 5)    // 4 vertices, 2 triangles (2 faces)
                {
                    Vector3 newFaceOne = { faceVertices[0], faceVertices[1], faceVertices[2] };
                    newMesh->faces[numFaces] = newFaceOne;
                    numFaces++;
                    Vector3 newFaceTwo = { faceVertices[1], faceVertices[2], faceVertices[3] };
                    newMesh->faces[numFaces] = newFaceTwo;
                    numFaces++;
                }
                free(faceVertices);
            }
            else if (strcmp(split[0], "o") == 0  || (strcmp(split[0], "g") == 0))
            {
                // TODO: implement groups
                continue;
            }
            else
            {
                continue;
            }

            free(split);
        }
        fclose(file);

        newMesh->numVertices = numVertices;
        newMesh->numFaces = numFaces;
        return newMesh; 
    }
}

MeshObject *BuildFractalMountain(Object *fractal)
{
    MeshObject *newMountainMesh = calloc(1, sizeof (MeshObject));

    Vertex **triangles = calloc(2, sizeof (Vertex *));
    Vertex **newTriangles = calloc(2, sizeof (Vertex *));
    int triCount = 2;

    /*
    for (int i = 0; i < 4; i++)
    {
        fractal->baseVertices[i].vector = (Vector3) {
            fractal->baseVertices[i].vector.x * fractal->scale.x,
            fractal->baseVertices[i].vector.y * fractal->scale.y,
            fractal->baseVertices[i].vector.y * fractal->scale.z,
        }; 
    }
    */

    Vertex baseTriangleOne[3] = {
        fractal->baseVertices[(int)fractal->baseIndex[0].x],
        fractal->baseVertices[(int)fractal->baseIndex[0].y],
        fractal->baseVertices[(int)fractal->baseIndex[0].z]
    };
    Vertex baseTriangleTwo[3] = {
        fractal->baseVertices[(int)fractal->baseIndex[1].x],
        fractal->baseVertices[(int)fractal->baseIndex[1].y],
        fractal->baseVertices[(int)fractal->baseIndex[1].z]
    };

    triangles[0] = baseTriangleOne;
    triangles[1] = baseTriangleTwo;

    for (int i = 0; i < fractal->iterations; i++)
    {
        newTriangles = realloc(newTriangles, triCount * 4 * sizeof (Vertex *));
        int newTrianglesAdded = 0;
        for (int j = 0; j < triCount; j++)
        {
            // find the 3 midpoints of each vertex of each triangle
            // TODO: calculate and attempt to store in map instead of using compound literal
            Vertex midpoints[3] = {
                (Vertex) {
                    (triangles[j][0].vector.x + triangles[j][1].vector.x) / 2.0f,
                    (triangles[j][0].vector.y + triangles[j][1].vector.y) / 2.0f,
                    (triangles[j][0].vector.z + triangles[j][1].vector.z) / 2.0f,
                    triangles[j][0].color
                },
                (Vertex) {
                    (triangles[j][1].vector.x + triangles[j][2].vector.x) / 2.0f,
                    (triangles[j][1].vector.y + triangles[j][2].vector.y) / 2.0f,
                    (triangles[j][1].vector.z + triangles[j][2].vector.z) / 2.0f,
                    triangles[j][1].color
                },
                (Vertex) {
                    (triangles[j][2].vector.x + triangles[j][0].vector.x) / 2.0f,
                    (triangles[j][2].vector.y + triangles[j][0].vector.y) / 2.0f,
                    (triangles[j][2].vector.z + triangles[j][0].vector.z) / 2.0f,
                    triangles[j][2].color
                }
            };

            // randomly raise or lower the y coordinate of the midpoints
            // multiplying by 2^-i makes each iteration's changes more granular
            for (int k = 0; k < 3; k++)
            {
                midpoints[k].vector.y += (((float)rand() / (float)RAND_MAX) - 0.25f) * powf(2.0f, (float)-i);
            }

            // build new set of triangles
            Vertex *newTriangleOne = calloc(3, sizeof(Vertex));     // A, AB, AC
            newTriangleOne[0] = triangles[j][0];
            newTriangleOne[1] = midpoints[0];
            newTriangleOne[2] = midpoints[2];

            Vertex *newTriangleTwo = calloc(3, sizeof(Vertex));     // B, AB, BC
            newTriangleTwo[0] = triangles[j][1];
            newTriangleTwo[1] = midpoints[0];
            newTriangleTwo[2] = midpoints[1];

            Vertex *newTriangleThree = calloc(3, sizeof(Vertex));   // C, BC, CA
            newTriangleThree[0] = triangles[j][2];
            newTriangleThree[1] = midpoints[1];
            newTriangleThree[2] = midpoints[2];

            Vertex *newTriangleFour = calloc(3, sizeof(Vertex));    // AB, BC, CA
            newTriangleFour[0] = midpoints[0];
            newTriangleFour[1] = midpoints[1];
            newTriangleFour[2] = midpoints[2];

            newTriangles[newTrianglesAdded]   = newTriangleOne;
            newTriangles[newTrianglesAdded+1] = newTriangleTwo;
            newTriangles[newTrianglesAdded+2] = newTriangleThree;
            newTriangles[newTrianglesAdded+3] = newTriangleFour;
            newTrianglesAdded += 4;
        }

        triCount *= 4;
        triangles = realloc(triangles, triCount * sizeof (Vertex *));
        for (int j = 0; j < triCount; j++)
        {
            triangles[j] = newTriangles[j];
        }
    }

    for (int i = 0; i < triCount; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            triangles[i][j].vector = (Vector3) {
                triangles[i][j].vector.x * fractal->scale.x,
                triangles[i][j].vector.y * fractal->scale.y,
                triangles[i][j].vector.z * fractal->scale.z
            };
        }
    }

    newMountainMesh->vertices = calloc(3 * triCount, sizeof(Vector3));
    newMountainMesh->vertexColors = calloc(3 * triCount, sizeof(Color));
    newMountainMesh->faces = calloc(triCount, sizeof(Vector3));
    int verticesIn = 0;
    for (int i = 0; i < triCount; i++)
    {
        newMountainMesh->vertices[verticesIn] = triangles[i][0].vector;
        newMountainMesh->vertexColors[verticesIn] = triangles[i][0].color;
        verticesIn++;

        newMountainMesh->vertices[verticesIn] = triangles[i][1].vector;
        newMountainMesh->vertexColors[verticesIn] = triangles[i][1].color;
        verticesIn++;

        newMountainMesh->vertices[verticesIn] = triangles[i][2].vector;
        newMountainMesh->vertexColors[verticesIn] = triangles[i][2].color;
        verticesIn++;
        
        newMountainMesh->faces[i] = (Vector3) { (float)verticesIn-2, (float)verticesIn-1, (float)verticesIn };
    }
    newMountainMesh->numFaces = triCount;
    newMountainMesh->numVertices = 3 * triCount;

    return newMountainMesh;
}

Object *LoadObjectsFromJSON(cJSON *jsonObjects, int *numObjects, cJSON *jsonSetUp)
{
    int objectCount = cJSON_GetArraySize(jsonObjects);
    Object *objects = calloc(objectCount, sizeof(Object));
    *numObjects = objectCount;

    int i = 0;
    cJSON *object = NULL;
    cJSON_ArrayForEach(object, jsonObjects)
    {
        Object *objectPtr = &objects[i++];

        // type-agnostic fields
        cJSON *itemType = cJSON_GetObjectItemCaseSensitive(object, "type");
        if (!itemType) continue;
        const char *type = itemType->valuestring;

        cJSON *itemName = cJSON_GetObjectItemCaseSensitive(object, "name");
        if (!itemName) continue;
        strncpy(objectPtr->name, itemName->valuestring, 64);

        cJSON *itemColor = cJSON_GetObjectItemCaseSensitive(object, "color");
        if (itemColor)
        {
            objectPtr->color = ColorFromString(itemColor->valuestring);
        }
        else
        {
            objectPtr->color = WHITE;
        }

        // type-specific fields
        if (strcmp(type, "CIRCLE") == 0)
        {
            objectPtr->type = OBJ_CIRCLE;

            cJSON *circleDiameter = cJSON_GetObjectItemCaseSensitive(object, "diameter");
            if (!circleDiameter) continue;
            objectPtr->diameter = (float)circleDiameter->valuedouble;
        }
        else if (strcmp(type, "SQUARE") == 0)
        {
            objectPtr->type = OBJ_SQUARE;

            cJSON *squareWidth = cJSON_GetObjectItemCaseSensitive(object, "width");
            if (!squareWidth) continue;
            objectPtr->width = (float)squareWidth->valuedouble;
        }
        else if (strcmp(type, "RECTANGLE") == 0)
        {
            objectPtr->type = OBJ_RECTANGLE;

            cJSON *rectangleWidth = cJSON_GetObjectItemCaseSensitive(object, "width");
            if (!rectangleWidth) continue;
            objectPtr->width = (float)rectangleWidth->valuedouble;

            cJSON *rectangleHeight = cJSON_GetObjectItemCaseSensitive(object, "height");
            if (!rectangleHeight) continue;
            objectPtr->height = (float)rectangleHeight->valuedouble;
        }
        else if (strcmp(type, "OBJ_FILE") == 0)
        {
            objectPtr->type = OBJ_WAVEFRONT_OBJ;

            cJSON *objFilename = cJSON_GetObjectItemCaseSensitive(object, "filename");
            if (!objFilename) continue;
            strncpy(objectPtr->filename, objFilename->valuestring, 63);

            cJSON *objVertexExchange = cJSON_GetObjectItemCaseSensitive(object, "vertex exchange");
            if (!objVertexExchange) continue;
            objectPtr->vertexExchange = GetVertexExchange(objVertexExchange->valuestring);

            cJSON *objJustification = cJSON_GetObjectItemCaseSensitive(object, "justification");
            if (!objJustification) continue;
            objectPtr->justification = GetJustification(objJustification->valuestring);

            cJSON *objScale = cJSON_GetObjectItemCaseSensitive(object, "scale");
            if (!objScale) continue;
            objectPtr->objScale = (float)objScale->valuedouble;

            objectPtr->objMesh = OBJFileToMesh(objectPtr->filename);
        }
        else if (strcmp(type, "FRACTAL") == 0)
        {
            objectPtr->type = OBJ_FRACTAL;

            cJSON *fractalType = cJSON_GetObjectItemCaseSensitive(object, "fractal type");
            if (!fractalType) continue;
            strncpy(objectPtr->fractalType, fractalType->valuestring, 63);

            cJSON *fractalIterations = cJSON_GetObjectItemCaseSensitive(object, "iterations");
            if (!fractalIterations) continue;
            objectPtr->iterations = (int)fractalIterations->valueint;

            cJSON *fractalIsSeeded = cJSON_GetObjectItemCaseSensitive(object, "seed");
            if (!fractalIsSeeded) continue;
            if (strcmp(fractalIsSeeded->valuestring, "random") == 0)
            {
                objectPtr->isSeeded = false;
                objectPtr->seed = (int)rand();
            }
            else
            {
                objectPtr->isSeeded = true;
                objectPtr->seed = (int)fractalIsSeeded->valueint;
            }

            cJSON *fractalScale = cJSON_GetObjectItemCaseSensitive(object, "scale");
            if (fractalScale)
            {
                cJSON *scaleX = cJSON_GetObjectItemCaseSensitive(fractalScale, "x");
                if (!scaleX) continue;
                objectPtr->scale.x = (float)scaleX->valueint;

                cJSON *scaleY = cJSON_GetObjectItemCaseSensitive(fractalScale, "y");
                if (!scaleY) continue;
                objectPtr->scale.y = (float)scaleY->valueint;

                cJSON *scaleZ = cJSON_GetObjectItemCaseSensitive(fractalScale, "z");
                if (!scaleZ) continue;
                objectPtr->scale.z = (float)scaleZ->valueint;
            }
            else
            {
                objectPtr->scale.x = 500.0f;
                objectPtr->scale.y = 500.0f;
                objectPtr->scale.z = 500.0f;
            }

            cJSON *fractalBaseVertex = cJSON_GetObjectItemCaseSensitive(object, "base_vertex");
            if (fractalBaseVertex)
            {
                cJSON *baseVertex = NULL;
                int j = 0;
                cJSON_ArrayForEach(baseVertex, fractalBaseVertex)
                {
                    cJSON *vertexX = cJSON_GetObjectItemCaseSensitive(baseVertex, "x");
                    if (!vertexX) continue;
                    objectPtr->baseVertices[j].vector.x = (float)vertexX->valuedouble;

                    cJSON *vertexY = cJSON_GetObjectItemCaseSensitive(baseVertex, "y");
                    if (!vertexY) continue;
                    objectPtr->baseVertices[j].vector.y = (float)vertexY->valuedouble;

                    cJSON *vertexZ = cJSON_GetObjectItemCaseSensitive(baseVertex, "z");
                    if (!vertexZ) continue;
                    objectPtr->baseVertices[j].vector.z = (float)vertexZ->valuedouble;

                    cJSON *vertexColor = cJSON_GetObjectItemCaseSensitive(baseVertex, "c");
                    if (vertexColor)
                    {
                        objectPtr->baseVertices[j].color = ColorFromString(vertexColor->valuestring);
                    }
                    else
                    {
                        objectPtr->baseVertices[j].color = WHITE;
                    }

                    j++;
                }
            }

            cJSON *fractalBaseIndex = cJSON_GetObjectItemCaseSensitive(object, "base_index");
            if (fractalBaseIndex)
            {
                cJSON *baseIndex = NULL;
                int j = 0;
                cJSON_ArrayForEach(baseIndex, fractalBaseIndex)
                {
                    cJSON *vertex0 = cJSON_GetObjectItemCaseSensitive(baseIndex, "0");
                    if (!vertex0) continue;
                    objectPtr->baseIndex[j].x = (float)vertex0->valueint;

                    cJSON *vertex1 = cJSON_GetObjectItemCaseSensitive(baseIndex, "1");
                    if (!vertex1) continue;
                    objectPtr->baseIndex[j].y = (float)vertex1->valueint;

                    cJSON *vertex2 = cJSON_GetObjectItemCaseSensitive(baseIndex, "2");
                    if (!vertex2) continue;
                    objectPtr->baseIndex[j].z = (float)vertex2->valueint;

                    j++;
                }
            }

            Object *tmp = objectPtr;
            MeshObject *mountainMesh = BuildFractalMountain(tmp);
            if (mountainMesh) objectPtr->fractalMesh = mountainMesh;
        }
        else if (strcmp(type, "TEXT") == 0)
        {
            objectPtr->type = OBJ_TEXT;

            cJSON *textFontSize = cJSON_GetObjectItemCaseSensitive(object, "fontsize");
            if (!textFontSize) continue;
            objectPtr->fontsize = textFontSize->valueint;

            cJSON *textFontName = cJSON_GetObjectItemCaseSensitive(object, "fontname");
            if (!textFontName) continue;
            strncpy(objectPtr->fontname, textFontName->valuestring, 63);

            cJSON *textFontFamily = cJSON_GetObjectItemCaseSensitive(object, "fontfamily");
            if (!textFontFamily) continue;
            strncpy(objectPtr->fontfamily, textFontFamily->valuestring, 63);

            cJSON *textText = cJSON_GetObjectItemCaseSensitive(object, "text");
            if (!textText) continue;
            objectPtr->textLineCount = cJSON_GetArraySize(textText);
            objectPtr->textLines = calloc(objectPtr->textLineCount, sizeof(char *));
            for (int j = 0; j < objectPtr->textLineCount; j++)
            {
                cJSON *linePtr = cJSON_GetArrayItem(textText, j);
                if (!linePtr) continue;
                const char *line = linePtr->valuestring;
                objectPtr->textLines[j] = malloc(strlen(line) + 1);
                strcpy(objectPtr->textLines[j], line);
            }
        }
    }

    // read in the JSON's "Set Up" array
    cJSON *setupItem = NULL;
    cJSON_ArrayForEach(setupItem, jsonSetUp)
    {
        cJSON *setupName = cJSON_GetObjectItemCaseSensitive(setupItem, "name");
        if (!setupName) continue;
        const char *name = setupName->valuestring;

        Object *objectPtr = NULL;
        for (int j = 0; j < *numObjects; j++)
        {
            if (strcmp(objects[j].name, name) == 0)
            {
                objectPtr = &objects[j];
                break;
            }
        }
        if (!objectPtr) continue;

        cJSON *moveTo = cJSON_GetObjectItemCaseSensitive(setupItem, "moveto");
        if (moveTo)
        {
            objectPtr->position.x = (float)cJSON_GetObjectItemCaseSensitive(moveTo, "x")->valuedouble;
            objectPtr->position.y = (float)cJSON_GetObjectItemCaseSensitive(moveTo, "y")->valuedouble;
            objectPtr->position.z = (float)cJSON_GetObjectItemCaseSensitive(moveTo, "z")->valuedouble;
        }

        cJSON *rotateTo = cJSON_GetObjectItemCaseSensitive(setupItem, "rotateto");
        if (rotateTo)
        {
            objectPtr->rotation.pitch = (float)cJSON_GetObjectItemCaseSensitive(rotateTo, "pitch")->valuedouble * toRadians;
            objectPtr->rotation.yaw = (float)cJSON_GetObjectItemCaseSensitive(rotateTo, "yaw")->valuedouble * toRadians;
            objectPtr->rotation.roll = (float)cJSON_GetObjectItemCaseSensitive(rotateTo, "roll")->valuedouble * toRadians;
        }
    }

    return objects;
}

cameraWithName *LoadCamerasFromJSON(cJSON *jsonCameras, int *numCameras)
{
    int cameraCount = cJSON_GetArraySize(jsonCameras);
    cameraWithName *cameras = calloc(cameraCount, sizeof(cameraWithName));
    *numCameras = cameraCount;

    int i = 0;
    cJSON *camera = NULL;
    cJSON_ArrayForEach(camera, jsonCameras)
    {
        cameraWithName *cameraPtr = &cameras[i++];

        cJSON *cameraName = cJSON_GetObjectItemCaseSensitive(camera, "name");
        if (!cameraName) continue;
        strncpy(cameraPtr->name, cameraName->valuestring, 63);

        cJSON *cameraPosition = cJSON_GetObjectItemCaseSensitive(camera, "position");
        if (cameraPosition)
        {
            cameraPtr->camera.position.x = (float)cJSON_GetObjectItemCaseSensitive(cameraPosition, "x")->valuedouble;
            cameraPtr->camera.position.y = (float)cJSON_GetObjectItemCaseSensitive(cameraPosition, "y")->valuedouble;
            cameraPtr->camera.position.z = (float)cJSON_GetObjectItemCaseSensitive(cameraPosition, "z")->valuedouble;
        }

        cJSON *cameraLookAt = cJSON_GetObjectItemCaseSensitive(camera, "lookat");
        if (cameraLookAt)
        {
            cameraPtr->camera.target.x = (float)cJSON_GetObjectItemCaseSensitive(cameraLookAt, "x")->valuedouble;
            cameraPtr->camera.target.y = (float)cJSON_GetObjectItemCaseSensitive(cameraLookAt, "y")->valuedouble;
            cameraPtr->camera.target.z = (float)cJSON_GetObjectItemCaseSensitive(cameraLookAt, "z")->valuedouble;
        }

        cJSON *cameraFov = cJSON_GetObjectItemCaseSensitive(camera, "fov");
        if (!cameraFov) continue;
        cameraPtr->camera.fovy = (float)cameraFov->valuedouble;

        cameraPtr->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };

        // render pipeline doesn't use the built-in `projection` value from Camera3D, but I'm assigning it anyway just in case; this is C after all
        cameraPtr->camera.projection = CAMERA_PERSPECTIVE;
    }

    return cameras;
}

void *LoadSceneFromJSON(const char *filename, Object **objects, int *numObjects, cameraWithName **cameras, int *numCameras)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("File read error!\n");
        *numObjects = 0;
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);
    char *buffer = malloc(length + 1);
    fread(buffer, 1, length, file);
    fclose(file);
    buffer[length] = '\0';

    cJSON *jsonRoot = cJSON_Parse(buffer);
    if (jsonRoot == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(jsonRoot);
        *numObjects = 0;
        return NULL;
    }
    cJSON *jsonCameras = cJSON_GetObjectItemCaseSensitive(jsonRoot, "Cameras");
    cJSON *jsonObjects = cJSON_GetObjectItemCaseSensitive(jsonRoot, "Objects");
    cJSON *jsonSetUp = cJSON_GetObjectItemCaseSensitive(jsonRoot, "Set Up");

    *objects = LoadObjectsFromJSON(jsonObjects, numObjects, jsonSetUp);
    *cameras = LoadCamerasFromJSON(jsonCameras, numCameras);

    free(buffer);
    cJSON_Delete(jsonRoot);
}