@echo off
echo Building Memory Management Examples (Windows)
echo ============================================
echo.

if not exist bin mkdir bin

echo Building 01_memory_bugs...
gcc 01_memory_bugs.c -o bin\01_memory_bugs.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 02_arena_allocator...
gcc 02_arena_allocator.c -o bin\02_arena_allocator.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 03_memory_pool...
gcc 03_memory_pool.c -o bin\03_memory_pool.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 04_stack_allocator...
gcc 04_stack_allocator.c -o bin\04_stack_allocator.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 05_tracking_allocator...
gcc 05_tracking_allocator.c -o bin\05_tracking_allocator.exe
if %ERRORLEVEL% NEQ 0 goto error

echo.
echo All examples built successfully!
echo Run them from bin\
echo Example: bin\02_arena_allocator.exe
pause
exit /b 0

:error
echo.
echo Build failed!
pause
exit /b 1
