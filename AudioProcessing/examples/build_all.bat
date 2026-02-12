@echo off
echo.
echo Building Audio Processing Examples (Windows)
echo ============================================

mkdir bin 2>nul

echo Building 01_wav_reader...
gcc -o bin/01_wav_reader.exe 01_wav_reader.c -Wall -lm
if %errorlevel% neq 0 goto error

echo Building 02_wav_writer...
gcc -o bin/02_wav_writer.exe 02_wav_writer.c -Wall -lm
if %errorlevel% neq 0 goto error

echo Building 03_audio_effects...
gcc -o bin/03_audio_effects.exe 03_audio_effects.c -Wall -lm
if %errorlevel% neq 0 goto error

echo Building 04_frequency_analysis...
gcc -o bin/04_frequency_analysis.exe 04_frequency_analysis.c -Wall -lm
if %errorlevel% neq 0 goto error

echo Building 05_simple_synth...
gcc -o bin/05_simple_synth.exe 05_simple_synth.c -Wall -lm
if %errorlevel% neq 0 goto error

echo Building 06_audio_mixer...
gcc -o bin/06_audio_mixer.exe 06_audio_mixer.c -Wall -lm
if %errorlevel% neq 0 goto error

echo.
echo ============================================
echo All examples built successfully!
echo.
echo Executables in bin/ directory:
dir /b bin\*.exe
echo.
echo Try them:
echo   bin\01_wav_reader.exe
echo   bin\02_wav_writer.exe
echo   bin\03_audio_effects.exe
echo   bin\04_frequency_analysis.exe
echo   bin\05_simple_synth.exe
echo   bin\06_audio_mixer.exe
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
