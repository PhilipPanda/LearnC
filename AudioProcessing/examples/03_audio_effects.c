/*
 * Audio Effects
 * 
 * Learn to apply common audio effects:
 * - Volume control and fades
 * - Delay and echo
 * - Simple reverb
 * - Distortion
 * - Low-pass and high-pass filters
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

// Generate test tone
void generate_tone(float* buffer, int num_samples, int sample_rate, float frequency) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        buffer[i] = 0.5f * sinf(2.0f * PI * frequency * time);
    }
}

// Effect 1: Volume control
void apply_volume(float* audio, int num_samples, float gain) {
    for (int i = 0; i < num_samples; i++) {
        audio[i] *= gain;
    }
}

// Effect 2: Fade in
void apply_fade_in(float* audio, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        float gain = (float)i / num_samples;
        audio[i] *= gain;
    }
}

// Effect 3: Fade out
void apply_fade_out(float* audio, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        float gain = 1.0f - ((float)i / num_samples);
        audio[i] *= gain;
    }
}

// Effect 4: Delay/Echo
void apply_delay(float* audio, int num_samples, int sample_rate, 
                float delay_seconds, float feedback, float mix) {
    int delay_samples = (int)(sample_rate * delay_seconds);
    if (delay_samples >= num_samples) return;
    
    float* delay_buffer = calloc(delay_samples, sizeof(float));
    float* output = malloc(num_samples * sizeof(float));
    int write_pos = 0;
    
    for (int i = 0; i < num_samples; i++) {
        float delayed = delay_buffer[write_pos];
        float processed = audio[i] + delayed * feedback;
        output[i] = audio[i] * (1.0f - mix) + processed * mix;
        delay_buffer[write_pos] = processed;
        write_pos = (write_pos + 1) % delay_samples;
    }
    
    memcpy(audio, output, num_samples * sizeof(float));
    free(delay_buffer);
    free(output);
}

// Effect 5: Distortion (soft clipping)
void apply_distortion(float* audio, int num_samples, float drive) {
    for (int i = 0; i < num_samples; i++) {
        float x = audio[i] * drive;
        // Soft clipping
        if (x > 1.0f) x = 1.0f;
        else if (x < -1.0f) x = -1.0f;
        else x = x - (x * x * x) / 3.0f;
        audio[i] = x;
    }
}

// Effect 6: Low-pass filter
void apply_lowpass(float* audio, int num_samples, int sample_rate, float cutoff_freq) {
    float rc = 1.0f / (cutoff_freq * 2.0f * PI);
    float dt = 1.0f / sample_rate;
    float alpha = dt / (rc + dt);
    
    float prev_output = 0.0f;
    for (int i = 0; i < num_samples; i++) {
        prev_output = prev_output + alpha * (audio[i] - prev_output);
        audio[i] = prev_output;
    }
}

// Effect 7: High-pass filter
void apply_highpass(float* audio, int num_samples, int sample_rate, float cutoff_freq) {
    float rc = 1.0f / (cutoff_freq * 2.0f * PI);
    float dt = 1.0f / sample_rate;
    float alpha = rc / (rc + dt);
    
    float prev_input = 0.0f;
    float prev_output = 0.0f;
    
    for (int i = 0; i < num_samples; i++) {
        float output = alpha * (prev_output + audio[i] - prev_input);
        prev_input = audio[i];
        prev_output = output;
        audio[i] = output;
    }
}

// Effect 8: Tremolo (amplitude modulation)
void apply_tremolo(float* audio, int num_samples, int sample_rate, 
                  float rate_hz, float depth) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float lfo = sinf(2.0f * PI * rate_hz * time);
        float gain = 1.0f - depth * 0.5f * (1.0f + lfo);
        audio[i] *= gain;
    }
}

// Effect 9: Normalize (maximize volume without clipping)
void normalize(float* audio, int num_samples, float target_level) {
    float peak = 0.0f;
    for (int i = 0; i < num_samples; i++) {
        float abs_sample = fabsf(audio[i]);
        if (abs_sample > peak) peak = abs_sample;
    }
    
    if (peak > 0.0f) {
        float gain = target_level / peak;
        for (int i = 0; i < num_samples; i++) {
            audio[i] *= gain;
        }
    }
}

int main(void) {
    printf("=== Audio Effects Processor ===\n\n");
    
    int sample_rate = 44100;
    int duration = 3;
    int num_samples = sample_rate * duration;
    
    float* original = malloc(num_samples * sizeof(float));
    float* processed = malloc(num_samples * sizeof(float));
    
    if (!original || !processed) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    // Generate test tone
    printf("Generating test tone (440 Hz, 3 seconds)...\n\n");
    generate_tone(original, num_samples, sample_rate, 440.0f);
    
    // Save original
    write_wav("effect_00_original.wav", original, num_samples, sample_rate);
    printf("Created: effect_00_original.wav (original)\n");
    
    // Effect 1: Volume boost
    memcpy(processed, original, num_samples * sizeof(float));
    apply_volume(processed, num_samples, 2.0f);
    write_wav("effect_01_volume_boost.wav", processed, num_samples, sample_rate);
    printf("Created: effect_01_volume_boost.wav (2x volume)\n");
    
    // Effect 2: Volume reduction
    memcpy(processed, original, num_samples * sizeof(float));
    apply_volume(processed, num_samples, 0.25f);
    write_wav("effect_02_volume_quiet.wav", processed, num_samples, sample_rate);
    printf("Created: effect_02_volume_quiet.wav (25%% volume)\n");
    
    // Effect 3: Fade in
    memcpy(processed, original, num_samples * sizeof(float));
    apply_fade_in(processed, num_samples);
    write_wav("effect_03_fade_in.wav", processed, num_samples, sample_rate);
    printf("Created: effect_03_fade_in.wav (fade in)\n");
    
    // Effect 4: Fade out
    memcpy(processed, original, num_samples * sizeof(float));
    apply_fade_out(processed, num_samples);
    write_wav("effect_04_fade_out.wav", processed, num_samples, sample_rate);
    printf("Created: effect_04_fade_out.wav (fade out)\n");
    
    // Effect 5: Echo
    memcpy(processed, original, num_samples * sizeof(float));
    apply_delay(processed, num_samples, sample_rate, 0.3f, 0.5f, 0.5f);
    write_wav("effect_05_echo.wav", processed, num_samples, sample_rate);
    printf("Created: effect_05_echo.wav (echo, 300ms delay)\n");
    
    // Effect 6: Distortion
    memcpy(processed, original, num_samples * sizeof(float));
    apply_distortion(processed, num_samples, 5.0f);
    write_wav("effect_06_distortion.wav", processed, num_samples, sample_rate);
    printf("Created: effect_06_distortion.wav (distortion)\n");
    
    // Effect 7: Low-pass filter (muffled)
    memcpy(processed, original, num_samples * sizeof(float));
    apply_lowpass(processed, num_samples, sample_rate, 1000.0f);
    write_wav("effect_07_lowpass.wav", processed, num_samples, sample_rate);
    printf("Created: effect_07_lowpass.wav (low-pass 1000Hz)\n");
    
    // Effect 8: High-pass filter
    memcpy(processed, original, num_samples * sizeof(float));
    apply_highpass(processed, num_samples, sample_rate, 2000.0f);
    write_wav("effect_08_highpass.wav", processed, num_samples, sample_rate);
    printf("Created: effect_08_highpass.wav (high-pass 2000Hz)\n");
    
    // Effect 9: Tremolo
    memcpy(processed, original, num_samples * sizeof(float));
    apply_tremolo(processed, num_samples, sample_rate, 5.0f, 0.8f);
    write_wav("effect_09_tremolo.wav", processed, num_samples, sample_rate);
    printf("Created: effect_09_tremolo.wav (tremolo 5Hz)\n");
    
    // Effect 10: Normalize
    memcpy(processed, original, num_samples * sizeof(float));
    apply_volume(processed, num_samples, 0.1f);  // Make quiet first
    normalize(processed, num_samples, 0.9f);
    write_wav("effect_10_normalized.wav", processed, num_samples, sample_rate);
    printf("Created: effect_10_normalized.wav (normalized)\n");
    
    free(original);
    free(processed);
    
    printf("\n=== Summary ===\n");
    printf("Created 11 WAV files demonstrating audio effects:\n");
    printf("  00 - Original tone\n");
    printf("  01 - Volume boost (2x)\n");
    printf("  02 - Volume reduction (25%%)\n");
    printf("  03 - Fade in\n");
    printf("  04 - Fade out\n");
    printf("  05 - Echo/Delay\n");
    printf("  06 - Distortion\n");
    printf("  07 - Low-pass filter (removes highs)\n");
    printf("  08 - High-pass filter (removes lows)\n");
    printf("  09 - Tremolo (volume oscillation)\n");
    printf("  10 - Normalized (maximized)\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
