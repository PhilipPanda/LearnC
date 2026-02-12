# Audio Processing

Learn audio processing fundamentals in C. WAV file handling, audio effects, frequency analysis, synthesis, and mixing - everything you need to work with digital audio.

## What you get

- WAV file reading and writing
- Audio effects (volume, pan, delay, reverb, distortion)
- Frequency analysis with FFT
- Waveform synthesis (sine, square, triangle, sawtooth)
- Multi-track mixing
- Audio buffer management
- Real-time audio concepts

All examples work with standard WAV files. No external audio libraries - pure C implementation.

## Building

```bash
# Windows
cd examples
build_all.bat

# Linux / MSYS2
cd examples
./build_all.sh
```

## Documentation

- **[Audio Basics](docs/AUDIO_BASICS.md)** - Sample rates, bit depth, channels, PCM audio
- **[WAV Format](docs/WAV_FORMAT.md)** - Understanding and parsing WAV file structure
- **[Audio Effects](docs/AUDIO_EFFECTS.md)** - Implementing common audio effects
- **[FFT Basics](docs/FFT_BASICS.md)** - Frequency analysis and spectral processing
- **[Synthesizer](docs/SYNTHESIZER.md)** - Generating waveforms and tones

## Examples

| Example | What It Teaches |
|---------|----------------|
| 01_wav_reader | Reading WAV files, parsing headers, extracting PCM data |
| 02_wav_writer | Creating WAV files, writing headers, sample data |
| 03_audio_effects | Volume control, panning, delay, reverb, distortion |
| 04_frequency_analysis | FFT implementation, spectrum analysis, visualizing frequencies |
| 05_simple_synth | Generating sine, square, triangle, sawtooth waves |
| 06_audio_mixer | Mixing multiple audio tracks, normalization |

An example WAV file is included in `examples/assets/example-audio.wav` for testing.

Start with 01 and 02 to understand WAV format, then explore effects and synthesis.

## What this teaches

- Digital audio fundamentals (sampling, quantization)
- WAV file format and RIFF structure
- Audio buffer management
- DSP basics (filters, effects, convolution)
- Frequency domain analysis (FFT)
- Waveform synthesis
- Real-time audio processing concepts
- Floating-point vs integer audio

After this, you'll understand how audio software works and can build audio tools, effects processors, synthesizers, and analyzers.

## Quick Start

```bash
cd examples
build_all.bat

# Read and analyze a WAV file
bin\01_wav_reader.exe

# Generate a tone
bin\05_simple_synth.exe

# Apply effects
bin\03_audio_effects.exe
```

Digital audio is everywhere - games, music software, voice processing. Master these fundamentals and you can build anything from a simple tone generator to a complete DAW.
