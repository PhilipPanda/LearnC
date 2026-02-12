# FFT Basics

## The Big Picture

FFT (Fast Fourier Transform) converts audio from **time domain** (amplitude over time) to **frequency domain** (amplitude at each frequency). This lets you analyze what frequencies are present, build equalizers, and perform spectral effects.

## Time Domain vs Frequency Domain

```
Time Domain:                Frequency Domain (FFT):
Amplitude over time         Amplitude at each frequency

     ^                           ^
  1  |    /\                  1  |        |
     |   /  \                    |      | | |
  0  |--/----\----        0      |------|--------|----
     |        \  /                   100 200  500 Hz
 -1  |         \/

Shows "when"                Shows "what frequencies"
```

**Example:** A 440 Hz sine wave looks like:
- Time domain: Smooth oscillation
- Frequency domain: Single peak at 440 Hz

## What is FFT?

FFT decomposes audio into sine waves of different frequencies.

Any sound = sum of sine waves at different frequencies, phases, and amplitudes.

```c
// Complex audio signal:
audio = A₁·sin(2π·f₁·t) + A₂·sin(2π·f₂·t) + A₃·sin(2π·f₃·t) + ...

// FFT extracts: f₁, f₂, f₃... and their amplitudes A₁, A₂, A₃...
```

## Complex Numbers

FFT works with **complex numbers** because waves have amplitude AND phase.

```c
typedef struct {
    float real;  // Real part
    float imag;  // Imaginary part
} Complex;

// Magnitude (amplitude):
float magnitude(Complex c) {
    return sqrtf(c.real * c.real + c.imag * c.imag);
}

// Phase (angle):
float phase(Complex c) {
    return atan2f(c.imag, c.real);
}
```

## FFT Size

FFT size must be **power of 2**: 256, 512, 1024, 2048, 4096, etc.

```c
// Common FFT sizes:
#define FFT_SIZE_256   256
#define FFT_SIZE_512   512
#define FFT_SIZE_1024  1024
#define FFT_SIZE_2048  2048
#define FFT_SIZE_4096  4096
```

**Larger FFT:**
- Better frequency resolution
- More computation time
- More latency

**Smaller FFT:**
- Worse frequency resolution
- Faster computation
- Less latency

## Frequency Resolution

```c
float frequency_resolution(int fft_size, int sample_rate) {
    return (float)sample_rate / (float)fft_size;
}

// Examples:
// FFT 1024, sample rate 44100 Hz
// Resolution = 44100 / 1024 = 43 Hz per bin
// Bins: 0Hz, 43Hz, 86Hz, 129Hz, ...

// FFT 4096, sample rate 44100 Hz
// Resolution = 44100 / 4096 = 10.8 Hz per bin
// Better frequency precision!
```

## FFT Bins

FFT output has N/2 useful bins (due to Nyquist theorem).

```c
int num_bins = fft_size / 2;

// Frequency of bin i:
float bin_frequency(int bin, int fft_size, int sample_rate) {
    return (float)bin * sample_rate / fft_size;
}

// Example: FFT 1024, 44100 Hz
// Bin 0:  0 Hz (DC component)
// Bin 1:  43 Hz
// Bin 10: 430 Hz
// Bin 100: 4300 Hz
// Bin 512: 22050 Hz (Nyquist frequency)
```

## Simple FFT Implementation (Cooley-Tukey)

```c
#include <math.h>
#include <string.h>

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

// In-place FFT (power-of-2 size only)
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
            Complex temp = data[i];
            data[i] = data[j];
            data[j] = temp;
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

// Inverse FFT
void ifft(Complex* data, int n) {
    // Conjugate input
    for (int i = 0; i < n; i++) {
        data[i].imag = -data[i].imag;
    }
    
    // Forward FFT
    fft(data, n);
    
    // Conjugate and scale output
    for (int i = 0; i < n; i++) {
        data[i].real /= n;
        data[i].imag = -data[i].imag / n;
    }
}
```

## Using FFT on Audio

