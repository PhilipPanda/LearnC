# Synthesizer

## The Big Picture

A synthesizer generates audio from scratch using mathematics. Understanding synthesis teaches you how sound works at the deepest level and lets you create any sound imaginable.

## Oscillators

An **oscillator** generates a periodic waveform. Different waveforms have different timbres (tonal colors).

### Sine Wave

Purest tone - single frequency, no harmonics.

```c
#include <math.h>
#define PI 3.14159265358979323846

float sine_wave(float frequency, float time) {
    return sinf(2.0f * PI * frequency * time);
}

// Generate to buffer
void generate_sine(float* buffer, int num_samples, int sample_rate, float frequency) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        buffer[i] = sine_wave(frequency, time);
    }
}
```

**Sound:** Pure, smooth, flute-like  
**Use:** Sub-bass, clean tones, tuning reference

### Square Wave

Alternates between +1 and -1.

```c
float square_wave(float frequency, float time) {
    float phase = fmodf(time * frequency, 1.0f);  // 0.0 to 1.0
    return (phase < 0.5f) ? 1.0f : -1.0f;
}
```

**Sound:** Hollow, clarinet-like, rich in odd harmonics  
**Use:** Leads, bass, retro game sounds

### Sawtooth Wave

Linear ramp from -1 to +1.

```c
float sawtooth_wave(float frequency, float time) {
    float phase = fmodf(time * frequency, 1.0f);  // 0.0 to 1.0
    return 2.0f * phase - 1.0f;                   // -1.0 to +1.0
}
```

**Sound:** Bright, buzzy, rich in all harmonics  
**Use:** Brass, strings, aggressive leads

### Triangle Wave

Linear ramp up then down.

```c
float triangle_wave(float frequency, float time) {
    float phase = fmodf(time * frequency, 1.0f);  // 0.0 to 1.0
    if (phase < 0.5f) {
        return 4.0f * phase - 1.0f;  // Rise: -1 to +1
    } else {
        return 3.0f - 4.0f * phase;  // Fall: +1 to -1
    }
}
```

**Sound:** Softer than square, smoother than saw  
**Use:** Pads, smooth leads

### Pulse Wave (Variable Width)

Square wave with adjustable duty cycle.

```c
float pulse_wave(float frequency, float time, float pulse_width) {
    // pulse_width: 0.0 to 1.0 (0.5 = square wave)
    float phase = fmodf(time * frequency, 1.0f);
    return (phase < pulse_width) ? 1.0f : -1.0f;
}
```

### Noise

Random values for percussion and effects.

```c
float white_noise(void) {
    return 2.0f * ((float)rand() / RAND_MAX) - 1.0f;
}

// Better random (more uniform)
float white_noise_better(void) {
    static unsigned int seed = 1;
    seed = seed * 1103515245 + 12345;
    return 2.0f * ((float)(seed % 32768) / 32768.0f) - 1.0f;
}
```

**Sound:** Static, hiss  
**Use:** Hi-hats, snares, wind, texture

## Phase Accumulator

For continuous oscillation without recalculating time:

```c
typedef struct {
    float phase;          // Current phase (0.0 to 1.0)
    float phase_increment;
} Oscillator;

void oscillator_set_frequency(Oscillator* osc, float frequency, int sample_rate) {
    osc->phase_increment = frequency / sample_rate;
}

float oscillator_next_sine(Oscillator* osc) {
    float output = sinf(2.0f * PI * osc->phase);
    osc->phase += osc->phase_increment;
    if (osc->phase >= 1.0f) osc->phase -= 1.0f;  // Wrap
    return output;
}

float oscillator_next_saw(Oscillator* osc) {
    float output = 2.0f * osc->phase - 1.0f;
    osc->phase += osc->phase_increment;
    if (osc->phase >= 1.0f) osc->phase -= 1.0f;
    return output;
}
```

## ADSR Envelope

Controls volume over time - **A**ttack, **D**ecay, **S**ustain, **R**elease.

```
 Amplitude
    ^
1.0 |    /\
    |   /  \___________
    |  /               \
0.0 |_/                 \____
    |<-A-><-D-><--S--><-R->
        Time →
```

