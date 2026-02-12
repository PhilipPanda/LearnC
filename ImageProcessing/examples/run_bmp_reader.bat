@echo off
REM This script runs the BMP reader from the correct directory

cd /d "%~dp0"
echo Running BMP reader on example image...
echo.

REM Run from the examples directory (not bin)
bin\01_bmp_reader.exe

echo.
echo Done!
