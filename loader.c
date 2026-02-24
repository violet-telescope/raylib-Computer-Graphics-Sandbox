#include "loader.h"

extern float toRadians;

// possibly a better way to do this, this is just what Claude suggested so I'll use it for now
Color ColorFromString(const char *str)
{
    if (strcmp(str, "DARKMAGENTA") == 0) return (Color){139, 0, 139, 255};
    if (strcmp(str, "YELLOW")      == 0) return YELLOW;
    if (strcmp(str, "GREEN")       == 0) return GREEN;
    if (strcmp(str, "BLUE")        == 0) return BLUE;
    if (strcmp(str, "RED")         == 0) return RED;
    if (strcmp(str, "WHITE")       == 0) return RAYWHITE;
    if (strcmp(str, "BLACK")       == 0) return BLACK;
    // add more as needed
    return MAGENTA; // unless magenta *is* the color, this should make missing colors clear
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
        } else {
            objectPtr->color = WHITE;
        }

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