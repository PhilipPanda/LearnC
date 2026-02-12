# Audio Basics

## The Big Picture

Digital audio represents sound as a sequence of numbers. Understanding sampling, bit depth, and channels is fundamental to audio processing.

## What is Sound?

Sound is air pressure changing over time. A microphone converts this to voltage. An analog-to-digital converter (ADC) samples the voltage at regular intervals and converts it to numbers.

## Sampling

**Sample Rate** is how many times per second we measure the audio signal.

```
Common sample rates:
- 8000 Hz  - Telephone quality
- 16000 Hz - Voice applications
- 22050 Hz - Low-quality audio
- 44100 Hz - CD quality (most common)
- 48000 Hz - Professional audio, video
- 96000 Hz - High-resolution audio
- 192000 Hz - Ultra high-resolution
```

**Nyquist Theorem:** To capture frequency F, you need sample rate ≥ 2F.

Human hearing: 20 Hz to 20,000 Hz  
CD quality (44100 Hz): Captures up to 22,050 Hz (good enough!)

### Example: 1 second of audio

```c
#define SAMPLE_RATE 44100

float audio_buffer[SAMPLE_RATE];  // 1 second of mono audio

// Each element is one sample
// Index 0 = time 0.0 seconds
// Index 100 = time 100/44100 = 0.00227 seconds
// Index 44099 = time 44099/44100 = 0.9999 seconds
```

## Bit Depth

**Bit Depth** determines how precisely we store each sample value.

```
Common bit depths:
- 8-bit  = 256 levels (-128 to 127)
- 16-bit = 65,536 levels (-32,768 to 32,767)  [CD quality]
- 24-bit = 16,777,216 levels
- 32-bit float = ~4 billion levels (-1.0 to 1.0)
```

**Dynamic Range** = 6 dB per bit
- 8-bit: 48 dB
- 16-bit: 96 dB (CD quality)
- 24-bit: 144 dB (exceeds human hearing)

### In Code

```c
// 8-bit audio (unsigned)
uint8_t sample_8bit = 128;  // 0 = minimum, 255 = maximum, 128 = silence

// 16-bit audio (signed, most common)
int16_t sample_16bit = 0;  // -32768 = minimum, 32767 = maximum, 0 = silence

// 32-bit float (professional, easiest to process)
float sample_float = 0.0f;  // -1.0 = minimum, 1.0 = maximum, 0.0 = silence
```

### Converting Between Formats

```c
// 16-bit to float
int16_t sample_16 = 16384;
float sample_f = (float)sample_16 / 32768.0f;  // Result: 0.5

// Float to 16-bit
float sample_f = 0.75f;
int16_t sample_16 = (int16_t)(sample_f * 32767.0f);  // Result: 24575

// 8-bit unsigned to float
uint8_t sample_8 = 192;
float sample_f = (float)(sample_8 - 128) / 128.0f;  // Result: 0.5
```

## Channels

**Mono:** Single channel  
**Stereo:** Two channels (left and right)  
**Surround:** 5.1 (6 channels), 7.1 (8 channels), etc.

### Stereo Audio in Memory

Two ways to store stereo:

**Interleaved** (most common in files):
```c
int16_t stereo_buffer[10] = {
    L0, R0,  // First frame
    L1, R1,  // Second frame
    L2, R2,  // Third frame
    L3, R3,  // Fourth frame
    L4, R4   // Fifth frame
};

// Access left channel of frame 2:
int16_t left = stereo_buffer[2 * 2 + 0];  // Index 4
// Access right channel of frame 2:
int16_t right = stereo_buffer[2 * 2 + 1]; // Index 5
```

**Planar** (easier to process):
```c
int16_t left_buffer[5] = {L0, L1, L2, L3, L4};
int16_t right_buffer[5] = {R0, R1, R2, R3, R4};

// Access frame 2:
int16_t left = left_buffer[2];
int16_t right = right_buffer[2];
```

### Converting Stereo ↔ Mono

```c
// Stereo to mono (mix both channels)
float stereo_to_mono(float left, float right) {
    return (left + right) * 0.5f;
}

// Mono to stereo (duplicate)
void mono_to_stereo(float mono, float* left, float* right) {
    *left = mono;
    *right = mono;
}
```

## PCM Audio

**PCM (Pulse Code Modulation)** is the raw, uncompressed audio format. Each sample is stored directly.

WAV and AIFF files contain PCM data. MP3 and AAC are compressed (not PCM).

### PCM Audio Structure

```c
typedef struct {
    int sample_rate;      // e.g., 44100 Hz
    int channels;         // 1 = mono, 2 = stereo
    int bits_per_sample;  // 8, 16, 24, or 32
    int num_samples;      // Total samples per channel
    float* data;          // Audio samples (-1.0 to 1.0)
} AudioBuffer;
```

## Time and Samples

Convert between time and sample indices:

