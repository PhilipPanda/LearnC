# Audio Effects

## The Big Picture

Audio effects transform sound by manipulating samples mathematically. Understanding basic effects teaches you DSP (Digital Signal Processing) fundamentals applicable to all audio work.

All examples use normalized float audio (-1.0 to 1.0).

## Volume Control

The simplest effect - multiply all samples by a gain factor.

```c
void apply_volume(float* audio, int num_samples, float gain) {
    // gain = 0.0 → silence
    // gain = 0.5 → half volume (-6 dB)
    // gain = 1.0 → no change
    // gain = 2.0 → double volume (+6 dB)
    
    for (int i = 0; i < num_samples; i++) {
        audio[i] *= gain;
    }
}

// Prevent clipping
void clip(float* audio, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        if (audio[i] > 1.0f) audio[i] = 1.0f;
        if (audio[i] < -1.0f) audio[i] = -1.0f;
    }
}
```

### Smooth Volume Change (Fade)

```c
void fade_in(float* audio, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        float gain = (float)i / num_samples;  // 0.0 to 1.0
        audio[i] *= gain;
    }
}

void fade_out(float* audio, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        float gain = 1.0f - ((float)i / num_samples);  // 1.0 to 0.0
        audio[i] *= gain;
    }
}

// Crossfade between two audio buffers
void crossfade(float* audio_a, float* audio_b, float* output, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        float t = (float)i / num_samples;  // 0.0 to 1.0
        output[i] = audio_a[i] * (1.0f - t) + audio_b[i] * t;
    }
}
```

## Panning (Stereo)

Position a mono sound in the stereo field.

```c
void apply_pan(float mono_sample, float pan, float* left, float* right) {
    // pan = -1.0 → full left
    // pan =  0.0 → center
    // pan = +1.0 → full right
    
    // Simple linear panning
    *left = mono_sample * (1.0f - pan) * 0.5f;
    *right = mono_sample * (1.0f + pan) * 0.5f;
}

// Better: Constant power panning (preserves perceived loudness)
#include <math.h>
#define PI 3.14159265358979323846

void apply_pan_constant_power(float mono_sample, float pan, float* left, float* right) {
    // pan: -1.0 (left) to +1.0 (right)
    float angle = (pan + 1.0f) * 0.25f * PI;  // 0 to PI/2
    *left = mono_sample * cosf(angle);
    *right = mono_sample * sinf(angle);
}

// Pan entire buffer
void pan_buffer(float* mono, float* left, float* right, int num_samples, float pan) {
    for (int i = 0; i < num_samples; i++) {
        apply_pan_constant_power(mono[i], pan, &left[i], &right[i]);
    }
}
```

## Delay / Echo

Repeat audio after a time delay.

```c
typedef struct {
    float* buffer;       // Delay buffer
    int buffer_size;     // Size in samples
    int write_pos;       // Current write position
} DelayEffect;

DelayEffect* delay_create(int sample_rate, float delay_seconds) {
    DelayEffect* delay = malloc(sizeof(DelayEffect));
    delay->buffer_size = (int)(sample_rate * delay_seconds);
    delay->buffer = calloc(delay->buffer_size, sizeof(float));
    delay->write_pos = 0;
    return delay;
}

void delay_destroy(DelayEffect* delay) {
    free(delay->buffer);
    free(delay);
}

float delay_process(DelayEffect* delay, float input, float feedback, float mix) {
    // Read delayed sample
    float delayed = delay->buffer[delay->write_pos];
    
    // Mix input with delayed (feedback)
    float output = input + delayed * feedback;
    
    // Store in buffer
    delay->buffer[delay->write_pos] = output;
    
    // Advance write position (circular buffer)
    delay->write_pos = (delay->write_pos + 1) % delay->buffer_size;
    
    // Mix dry and wet
    return input * (1.0f - mix) + output * mix;
}

// Process entire buffer
void apply_delay(float* audio, int num_samples, int sample_rate, 
                 float delay_seconds, float feedback, float mix) {
    DelayEffect* delay = delay_create(sample_rate, delay_seconds);
    
    for (int i = 0; i < num_samples; i++) {
        audio[i] = delay_process(delay, audio[i], feedback, mix);
    }
    
    delay_destroy(delay);
}
```

## Reverb (Simple)

Simulates room acoustics using multiple delays.

