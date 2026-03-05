CC = C:/raylib/w64devkit/bin/gcc.exe

build:
	$(CC) main.c graphics_object.c graphics_math.c render.c loader.c cJSON.c -g -o GraphicsSandbox.exe -I. -L. -lraylib -lopengl32 -lgdi32 -lwinmm