@echo off
echo Building Game Programming Examples (Windows)
echo ============================================
echo.

if not exist bin mkdir bin

set RENDERER=..\..\Renderer\src\renderer.c
set FLAGS=-I..\..\Renderer\src

echo Building snake...
gcc snake.c %RENDERER% %FLAGS% -o bin\snake.exe -lgdi32
if %ERRORLEVEL% NEQ 0 goto error

echo Building pong...
gcc pong.c %RENDERER% %FLAGS% -o bin\pong.exe -lgdi32
if %ERRORLEVEL% NEQ 0 goto error

echo Building breakout...
gcc breakout.c %RENDERER% %FLAGS% -o bin\breakout.exe -lgdi32
if %ERRORLEVEL% NEQ 0 goto error

echo.
echo All games built successfully!
echo Run them from bin\
echo Example: bin\snake.exe
pause
exit /b 0

:error
echo.
echo Build failed!
pause
exit /b 1