```c
typedef enum {
    ENV_IDLE,
    ENV_ATTACK,
    ENV_DECAY,
    ENV_SUSTAIN,
    ENV_RELEASE
} EnvelopeState;

typedef struct {
    float attack_time;     // seconds
    float decay_time;      // seconds
    float sustain_level;   // 0.0 to 1.0
    float release_time;    // seconds
    
    EnvelopeState state;
    float current_level;
    float time_in_state;
} Envelope;

void envelope_note_on(Envelope* env) {
    env->state = ENV_ATTACK;
    env->time_in_state = 0.0f;
}

void envelope_note_off(Envelope* env) {
    env->state = ENV_RELEASE;
    env->time_in_state = 0.0f;
}

float envelope_process(Envelope* env, float dt) {
    env->time_in_state += dt;
    
    switch (env->state) {
        case ENV_ATTACK:
            env->current_level = env->time_in_state / env->attack_time;
            if (env->current_level >= 1.0f) {
                env->current_level = 1.0f;
                env->state = ENV_DECAY;
                env->time_in_state = 0.0f;
            }
            break;
            
        case ENV_DECAY:
            env->current_level = 1.0f - (1.0f - env->sustain_level) * 
                                (env->time_in_state / env->decay_time);
            if (env->time_in_state >= env->decay_time) {
                env->current_level = env->sustain_level;
                env->state = ENV_SUSTAIN;
            }
            break;
            
        case ENV_SUSTAIN:
            env->current_level = env->sustain_level;
            break;
            
        case ENV_RELEASE:
            env->current_level = env->sustain_level * 
                                (1.0f - env->time_in_state / env->release_time);
            if (env->time_in_state >= env->release_time) {
                env->current_level = 0.0f;
                env->state = ENV_IDLE;
            }
            break;
            
        case ENV_IDLE:
            env->current_level = 0.0f;
            break;
    }
    
    return env->current_level;
}
```

## Simple Synth Voice

Combines oscillator + envelope:

```c
typedef struct {
    Oscillator osc;
    Envelope env;
    float frequency;
    int sample_rate;
} SynthVoice;

void voice_note_on(SynthVoice* voice, float frequency) {
    voice->frequency = frequency;
    oscillator_set_frequency(&voice->osc, frequency, voice->sample_rate);
    envelope_note_on(&voice->env);
}

void voice_note_off(SynthVoice* voice) {
    envelope_note_off(&voice->env);
}

float voice_process(SynthVoice* voice) {
    float dt = 1.0f / voice->sample_rate;
    float envelope = envelope_process(&voice->env, dt);
    float oscillator = oscillator_next_sine(&voice->osc);
    return oscillator * envelope;
}

// Generate audio
void voice_render(SynthVoice* voice, float* buffer, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        buffer[i] = voice_process(voice);
    }
}
```

## MIDI Note to Frequency

```c
// MIDI note 69 = A4 = 440 Hz
float midi_to_frequency(int midi_note) {
    return 440.0f * powf(2.0f, (midi_note - 69) / 12.0f);
}

// Examples:
// Note 60 = Middle C = 261.63 Hz
// Note 69 = A4 = 440 Hz
// Note 81 = A5 = 880 Hz

// Note names
const char* note_names[] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

const char* midi_to_note_name(int midi_note) {
    int octave = (midi_note / 12) - 1;
    int note = midi_note % 12;
    static char buffer[16];
    snprintf(buffer, sizeof(buffer), "%s%d", note_names[note], octave);
    return buffer;
}
```

## Low-Frequency Oscillator (LFO)

Modulates other parameters (vibrato, tremolo, filter sweep).

```c
typedef struct {
    Oscillator osc;
    float depth;  // Amount of modulation
} LFO;

float lfo_process(LFO* lfo, float base_value) {
    float lfo_value = oscillator_next_sine(&lfo->osc);  // -1 to +1
    return base_value + lfo_value * lfo->depth;
}

// Example: Vibrato (pitch modulation)
void apply_vibrato_to_voice(SynthVoice* voice, LFO* lfo) {
    float modulated_freq = lfo_process(lfo, voice->frequency);
    oscillator_set_frequency(&voice->osc, modulated_freq, voice->sample_rate);
}
```

## Polyphony

Multiple voices playing simultaneously:

```c
#define MAX_VOICES 8

typedef struct {
    SynthVoice voices[MAX_VOICES];
    int num_voices;
} PolySynth;

void polysynth_note_on(PolySynth* synth, float frequency) {
    // Find free voice
    for (int i = 0; i < synth->num_voices; i++) {
        if (synth->voices[i].env.state == ENV_IDLE) {
            voice_note_on(&synth->voices[i], frequency);
            return;
        }
    }
    // No free voice - steal oldest (voice stealing)
    voice_note_on(&synth->voices[0], frequency);
}

void polysynth_render(PolySynth* synth, float* buffer, int num_samples) {
    // Clear buffer
    memset(buffer, 0, num_samples * sizeof(float));
    
    // Mix all voices
    float temp[num_samples];
    for (int v = 0; v < synth->num_voices; v++) {
        voice_render(&synth->voices[v], temp, num_samples);
        for (int i = 0; i < num_samples; i++) {
            buffer[i] += temp[i];
        }
    }
    
    // Normalize (prevent clipping)
    for (int i = 0; i < num_samples; i++) {
        buffer[i] /= synth->num_voices;
    }
}
```

## Additive Synthesis

Build complex tones from multiple sine waves (harmonics).

