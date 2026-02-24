# raylib Computer Graphics Sandbox
In-progress computer graphics sandbox developed for learning purposes and for use in CSC 4750 Computer Graphics at Tennessee Technological University
## Loading a Scene
To load a scene, create a JSON file in the directory root and ensure it is named `scene.json`. The scene's cameras, objects, setup positions, templtes (WIP), lights (WIP), and actions (WIP) will be automatically loaded into the app at launch. In the future, a function in the app will be able to update which JSON file you want to load from at any time.
## Troubleshooting
- If the application window is too big or too small, edit the `InitWindow()` call at the top of `main.c`'s `main` function.
- N.B. At low framerates (around 1-10 frames per second), the application is slow to update both its drawing logic *and* its UI logic. If the app seems unresponsive, try repeatedly clicking a higher position on the framerate slider.
## Known Issues
- Vertices in certain positions (off screen or otherwise) will be rendered, leading to incorrect drawing behaviour. 
    - WIP Fix: clamp the values of `w` such that objects off-screen are not drawn
    - OR implement proper near plane clipping, which does not discard objects only partially offscreen (Sutherlan-Hodgman algorithm)