@echo off
echo.
echo Building Image Processing Examples (Windows)
echo ============================================

mkdir bin 2>nul

echo Building 01_bmp_reader...
gcc -o bin/01_bmp_reader.exe 01_bmp_reader.c -Wall -lm
if %errorlevel% neq 0 goto error

echo.
echo ============================================
echo All examples built successfully!
echo.
echo Executables in bin/ directory:
dir /b bin\*.exe
echo.
echo Try them:
echo   bin\01_bmp_reader.exe path\to\image.bmp
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
