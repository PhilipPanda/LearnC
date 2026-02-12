/*
 * Frequency Analysis with FFT
 * 
 * Learn frequency domain audio processing:
 * - Fast Fourier Transform (FFT)
 * - Spectrum analysis
 * - Finding dominant frequencies
 * - Visualizing frequency content
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265358979323846

typedef struct {
    float real;
    float imag;
} Complex;

// Bit reversal for FFT
int reverse_bits(int x, int bits) {
    int result = 0;
    for (int i = 0; i < bits; i++) {
        if (x & (1 << i)) {
            result |= 1 << (bits - 1 - i);
        }
    }
    return result;
}

// In-place FFT (Cooley-Tukey algorithm)
void fft(Complex* data, int n) {
    // Bit reversal
    int bits = 0;
    int temp = n;
    while (temp > 1) {
        bits++;
        temp >>= 1;
    }
    
    for (int i = 0; i < n; i++) {
        int j = reverse_bits(i, bits);
        if (j > i) {
            Complex t = data[i];
            data[i] = data[j];
            data[j] = t;
        }
    }
    
    // FFT
    for (int size = 2; size <= n; size *= 2) {
        float angle = -2.0f * PI / size;
        Complex wlen = {cosf(angle), sinf(angle)};
        
        for (int i = 0; i < n; i += size) {
            Complex w = {1.0f, 0.0f};
            
            for (int j = 0; j < size / 2; j++) {
                Complex u = data[i + j];
                Complex t = {
                    w.real * data[i + j + size/2].real - w.imag * data[i + j + size/2].imag,
                    w.real * data[i + j + size/2].imag + w.imag * data[i + j + size/2].real
                };
                
                data[i + j].real = u.real + t.real;
                data[i + j].imag = u.imag + t.imag;
                
                data[i + j + size/2].real = u.real - t.real;
                data[i + j + size/2].imag = u.imag - t.imag;
                
                float w_temp = w.real;
                w.real = w.real * wlen.real - w.imag * wlen.imag;
                w.imag = w_temp * wlen.imag + w.imag * wlen.real;
            }
        }
    }
}

// Apply Hann window to reduce spectral leakage
void apply_hann_window(float* audio, int size) {
    for (int i = 0; i < size; i++) {
        float window = 0.5f * (1.0f - cosf(2.0f * PI * i / (size - 1)));
        audio[i] *= window;
    }
}

// Get magnitude from complex number
float magnitude(Complex c) {
    return sqrtf(c.real * c.real + c.imag * c.imag);
}

// Print spectrum as ASCII bar chart
void print_spectrum(Complex* fft_data, int fft_size, int sample_rate, int max_freq) {
    int num_bins = fft_size / 2;
    float max_magnitude = 0.0f;
    
    // Find max for scaling
    for (int i = 0; i < num_bins; i++) {
        float mag = magnitude(fft_data[i]);
        if (mag > max_magnitude) max_magnitude = mag;
    }
    
    printf("\n=== Frequency Spectrum ===\n\n");
    
    // Print bars (show every 4th bin up to max_freq)
    for (int i = 0; i < num_bins; i += 4) {
        float freq = (float)i * sample_rate / fft_size;
        if (freq > max_freq) break;
        
        float mag = magnitude(fft_data[i]);
        int bar_length = (int)(50 * mag / max_magnitude);
        
        printf("%6.0f Hz |", freq);
        for (int j = 0; j < bar_length; j++) {
            printf("#");
        }
        printf(" %.1f\n", mag);
    }
    printf("\n");
}

// Find dominant frequency
float find_dominant_frequency(Complex* fft_data, int fft_size, int sample_rate) {
    int num_bins = fft_size / 2;
    float max_magnitude = 0.0f;
    int max_bin = 0;
    
    for (int i = 1; i < num_bins; i++) {  // Skip DC (bin 0)
        float mag = magnitude(fft_data[i]);
        if (mag > max_magnitude) {
            max_magnitude = mag;
            max_bin = i;
        }
    }
    
    return (float)max_bin * sample_rate / fft_size;
}

// Analyze audio and print frequency content
void analyze_audio(float* audio, int num_samples, int sample_rate, const char* label) {
    int fft_size = 1024;
    if (num_samples < fft_size) fft_size = num_samples;
    
    printf("\n========================================\n");
    printf("Analyzing: %s\n", label);
    printf("========================================\n");
    
    // Prepare FFT input
    Complex* fft_data = calloc(fft_size, sizeof(Complex));
    float* windowed = malloc(fft_size * sizeof(float));
    
    // Copy and window the audio
    for (int i = 0; i < fft_size; i++) {
        windowed[i] = (i < num_samples) ? audio[i] : 0.0f;
    }
    apply_hann_window(windowed, fft_size);
    
    // Convert to complex
    for (int i = 0; i < fft_size; i++) {
        fft_data[i].real = windowed[i];
        fft_data[i].imag = 0.0f;
    }
    
    // Perform FFT
    fft(fft_data, fft_size);
    
    // Find and print dominant frequency
    float dominant_freq = find_dominant_frequency(fft_data, fft_size, sample_rate);
    printf("\nDominant Frequency: %.1f Hz\n", dominant_freq);
    
    // Print top 5 frequencies
    printf("\nTop 5 Frequencies:\n");
    for (int rank = 0; rank < 5; rank++) {
        float max_mag = 0.0f;
        int max_bin = 0;
        
        for (int i = 1; i < fft_size / 2; i++) {
            float mag = magnitude(fft_data[i]);
            if (mag > max_mag) {
                max_mag = mag;
                max_bin = i;
            }
        }
        
        float freq = (float)max_bin * sample_rate / fft_size;
        printf("  %d. %.1f Hz (magnitude: %.1f)\n", rank + 1, freq, max_mag);
        
        // Zero out this bin for next iteration
        fft_data[max_bin].real = 0.0f;
        fft_data[max_bin].imag = 0.0f;
    }
    
    // Reload FFT data for spectrum visualization
    for (int i = 0; i < fft_size; i++) {
        fft_data[i].real = windowed[i];
        fft_data[i].imag = 0.0f;
    }
    fft(fft_data, fft_size);
    
    // Print spectrum (up to 5000 Hz)
    print_spectrum(fft_data, fft_size, sample_rate, 5000);
    
    free(fft_data);
    free(windowed);
}

// Generate test signals
void generate_sine(float* buffer, int num_samples, int sample_rate, float frequency) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        buffer[i] = 0.5f * sinf(2.0f * PI * frequency * time);
    }
}

void generate_chord(float* buffer, int num_samples, int sample_rate, 
                   float* frequencies, int num_notes) {
    memset(buffer, 0, num_samples * sizeof(float));
    
    for (int n = 0; n < num_notes; n++) {
        for (int i = 0; i < num_samples; i++) {
            float time = (float)i / sample_rate;
            buffer[i] += 0.5f * sinf(2.0f * PI * frequencies[n] * time) / num_notes;
        }
    }
}

void generate_square(float* buffer, int num_samples, int sample_rate, float frequency) {
    for (int i = 0; i < num_samples; i++) {
        float time = (float)i / sample_rate;
        float phase = fmodf(time * frequency, 1.0f);
        buffer[i] = (phase < 0.5f) ? 0.5f : -0.5f;
    }
}

int main(void) {
    printf("=== Frequency Analysis with FFT ===\n\n");
    printf("This demonstrates FFT analysis of different audio signals.\n");
    printf("The FFT converts audio from time domain to frequency domain.\n");
    
    int sample_rate = 44100;
    int duration = 1;
    int num_samples = sample_rate * duration;
    
    float* audio = malloc(num_samples * sizeof(float));
    if (!audio) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    // Test 1: Pure sine wave (440 Hz - A4 note)
    generate_sine(audio, num_samples, sample_rate, 440.0f);
    analyze_audio(audio, num_samples, sample_rate, "Pure Sine Wave (440 Hz)");
    
    // Test 2: Higher frequency sine wave (880 Hz - A5 note)
    generate_sine(audio, num_samples, sample_rate, 880.0f);
    analyze_audio(audio, num_samples, sample_rate, "Pure Sine Wave (880 Hz)");
    
    // Test 3: C major chord (multiple frequencies)
    float chord_notes[] = {261.63f, 329.63f, 392.00f};  // C4, E4, G4
    generate_chord(audio, num_samples, sample_rate, chord_notes, 3);
    analyze_audio(audio, num_samples, sample_rate, "C Major Chord (C-E-G)");
    
    // Test 4: Square wave (rich in harmonics)
    generate_square(audio, num_samples, sample_rate, 440.0f);
    analyze_audio(audio, num_samples, sample_rate, "Square Wave (440 Hz)");
    
    // Test 5: Complex signal (fundamental + harmonics)
    float harmonics[] = {440.0f, 880.0f, 1320.0f, 1760.0f};
    generate_chord(audio, num_samples, sample_rate, harmonics, 4);
    analyze_audio(audio, num_samples, sample_rate, "Harmonic Series (440 Hz + overtones)");
    
    free(audio);
    
    printf("\n=== Key Observations ===\n\n");
    printf("1. Pure sine waves show a single peak at their frequency\n");
    printf("2. Chords show multiple peaks (one for each note)\n");
    printf("3. Square waves show odd harmonics (1x, 3x, 5x, 7x...)\n");
    printf("4. Harmonic series shows peaks at integer multiples\n");
    printf("\n");
    printf("FFT reveals what frequencies are present in audio,\n");
    printf("which is essential for:\n");
    printf("  - Pitch detection\n");
    printf("  - Equalizers\n");
    printf("  - Spectrum analyzers\n");
    printf("  - Audio compression\n");
    printf("  - Noise reduction\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
