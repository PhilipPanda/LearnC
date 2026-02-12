@echo off
echo Building C Renderer Examples (Windows)
echo ======================================

if not exist bin mkdir bin

echo.
echo Building 01_basic_shapes...
gcc -o bin/01_basic_shapes.exe 01_basic_shapes.c ../src/renderer.c -I ../src -lgdi32 -luser32 -lkernel32 -lm -O2 -Wall

echo Building 02_image_rendering...
gcc -o bin/02_image_rendering.exe 02_image_rendering.c ../src/renderer.c -I ../src -lgdi32 -luser32 -lkernel32 -lm -O2 -Wall

echo Building 03_3d_cube...
gcc -o bin/03_3d_cube.exe 03_3d_cube.c ../src/renderer.c -I ../src -lgdi32 -luser32 -lkernel32 -lm -O2 -Wall

echo Building 04_3d_pyramid...
gcc -o bin/04_3d_pyramid.exe 04_3d_pyramid.c ../src/renderer.c -I ../src -lgdi32 -luser32 -lkernel32 -lm -O2 -Wall

echo Building 05_particles...
gcc -o bin/05_particles.exe 05_particles.c ../src/renderer.c -I ../src -lgdi32 -luser32 -lkernel32 -lm -O2 -Wall

echo Building 06_textured_cube...
gcc -o bin/06_textured_cube.exe 06_textured_cube.c ../src/renderer.c -I ../src -lgdi32 -luser32 -lkernel32 -lm -O2 -Wall

echo Building 07_simple_game...
gcc -o bin/07_simple_game.exe 07_simple_game.c ../src/renderer.c -I ../src -lgdi32 -luser32 -lkernel32 -lm -O2 -Wall

echo.
echo All examples built! Run them from bin/
echo Example: bin\01_basic_shapes.exe
pause
