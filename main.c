/******************************************************************************
    raylib Computer Graphics Sandbox
    last updated 2026-03-01
    using raylib, raygui, and cJSON

    built for the purpose of understanding computer graphics at a
    fundamental level and to follow along in CSC 4750 Computer
    Graphics at Tennessee Technological University
******************************************************************************/

#include "raylib.h"
#include "graphics_object.h"
#include "render.h"
#include "loader.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

const int CONTROLS_WIDTH = 350; // how wide the GUI section on the right side should be

int main(void)
{
    // Initialize
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(1600, 1200, "raylib sandbox");

    float targetFPS = 60.0f;
    SetTargetFPS(targetFPS);

    Color backgroundColor = BLACK;

    int trueScreenWidth = GetScreenWidth()-CONTROLS_WIDTH;
    bool drawControls = true;
    bool drawGrid = false; // currently unimplemented
    bool drawAxes = false;

    static int objectDropdownActive = 0;
    static bool objectDropdownEditMode = false;
    static int cameraDropdownActive = 0;
    static bool cameraDropdownEditMode = false;

    Object *loadedObjects = NULL;
    cameraWithName *loadedCameras = NULL;
    int numObjects;
    int numCameras;
    LoadSceneFromJSON("scene.json", &loadedObjects, &numObjects, &loadedCameras, &numCameras);

    while (!WindowShouldClose()) // ESC key or Windows [X]
    {
        // Update
        SetTargetFPS(targetFPS);
        int controlsXPosition = (GetScreenWidth()-CONTROLS_WIDTH);
        Object *objects = loadedObjects;
        cameraWithName *cameras = loadedCameras;

        // Draw
        BeginDrawing();
            ClearBackground(backgroundColor); // draw background with color
            RenderScene(objects, numObjects, cameras[cameraDropdownActive].camera, drawAxes);
            DrawText(TextFormat("FPS: %i", GetFPS()), 0, 0, 20, LIGHTGRAY); // FPS display
            GuiCheckBox((Rectangle){ 10, (GetScreenHeight() - 30), 20, 20 }, "Show Controls", &drawControls); // controls panel toggle
            
            static Vector2 scrollOffset = { 0, 0 };
            Rectangle controlsBounds = { controlsXPosition, 0, CONTROLS_WIDTH, GetScreenHeight() };
            Rectangle controlsContentBounds = { controlsXPosition, 0, CONTROLS_WIDTH, 800 };
            Rectangle controlsViewBounds = { 0 };
            if (drawControls) GuiScrollPanel(controlsBounds, "Controls", controlsContentBounds, &scrollOffset, &controlsViewBounds);

            // Draw GUI controls panel
            BeginScissorMode(controlsViewBounds.x, controlsViewBounds.y, controlsViewBounds.width, controlsViewBounds.height);
                if (objectDropdownEditMode || cameraDropdownEditMode) GuiLock();
                GuiSliderBar((Rectangle){(controlsXPosition+80), 50+scrollOffset.y, 200, 20}, "Target FPS", TextFormat("%.0f", targetFPS), &targetFPS, 0, 200);
                DrawText(TextFormat("FPS Mode: %s", (targetFPS == 0.0f)? "Uncapped" : "Capped"), controlsXPosition+80, 80+scrollOffset.y, 20, (targetFPS == 0.0f)? RED : DARKGRAY);
                
                GuiCheckBox((Rectangle){(controlsXPosition+80), 110+scrollOffset.y, 20, 20}, "Draw Grid", &drawGrid);
                GuiCheckBox((Rectangle){(controlsXPosition+160), 110+scrollOffset.y, 20, 20}, "Draw (x, y, z) Axis", &drawAxes);

                GuiColorPicker((Rectangle){(controlsXPosition+80), 150+scrollOffset.y, 100, 100}, "Background Color", &backgroundColor);

                GuiSliderBar((Rectangle){(controlsXPosition+80), 330+scrollOffset.y, 200, 20}, "Object X", TextFormat("%.2f", objects[objectDropdownActive].position.x),
                    &objects[objectDropdownActive].position.x, -2000, 2000);
                GuiSliderBar((Rectangle){(controlsXPosition+80), 360+scrollOffset.y, 200, 20}, "Object Y", TextFormat("%.2f", objects[objectDropdownActive].position.y),
                    &objects[objectDropdownActive].position.y, -2000, 2000);
                GuiSliderBar((Rectangle){(controlsXPosition+80), 390+scrollOffset.y, 200, 20}, "Object Z", TextFormat("%.2f", objects[objectDropdownActive].position.z),
                    &objects[objectDropdownActive].position.z, -2000, 2000);

                GuiSliderBar((Rectangle){(controlsXPosition+80), 420+scrollOffset.y, 200, 20}, "Object Pitch", TextFormat("%.2f", objects[objectDropdownActive].rotation.pitch),
                    &objects[objectDropdownActive].rotation.pitch, -PI, PI);
                GuiSliderBar((Rectangle){(controlsXPosition+80), 450+scrollOffset.y, 200, 20}, "Object Roll",  TextFormat("%.2f", objects[objectDropdownActive].rotation.roll),
                    &objects[objectDropdownActive].rotation.roll, -PI, PI);
                GuiSliderBar((Rectangle){(controlsXPosition+80), 480+scrollOffset.y, 200, 20}, "Object Yaw",   TextFormat("%.2f", objects[objectDropdownActive].rotation.yaw),
                    &objects[objectDropdownActive].rotation.yaw, -PI, PI);

                GuiSliderBar((Rectangle){(controlsXPosition+80), 570+scrollOffset.y, 200, 20}, "Camera X", TextFormat("%.2f", cameras[cameraDropdownActive].camera.position.x),
                    &cameras[cameraDropdownActive].camera.position.x, -2000, 2000);
                GuiSliderBar((Rectangle){(controlsXPosition+80), 600+scrollOffset.y, 200, 20}, "Camera Y", TextFormat("%.2f", cameras[cameraDropdownActive].camera.position.y),
                    &cameras[cameraDropdownActive].camera.position.y, -2000, 2000);
                GuiSliderBar((Rectangle){(controlsXPosition+80), 630+scrollOffset.y, 200, 20}, "Camera Z", TextFormat("%.2f", cameras[cameraDropdownActive].camera.position.z),
                    &cameras[cameraDropdownActive].camera.position.z, -2000, 2000);

                GuiSliderBar((Rectangle){(controlsXPosition+80), 660+scrollOffset.y, 200, 20}, "Target X", TextFormat("%.2f", cameras[cameraDropdownActive].camera.target.x),
                    &cameras[cameraDropdownActive].camera.target.x, -2000, 2000);
                GuiSliderBar((Rectangle){(controlsXPosition+80), 690+scrollOffset.y, 200, 20}, "Target Y", TextFormat("%.2f", cameras[cameraDropdownActive].camera.target.y),
                    &cameras[cameraDropdownActive].camera.target.y, -2000, 2000);
                GuiSliderBar((Rectangle){(controlsXPosition+80), 720+scrollOffset.y, 200, 20}, "Target Z", TextFormat("%.2f", cameras[cameraDropdownActive].camera.target.z),
                    &cameras[cameraDropdownActive].camera.target.z, -2000, 2000);

                GuiUnlock(); // dropdowns must be drawn on top of the UI as to not disturb elements underneath them

                char objectDropdownText[512] = {0};
                for (int i = 0; i < numObjects; i++)
                {
                    strcat(objectDropdownText, objects[i].name);
                    if (i < numObjects - 1)
                    {
                        strcat(objectDropdownText, ";");
                    }
                }
                if (GuiDropdownBox((Rectangle){(controlsXPosition+80), 270+scrollOffset.y, 200, 40}, objectDropdownText, &objectDropdownActive, objectDropdownEditMode))
                {
                    objectDropdownEditMode = !objectDropdownEditMode;
                }

                char cameraDropdownText[512] = {0};
                for (int i = 0; i < numCameras; i++)
                {
                    strcat(cameraDropdownText, cameras[i].name);
                    if (i < numCameras - 1)
                    {
                        strcat(cameraDropdownText, ";");
                    }
                }
                if (GuiDropdownBox((Rectangle){(controlsXPosition+80), 520+scrollOffset.y, 200, 40}, cameraDropdownText, &cameraDropdownActive, cameraDropdownEditMode))
                {
                    cameraDropdownEditMode = !cameraDropdownEditMode;
                }
            EndScissorMode();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}