@echo off
echo Building C Basics Examples
echo ==========================

if not exist bin mkdir bin

echo.
echo Building examples...
for %%f in (*.c) do (
    echo   %%f
    gcc -Wall -Wextra -std=c99 %%f -o bin\%%~nf.exe
)

echo.
echo Done! Run examples from bin/
echo Example: bin\01_hello_world.exe
pause
