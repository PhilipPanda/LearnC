/*
 * Audio Mixer
 * 
 * Learn multi-track audio mixing:
 * - Combining multiple audio sources
 * - Volume control per track
 * - Panning (stereo positioning)
 * - Normalization
 * - Creating multi-layered compositions
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265358979323846
#define MAX_TRACKS 8

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

// Audio track
typedef struct {
    float* data;
    int length;
    float volume;      // 0.0 to 1.0
    float pan;         // -1.0 (left) to 1.0 (right)
    int offset;        // Start position in samples
    const char* name;
} AudioTrack;

// Write stereo WAV file
int write_stereo_wav(const char* filename, float* left, float* right, 
                    int num_samples, int sample_rate) {
    FILE* file = fopen(filename, "wb");
    if (!file) return -1;
    
    int16_t* stereo = malloc(num_samples * 2 * sizeof(int16_t));
    for (int i = 0; i < num_samples; i++) {
        float l = fmaxf(-1.0f, fminf(1.0f, left[i]));
        float r = fmaxf(-1.0f, fminf(1.0f, right[i]));
        stereo[i * 2 + 0] = (int16_t)(l * 32767.0f);
        stereo[i * 2 + 1] = (int16_t)(r * 32767.0f);
    }
    
    WavHeader header = {0};
    int data_size = num_samples * 2 * 2;  // stereo * 16-bit
    memcpy(header.riff_id, "RIFF", 4);
    header.file_size = 36 + data_size;
    memcpy(header.wave_id, "WAVE", 4);
    memcpy(header.fmt_id, "fmt ", 4);
    header.fmt_size = 16;
    header.audio_format = 1;
    header.num_channels = 2;  // Stereo
    header.sample_rate = sample_rate;
    header.bits_per_sample = 16;
    header.block_align = 4;  // 2 channels * 2 bytes
    header.byte_rate = sample_rate * 4;
    memcpy(header.data_id, "data", 4);
    header.data_size = data_size;
    
    fwrite(&header, sizeof(header), 1, file);
    fwrite(stereo, data_size, 1, file);
    fclose(file);
    free(stereo);
    return 0;
}

// Generate sine wave
void generate_sine(float* buffer, int num_samples, int sample_rate, float frequency) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        buffer[i] = sinf(2.0f * PI * frequency * time);
    }
}

// Generate drum hit (kick, snare, hat)
void generate_kick(float* buffer, int num_samples, int sample_rate) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float frequency = 150.0f * expf(-time * 10.0f) + 50.0f;
        float tone = sinf(2.0f * PI * frequency * time);
        float envelope = expf(-time * 8.0f);
        buffer[i] = tone * envelope;
    }
}

void generate_snare(float* buffer, int num_samples, int sample_rate) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float tone = sinf(2.0f * PI * 180.0f * time) * 0.3f +
                    sinf(2.0f * PI * 330.0f * time) * 0.3f;
        float noise = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * 0.4f;
        float envelope = expf(-time * 15.0f);
        buffer[i] = (tone + noise) * envelope;
    }
}

void generate_hihat(float* buffer, int num_samples, int sample_rate) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float noise = ((float)rand() / RAND_MAX * 2.0f - 1.0f);
        float envelope = expf(-time * 50.0f);
        buffer[i] = noise * envelope;
    }
}

// Apply panning (constant power)
void apply_pan(float mono, float pan, float* left, float* right) {
    float angle = (pan + 1.0f) * 0.25f * PI;
    *left = mono * cosf(angle);
    *right = mono * sinf(angle);
}

// Mix tracks into stereo output
void mix_tracks(AudioTrack* tracks, int num_tracks, float* left, float* right,
               int output_length, int sample_rate) {
    // Clear output buffers
    memset(left, 0, output_length * sizeof(float));
    memset(right, 0, output_length * sizeof(float));
    
    // Mix each track
    for (int t = 0; t < num_tracks; t++) {
        AudioTrack* track = &tracks[t];
        
        for (int i = 0; i < track->length; i++) {
            int pos = track->offset + i;
            if (pos >= output_length) break;
            
            float sample = track->data[i] * track->volume;
            float l, r;
            apply_pan(sample, track->pan, &l, &r);
            
            left[pos] += l;
            right[pos] += r;
        }
    }
}

// Normalize stereo output
void normalize_stereo(float* left, float* right, int num_samples, float target_level) {
    float peak = 0.0f;
    
    for (int i = 0; i < num_samples; i++) {
        float l = fabsf(left[i]);
        float r = fabsf(right[i]);
        if (l > peak) peak = l;
        if (r > peak) peak = r;
    }
    
    if (peak > 0.0f) {
        float gain = target_level / peak;
        for (int i = 0; i < num_samples; i++) {
            left[i] *= gain;
            right[i] *= gain;
        }
    }
}

// Print mix status
void print_mix_status(AudioTrack* tracks, int num_tracks) {
    printf("\n=== Mix Status ===\n");
    for (int i = 0; i < num_tracks; i++) {
        printf("Track %d: %s\n", i + 1, tracks[i].name);
        printf("  Volume: %.0f%%\n", tracks[i].volume * 100);
        printf("  Pan: ");
        if (tracks[i].pan < -0.1f) {
            printf("Left (%.0f%%)\n", (1.0f + tracks[i].pan) * 100);
        } else if (tracks[i].pan > 0.1f) {
            printf("Right (%.0f%%)\n", tracks[i].pan * 100);
        } else {
            printf("Center\n");
        }
        printf("  Offset: %.2f seconds\n", (float)tracks[i].offset / 44100);
        printf("  Length: %.2f seconds\n\n", (float)tracks[i].length / 44100);
    }
}

int main(void) {
    printf("=== Audio Mixer ===\n\n");
    printf("This demonstrates multi-track audio mixing.\n");
    printf("We'll create a simple drum beat with multiple layers.\n");
    
    int sample_rate = 44100;
    int total_duration = 4;  // 4 seconds
    int output_length = sample_rate * total_duration;
    
    float* left = calloc(output_length, sizeof(float));
    float* right = calloc(output_length, sizeof(float));
    
    if (!left || !right) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    // Create tracks
    AudioTrack tracks[MAX_TRACKS];
    int num_tracks = 0;
    
    // Track 1: Kick drum pattern (on beats 1 and 3)
    printf("Creating kick drum pattern...\n");
    int kick_length = sample_rate / 4;  // 0.25 seconds
    float* kick = malloc(kick_length * sizeof(float));
    generate_kick(kick, kick_length, sample_rate);
    
    // Create 4 kick hits
    for (int beat = 0; beat < 4; beat++) {
        if (beat % 2 == 0) {  // Beats 1 and 3
            tracks[num_tracks].data = kick;
            tracks[num_tracks].length = kick_length;
            tracks[num_tracks].volume = 1.0f;
            tracks[num_tracks].pan = 0.0f;  // Center
            tracks[num_tracks].offset = sample_rate * beat;
            tracks[num_tracks].name = "Kick Drum";
            num_tracks++;
        }
    }
    
    // Track 2: Snare drum pattern (on beats 2 and 4)
    printf("Creating snare drum pattern...\n");
    int snare_length = sample_rate / 4;
    float* snare = malloc(snare_length * sizeof(float));
    generate_snare(snare, snare_length, sample_rate);
    
    for (int beat = 0; beat < 4; beat++) {
        if (beat % 2 == 1) {  // Beats 2 and 4
            tracks[num_tracks].data = snare;
            tracks[num_tracks].length = snare_length;
            tracks[num_tracks].volume = 0.8f;
            tracks[num_tracks].pan = 0.0f;  // Center
            tracks[num_tracks].offset = sample_rate * beat;
            tracks[num_tracks].name = "Snare Drum";
            num_tracks++;
        }
    }
    
    // Track 3: Hi-hat pattern (8th notes)
    printf("Creating hi-hat pattern...\n");
    int hihat_length = sample_rate / 8;
    float* hihat = malloc(hihat_length * sizeof(float));
    generate_hihat(hihat, hihat_length, sample_rate);
    
    for (int eighth = 0; eighth < 8; eighth++) {
        tracks[num_tracks].data = hihat;
        tracks[num_tracks].length = hihat_length;
        tracks[num_tracks].volume = 0.3f;
        tracks[num_tracks].pan = 0.3f;  // Slightly right
        tracks[num_tracks].offset = sample_rate * eighth / 2;
        tracks[num_tracks].name = "Hi-Hat";
        num_tracks++;
    }
    
    // Track 4: Bass line (sustained)
    printf("Creating bass line...\n");
    int bass_length = sample_rate * 2;  // 2 seconds per note
    float* bass1 = malloc(bass_length * sizeof(float));
    float* bass2 = malloc(bass_length * sizeof(float));
    generate_sine(bass1, bass_length, sample_rate, 110.0f);  // A2
    generate_sine(bass2, bass_length, sample_rate, 82.41f);  // E2
    
    tracks[num_tracks].data = bass1;
    tracks[num_tracks].length = bass_length;
    tracks[num_tracks].volume = 0.4f;
    tracks[num_tracks].pan = -0.2f;  // Slightly left
    tracks[num_tracks].offset = 0;
    tracks[num_tracks].name = "Bass (A2)";
    num_tracks++;
    
    tracks[num_tracks].data = bass2;
    tracks[num_tracks].length = bass_length;
    tracks[num_tracks].volume = 0.4f;
    tracks[num_tracks].pan = -0.2f;
    tracks[num_tracks].offset = sample_rate * 2;
    tracks[num_tracks].name = "Bass (E2)";
    num_tracks++;
    
    // Print mix configuration
    print_mix_status(tracks, num_tracks);
    
    // Mix all tracks
    printf("Mixing %d tracks...\n", num_tracks);
    mix_tracks(tracks, num_tracks, left, right, output_length, sample_rate);
    
    // Normalize
    printf("Normalizing output...\n");
    normalize_stereo(left, right, output_length, 0.9f);
    
    // Write output
    printf("Writing output file...\n");
    write_stereo_wav("mixed_output.wav", left, right, output_length, sample_rate);
    
    printf("\n=== Success! ===\n");
    printf("Created: mixed_output.wav\n");
    printf("  - %d tracks mixed\n", num_tracks);
    printf("  - Stereo output\n");
    printf("  - %d seconds\n", total_duration);
    printf("  - 44100 Hz, 16-bit\n");
    printf("\nThe mix contains:\n");
    printf("  - Kick drum (beats 1, 3)\n");
    printf("  - Snare drum (beats 2, 4)\n");
    printf("  - Hi-hats (8th notes)\n");
    printf("  - Bass line (sustained notes)\n");
    printf("\nEach track has:\n");
    printf("  - Individual volume control\n");
    printf("  - Stereo panning\n");
    printf("  - Precise timing/offset\n");
    
    // Cleanup
    free(kick);
    free(snare);
    free(hihat);
    free(bass1);
    free(bass2);
    free(left);
    free(right);
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
