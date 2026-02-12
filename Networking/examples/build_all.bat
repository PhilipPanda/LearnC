@echo off
echo Building C Networking Examples (Windows)
echo =======================================
echo.

if not exist bin mkdir bin

echo Building 01_simple_server...
gcc 01_simple_server.c -o bin\01_simple_server.exe -lws2_32
if %ERRORLEVEL% NEQ 0 goto error

echo Building 02_simple_client...
gcc 02_simple_client.c -o bin\02_simple_client.exe -lws2_32
if %ERRORLEVEL% NEQ 0 goto error

echo Building 03_echo_server...
gcc 03_echo_server.c -o bin\03_echo_server.exe -lws2_32
if %ERRORLEVEL% NEQ 0 goto error

echo.
echo All examples built successfully!
echo Run them from bin\
echo Example: bin\01_simple_server.exe
pause
exit /b 0

:error
echo.
echo Build failed!
pause
exit /b 1
