/*
 * Simple Synthesizer
 * 
 * Learn audio synthesis fundamentals:
 * - Oscillators (sine, square, saw, triangle)
 * - ADSR envelopes
 * - MIDI note to frequency conversion
 * - Simple polyphony
 * - Generating musical sequences
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265358979323846

#pragma pack(push, 1)
typedef struct {
    char     riff_id[4];
    uint32_t file_size;
    char     wave_id[4];
    char     fmt_id[4];
    uint32_t fmt_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char     data_id[4];
    uint32_t data_size;
} WavHeader;
#pragma pack(pop)

// Oscillator types
typedef enum {
    OSC_SINE,
    OSC_SQUARE,
    OSC_SAWTOOTH,
    OSC_TRIANGLE
} OscType;

// Oscillator state
typedef struct {
    float phase;
    float phase_increment;
    OscType type;
} Oscillator;

// ADSR Envelope
typedef enum {
    ENV_IDLE,
    ENV_ATTACK,
    ENV_DECAY,
    ENV_SUSTAIN,
    ENV_RELEASE
} EnvState;

typedef struct {
    float attack_time;
    float decay_time;
    float sustain_level;
    float release_time;
    EnvState state;
    float current_level;
    float time_in_state;
} Envelope;

// Synth voice (oscillator + envelope)
typedef struct {
    Oscillator osc;
    Envelope env;
    int sample_rate;
    int active;
} SynthVoice;

// MIDI note to frequency conversion
float midi_to_frequency(int midi_note) {
    return 440.0f * powf(2.0f, (midi_note - 69) / 12.0f);
}

// Set oscillator frequency
void oscillator_set_frequency(Oscillator* osc, float frequency, int sample_rate) {
    osc->phase_increment = frequency / sample_rate;
}

// Process oscillator (generate next sample)
float oscillator_process(Oscillator* osc) {
    float output = 0.0f;
    
    switch (osc->type) {
        case OSC_SINE:
            output = sinf(2.0f * PI * osc->phase);
            break;
            
        case OSC_SQUARE:
            output = (osc->phase < 0.5f) ? 1.0f : -1.0f;
            break;
            
        case OSC_SAWTOOTH:
            output = 2.0f * osc->phase - 1.0f;
            break;
            
        case OSC_TRIANGLE:
            if (osc->phase < 0.5f) {
                output = 4.0f * osc->phase - 1.0f;
            } else {
                output = 3.0f - 4.0f * osc->phase;
            }
            break;
    }
    
    // Advance phase
    osc->phase += osc->phase_increment;
    if (osc->phase >= 1.0f) osc->phase -= 1.0f;
    
    return output;
}

// Initialize envelope
void envelope_init(Envelope* env, float attack, float decay, float sustain, float release) {
    env->attack_time = attack;
    env->decay_time = decay;
    env->sustain_level = sustain;
    env->release_time = release;
    env->state = ENV_IDLE;
    env->current_level = 0.0f;
    env->time_in_state = 0.0f;
}

// Trigger envelope
void envelope_note_on(Envelope* env) {
    env->state = ENV_ATTACK;
    env->time_in_state = 0.0f;
}

void envelope_note_off(Envelope* env) {
    env->state = ENV_RELEASE;
    env->time_in_state = 0.0f;
}

// Process envelope (get current level)
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

// Initialize synth voice
void voice_init(SynthVoice* voice, int sample_rate, OscType osc_type) {
    voice->sample_rate = sample_rate;
    voice->osc.phase = 0.0f;
    voice->osc.type = osc_type;
    voice->active = 0;
    envelope_init(&voice->env, 0.01f, 0.1f, 0.7f, 0.2f);  // A=10ms, D=100ms, S=70%, R=200ms
}

// Play note
void voice_note_on(SynthVoice* voice, float frequency) {
    oscillator_set_frequency(&voice->osc, frequency, voice->sample_rate);
    envelope_note_on(&voice->env);
    voice->active = 1;
}

void voice_note_off(SynthVoice* voice) {
    envelope_note_off(&voice->env);
}

// Process voice (generate sample)
float voice_process(SynthVoice* voice) {
    if (!voice->active) return 0.0f;
    
    float dt = 1.0f / voice->sample_rate;
    float envelope = envelope_process(&voice->env, dt);
    float osc_out = oscillator_process(&voice->osc);
    
    if (voice->env.state == ENV_IDLE) {
        voice->active = 0;
    }
    
    return osc_out * envelope;
}

// Write WAV file
int write_wav(const char* filename, float* samples, int num_samples, int sample_rate) {
    FILE* file = fopen(filename, "wb");
    if (!file) return -1;
    
    int16_t* int_samples = malloc(num_samples * sizeof(int16_t));
    for (int i = 0; i < num_samples; i++) {
        float clamped = fmaxf(-1.0f, fminf(1.0f, samples[i]));
        int_samples[i] = (int16_t)(clamped * 32767.0f);
    }
    
    WavHeader header = {0};
    int data_size = num_samples * 2;
    memcpy(header.riff_id, "RIFF", 4);
    header.file_size = 36 + data_size;
    memcpy(header.wave_id, "WAVE", 4);
    memcpy(header.fmt_id, "fmt ", 4);
    header.fmt_size = 16;
    header.audio_format = 1;
    header.num_channels = 1;
    header.sample_rate = sample_rate;
    header.bits_per_sample = 16;
    header.block_align = 2;
    header.byte_rate = sample_rate * 2;
    memcpy(header.data_id, "data", 4);
    header.data_size = data_size;
    
    fwrite(&header, sizeof(header), 1, file);
    fwrite(int_samples, data_size, 1, file);
    fclose(file);
    free(int_samples);
    return 0;
}

int main(void) {
    printf("=== Simple Synthesizer ===\n\n");
    
    int sample_rate = 44100;
    int num_samples = sample_rate * 8;  // 8 seconds
    float* buffer = calloc(num_samples, sizeof(float));
    
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    // Demo 1: Different waveforms
    printf("Generating waveform comparison...\n");
    
    OscType waveforms[] = {OSC_SINE, OSC_SQUARE, OSC_SAWTOOTH, OSC_TRIANGLE};
    const char* waveform_names[] = {"Sine", "Square", "Sawtooth", "Triangle"};
    
    for (int w = 0; w < 4; w++) {
        memset(buffer, 0, num_samples * sizeof(float));
        SynthVoice voice;
        voice_init(&voice, sample_rate, waveforms[w]);
        
        // Play A4 (440 Hz) for 2 seconds
        voice_note_on(&voice, 440.0f);
        
        int note_length = sample_rate * 2;
        for (int i = 0; i < note_length; i++) {
            if (i == sample_rate * 1.5f) {  // Release after 1.5s
                voice_note_off(&voice);
            }
            buffer[i] = voice_process(&voice) * 0.3f;
        }
        
        char filename[64];
        snprintf(filename, sizeof(filename), "synth_%s.wav", waveform_names[w]);
        write_wav(filename, buffer, num_samples, sample_rate);
        printf("  Created: %s\n", filename);
    }
    
    // Demo 2: Musical scale (C major scale)
    printf("\nGenerating C major scale...\n");
    memset(buffer, 0, num_samples * sizeof(float));
    
    int scale_notes[] = {60, 62, 64, 65, 67, 69, 71, 72};  // C4 to C5
    SynthVoice voice;
    voice_init(&voice, sample_rate, OSC_SINE);
    
    int pos = 0;
    int note_duration = sample_rate / 2;  // 0.5 seconds per note
    
    for (int n = 0; n < 8; n++) {
        float freq = midi_to_frequency(scale_notes[n]);
        voice_note_on(&voice, freq);
        
        for (int i = 0; i < note_duration; i++) {
            if (i == note_duration - sample_rate * 0.05f) {
                voice_note_off(&voice);
            }
            buffer[pos++] = voice_process(&voice) * 0.3f;
        }
    }
    
    write_wav("synth_scale.wav", buffer, num_samples, sample_rate);
    printf("  Created: synth_scale.wav\n");
    
    // Demo 3: Simple melody
    printf("\nGenerating simple melody...\n");
    memset(buffer, 0, num_samples * sizeof(float));
    
    // Twinkle Twinkle Little Star (first line)
    int melody[] = {60, 60, 67, 67, 69, 69, 67};  // C C G G A A G
    int durations[] = {2, 2, 2, 2, 2, 2, 4};      // Quarter notes and half note
    
    voice_init(&voice, sample_rate, OSC_TRIANGLE);
    pos = 0;
    
    for (int n = 0; n < 7; n++) {
        float freq = midi_to_frequency(melody[n]);
        int note_len = sample_rate * durations[n] / 8;  // 8th note = 1/8 second
        
        voice_note_on(&voice, freq);
        
        for (int i = 0; i < note_len; i++) {
            if (i == note_len - sample_rate * 0.05f) {
                voice_note_off(&voice);
            }
            buffer[pos++] = voice_process(&voice) * 0.4f;
        }
    }
    
    write_wav("synth_melody.wav", buffer, num_samples, sample_rate);
    printf("  Created: synth_melody.wav\n");
    
    // Demo 4: Chord progression
    printf("\nGenerating chord progression...\n");
    memset(buffer, 0, num_samples * sizeof(float));
    
    // C major - F major - G major - C major
    int chords[4][3] = {
        {60, 64, 67},  // C-E-G (C major)
        {65, 69, 72},  // F-A-C (F major)
        {67, 71, 74},  // G-B-D (G major)
        {60, 64, 67}   // C-E-G (C major)
    };
    
    SynthVoice voices[3];
    for (int i = 0; i < 3; i++) {
        voice_init(&voices[i], sample_rate, OSC_SINE);
    }
    
    pos = 0;
    int chord_duration = sample_rate;  // 1 second per chord
    
    for (int c = 0; c < 4; c++) {
        // Start all notes in chord
        for (int v = 0; v < 3; v++) {
            float freq = midi_to_frequency(chords[c][v]);
            voice_note_on(&voices[v], freq);
        }
        
        // Play chord
        for (int i = 0; i < chord_duration; i++) {
            if (i == chord_duration - sample_rate * 0.1f) {
                for (int v = 0; v < 3; v++) {
                    voice_note_off(&voices[v]);
                }
            }
            
            float sample = 0.0f;
            for (int v = 0; v < 3; v++) {
                sample += voice_process(&voices[v]);
            }
            buffer[pos++] = sample * 0.15f;
        }
    }
    
    write_wav("synth_chords.wav", buffer, num_samples, sample_rate);
    printf("  Created: synth_chords.wav\n");
    
    free(buffer);
    
    printf("\n=== Summary ===\n");
    printf("Created 8 synthesized audio files:\n");
    printf("  synth_Sine.wav      - Pure sine wave\n");
    printf("  synth_Square.wav    - Square wave (hollow sound)\n");
    printf("  synth_Sawtooth.wav  - Sawtooth wave (bright sound)\n");
    printf("  synth_Triangle.wav  - Triangle wave (soft sound)\n");
    printf("  synth_scale.wav     - C major scale\n");
    printf("  synth_melody.wav    - Twinkle Twinkle Little Star\n");
    printf("  synth_chords.wav    - C-F-G-C chord progression\n");
    printf("\n");
    printf("Each demonstrates synthesis fundamentals:\n");
    printf("  - Oscillators generate waveforms\n");
    printf("  - ADSR envelopes shape amplitude\n");
    printf("  - MIDI notes convert to frequencies\n");
    printf("  - Multiple voices create harmony\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