```c
void additive_synthesis(float* buffer, int num_samples, int sample_rate,
                       float fundamental, float* harmonic_amplitudes, int num_harmonics) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float sample = 0.0f;
        
        for (int h = 0; h < num_harmonics; h++) {
            float frequency = fundamental * (h + 1);  // 1x, 2x, 3x, etc.
            float amplitude = harmonic_amplitudes[h];
            sample += amplitude * sinf(2.0f * PI * frequency * time);
        }
        
        buffer[i] = sample;
    }
}

// Example: Organ-like tone
float organ_harmonics[] = {1.0f, 0.5f, 0.25f, 0.125f, 0.0625f};
additive_synthesis(buffer, num_samples, sample_rate, 440.0f, organ_harmonics, 5);
```

## Subtractive Synthesis

Start with harmonically rich waveform (saw/square), then filter.

```c
// Generate rich waveform
void generate_sawtooth(float* buffer, int num_samples, int sample_rate, float frequency);

// Apply low-pass filter
void apply_lowpass_filter(float* buffer, int num_samples, float cutoff_frequency);

// Modulate filter with envelope
void subtractive_synth(float* buffer, int num_samples, int sample_rate) {
    generate_sawtooth(buffer, num_samples, sample_rate, 440.0f);
    
    // Filter cutoff envelope (sweeps from bright to dark)
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / num_samples;
        float cutoff = 5000.0f * (1.0f - time);  // 5000 Hz to 0 Hz
        // Apply filter at this cutoff...
    }
}
```

## FM Synthesis

One oscillator modulates the frequency of another.

```c
typedef struct {
    Oscillator carrier;     // Audible frequency
    Oscillator modulator;   // Modulates carrier
    float mod_depth;        // Modulation intensity
} FMSynth;

float fm_synth_process(FMSynth* fm) {
    // Get modulator output
    float mod = oscillator_next_sine(&fm->modulator);
    
    // Modulate carrier frequency
    float modulated_phase = fm->carrier.phase + mod * fm->mod_depth;
    
    // Generate carrier output
    float output = sinf(2.0f * PI * modulated_phase);
    
    // Advance carrier phase
    fm->carrier.phase += fm->carrier.phase_increment;
    if (fm->carrier.phase >= 1.0f) fm->carrier.phase -= 1.0f;
    
    return output;
}

// FM creates complex, bell-like timbres (used in DX7, classic synth)
```

## Chord Generation

```c
void generate_chord(float* buffer, int num_samples, int sample_rate, 
                   float* frequencies, int num_notes) {
    memset(buffer, 0, num_samples * sizeof(float));
    
    for (int n = 0; n < num_notes; n++) {
        for (int i = 0; i < num_samples; i++) {
            float time = (float)i / sample_rate;
            buffer[i] += sine_wave(frequencies[n], time) / num_notes;
        }
    }
}

// C Major chord (C, E, G)
float c_major[] = {261.63f, 329.63f, 392.00f};
generate_chord(buffer, num_samples, sample_rate, c_major, 3);
```

## Drum Synthesis

### Kick Drum

```c
void generate_kick(float* buffer, int num_samples, int sample_rate) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        
        // Frequency sweep (high to low)
        float frequency = 150.0f * expf(-time * 10.0f) + 50.0f;
        
        // Sine wave at frequency
        float tone = sinf(2.0f * PI * frequency * time);
        
        // Amplitude envelope
        float envelope = expf(-time * 8.0f);
        
        buffer[i] = tone * envelope;
    }
}
```

### Snare Drum

```c
void generate_snare(float* buffer, int num_samples, int sample_rate) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        
        // Tone (two sine waves)
        float tone = sinf(2.0f * PI * 180.0f * time) * 0.3f +
                    sinf(2.0f * PI * 330.0f * time) * 0.3f;
        
        // Noise
        float noise = white_noise() * 0.4f;
        
        // Envelope
        float envelope = expf(-time * 15.0f);
        
        buffer[i] = (tone + noise) * envelope;
    }
}
```

### Hi-Hat

```c
void generate_hihat(float* buffer, int num_samples, int sample_rate) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        
        // High-frequency noise
        float noise = white_noise();
        
        // Short envelope
        float envelope = expf(-time * 50.0f);
        
        buffer[i] = noise * envelope * 0.3f;
    }
}
```

## Summary

| Waveform | Character | Harmonics |
|----------|-----------|-----------|
| Sine | Pure, smooth | None (fundamental only) |
| Square | Hollow, clarinet | Odd harmonics |
| Sawtooth | Bright, buzzy | All harmonics |
| Triangle | Soft, mellow | Odd harmonics (weak) |
| Noise | Static | All frequencies |

**Synthesis Types:**
- **Additive:** Sum of sine waves
- **Subtractive:** Filter rich waveform
- **FM:** Frequency modulation
- **Wavetable:** Playback recorded cycles

**Key Concepts:**
- Oscillator generates waveform
- Envelope shapes amplitude over time
- LFO modulates parameters
- Polyphony = multiple voices
- MIDI notes map to frequencies

With these building blocks, you can create any sound - from realistic instruments to alien soundscapes!
