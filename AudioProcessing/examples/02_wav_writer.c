/*
 * WAV File Writer
 * 
 * Learn to create WAV files:
 * - Building WAV headers
 * - Writing audio samples
 * - Creating different formats
 * - Generating test tones
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
int write_wav(const char* filename, int16_t* samples, int num_samples,
              int sample_rate, int num_channels) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to create file");
        return -1;
    }
    
    int bytes_per_sample = 2;  // 16-bit
    int data_size = num_samples * num_channels * bytes_per_sample;
    
    WavHeader header = {0};
    memcpy(header.riff_id, "RIFF", 4);
    header.file_size = 36 + data_size;
    memcpy(header.wave_id, "WAVE", 4);
    memcpy(header.fmt_id, "fmt ", 4);
    header.fmt_size = 16;
    header.audio_format = 1;  // PCM
    header.num_channels = num_channels;
    header.sample_rate = sample_rate;
    header.bits_per_sample = 16;
    header.block_align = num_channels * bytes_per_sample;
    header.byte_rate = sample_rate * header.block_align;
    memcpy(header.data_id, "data", 4);
    header.data_size = data_size;
    
    fwrite(&header, sizeof(header), 1, file);
    fwrite(samples, data_size, 1, file);
    
    fclose(file);
    return 0;
}

// Generate sine wave
void generate_sine_wave(int16_t* buffer, int num_samples, int sample_rate,
                       float frequency, float amplitude) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float value = amplitude * sinf(2.0f * PI * frequency * time);
        buffer[i] = (int16_t)(value * 32767.0f);
    }
}

// Generate square wave
void generate_square_wave(int16_t* buffer, int num_samples, int sample_rate,
                         float frequency, float amplitude) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float phase = fmodf(time * frequency, 1.0f);
        float value = (phase < 0.5f) ? amplitude : -amplitude;
        buffer[i] = (int16_t)(value * 32767.0f);
    }
}

// Generate sawtooth wave
void generate_sawtooth_wave(int16_t* buffer, int num_samples, int sample_rate,
                           float frequency, float amplitude) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float phase = fmodf(time * frequency, 1.0f);
        float value = amplitude * (2.0f * phase - 1.0f);
        buffer[i] = (int16_t)(value * 32767.0f);
    }
}

// Generate triangle wave
void generate_triangle_wave(int16_t* buffer, int num_samples, int sample_rate,
                           float frequency, float amplitude) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float phase = fmodf(time * frequency, 1.0f);
        float value;
        if (phase < 0.5f) {
            value = amplitude * (4.0f * phase - 1.0f);
        } else {
            value = amplitude * (3.0f - 4.0f * phase);
        }
        buffer[i] = (int16_t)(value * 32767.0f);
    }
}

// Generate chord (multiple frequencies)
void generate_chord(int16_t* buffer, int num_samples, int sample_rate,
                   float* frequencies, int num_notes, float amplitude) {
    // Clear buffer
    memset(buffer, 0, num_samples * sizeof(int16_t));
    
    int16_t* temp = malloc(num_samples * sizeof(int16_t));
    
    for (int n = 0; n < num_notes; n++) {
        generate_sine_wave(temp, num_samples, sample_rate, frequencies[n], amplitude / num_notes);
        for (int i = 0; i < num_samples; i++) {
            buffer[i] += temp[i];
        }
    }
    
    free(temp);
}

// Generate melody
void generate_melody(int16_t* buffer, int num_samples, int sample_rate) {
    // Simple melody: C-D-E-F-G (do-re-mi-fa-sol)
    float notes[] = {261.63f, 293.66f, 329.63f, 349.23f, 392.00f};  // C4-D4-E4-F4-G4
    int num_notes = 5;
    int note_duration = num_samples / num_notes;
    
    int16_t* temp = malloc(note_duration * sizeof(int16_t));
    
    for (int n = 0; n < num_notes; n++) {
        // Generate note with envelope (fade in/out)
        for (int i = 0; i < note_duration; i++) {
            float time = (float)i / sample_rate;
            float value = sinf(2.0f * PI * notes[n] * time);
            
            // Envelope
            float envelope = 1.0f;
            if (i < sample_rate * 0.01f) {  // 10ms fade in
                envelope = (float)i / (sample_rate * 0.01f);
            } else if (i > note_duration - sample_rate * 0.01f) {  // 10ms fade out
                envelope = (float)(note_duration - i) / (sample_rate * 0.01f);
            }
            
            temp[i] = (int16_t)(value * envelope * 0.5f * 32767.0f);
        }
        
        // Copy to buffer
        memcpy(buffer + n * note_duration, temp, note_duration * sizeof(int16_t));
    }
    
    free(temp);
}

// Generate stereo test (panning)
void generate_stereo_panning(int16_t* buffer, int num_samples, int sample_rate) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float tone = sinf(2.0f * PI * 440.0f * time);
        
        // Pan from left to right over duration
        float pan = (float)i / num_samples;  // 0.0 to 1.0
        
        float left = tone * (1.0f - pan);
        float right = tone * pan;
        
        buffer[i * 2 + 0] = (int16_t)(left * 0.5f * 32767.0f);   // Left channel
        buffer[i * 2 + 1] = (int16_t)(right * 0.5f * 32767.0f);  // Right channel
    }
}

int main(void) {
    printf("=== WAV File Writer ===\n\n");
    
    int sample_rate = 44100;
    int duration = 2;  // seconds
    int num_samples = sample_rate * duration;
    
    int16_t* buffer = malloc(num_samples * sizeof(int16_t));
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    // 1. Sine wave (pure tone)
    printf("Generating sine wave (440 Hz)...\n");
    generate_sine_wave(buffer, num_samples, sample_rate, 440.0f, 0.5f);
    write_wav("01_sine_440hz.wav", buffer, num_samples, sample_rate, 1);
    printf("  Created: 01_sine_440hz.wav\n\n");
    
    // 2. Square wave
    printf("Generating square wave (440 Hz)...\n");
    generate_square_wave(buffer, num_samples, sample_rate, 440.0f, 0.5f);
    write_wav("02_square_440hz.wav", buffer, num_samples, sample_rate, 1);
    printf("  Created: 02_square_440hz.wav\n\n");
    
    // 3. Sawtooth wave
    printf("Generating sawtooth wave (440 Hz)...\n");
    generate_sawtooth_wave(buffer, num_samples, sample_rate, 440.0f, 0.5f);
    write_wav("03_sawtooth_440hz.wav", buffer, num_samples, sample_rate, 1);
    printf("  Created: 03_sawtooth_440hz.wav\n\n");
    
    // 4. Triangle wave
    printf("Generating triangle wave (440 Hz)...\n");
    generate_triangle_wave(buffer, num_samples, sample_rate, 440.0f, 0.5f);
    write_wav("04_triangle_440hz.wav", buffer, num_samples, sample_rate, 1);
    printf("  Created: 04_triangle_440hz.wav\n\n");
    
    // 5. Chord (C major: C-E-G)
    printf("Generating C major chord...\n");
    float chord_notes[] = {261.63f, 329.63f, 392.00f};  // C4, E4, G4
    generate_chord(buffer, num_samples, sample_rate, chord_notes, 3, 0.7f);
    write_wav("05_chord_c_major.wav", buffer, num_samples, sample_rate, 1);
    printf("  Created: 05_chord_c_major.wav\n\n");
    
    // 6. Melody
    printf("Generating melody (C-D-E-F-G)...\n");
    generate_melody(buffer, num_samples, sample_rate);
    write_wav("06_melody.wav", buffer, num_samples, sample_rate, 1);
    printf("  Created: 06_melody.wav\n\n");
    
    free(buffer);
    
    // 7. Stereo panning test
    printf("Generating stereo panning test...\n");
    int16_t* stereo_buffer = malloc(num_samples * 2 * sizeof(int16_t));
    if (stereo_buffer) {
        generate_stereo_panning(stereo_buffer, num_samples, sample_rate);
        write_wav("07_stereo_panning.wav", stereo_buffer, num_samples, sample_rate, 2);
        printf("  Created: 07_stereo_panning.wav\n\n");
        free(stereo_buffer);
    }
    
    printf("=== Summary ===\n");
    printf("Created 7 WAV files:\n");
    printf("  01_sine_440hz.wav       - Pure tone (sine wave)\n");
    printf("  02_square_440hz.wav     - Hollow tone (square wave)\n");
    printf("  03_sawtooth_440hz.wav   - Bright tone (sawtooth)\n");
    printf("  04_triangle_440hz.wav   - Soft tone (triangle)\n");
    printf("  05_chord_c_major.wav    - C major chord\n");
    printf("  06_melody.wav           - Simple melody\n");
    printf("  07_stereo_panning.wav   - Stereo panning demo\n");
    printf("\n");
    printf("All files:\n");
    printf("  - Sample rate: 44100 Hz (CD quality)\n");
    printf("  - Bit depth: 16-bit\n");
    printf("  - Duration: 2 seconds\n");
    printf("  - Format: PCM WAV\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
