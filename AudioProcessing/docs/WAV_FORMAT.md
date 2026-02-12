# WAV Format

## The Big Picture

WAV (Waveform Audio File Format) is a container for uncompressed PCM audio. It uses the RIFF (Resource Interchange File Format) structure. Understanding WAV format lets you read and write audio files without libraries.

## File Structure

A WAV file has three main parts:

```
┌─────────────────────┐
│   RIFF Header       │ 12 bytes
├─────────────────────┤
│   fmt Chunk         │ 24+ bytes (format info)
├─────────────────────┤
│   data Chunk        │ 8 bytes + audio data
└─────────────────────┘
```

## RIFF Header

First 12 bytes of the file:

```c
typedef struct {
    char     chunk_id[4];     // "RIFF"
    uint32_t chunk_size;      // File size - 8
    char     format[4];       // "WAVE"
} WavRiffHeader;

// Example in hex:
// 52 49 46 46  ("RIFF")
// xx xx xx xx  (file size - 8)
// 57 41 56 45  ("WAVE")
```

## fmt Chunk

Describes the audio format (usually 24 bytes):

```c
typedef struct {
    char     chunk_id[4];        // "fmt "
    uint32_t chunk_size;         // 16 for PCM
    uint16_t audio_format;       // 1 = PCM, 3 = IEEE float
    uint16_t num_channels;       // 1 = mono, 2 = stereo
    uint32_t sample_rate;        // e.g., 44100
    uint32_t byte_rate;          // sample_rate * num_channels * bytes_per_sample
    uint16_t block_align;        // num_channels * bytes_per_sample
    uint16_t bits_per_sample;    // 8, 16, 24, or 32
} WavFmtChunk;
```

### Audio Format Values

- `1` = PCM (integer samples)
- `3` = IEEE Float (32-bit float)
- `6` = A-law (compressed)
- `7` = μ-law (compressed)

Most common is PCM (1).

### Calculating Values

```c
// For 44100 Hz, stereo, 16-bit:
sample_rate = 44100
num_channels = 2
bits_per_sample = 16

bytes_per_sample = bits_per_sample / 8 = 2
block_align = num_channels * bytes_per_sample = 2 * 2 = 4
byte_rate = sample_rate * block_align = 44100 * 4 = 176400
```

## data Chunk

Contains the actual audio samples:

```c
typedef struct {
    char     chunk_id[4];     // "data"
    uint32_t chunk_size;      // Size of audio data in bytes
    // Followed by audio samples
} WavDataChunk;

// chunk_size = num_samples * num_channels * bytes_per_sample
```

## Complete WAV Structure in C

```c
#include <stdint.h>

#pragma pack(push, 1)  // Ensure no padding

typedef struct {
    // RIFF Header
    char     riff_id[4];           // "RIFF"
    uint32_t file_size;            // File size - 8
    char     wave_id[4];           // "WAVE"
    
    // fmt Chunk
    char     fmt_id[4];            // "fmt "
    uint32_t fmt_size;             // 16 for PCM
    uint16_t audio_format;         // 1 = PCM
    uint16_t num_channels;         // 1 or 2
    uint32_t sample_rate;          // e.g., 44100
    uint32_t byte_rate;            // sample_rate * block_align
    uint16_t block_align;          // num_channels * bytes_per_sample
    uint16_t bits_per_sample;      // 16
    
    // data Chunk Header
    char     data_id[4];           // "data"
    uint32_t data_size;            // Size of audio data
    
    // Audio samples follow here...
} WavHeader;

#pragma pack(pop)
```

## Reading a WAV File

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int sample_rate;
    int num_channels;
    int bits_per_sample;
    int num_samples;
    void* data;
} AudioData;

AudioData* read_wav(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }
    
    WavHeader header;
    if (fread(&header, sizeof(header), 1, file) != 1) {
        fprintf(stderr, "Failed to read header\n");
        fclose(file);
        return NULL;
    }
    
    // Verify it's a WAV file
    if (memcmp(header.riff_id, "RIFF", 4) != 0 ||
        memcmp(header.wave_id, "WAVE", 4) != 0 ||
        memcmp(header.fmt_id, "fmt ", 4) != 0 ||
        memcmp(header.data_id, "data", 4) != 0) {
        fprintf(stderr, "Not a valid WAV file\n");
        fclose(file);
        return NULL;
    }
    
    // Allocate audio data
    AudioData* audio = malloc(sizeof(AudioData));
    audio->sample_rate = header.sample_rate;
    audio->num_channels = header.num_channels;
    audio->bits_per_sample = header.bits_per_sample;
    audio->num_samples = header.data_size / (header.num_channels * (header.bits_per_sample / 8));
    audio->data = malloc(header.data_size);
    
    // Read audio samples
    if (fread(audio->data, header.data_size, 1, file) != 1) {
        fprintf(stderr, "Failed to read audio data\n");
        free(audio->data);
        free(audio);
        fclose(file);
        return NULL;
    }
    
    fclose(file);
    return audio;
}
```

## Writing a WAV File

```c
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
    
    // RIFF header
    memcpy(header.riff_id, "RIFF", 4);
    header.file_size = 36 + data_size;
    memcpy(header.wave_id, "WAVE", 4);
    
    // fmt chunk
    memcpy(header.fmt_id, "fmt ", 4);
    header.fmt_size = 16;
    header.audio_format = 1;  // PCM
    header.num_channels = num_channels;
    header.sample_rate = sample_rate;
    header.bits_per_sample = 16;
    header.block_align = num_channels * bytes_per_sample;
    header.byte_rate = sample_rate * header.block_align;
    
    // data chunk
    memcpy(header.data_id, "data", 4);
    header.data_size = data_size;
    
    // Write header
    if (fwrite(&header, sizeof(header), 1, file) != 1) {
        fprintf(stderr, "Failed to write header\n");
        fclose(file);
        return -1;
    }
    
    // Write samples
    if (fwrite(samples, data_size, 1, file) != 1) {
        fprintf(stderr, "Failed to write samples\n");
        fclose(file);
        return -1;
    }
    
    fclose(file);
    return 0;
}
```

## Sample Data Layout

### 16-bit Mono

```
Sample 0: [byte0][byte1]
Sample 1: [byte0][byte1]
Sample 2: [byte0][byte1]
...

