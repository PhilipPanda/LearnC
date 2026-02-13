@echo off
REM Build all cryptography examples (Windows)

echo ========================================
echo Building Cryptography Examples
echo ========================================
echo.

REM Create bin directory
if not exist "bin" mkdir bin

REM Build each example
echo Building 01_hash_functions...
gcc -o bin\01_hash_functions.exe 01_hash_functions.c -lm
if %errorlevel% neq 0 (
    echo ERROR: Failed to build 01_hash_functions
    pause
    exit /b 1
)

echo Building 02_symmetric_crypto...
gcc -o bin\02_symmetric_crypto.exe 02_symmetric_crypto.c -lm
if %errorlevel% neq 0 (
    echo ERROR: Failed to build 02_symmetric_crypto
    pause
    exit /b 1
)

echo Building 03_password_hashing...
gcc -o bin\03_password_hashing.exe 03_password_hashing.c -lm
if %errorlevel% neq 0 (
    echo ERROR: Failed to build 03_password_hashing
    pause
    exit /b 1
)

echo Building 04_base64...
gcc -o bin\04_base64.exe 04_base64.c -lm
if %errorlevel% neq 0 (
    echo ERROR: Failed to build 04_base64
    pause
    exit /b 1
)

echo Building 05_rsa_basics...
gcc -o bin\05_rsa_basics.exe 05_rsa_basics.c -lm
if %errorlevel% neq 0 (
    echo ERROR: Failed to build 05_rsa_basics
    pause
    exit /b 1
)

echo Building 06_file_encryption...
gcc -o bin\06_file_encryption.exe 06_file_encryption.c -lm
if %errorlevel% neq 0 (
    echo ERROR: Failed to build 06_file_encryption
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build Complete!
echo ========================================
echo.
echo All examples built successfully in bin\ directory:
echo   - 01_hash_functions.exe
echo   - 02_symmetric_crypto.exe
echo   - 03_password_hashing.exe
echo   - 04_base64.exe
echo   - 05_rsa_basics.exe
echo   - 06_file_encryption.exe
echo.
echo To run examples:
echo   bin\01_hash_functions.exe
echo   bin\02_symmetric_crypto.exe
echo   ... etc
echo.
pause
