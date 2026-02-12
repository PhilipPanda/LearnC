@echo off
echo.
echo Building Text Editor Examples (Windows)
echo ============================================

mkdir bin 2>nul

echo Building 01_simple_editor...
gcc -o bin/01_simple_editor.exe 01_simple_editor.c -Wall
if %errorlevel% neq 0 goto error

echo.
echo ============================================
echo All examples built successfully!
echo.
echo Executables in bin/ directory:
dir /b bin\*.exe
echo.
echo Try it:
echo   bin\01_simple_editor.exe test.txt
echo.
echo Controls:
echo   Arrow keys - Move cursor
echo   Backspace  - Delete
echo   Enter      - New line
echo   Ctrl+S     - Save
echo   Ctrl+Q     - Quit
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
