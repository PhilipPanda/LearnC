/*
 * WAV File Reader
 * 
 * Learn the fundamentals:
 * - Reading WAV file headers
 * - Parsing RIFF structure
 * - Extracting audio metadata
 * - Reading PCM samples
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#pragma pack(push, 1)

typedef struct {
    char     riff_id[4];        // "RIFF"
    uint32_t file_size;         // File size - 8
    char     wave_id[4];        // "WAVE"
} RiffHeader;

typedef struct {
    char     chunk_id[4];
    uint32_t chunk_size;
} ChunkHeader;

typedef struct {
    uint16_t audio_format;      // 1 = PCM
    uint16_t num_channels;      // 1 = mono, 2 = stereo
    uint32_t sample_rate;       // e.g., 44100
    uint32_t byte_rate;         // sample_rate * block_align
    uint16_t block_align;       // num_channels * bytes_per_sample
    uint16_t bits_per_sample;   // 8, 16, 24, or 32
} FmtChunk;

typedef struct {
    uint32_t file_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data_size;
    long data_offset;           // File position where data starts
} WavInfo;

#pragma pack(pop)

// Print WAV file information
void print_wav_info(const WavInfo* info) {
    printf("=== WAV File Information ===\n\n");
    
    printf("Format:          WAV (RIFF)\n");
    printf("File Size:       %u bytes\n", info->file_size + 8);
    printf("\n");
    
    // Format chunk
    printf("--- Audio Format ---\n");
    printf("Audio Format:    %u (%s)\n", info->audio_format,
           info->audio_format == 1 ? "PCM" : "Other");
    printf("Channels:        %u (%s)\n", info->num_channels,
           info->num_channels == 1 ? "Mono" : info->num_channels == 2 ? "Stereo" : "Multichannel");
    printf("Sample Rate:     %u Hz\n", info->sample_rate);
    printf("Bit Depth:       %u bits\n", info->bits_per_sample);
    printf("Byte Rate:       %u bytes/sec\n", info->byte_rate);
    printf("Block Align:     %u bytes\n", info->block_align);
    printf("\n");
    
    // Data chunk
    printf("--- Audio Data ---\n");
    printf("Data Size:       %u bytes\n", info->data_size);
    
    int bytes_per_sample = info->bits_per_sample / 8;
    int num_samples = info->data_size / (info->num_channels * bytes_per_sample);
    float duration = (float)num_samples / info->sample_rate;
    
    printf("Total Samples:   %d (per channel)\n", num_samples);
    printf("Duration:        %.2f seconds\n", duration);
    printf("\n");
    
    // Quality assessment
    printf("--- Quality ---\n");
    if (info->sample_rate >= 44100 && info->bits_per_sample >= 16) {
        printf("Quality:         CD Quality or better\n");
    } else if (info->sample_rate >= 22050 && info->bits_per_sample >= 16) {
        printf("Quality:         Good\n");
    } else {
        printf("Quality:         Low\n");
    }
    
    // Bitrate
    int bitrate = info->sample_rate * info->num_channels * info->bits_per_sample;
    printf("Bitrate:         %d kbps (uncompressed)\n", bitrate / 1000);
}

// Analyze audio samples
void analyze_samples(const int16_t* samples, int num_samples, int num_channels) {
    printf("\n=== Audio Analysis ===\n\n");
    
    int total_samples = num_samples * num_channels;
    
    // Find min, max, and average
    int16_t min_sample = samples[0];
    int16_t max_sample = samples[0];
    int64_t sum = 0;
    
    for (int i = 0; i < total_samples; i++) {
        if (samples[i] < min_sample) min_sample = samples[i];
        if (samples[i] > max_sample) max_sample = samples[i];
        sum += abs(samples[i]);
    }
    
    float avg_amplitude = (float)sum / total_samples;
    
    int16_t peak_sample = (abs(min_sample) > abs(max_sample)) ? abs(min_sample) : abs(max_sample);
    
    printf("Min Sample:      %d\n", min_sample);
    printf("Max Sample:      %d\n", max_sample);
    printf("Peak Amplitude:  %.2f%%\n", (float)peak_sample / 32768.0f * 100.0f);
    printf("Avg Amplitude:   %.2f\n", avg_amplitude);
    
    // Check for clipping
    int clipped_samples = 0;
    for (int i = 0; i < total_samples; i++) {
        if (samples[i] == 32767 || samples[i] == -32768) {
            clipped_samples++;
        }
    }
    
    if (clipped_samples > 0) {
        printf("\nWarning: %d samples are clipped (%.2f%%)\n",
               clipped_samples, (float)clipped_samples / total_samples * 100.0f);
    } else {
        printf("\nNo clipping detected.\n");
    }
}

// Try to open file from multiple possible paths
FILE* try_open_file(const char* filename, char* resolved_path, size_t path_size) {
    // Try exact path first
    FILE* file = fopen(filename, "rb");
    if (file) {
        snprintf(resolved_path, path_size, "%s", filename);
        return file;
    }
    
    // If it's a relative path starting with "assets/", try from parent directory
    if (strncmp(filename, "assets/", 7) == 0) {
        char alt_path[512];
        snprintf(alt_path, sizeof(alt_path), "../%s", filename);
        file = fopen(alt_path, "rb");
        if (file) {
            snprintf(resolved_path, path_size, "%s", alt_path);
            return file;
        }
    }
    
    return NULL;
}

// Read and analyze WAV file
void read_wav(const char* filename) {
    printf("Reading WAV file: %s\n\n", filename);
    
    char resolved_path[512];
    FILE* file = try_open_file(filename, resolved_path, sizeof(resolved_path));
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        fprintf(stderr, "Make sure the file exists and you're running from the correct directory.\n");
        fprintf(stderr, "Try running from the 'examples' directory, not from 'bin'.\n");
        return;
    }
    
    printf("Successfully opened: %s\n\n", resolved_path);
    
    // Read RIFF header
    RiffHeader riff;
    if (fread(&riff, sizeof(riff), 1, file) != 1) {
        fprintf(stderr, "Failed to read RIFF header\n");
        fclose(file);
        return;
    }
    
    // Verify RIFF/WAVE
    if (memcmp(riff.riff_id, "RIFF", 4) != 0) {
        fprintf(stderr, "Error: Not a valid RIFF file\n");
        fclose(file);
        return;
    }
    
    if (memcmp(riff.wave_id, "WAVE", 4) != 0) {
        fprintf(stderr, "Error: Not a WAVE file\n");
        fclose(file);
        return;
    }
    
    // Initialize WAV info
    WavInfo info = {0};
    info.file_size = riff.file_size;
    int found_fmt = 0;
    int found_data = 0;
    
    // Read chunks until we find fmt and data
    while (!feof(file) && (!found_fmt || !found_data)) {
        ChunkHeader chunk;
        if (fread(&chunk, sizeof(chunk), 1, file) != 1) {
            break;  // End of file or error
        }
        
        if (memcmp(chunk.chunk_id, "fmt ", 4) == 0) {
            // Read format chunk
            FmtChunk fmt;
            size_t to_read = chunk.chunk_size < sizeof(fmt) ? chunk.chunk_size : sizeof(fmt);
            if (fread(&fmt, to_read, 1, file) == 1) {
                info.audio_format = fmt.audio_format;
                info.num_channels = fmt.num_channels;
                info.sample_rate = fmt.sample_rate;
                info.byte_rate = fmt.byte_rate;
                info.block_align = fmt.block_align;
                info.bits_per_sample = fmt.bits_per_sample;
                found_fmt = 1;
            }
            
            // Skip any extra bytes in fmt chunk
            if (chunk.chunk_size > sizeof(fmt)) {
                fseek(file, chunk.chunk_size - sizeof(fmt), SEEK_CUR);
            }
        } else if (memcmp(chunk.chunk_id, "data", 4) == 0) {
            // Found data chunk
            info.data_size = chunk.chunk_size;
            info.data_offset = ftell(file);
            found_data = 1;
            // Don't read data yet, just note where it is
            break;
        } else {
            // Skip unknown chunk
            printf("Skipping chunk: %.4s (%u bytes)\n", chunk.chunk_id, chunk.chunk_size);
            fseek(file, chunk.chunk_size, SEEK_CUR);
            
            // WAV chunks are word-aligned, skip padding byte if odd size
            if (chunk.chunk_size % 2 == 1) {
                fseek(file, 1, SEEK_CUR);
            }
        }
    }
    
    if (!found_fmt) {
        fprintf(stderr, "Error: fmt chunk not found\n");
        fclose(file);
        return;
    }
    
    if (!found_data) {
        fprintf(stderr, "Error: data chunk not found\n");
        fclose(file);
        return;
    }
    
    // Print information
    printf("\n");
    print_wav_info(&info);
    
    // Seek to data and read samples for analysis
    fseek(file, info.data_offset, SEEK_SET);
    
    int bytes_per_sample = info.bits_per_sample / 8;
    int num_samples = info.data_size / (info.num_channels * bytes_per_sample);
    int samples_to_read = num_samples < 100000 ? num_samples : 100000;
    
    if (info.bits_per_sample == 16) {
        int16_t* samples = malloc(samples_to_read * info.num_channels * sizeof(int16_t));
        if (samples) {
            size_t read_size = samples_to_read * info.num_channels * sizeof(int16_t);
            if (fread(samples, 1, read_size, file) == read_size) {
                analyze_samples(samples, samples_to_read, info.num_channels);
            }
            free(samples);
        }
    } else {
        printf("\nNote: Sample analysis only supports 16-bit audio.\n");
    }
    
    fclose(file);
}

int main(int argc, char* argv[]) {
    printf("=== WAV File Reader ===\n\n");
    
    const char* filename;
    
    if (argc > 1) {
        // Read user-specified file
        filename = argv[1];
    } else {
        // Use example audio file
        filename = "assets/example-audio.wav";
        printf("Reading example file: %s\n", filename);
        printf("(You can also specify your own file: %s <filename.wav>)\n\n", argv[0]);
    }
    
    read_wav(filename);
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
