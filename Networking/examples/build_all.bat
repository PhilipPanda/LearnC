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

echo Building 04_chat_server...
gcc 04_chat_server.c -o bin\04_chat_server.exe -lws2_32
if %ERRORLEVEL% NEQ 0 goto error

echo Building 05_udp_sender...
gcc 05_udp_sender.c -o bin\05_udp_sender.exe -lws2_32
if %ERRORLEVEL% NEQ 0 goto error

echo Building 06_udp_receiver...
gcc 06_udp_receiver.c -o bin\06_udp_receiver.exe -lws2_32
if %ERRORLEVEL% NEQ 0 goto error

echo Building 07_http_client...
gcc 07_http_client.c -o bin\07_http_client.exe -lws2_32
if %ERRORLEVEL% NEQ 0 goto error

echo Building 08_file_transfer...
gcc 08_file_transfer.c -o bin\08_file_transfer.exe -lws2_32
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
