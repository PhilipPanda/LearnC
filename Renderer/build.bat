@echo off
if not exist bin mkdir bin
echo Building C Renderer (Windows)...
gcc -o bin/CRenderer.exe src/main.c src/renderer.c -I src -lgdi32 -luser32 -lkernel32 -lm -O2 -Wall
if %ERRORLEVEL% EQU 0 (
    echo Build successful! Run: bin\CRenderer.exe
) else (
    echo Build failed!
)
