@echo off
echo.
echo Building Scripting Language Examples (Windows)
echo ============================================

mkdir bin 2>nul

echo Building 01_tokenizer...
gcc -o bin/01_tokenizer.exe 01_tokenizer.c -Wall
if %errorlevel% neq 0 goto error

echo Building 02_calculator...
gcc -o bin/02_calculator.exe 02_calculator.c -Wall
if %errorlevel% neq 0 goto error

echo Building 03_variables...
gcc -o bin/03_variables.exe 03_variables.c -Wall
if %errorlevel% neq 0 goto error

echo Building 04_control_flow...
gcc -o bin/04_control_flow.exe 04_control_flow.c -Wall
if %errorlevel% neq 0 goto error

echo.
echo ============================================
echo All examples built successfully!
echo.
echo Executables in bin/ directory:
dir /b bin\*.exe
echo.
echo Try the calculator: bin\02_calculator.exe
echo Try variables: bin\03_variables.exe
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
