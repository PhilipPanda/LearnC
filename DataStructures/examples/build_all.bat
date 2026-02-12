@echo off
echo Building C Data Structure Examples (Windows)
echo ============================================
echo.

if not exist bin mkdir bin

echo Building 01_linked_list...
gcc 01_linked_list.c -o bin\01_linked_list.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 02_doubly_linked_list...
gcc 02_doubly_linked_list.c -o bin\02_doubly_linked_list.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 03_stack...
gcc 03_stack.c -o bin\03_stack.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 04_queue...
gcc 04_queue.c -o bin\04_queue.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 05_binary_tree...
gcc 05_binary_tree.c -o bin\05_binary_tree.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 06_hash_table...
gcc 06_hash_table.c -o bin\06_hash_table.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 07_graph...
gcc 07_graph.c -o bin\07_graph.exe
if %ERRORLEVEL% NEQ 0 goto error

echo Building 08_heap...
gcc 08_heap.c -o bin\08_heap.exe
if %ERRORLEVEL% NEQ 0 goto error

echo.
echo All examples built successfully!
echo Run them from bin\
echo Example: bin\01_linked_list.exe
pause
exit /b 0

:error
echo.
echo Build failed!
pause
exit /b 1