```c
#define NUM_COMB_FILTERS 4

typedef struct {
    DelayEffect* delays[NUM_COMB_FILTERS];
    int sample_rate;
} ReverbEffect;

ReverbEffect* reverb_create(int sample_rate) {
    ReverbEffect* reverb = malloc(sizeof(ReverbEffect));
    reverb->sample_rate = sample_rate;
    
    // Different delay times create room effect
    float delay_times[NUM_COMB_FILTERS] = {0.0297f, 0.0371f, 0.0411f, 0.0437f};
    
    for (int i = 0; i < NUM_COMB_FILTERS; i++) {
        reverb->delays[i] = delay_create(sample_rate, delay_times[i]);
    }
    
    return reverb;
}

void reverb_destroy(ReverbEffect* reverb) {
    for (int i = 0; i < NUM_COMB_FILTERS; i++) {
        delay_destroy(reverb->delays[i]);
    }
    free(reverb);
}

float reverb_process(ReverbEffect* reverb, float input) {
    float output = 0.0f;
    float feedback = 0.7f;
    
    // Sum multiple delayed versions
    for (int i = 0; i < NUM_COMB_FILTERS; i++) {
        output += delay_process(reverb->delays[i], input, feedback, 1.0f);
    }
    
    // Average and mix with dry signal
    output /= NUM_COMB_FILTERS;
    return input * 0.5f + output * 0.5f;
}
```

## Distortion

Nonlinear waveshaping creates harmonics (distortion).

```c
// Hard clipping
float distortion_clip(float input, float threshold) {
    if (input > threshold) return threshold;
    if (input < -threshold) return -threshold;
    return input;
}

// Soft clipping (smoother)
float distortion_soft_clip(float input) {
    if (input > 1.0f) return 1.0f;
    if (input < -1.0f) return -1.0f;
    // Cubic soft clipping
    return input - (input * input * input) / 3.0f;
}

// Overdrive (asymmetric)
float distortion_overdrive(float input, float drive) {
    float x = input * drive;
    if (x > 1.0f) return 1.0f;
    if (x < -1.0f) return -1.0f;
    return x * (1.5f - 0.5f * x * x);
}

// Apply to buffer
void apply_distortion(float* audio, int num_samples, float drive) {
    for (int i = 0; i < num_samples; i++) {
        audio[i] = distortion_overdrive(audio[i], drive);
    }
}
```

## Low-Pass Filter

Removes high frequencies (makes sound "darker" or "muffled").

```c
typedef struct {
    float prev_output;
    float alpha;  // Filter coefficient
} LowPassFilter;

LowPassFilter* lowpass_create(int sample_rate, float cutoff_freq) {
    LowPassFilter* filter = malloc(sizeof(LowPassFilter));
    filter->prev_output = 0.0f;
    
    // Calculate filter coefficient
    float rc = 1.0f / (cutoff_freq * 2.0f * PI);
    float dt = 1.0f / sample_rate;
    filter->alpha = dt / (rc + dt);
    
    return filter;
}

float lowpass_process(LowPassFilter* filter, float input) {
    // Simple one-pole low-pass filter
    filter->prev_output = filter->prev_output + 
                          filter->alpha * (input - filter->prev_output);
    return filter->prev_output;
}

void apply_lowpass(float* audio, int num_samples, int sample_rate, float cutoff_freq) {
    LowPassFilter* filter = lowpass_create(sample_rate, cutoff_freq);
    
    for (int i = 0; i < num_samples; i++) {
        audio[i] = lowpass_process(filter, audio[i]);
    }
    
    free(filter);
}
```

## High-Pass Filter

Removes low frequencies (removes rumble, makes sound "thinner").

```c
typedef struct {
    float prev_input;
    float prev_output;
    float alpha;
} HighPassFilter;

HighPassFilter* highpass_create(int sample_rate, float cutoff_freq) {
    HighPassFilter* filter = malloc(sizeof(HighPassFilter));
    filter->prev_input = 0.0f;
    filter->prev_output = 0.0f;
    
    float rc = 1.0f / (cutoff_freq * 2.0f * PI);
    float dt = 1.0f / sample_rate;
    filter->alpha = rc / (rc + dt);
    
    return filter;
}

float highpass_process(HighPassFilter* filter, float input) {
    float output = filter->alpha * (filter->prev_output + input - filter->prev_input);
    filter->prev_input = input;
    filter->prev_output = output;
    return output;
}
```

## Tremolo

Periodic volume change (amplitude modulation).

```c
void apply_tremolo(float* audio, int num_samples, int sample_rate, 
                   float rate_hz, float depth) {
    // rate_hz: How fast the volume oscillates (e.g., 5 Hz)
    // depth: 0.0 to 1.0 (how much volume changes)
    
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float lfo = sinf(2.0f * PI * rate_hz * time);  // -1 to 1
        float gain = 1.0f - depth * 0.5f * (1.0f + lfo);  // Modulate volume
        audio[i] *= gain;
    }
}
```

## Vibrato

