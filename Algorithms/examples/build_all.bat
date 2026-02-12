@echo off
echo Building C Algorithm Examples (Windows)
echo ========================================
echo.

if not exist bin mkdir bin

echo Building 01_bubble_sort...
gcc 01_bubble_sort.c -o bin\01_bubble_sort.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 02_selection_sort...
gcc 02_selection_sort.c -o bin\02_selection_sort.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 03_insertion_sort...
gcc 03_insertion_sort.c -o bin\03_insertion_sort.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 04_merge_sort...
gcc 04_merge_sort.c -o bin\04_merge_sort.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 05_quick_sort...
gcc 05_quick_sort.c -o bin\05_quick_sort.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 06_binary_search...
gcc 06_binary_search.c -o bin\06_binary_search.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 07_recursion...
gcc 07_recursion.c -o bin\07_recursion.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 08_backtracking...
gcc 08_backtracking.c -o bin\08_backtracking.exe
if %ERRORLEVEL% NEQ 0 goto error

echo.
echo All examples built successfully!
echo Run them from bin\
echo Example: bin\01_bubble_sort.exe
pause
exit /b 0

:error
echo.
echo Build failed!
pause
exit /b 1
