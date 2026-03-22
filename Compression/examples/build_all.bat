@echo off
REM Build all compression examples (Windows)

echo ========================================
echo Building Compression Examples
echo ========================================
echo.

REM Create bin directory
if not exist "bin" mkdir bin

REM Build each example
echo Building 01_rle...
gcc -o bin\01_rle.exe 01_rle.c -lm
if %errorlevel% neq 0 (
    echo ERROR: Failed to build 01_rle
    pause
    exit /b 1
)

echo Building 02_huffman...
gcc -o bin\02_huffman.exe 02_huffman.c -lm
if %errorlevel% neq 0 (
    echo ERROR: Failed to build 02_huffman
    pause
    exit /b 1
)

echo Building 03_lz77...
gcc -o bin\03_lz77.exe 03_lz77.c -lm
if %errorlevel% neq 0 (
    echo ERROR: Failed to build 03_lz77
    pause
    exit /b 1
)

echo Building 04_file_compression...
gcc -o bin\04_file_compression.exe 04_file_compression.c -lm
if %errorlevel% neq 0 (
    echo ERROR: Failed to build 04_file_compression
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build Complete!
echo ========================================
echo.
echo All examples built successfully in bin\ directory:
echo   - 01_rle.exe
echo   - 02_huffman.exe
echo   - 03_lz77.exe
echo   - 04_file_compression.exe
echo.
echo To run examples:
echo   bin\01_rle.exe
echo   bin\02_huffman.exe
echo   ... etc
echo.
pause