Each sample is signed 16-bit little-endian.
```

### 16-bit Stereo (Interleaved)

```
Frame 0: [L_byte0][L_byte1][R_byte0][R_byte1]
Frame 1: [L_byte0][L_byte1][R_byte0][R_byte1]
Frame 2: [L_byte0][L_byte1][R_byte0][R_byte1]
...

Left and right samples alternate.
```

### Extracting Samples

```c
// 16-bit mono
int16_t get_mono_sample(uint8_t* data, int index) {
    return *(int16_t*)&data[index * 2];
}

// 16-bit stereo
void get_stereo_sample(uint8_t* data, int frame, int16_t* left, int16_t* right) {
    int offset = frame * 4;  // 4 bytes per frame
    *left = *(int16_t*)&data[offset + 0];
    *right = *(int16_t*)&data[offset + 2];
}
```

## Endianness

WAV files use **little-endian** byte order (least significant byte first).

```c
// Example: Value 1000 (0x03E8) in 16-bit:
// Little-endian (WAV): E8 03
// Big-endian: 03 E8

// Most modern CPUs are little-endian, so usually no conversion needed.
// If needed:
uint16_t swap_endian_16(uint16_t value) {
    return (value >> 8) | (value << 8);
}

uint32_t swap_endian_32(uint32_t value) {
    return ((value >> 24) & 0x000000FF) |
           ((value >> 8)  & 0x0000FF00) |
           ((value << 8)  & 0x00FF0000) |
           ((value << 24) & 0xFF000000);
}
```

## 8-bit Audio

8-bit WAV uses **unsigned** samples (0-255, center = 128):

```c
uint8_t sample_8bit = 200;  // Louder than center

// Convert to signed:
int8_t signed_sample = sample_8bit - 128;  // 200 - 128 = 72
```

## 24-bit Audio

24-bit samples are stored in 3 bytes (no 24-bit integer type in C):

```c
int32_t read_24bit_sample(uint8_t* data, int index) {
    int offset = index * 3;
    int32_t value = (data[offset + 0]) |
                    (data[offset + 1] << 8) |
                    (data[offset + 2] << 16);
    
    // Sign extend (24-bit to 32-bit)
    if (value & 0x800000) {
        value |= 0xFF000000;
    }
    
    return value;
}

void write_24bit_sample(uint8_t* data, int index, int32_t value) {
    int offset = index * 3;
    data[offset + 0] = value & 0xFF;
    data[offset + 1] = (value >> 8) & 0xFF;
    data[offset + 2] = (value >> 16) & 0xFF;
}
```

## 32-bit Float WAV

For floating-point audio (audio_format = 3):

```c
typedef struct {
    // ... same RIFF and WAVE headers ...
    uint16_t audio_format;  // 3 (IEEE float)
    // ... rest of fmt chunk ...
} WavHeaderFloat;

// Samples are already float:
float sample = *(float*)&data[index * 4];
```

## Extended Formats

Some WAV files have extra chunks (LIST, INFO, etc.). To handle these:

```c
typedef struct {
    char     chunk_id[4];
    uint32_t chunk_size;
} ChunkHeader;

void skip_chunk(FILE* file, ChunkHeader* chunk) {
    fseek(file, chunk->chunk_size, SEEK_CUR);
}

// Read until you find "data" chunk:
ChunkHeader chunk;
while (fread(&chunk, sizeof(chunk), 1, file) == 1) {
    if (memcmp(chunk.chunk_id, "data", 4) == 0) {
        // Found data chunk!
        break;
    }
    skip_chunk(file, &chunk);
}
```

## Common Issues

### 1. File Size Calculation

```c
// Correct:
file_size = 36 + data_size;  // Everything except first 8 bytes

// data_size = num_samples * num_channels * bytes_per_sample
```

### 2. Sample vs Frame

- **Sample:** Single value (one channel at one time)
- **Frame:** All channels at one time (stereo frame = 2 samples)

```c
// Stereo:
num_frames = num_samples / num_channels;
```

### 3. Struct Padding

Always use `#pragma pack(push, 1)` to prevent compiler from adding padding bytes.

### 4. Signed vs Unsigned

- 8-bit: Unsigned (0-255)
- 16-bit, 24-bit, 32-bit: Signed

## Quick Reference

```c
// File size
file_size = 36 + (num_samples * num_channels * bytes_per_sample)

// Byte rate
byte_rate = sample_rate * num_channels * bytes_per_sample

// Block align
block_align = num_channels * bytes_per_sample

// Duration
duration_seconds = num_samples / (sample_rate * num_channels)

// Convert sample index to time
time = (float)sample_index / (sample_rate * num_channels)
```

## Summary

- WAV = RIFF + fmt + data chunks
- RIFF header: "RIFF" + size + "WAVE"
- fmt chunk: Audio format info (sample rate, channels, bit depth)
- data chunk: Raw PCM samples
- Little-endian byte order
- Use `#pragma pack(push, 1)` for structs
- Always validate chunk IDs
- Handle extra chunks by skipping them

WAV format is simple once you understand RIFF structure. With this knowledge, you can read and write WAV files without any libraries!