```c
// Time to sample
int time_to_sample(float time_seconds, int sample_rate) {
    return (int)(time_seconds * sample_rate);
}

// Sample to time
float sample_to_time(int sample_index, int sample_rate) {
    return (float)sample_index / (float)sample_rate;
}

// Example:
// At 44100 Hz, sample 44100 = 1.0 second
// At 44100 Hz, sample 22050 = 0.5 seconds
```

## Amplitude and Volume

**Amplitude** is the "height" of the waveform. Larger amplitude = louder.

```c
// Change volume (multiply all samples)
void change_volume(float* audio, int num_samples, float volume) {
    // volume = 0.0 (silent)
    // volume = 0.5 (50% volume, -6 dB)
    // volume = 1.0 (unchanged)
    // volume = 2.0 (200% volume, +6 dB, may clip!)
    
    for (int i = 0; i < num_samples; i++) {
        audio[i] *= volume;
    }
}

// Ensure samples don't exceed range (prevent distortion)
void clip_audio(float* audio, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        if (audio[i] > 1.0f) audio[i] = 1.0f;
        if (audio[i] < -1.0f) audio[i] = -1.0f;
    }
}
```

## Decibels (dB)

Audio is measured in decibels (logarithmic scale).

```c
#include <math.h>

// Convert linear gain to dB
float gain_to_db(float gain) {
    return 20.0f * log10f(gain);
}

// Convert dB to linear gain
float db_to_gain(float db) {
    return powf(10.0f, db / 20.0f);
}

// Examples:
// 0 dB = 1.0 (no change)
// -6 dB ≈ 0.5 (half volume)
// -12 dB ≈ 0.25 (quarter volume)
// +6 dB ≈ 2.0 (double volume)
// -∞ dB = 0.0 (silence)
```

## Silence

Silence is **not** absence of data - it's samples with value 0.

```c
// Create 1 second of silence at 44100 Hz
#define SAMPLE_RATE 44100
float silence[SAMPLE_RATE];
memset(silence, 0, sizeof(silence));

// Or:
for (int i = 0; i < SAMPLE_RATE; i++) {
    silence[i] = 0.0f;
}
```

## Generating a Simple Tone

```c
#include <math.h>

#define PI 3.14159265358979323846
#define SAMPLE_RATE 44100

// Generate sine wave at given frequency
void generate_tone(float* buffer, int num_samples, float frequency) {
    for (int i = 0; i < num_samples; i++) {
        float t = (float)i / SAMPLE_RATE;  // Time in seconds
        buffer[i] = sinf(2.0f * PI * frequency * t);
    }
}

// Generate 1 second of 440 Hz (A4 note)
float tone[SAMPLE_RATE];
generate_tone(tone, SAMPLE_RATE, 440.0f);
```

## Reading Audio Frames

For stereo interleaved audio:

```c
void process_stereo_frames(int16_t* interleaved, int num_frames) {
    for (int frame = 0; frame < num_frames; frame++) {
        int16_t left = interleaved[frame * 2 + 0];
        int16_t right = interleaved[frame * 2 + 1];
        
        // Process left and right...
        
        interleaved[frame * 2 + 0] = left;
        interleaved[frame * 2 + 1] = right;
    }
}
```

## Audio Latency

**Latency** = delay between input and output.

Caused by:
- Buffer size (larger buffer = more latency)
- Processing time
- Hardware delays

```c
// Buffer size vs latency
// Sample rate = 48000 Hz, buffer size = 512 samples
// Latency = 512 / 48000 = 0.0107 seconds = 10.7 ms

float calculate_latency_ms(int buffer_size, int sample_rate) {
    return (float)buffer_size / (float)sample_rate * 1000.0f;
}

// Typical values:
// 64 samples @ 48kHz = 1.3 ms (excellent)
// 256 samples @ 48kHz = 5.3 ms (good)
// 512 samples @ 48kHz = 10.7 ms (acceptable)
// 2048 samples @ 48kHz = 42.7 ms (noticeable delay)
```

## Memory Requirements

```c
// Calculate audio buffer size
size_t audio_buffer_size(int sample_rate, int channels, int duration_seconds, int bytes_per_sample) {
    return sample_rate * channels * duration_seconds * bytes_per_sample;
}

// Examples:
// 1 minute of CD quality (44100 Hz, stereo, 16-bit):
// 44100 * 2 * 60 * 2 = 10,584,000 bytes ≈ 10 MB

// 1 minute of 32-bit float (48000 Hz, stereo):
// 48000 * 2 * 60 * 4 = 23,040,000 bytes ≈ 23 MB
```

## Summary

- **Sample Rate:** Samples per second (44100 Hz = CD quality)
- **Bit Depth:** Precision of each sample (16-bit = CD quality)
- **Channels:** Mono (1), Stereo (2), or more
- **PCM:** Raw, uncompressed audio samples
- **Amplitude:** Sample values determine loudness
- **Silence:** Samples with value 0
- **Interleaved:** LRLRLR... (common in files)
- **Planar:** LLL...RRR... (easier to process)

Understanding these basics is essential for all audio processing work. Every audio application - from games to music production to voice calls - uses these concepts.
