/*
 * Color Operations
 * 
 * Learn color manipulation:
 * - Brightness and contrast
 * - Grayscale conversion
 * - Sepia tone
 * - Color inversion
 * - Saturation adjustments
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t type; uint32_t file_size; uint16_t reserved1; uint16_t reserved2;
    uint32_t offset; uint32_t header_size; int32_t width; int32_t height;
    uint16_t planes; uint16_t bits_per_pixel; uint32_t compression;
    uint32_t image_size; int32_t x_pixels_per_m; int32_t y_pixels_per_m;
    uint32_t colors_used; uint32_t colors_important;
} BMPHeader;
#pragma pack(pop)

typedef struct { int width; int height; uint8_t* pixels; } Image;

Image* read_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;
    
    BMPHeader header;
    fread(&header, sizeof(header), 1, file);
    if (header.type != 0x4D42 || header.bits_per_pixel != 24) {
        fclose(file); return NULL;
    }
    
    Image* img = malloc(sizeof(Image));
    img->width = header.width; img->height = abs(header.height);
    img->pixels = malloc(img->width * img->height * 3);
    
    fseek(file, header.offset, SEEK_SET);
    int row_size = ((img->width * 3 + 3) / 4) * 4;
    uint8_t* row = malloc(row_size);
    
    for (int y = 0; y < img->height; y++) {
        fread(row, row_size, 1, file);
        int img_y = img->height - 1 - y;
        for (int x = 0; x < img->width; x++) {
            int idx = (img_y * img->width + x) * 3;
            img->pixels[idx] = row[x*3+2]; img->pixels[idx+1] = row[x*3+1]; img->pixels[idx+2] = row[x*3];
        }
    }
    free(row); fclose(file);
    return img;
}

int write_bmp(const char* filename, Image* img) {
    FILE* file = fopen(filename, "wb");
    if (!file) return -1;
    
    int row_size = ((img->width * 3 + 3) / 4) * 4;
    int padding = row_size - img->width * 3;
    
    BMPHeader header = {0};
    header.type = 0x4D42; header.file_size = 54 + row_size * img->height;
    header.offset = 54; header.header_size = 40; header.width = img->width;
    header.height = img->height; header.planes = 1; header.bits_per_pixel = 24;
    header.x_pixels_per_m = 2835; header.y_pixels_per_m = 2835;
    fwrite(&header, sizeof(header), 1, file);
    
    uint8_t pad[3] = {0};
    for (int y = img->height - 1; y >= 0; y--) {
        for (int x = 0; x < img->width; x++) {
            int idx = (y * img->width + x) * 3;
            fputc(img->pixels[idx+2], file); fputc(img->pixels[idx+1], file); fputc(img->pixels[idx], file);
        }
        fwrite(pad, padding, 1, file);
    }
    fclose(file);
    return 0;
}

Image* copy_image(Image* src) {
    Image* img = malloc(sizeof(Image));
    img->width = src->width; img->height = src->height;
    img->pixels = malloc(img->width * img->height * 3);
    memcpy(img->pixels, src->pixels, img->width * img->height * 3);
    return img;
}

void brightness(Image* img, int amount) {
    for (int i = 0; i < img->width * img->height * 3; i++) {
        int val = img->pixels[i] + amount;
        img->pixels[i] = (val < 0) ? 0 : (val > 255) ? 255 : val;
    }
}

void contrast(Image* img, float factor) {
    for (int i = 0; i < img->width * img->height * 3; i++) {
        int val = (int)((img->pixels[i] - 128) * factor + 128);
        img->pixels[i] = (val < 0) ? 0 : (val > 255) ? 255 : val;
    }
}

void grayscale(Image* img) {
    for (int i = 0; i < img->width * img->height; i++) {
        int idx = i * 3;
        uint8_t gray = (uint8_t)(0.299f * img->pixels[idx] + 0.587f * img->pixels[idx+1] + 0.114f * img->pixels[idx+2]);
        img->pixels[idx] = img->pixels[idx+1] = img->pixels[idx+2] = gray;
    }
}

void sepia(Image* img) {
    for (int i = 0; i < img->width * img->height; i++) {
        int idx = i * 3;
        uint8_t r = img->pixels[idx], g = img->pixels[idx+1], b = img->pixels[idx+2];
        int tr = (int)(0.393*r + 0.769*g + 0.189*b);
        int tg = (int)(0.349*r + 0.686*g + 0.168*b);
        int tb = (int)(0.272*r + 0.534*g + 0.131*b);
        img->pixels[idx] = (tr > 255) ? 255 : tr;
        img->pixels[idx+1] = (tg > 255) ? 255 : tg;
        img->pixels[idx+2] = (tb > 255) ? 255 : tb;
    }
}

void invert(Image* img) {
    for (int i = 0; i < img->width * img->height * 3; i++) {
        img->pixels[i] = 255 - img->pixels[i];
    }
}

void threshold(Image* img, uint8_t thresh) {
    for (int i = 0; i < img->width * img->height; i++) {
        int idx = i * 3;
        uint8_t gray = (img->pixels[idx] + img->pixels[idx+1] + img->pixels[idx+2]) / 3;
        uint8_t value = (gray >= thresh) ? 255 : 0;
        img->pixels[idx] = img->pixels[idx+1] = img->pixels[idx+2] = value;
    }
}

void gamma_correction(Image* img, float gamma) {
    uint8_t lookup[256];
    for (int i = 0; i < 256; i++) {
        lookup[i] = (uint8_t)(powf(i / 255.0f, gamma) * 255.0f);
    }
    for (int i = 0; i < img->width * img->height * 3; i++) {
        img->pixels[i] = lookup[img->pixels[i]];
    }
}

int main(int argc, char* argv[]) {
    printf("=== Color Operations ===\n\n");
    
    const char* input_file = (argc > 1) ? argv[1] : "assets/example-image.bmp";
    printf("Loading: %s\n", input_file);
    
    Image* original = read_bmp(input_file);
    if (!original) {
        fprintf(stderr, "Failed to load image. Make sure it's a 24-bit BMP file.\n");
        return 1;
    }
    printf("Loaded %dx%d image\n\n", original->width, original->height);
    
    printf("Applying color operations...\n");
    
    printf("  1. Brightness +50...\n");
    Image* img = copy_image(original);
    brightness(img, 50);
    write_bmp("color_01_bright.bmp", img);
    free(img->pixels); free(img);
    
    printf("  2. Brightness -50...\n");
    img = copy_image(original);
    brightness(img, -50);
    write_bmp("color_02_dark.bmp", img);
    free(img->pixels); free(img);
    
    printf("  3. High contrast...\n");
    img = copy_image(original);
    contrast(img, 2.0f);
    write_bmp("color_03_high_contrast.bmp", img);
    free(img->pixels); free(img);
    
    printf("  4. Low contrast...\n");
    img = copy_image(original);
    contrast(img, 0.5f);
    write_bmp("color_04_low_contrast.bmp", img);
    free(img->pixels); free(img);
    
    printf("  5. Grayscale...\n");
    img = copy_image(original);
    grayscale(img);
    write_bmp("color_05_grayscale.bmp", img);
    free(img->pixels); free(img);
    
    printf("  6. Sepia tone...\n");
    img = copy_image(original);
    sepia(img);
    write_bmp("color_06_sepia.bmp", img);
    free(img->pixels); free(img);
    
    printf("  7. Invert (negative)...\n");
    img = copy_image(original);
    invert(img);
    write_bmp("color_07_invert.bmp", img);
    free(img->pixels); free(img);
    
    printf("  8. Threshold (binary)...\n");
    img = copy_image(original);
    threshold(img, 128);
    write_bmp("color_08_threshold.bmp", img);
    free(img->pixels); free(img);
    
    printf("  9. Gamma correction (darker)...\n");
    img = copy_image(original);
    gamma_correction(img, 1.5f);
    write_bmp("color_09_gamma_dark.bmp", img);
    free(img->pixels); free(img);
    
    printf("  10. Gamma correction (brighter)...\n");
    img = copy_image(original);
    gamma_correction(img, 0.7f);
    write_bmp("color_10_gamma_bright.bmp", img);
    free(img->pixels); free(img);
    
    free(original->pixels); free(original);
    
    printf("\n=== Summary ===\n");
    printf("Created 10 color-adjusted images:\n");
    printf("  color_01_bright.bmp        - Brightness +50\n");
    printf("  color_02_dark.bmp          - Brightness -50\n");
    printf("  color_03_high_contrast.bmp - High contrast\n");
    printf("  color_04_low_contrast.bmp  - Low contrast\n");
    printf("  color_05_grayscale.bmp     - Grayscale\n");
    printf("  color_06_sepia.bmp         - Sepia tone\n");
    printf("  color_07_invert.bmp        - Negative\n");
    printf("  color_08_threshold.bmp     - Binary threshold\n");
    printf("  color_09_gamma_dark.bmp    - Gamma darker\n");
    printf("  color_10_gamma_bright.bmp  - Gamma brighter\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
