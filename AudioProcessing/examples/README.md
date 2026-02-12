# Audio Processing Examples

## Building

### Windows
```bash
build_all.bat
```

### Linux / macOS
```bash
chmod +x build_all.sh
./build_all.sh
```

## Running the Examples

**Important:** Always run the examples from the `AudioProcessing/examples` directory (not from the `bin` directory) so relative paths work correctly.

```bash
# Navigate to the examples directory first
cd AudioProcessing/examples

# Then run any example
bin\01_wav_reader.exe
```

### 01 - WAV File Reader

Analyzes WAV files and displays detailed information about format, sample rate, bit depth, and audio content.

```bash
# Read the included example file
bin\01_wav_reader.exe

# Or use the batch file (Windows)
run_wav_reader.bat

# Or analyze your own WAV file
bin\01_wav_reader.exe path\to\your\file.wav
```

**What it shows:**
- File format and size
- Sample rate, channels, bit depth
- Duration and number of samples
- Peak amplitude and clipping detection
- Average amplitude

### 02 - WAV File Writer

Generates various waveforms and saves them as WAV files.

```bash
bin\02_wav_writer.exe
```

**Creates:**
- Sine, square, sawtooth, triangle waves at 440 Hz
- C major chord
- Simple melody (Twinkle Twinkle)
- Stereo panning demonstration

### 03 - Audio Effects

Applies various audio effects to a test tone.

```bash
bin\03_audio_effects.exe
```

**Demonstrates:**
- Volume control (boost/reduction)
- Fade in/out
- Echo/delay
- Distortion
- Low-pass and high-pass filters
- Tremolo
- Normalization

### 04 - Frequency Analysis

Performs FFT analysis to show frequency content of audio signals.

```bash
bin\04_frequency_analysis.exe
```

**Analyzes:**
- Pure sine waves
- Chords (multiple frequencies)
- Square waves (with harmonics)
- Complex waveforms

Shows spectrum visualization and dominant frequencies.

### 05 - Simple Synthesizer

Generates synthesized music using oscillators and ADSR envelopes.

```bash
bin\05_simple_synth.exe
```

**Creates:**
- Different waveform types
- C major scale
- Simple melody
- Chord progressions

### 06 - Audio Mixer

Mixes multiple audio tracks into a stereo output.

```bash
bin\06_audio_mixer.exe
```

**Demonstrates:**
- Multi-track mixing
- Per-track volume control
- Stereo panning
- Drum beat creation (kick, snare, hi-hat)
- Bass line synthesis
- Audio normalization

## Example Audio File

The `assets/example-audio.wav` file is included for testing the WAV reader. You can also use your own WAV files.

## Output Files

Most examples create WAV files in the current directory. You can play these files with any audio player (Windows Media Player, VLC, foobar2000, etc.) to hear the results.

## Learning Path

1. Start with `01_wav_reader` and `02_wav_writer` to understand WAV file format
2. Try `03_audio_effects` to learn audio processing techniques
3. Explore `04_frequency_analysis` to understand FFT and frequency domain
4. Experiment with `05_simple_synth` to learn synthesis
5. Finally, try `06_audio_mixer` to combine everything

## Troubleshooting

**"Cannot find file"**
- Make sure you're running from the `examples` directory
- Or provide full path to the file

**"No sound"**
- Generated WAV files are standard format
- Try opening with Windows Media Player or VLC
- Check your audio output settings

**Compilation errors**
- Make sure you have GCC installed
- On Windows: Install MinGW or MSYS2
- On Linux: Install gcc and make (`sudo apt install build-essential`)
