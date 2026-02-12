@echo off
echo.
echo Building File I/O Examples (Windows)
echo ============================================

mkdir bin 2>nul

echo Building 01_text_basics...
gcc -o bin/01_text_basics.exe 01_text_basics.c -Wall
if %errorlevel% neq 0 goto error

echo Building 02_word_count...
gcc -o bin/02_word_count.exe 02_word_count.c -Wall
if %errorlevel% neq 0 goto error

echo Building 03_binary_data...
gcc -o bin/03_binary_data.exe 03_binary_data.c -Wall
if %errorlevel% neq 0 goto error

echo.
echo ============================================
echo All examples built successfully!
echo.
echo Executables in bin/ directory:
dir /b bin\*.exe
echo.
echo Try them:
echo   bin\01_text_basics.exe
echo   bin\02_word_count.exe
echo   bin\03_binary_data.exe
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
