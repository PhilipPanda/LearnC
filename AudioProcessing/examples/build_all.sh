#!/bin/bash

echo ""
echo "Building Audio Processing Examples (Linux)"
echo "============================================"

mkdir -p bin

echo "Building 01_wav_reader..."
gcc -o bin/01_wav_reader 01_wav_reader.c -Wall -lm || exit 1

echo "Building 02_wav_writer..."
gcc -o bin/02_wav_writer 02_wav_writer.c -Wall -lm || exit 1

echo "Building 03_audio_effects..."
gcc -o bin/03_audio_effects 03_audio_effects.c -Wall -lm || exit 1

echo "Building 04_frequency_analysis..."
gcc -o bin/04_frequency_analysis 04_frequency_analysis.c -Wall -lm || exit 1

echo "Building 05_simple_synth..."
gcc -o bin/05_simple_synth 05_simple_synth.c -Wall -lm || exit 1

echo "Building 06_audio_mixer..."
gcc -o bin/06_audio_mixer 06_audio_mixer.c -Wall -lm || exit 1

echo ""
echo "============================================"
echo "All examples built successfully!"
echo ""
echo "Executables in bin/ directory:"
ls -1 bin/
echo ""
echo "Try them:"
echo "  ./bin/01_wav_reader"
echo "  ./bin/02_wav_writer"
echo "  ./bin/03_audio_effects"
echo "  ./bin/04_frequency_analysis"
echo "  ./bin/05_simple_synth"
echo "  ./bin/06_audio_mixer"
echo ""