```c
void analyze_audio(float* audio, int num_samples, int sample_rate) {
    int fft_size = 1024;
    
    // Convert to complex (real audio, zero imaginary)
    Complex* fft_data = malloc(fft_size * sizeof(Complex));
    for (int i = 0; i < fft_size; i++) {
        if (i < num_samples) {
            fft_data[i].real = audio[i];
        } else {
            fft_data[i].real = 0.0f;  // Zero padding
        }
        fft_data[i].imag = 0.0f;
    }
    
    // Perform FFT
    fft(fft_data, fft_size);
    
    // Get magnitude spectrum
    int num_bins = fft_size / 2;
    for (int i = 0; i < num_bins; i++) {
        float magnitude = sqrtf(fft_data[i].real * fft_data[i].real + 
                               fft_data[i].imag * fft_data[i].imag);
        float frequency = (float)i * sample_rate / fft_size;
        
        printf("Bin %d: %.1f Hz, Amplitude: %.4f\n", i, frequency, magnitude);
    }
    
    free(fft_data);
}
```

## Windowing

Applying a window function reduces spectral leakage.

```c
// Hann window (common choice)
void apply_hann_window(float* audio, int size) {
    for (int i = 0; i < size; i++) {
        float window = 0.5f * (1.0f - cosf(2.0f * PI * i / (size - 1)));
        audio[i] *= window;
    }
}

// Hamming window
void apply_hamming_window(float* audio, int size) {
    for (int i = 0; i < size; i++) {
        float window = 0.54f - 0.46f * cosf(2.0f * PI * i / (size - 1));
        audio[i] *= window;
    }
}

// Use before FFT:
apply_hann_window(audio, fft_size);
fft(fft_data, fft_size);
```

## Spectrum Visualization

```c
void print_spectrum(Complex* fft_data, int fft_size, int sample_rate) {
    int num_bins = fft_size / 2;
    float max_magnitude = 0.0f;
    
    // Find max for scaling
    for (int i = 0; i < num_bins; i++) {
        float mag = sqrtf(fft_data[i].real * fft_data[i].real + 
                         fft_data[i].imag * fft_data[i].imag);
        if (mag > max_magnitude) {
            max_magnitude = mag;
        }
    }
    
    // Print ASCII bars
    for (int i = 0; i < num_bins; i += 4) {  // Every 4th bin
        float mag = sqrtf(fft_data[i].real * fft_data[i].real + 
                         fft_data[i].imag * fft_data[i].imag);
        float freq = (float)i * sample_rate / fft_size;
        int bar_length = (int)(50 * mag / max_magnitude);
        
        printf("%6.0f Hz |", freq);
        for (int j = 0; j < bar_length; j++) {
            printf("#");
        }
        printf("\n");
    }
}
```

## Finding Dominant Frequency

```c
float find_dominant_frequency(Complex* fft_data, int fft_size, int sample_rate) {
    int num_bins = fft_size / 2;
    float max_magnitude = 0.0f;
    int max_bin = 0;
    
    // Skip DC (bin 0)
    for (int i = 1; i < num_bins; i++) {
        float mag = sqrtf(fft_data[i].real * fft_data[i].real + 
                         fft_data[i].imag * fft_data[i].imag);
        if (mag > max_magnitude) {
            max_magnitude = mag;
            max_bin = i;
        }
    }
    
    return (float)max_bin * sample_rate / fft_size;
}
```

## Spectrogram (Time-Frequency)

Process overlapping windows to see frequencies change over time.

```c
void compute_spectrogram(float* audio, int num_samples, int sample_rate,
                        int fft_size, int hop_size) {
    Complex* fft_data = malloc(fft_size * sizeof(Complex));
    int num_bins = fft_size / 2;
    
    // Process overlapping windows
    for (int start = 0; start + fft_size <= num_samples; start += hop_size) {
        // Copy window
        for (int i = 0; i < fft_size; i++) {
            fft_data[i].real = audio[start + i];
            fft_data[i].imag = 0.0f;
        }
        
        // Window and FFT
        apply_hann_window(&audio[start], fft_size);
        fft(fft_data, fft_size);
        
        // Process spectrum for this time window
        float time = (float)start / sample_rate;
        printf("Time %.2fs: ", time);
        
        for (int i = 0; i < num_bins; i += 16) {
            float mag = sqrtf(fft_data[i].real * fft_data[i].real + 
                             fft_data[i].imag * fft_data[i].imag);
            int bar = (int)(mag * 10);
            printf("%c", " .:*#"[bar > 4 ? 4 : bar]);
        }
        printf("\n");
    }
    
    free(fft_data);
}
```

