@echo off
echo.
echo Building Image Processing Examples (Windows)
echo ============================================

mkdir bin 2>nul

echo Building 01_bmp_reader...
gcc -o bin/01_bmp_reader.exe 01_bmp_reader.c -Wall -lm
if %errorlevel% neq 0 goto error

echo Building 02_bmp_writer...
gcc -o bin/02_bmp_writer.exe 02_bmp_writer.c -Wall -lm
if %errorlevel% neq 0 goto error

echo Building 03_image_filters...
gcc -o bin/03_image_filters.exe 03_image_filters.c -Wall -lm
if %errorlevel% neq 0 goto error

echo Building 04_color_operations...
gcc -o bin/04_color_operations.exe 04_color_operations.c -Wall -lm
if %errorlevel% neq 0 goto error

echo Building 05_transformations...
gcc -o bin/05_transformations.exe 05_transformations.c -Wall -lm
if %errorlevel% neq 0 goto error

echo Building 06_image_effects...
gcc -o bin/06_image_effects.exe 06_image_effects.c -Wall -lm
if %errorlevel% neq 0 goto error

echo.
echo ============================================
echo All examples built successfully!
echo.
echo Executables in bin/ directory:
dir /b bin\*.exe
echo.
echo Try them:
echo   bin\01_bmp_reader.exe
echo   bin\02_bmp_writer.exe
echo   bin\03_image_filters.exe
echo   bin\04_color_operations.exe
echo   bin\05_transformations.exe
echo   bin\06_image_effects.exe
echo.
pause
goto end

:error
echo.
echo Build failed!
echo.
pause
exit /b 1

:end
