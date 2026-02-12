/*
 * BMP File Reader
 * 
 * Learn the fundamentals:
 * - Reading BMP file headers
 * - Parsing bitmap structure
 * - Extracting pixel data
 * - Understanding BMP format
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#pragma pack(push, 1)

typedef struct {
    uint16_t type;          // "BM"
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
    uint32_t header_size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    int32_t  x_pixels_per_m;
    int32_t  y_pixels_per_m;
    uint32_t colors_used;
    uint32_t colors_important;
} BMPHeader;

#pragma pack(pop)

void print_bmp_info(BMPHeader* header) {
    printf("=== BMP File Information ===\n\n");
    
    printf("Signature: %c%c\n", header->type & 0xFF, (header->type >> 8) & 0xFF);
    printf("File Size: %u bytes\n", header->file_size);
    printf("Data Offset: %u bytes\n", header->offset);
    printf("\n");
    
    printf("--- Image Info ---\n");
    printf("Width: %d pixels\n", header->width);
    printf("Height: %d pixels\n", abs(header->height));
    printf("Bits Per Pixel: %u\n", header->bits_per_pixel);
    printf("Compression: %u (%s)\n", header->compression,
           header->compression == 0 ? "None" : "Compressed");
    
    if (header->image_size > 0) {
        printf("Image Size: %u bytes\n", header->image_size);
    }
    
    printf("\n--- Color Info ---\n");
    printf("Color Planes: %u\n", header->planes);
    
    if (header->bits_per_pixel == 24) {
        printf("Format: 24-bit RGB (True Color)\n");
        printf("Total Colors: 16,777,216\n");
    } else if (header->bits_per_pixel == 32) {
        printf("Format: 32-bit RGBA\n");
    } else if (header->bits_per_pixel == 8) {
        printf("Format: 8-bit Indexed Color\n");
        printf("Palette Colors: %u\n", header->colors_used ? header->colors_used : 256);
    }
    
    printf("\n--- Resolution ---\n");
    if (header->x_pixels_per_m > 0) {
        int dpi_x = (int)(header->x_pixels_per_m * 0.0254);
        int dpi_y = (int)(header->y_pixels_per_m * 0.0254);
        printf("DPI: %d × %d\n", dpi_x, dpi_y);
    }
    
    printf("\n--- Memory Requirements ---\n");
    int row_size = ((header->width * header->bits_per_pixel / 8 + 3) / 4) * 4;
    int padding = row_size - (header->width * header->bits_per_pixel / 8);
    printf("Row Size (with padding): %d bytes\n", row_size);
    printf("Padding per row: %d bytes\n", padding);
    printf("Total pixel data: %d bytes\n", row_size * abs(header->height));
}

void analyze_pixels(uint8_t* pixels, int width, int height) {
    printf("\n=== Pixel Analysis ===\n\n");
    
    long long sum_r = 0, sum_g = 0, sum_b = 0;
    uint8_t min_r = 255, min_g = 255, min_b = 255;
    uint8_t max_r = 0, max_g = 0, max_b = 0;
    
    for (int i = 0; i < width * height; i++) {
        uint8_t r = pixels[i*3];
        uint8_t g = pixels[i*3+1];
        uint8_t b = pixels[i*3+2];
        
        sum_r += r; sum_g += g; sum_b += b;
        if (r < min_r) min_r = r;
        if (g < min_g) min_g = g;
        if (b < min_b) min_b = b;
        if (r > max_r) max_r = r;
        if (g > max_g) max_g = g;
        if (b > max_b) max_b = b;
    }
    
    int total_pixels = width * height;
    printf("Average Color: RGB(%d, %d, %d)\n",
           (int)(sum_r / total_pixels),
           (int)(sum_g / total_pixels),
           (int)(sum_b / total_pixels));
    
    printf("Min Values: RGB(%d, %d, %d)\n", min_r, min_g, min_b);
    printf("Max Values: RGB(%d, %d, %d)\n", max_r, max_g, max_b);
    
    // Brightness
    int brightness = (int)((sum_r + sum_g + sum_b) / (total_pixels * 3));
    printf("Average Brightness: %d / 255 (%.1f%%)\n", brightness, brightness * 100.0f / 255);
}

// Try to open file from multiple possible paths
FILE* try_open_image(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file) return file;
    
    // If it's a relative path starting with "assets/", try from parent directory
    if (strncmp(filename, "assets/", 7) == 0) {
        char alt_path[512];
        snprintf(alt_path, sizeof(alt_path), "../%s", filename);
        file = fopen(alt_path, "rb");
        if (file) return file;
    }
    
    return NULL;
}

int read_bmp(const char* filename) {
    printf("Reading BMP file: %s\n\n", filename);
    
    FILE* file = try_open_image(filename);
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        fprintf(stderr, "Make sure the file exists and you're running from the correct directory.\n");
        fprintf(stderr, "Try running from the 'examples' directory, not from 'bin'.\n");
        return -1;
    }
    
    printf("Successfully opened file!\n\n");
    
    // Read header
    BMPHeader header;
    if (fread(&header, sizeof(header), 1, file) != 1) {
        fprintf(stderr, "Failed to read BMP header\n");
        fclose(file);
        return -1;
    }
    
    // Verify BMP
    if (header.type != 0x4D42) {
        fprintf(stderr, "Not a BMP file (invalid signature)\n");
        fclose(file);
        return -1;
    }
    
    print_bmp_info(&header);
    
    // Read pixel data (if 24-bit)
    if (header.bits_per_pixel == 24) {
        fseek(file, header.offset, SEEK_SET);
        
        int row_size = ((header.width * 3 + 3) / 4) * 4;
        uint8_t* row_data = malloc(row_size);
        uint8_t* pixels = malloc(header.width * abs(header.height) * 3);
        
        // Read rows (bottom-up in BMP)
        for (int y = 0; y < abs(header.height); y++) {
            fread(row_data, row_size, 1, file);
            
            // Convert BGR to RGB
            int img_y = abs(header.height) - 1 - y;
            for (int x = 0; x < header.width; x++) {
                int idx = (img_y * header.width + x) * 3;
                pixels[idx] = row_data[x*3 + 2];      // R (from B)
                pixels[idx+1] = row_data[x*3 + 1];    // G
                pixels[idx+2] = row_data[x*3];        // B (from R)
            }
        }
        
        analyze_pixels(pixels, header.width, abs(header.height));
        
        free(row_data);
        free(pixels);
    }
    
    fclose(file);
    return 0;
}

int main(int argc, char* argv[]) {
    printf("=== BMP File Reader ===\n\n");
    
    const char* filename;
    
    if (argc < 2) {
        // Use example image
        filename = "assets/example-image.bmp";
        printf("Reading example file: %s\n", filename);
        printf("(You can also specify your own file: %s <filename.bmp>)\n\n", argv[0]);
    } else {
        filename = argv[1];
    }
    
    if (read_bmp(filename) != 0) {
        return 1;
    }
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
