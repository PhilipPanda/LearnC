@echo off
echo.
echo Building Multithreading Examples (Windows)
echo ============================================

mkdir -Force bin 2>nul

echo Building 01_basic_threads...
gcc -o bin/01_basic_threads.exe 01_basic_threads.c -Wall
if %errorlevel% neq 0 goto error

echo Building 02_race_condition...
gcc -o bin/02_race_condition.exe 02_race_condition.c -Wall
if %errorlevel% neq 0 goto error

echo Building 03_mutex...
gcc -o bin/03_mutex.exe 03_mutex.c -Wall
if %errorlevel% neq 0 goto error

echo Building 04_producer_consumer...
gcc -o bin/04_producer_consumer.exe 04_producer_consumer.c -Wall
if %errorlevel% neq 0 goto error

echo Building 05_deadlock...
gcc -o bin/05_deadlock.exe 05_deadlock.c -Wall
if %errorlevel% neq 0 goto error

echo.
echo ============================================
echo All examples built successfully!
echo.
echo Executables in bin/ directory:
dir /b bin\*.exe
echo.
echo Run any example to see threading in action.
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
