# raylib Computer Graphics Sandbox
raylib-Computer-Graphics-Sandbox is an in-progress sandbox application for 3D computer graphics developed for learning purposes and for use in CSC 4750 Computer Graphics at Tennessee Technological University written in C using [raylib](https://www.raylib.com/index.html), its [raygui library](https://github.com/raysan5/raygui), and [cJSON](https://github.com/DaveGamble/cJSON).
## Compilation and Running the App
The following instructions are accurate as of February 2026. If something fails, first verify that the instructions are still valid, and if not -- there are plenty of online resources explaining C compilation.
### Windows
Before you can run the app, you need to compile it. To compile, first install [raylib](https://www.raylib.com/index.html) on your system. The raylib Windows installer comes with `w64devkit`, a command prompt with PATHs preloaded for C compilation involving raylib, at `\raylib\w64devkit\` (`C:\raylib\w64devkit\` specifically, if you didn't change the install directory). Launch `w64devkit.exe` and `cd` into this repository's directory, then run the Makefile with `make build`. Finally, run `GraphicsSandbox.exe`. If it fails, ensure that the `CC` variable in the Makefile matches your `gcc.exe` path.
### Linux
If you're running Linux, you probably already know how to compile C code on your machine. If you don't, you should learn, it's pretty easy!
## Loading a Scene
To load a scene, create a JSON file in the directory root and ensure it is named `scene.json`. The scene's cameras, objects, setup positions, templates (WIP), lights (WIP), and actions (WIP) will be automatically loaded into the app at launch. In the future, a function in the app will be able to update which JSON file you want to load from at any time.
## Troubleshooting
- If the application window is too big or too small, edit the `InitWindow()` call at the top of `main.c`'s `main` function.
- N.B. At low framerates (around 1-10 frames per second), the application is slow to update both its drawing logic *and* its UI logic. If the app seems unresponsive, try repeatedly clicking a higher position on the framerate slider.
## Known Issues
- Vertices in certain positions (off screen or otherwise) will be rendered, leading to incorrect drawing behaviour. 
    - WIP Fix: clamp the values of `w` such that objects off-screen are not drawn
    - OR implement proper near plane clipping, which does not discard objects only partially offscreen (Sutherlan-Hodgman algorithm)