Periodic pitch change (frequency modulation).

```c
void apply_vibrato(float* audio, int num_samples, int sample_rate, 
                   float rate_hz, float depth_samples) {
    // Create output buffer
    float* output = calloc(num_samples, sizeof(float));
    
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float lfo = sinf(2.0f * PI * rate_hz * time);
        
        // Modulate read position
        float read_pos = i + lfo * depth_samples;
        
        // Linear interpolation between samples
        int pos0 = (int)read_pos;
        int pos1 = pos0 + 1;
        float frac = read_pos - pos0;
        
        if (pos0 >= 0 && pos1 < num_samples) {
            output[i] = audio[pos0] * (1.0f - frac) + audio[pos1] * frac;
        }
    }
    
    memcpy(audio, output, num_samples * sizeof(float));
    free(output);
}
```

## Normalize

Scale audio to use full dynamic range without clipping.

```c
void normalize(float* audio, int num_samples, float target_level) {
    // Find peak amplitude
    float peak = 0.0f;
    for (int i = 0; i < num_samples; i++) {
        float abs_sample = fabsf(audio[i]);
        if (abs_sample > peak) {
            peak = abs_sample;
        }
    }
    
    // Calculate gain needed
    if (peak > 0.0f) {
        float gain = target_level / peak;
        
        // Apply gain
        for (int i = 0; i < num_samples; i++) {
            audio[i] *= gain;
        }
    }
}
```

## Compressor (Simple)

Reduces dynamic range by lowering loud parts.

```c
float compressor_process(float input, float threshold, float ratio) {
    // threshold: Level above which compression starts (e.g., 0.5)
    // ratio: Compression amount (e.g., 4.0 means 4:1)
    
    float abs_input = fabsf(input);
    
    if (abs_input > threshold) {
        // How much over threshold
        float over = abs_input - threshold;
        
        // Compressed amount
        float compressed = threshold + over / ratio;
        
        // Apply to input (preserve sign)
        return compressed * (input / abs_input);
    }
    
    return input;
}

void apply_compression(float* audio, int num_samples, float threshold, float ratio) {
    for (int i = 0; i < num_samples; i++) {
        audio[i] = compressor_process(audio[i], threshold, ratio);
    }
}
```

## Ring Modulation

Multiply audio by a sine wave (creates metallic sound).

```c
void apply_ring_mod(float* audio, int num_samples, int sample_rate, float mod_freq) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float modulator = sinf(2.0f * PI * mod_freq * time);
        audio[i] *= modulator;
    }
}
```

## Bit Crusher

Reduces bit depth and sample rate for "lo-fi" effect.

```c
void apply_bit_crush(float* audio, int num_samples, int bits, int rate_divider) {
    int step = 0;
    float held_sample = 0.0f;
    float levels = powf(2.0f, bits);
    
    for (int i = 0; i < num_samples; i++) {
        if (step == 0) {
            // Quantize to N bits
            float quantized = floorf(audio[i] * levels + 0.5f) / levels;
            held_sample = quantized;
        }
        
        audio[i] = held_sample;
        
        step = (step + 1) % rate_divider;
    }
}
```

## Effect Chaining

Combine multiple effects:

```c
void process_effects_chain(float* audio, int num_samples, int sample_rate) {
    // Order matters!
    
    // 1. Compression (control dynamics)
    apply_compression(audio, num_samples, 0.7f, 4.0f);
    
    // 2. Distortion (add harmonics)
    apply_distortion(audio, num_samples, 2.0f);
    
    // 3. Delay (add space)
    apply_delay(audio, num_samples, sample_rate, 0.3f, 0.4f, 0.3f);
    
    // 4. Low-pass filter (smooth it out)
    apply_lowpass(audio, num_samples, sample_rate, 5000.0f);
    
    // 5. Normalize (prevent clipping)
    normalize(audio, num_samples, 0.95f);
}
```

## Summary

| Effect | What It Does | Key Parameters |
|--------|--------------|----------------|
| Volume | Changes loudness | Gain |
| Pan | Stereo positioning | Pan (-1 to 1) |
| Delay/Echo | Repeats audio | Time, feedback, mix |
| Reverb | Room acoustics | Multiple delays |
| Distortion | Adds harmonics | Drive, threshold |
| Low-Pass | Removes highs | Cutoff frequency |
| High-Pass | Removes lows | Cutoff frequency |
| Tremolo | Volume oscillation | Rate, depth |
| Vibrato | Pitch oscillation | Rate, depth |
| Compressor | Reduces dynamics | Threshold, ratio |
| Normalize | Maximize level | Target level |

These effects form the foundation of audio processing. Combine and modify them to create any sound you can imagine!