## Frequency Filtering with FFT

```c
void apply_frequency_filter(float* audio, int num_samples, int sample_rate,
                           float low_freq, float high_freq) {
    int fft_size = num_samples;  // Must be power of 2
    
    // Convert to complex
    Complex* fft_data = malloc(fft_size * sizeof(Complex));
    for (int i = 0; i < fft_size; i++) {
        fft_data[i].real = audio[i];
        fft_data[i].imag = 0.0f;
    }
    
    // Forward FFT
    fft(fft_data, fft_size);
    
    // Zero out frequencies outside range
    for (int i = 0; i < fft_size; i++) {
        float freq = (float)i * sample_rate / fft_size;
        if (freq < low_freq || freq > high_freq) {
            fft_data[i].real = 0.0f;
            fft_data[i].imag = 0.0f;
        }
    }
    
    // Inverse FFT
    ifft(fft_data, fft_size);
    
    // Convert back to real
    for (int i = 0; i < num_samples; i++) {
        audio[i] = fft_data[i].real;
    }
    
    free(fft_data);
}
```

## Performance Optimization

For better performance, use FFTW library:

```c
// Install FFTW: https://www.fftw.org/
#include <fftw3.h>

void fft_with_fftw(float* audio, int fft_size) {
    // Allocate
    fftwf_complex* in = fftwf_malloc(sizeof(fftwf_complex) * fft_size);
    fftwf_complex* out = fftwf_malloc(sizeof(fftwf_complex) * fft_size);
    
    // Create plan (can reuse for multiple FFTs)
    fftwf_plan plan = fftwf_plan_dft_1d(fft_size, in, out, 
                                        FFTW_FORWARD, FFTW_ESTIMATE);
    
    // Copy audio to input
    for (int i = 0; i < fft_size; i++) {
        in[i][0] = audio[i];  // Real
        in[i][1] = 0.0f;      // Imaginary
    }
    
    // Execute FFT
    fftwf_execute(plan);
    
    // Process output...
    
    // Cleanup
    fftwf_destroy_plan(plan);
    fftwf_free(in);
    fftwf_free(out);
}
```

## Common Applications

### Pitch Detection

```c
float detect_pitch(float* audio, int num_samples, int sample_rate) {
    // Use FFT to find dominant frequency
    // That's the pitch!
}
```

### Equalizer

```c
// Boost/cut specific frequency ranges
void apply_eq_band(Complex* fft_data, int fft_size, int sample_rate,
                  float center_freq, float bandwidth, float gain_db) {
    float gain = powf(10.0f, gain_db / 20.0f);
    
    for (int i = 0; i < fft_size / 2; i++) {
        float freq = (float)i * sample_rate / fft_size;
        float distance = fabsf(freq - center_freq);
        
        if (distance < bandwidth / 2) {
            fft_data[i].real *= gain;
            fft_data[i].imag *= gain;
        }
    }
}
```

### Noise Reduction

```c
// Estimate noise spectrum, subtract from signal
void noise_reduction(Complex* signal_fft, Complex* noise_fft, int fft_size) {
    for (int i = 0; i < fft_size; i++) {
        float signal_mag = sqrtf(signal_fft[i].real * signal_fft[i].real +
                                signal_fft[i].imag * signal_fft[i].imag);
        float noise_mag = sqrtf(noise_fft[i].real * noise_fft[i].real +
                               noise_fft[i].imag * noise_fft[i].imag);
        
        float cleaned_mag = fmaxf(signal_mag - noise_mag, 0.0f);
        
        // Preserve phase, reduce magnitude
        float scale = signal_mag > 0 ? cleaned_mag / signal_mag : 0;
        signal_fft[i].real *= scale;
        signal_fft[i].imag *= scale;
    }
}
```

## Summary

- FFT converts time domain → frequency domain
- Shows what frequencies are in the audio
- FFT size must be power of 2
- Resolution = sample_rate / fft_size
- Always apply window function before FFT
- Use magnitude for spectrum analysis: `sqrt(real² + imag²)`
- N-point FFT gives N/2 useful frequency bins
- Inverse FFT converts back: frequency → time

FFT is fundamental to audio analysis, filtering, pitch detection, noise reduction, and spectral effects!
