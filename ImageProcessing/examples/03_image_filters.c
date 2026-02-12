/*
 * Image Filters
 * 
 * Learn convolution and filtering:
 * - Blur (box and Gaussian)
 * - Sharpen
 * - Edge detection (Sobel)
 * - Emboss effect
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
typedef struct { float values[9]; } Kernel3x3;

Image* read_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;
    
    BMPHeader header;
    fread(&header, sizeof(header), 1, file);
    if (header.type != 0x4D42 || header.bits_per_pixel != 24) {
        fclose(file); return NULL;
    }
    
    Image* img = malloc(sizeof(Image));
    img->width = header.width;
    img->height = abs(header.height);
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

uint8_t apply_kernel_channel(Image* img, int x, int y, int ch, Kernel3x3* kernel) {
    float sum = 0; int idx = 0;
    for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
            int px = x + kx, py = y + ky;
            if (px < 0) px = 0; if (px >= img->width) px = img->width - 1;
            if (py < 0) py = 0; if (py >= img->height) py = img->height - 1;
            sum += img->pixels[(py * img->width + px) * 3 + ch] * kernel->values[idx++];
        }
    }
    if (sum < 0) sum = 0; if (sum > 255) sum = 255;
    return (uint8_t)sum;
}

void apply_filter(Image* img, Kernel3x3* kernel) {
    uint8_t* output = malloc(img->width * img->height * 3);
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int idx = (y * img->width + x) * 3;
            output[idx] = apply_kernel_channel(img, x, y, 0, kernel);
            output[idx+1] = apply_kernel_channel(img, x, y, 1, kernel);
            output[idx+2] = apply_kernel_channel(img, x, y, 2, kernel);
        }
    }
    memcpy(img->pixels, output, img->width * img->height * 3);
    free(output);
}

void blur(Image* img) {
    Kernel3x3 kernel = {{1/9.0f, 1/9.0f, 1/9.0f, 1/9.0f, 1/9.0f, 1/9.0f, 1/9.0f, 1/9.0f, 1/9.0f}};
    apply_filter(img, &kernel);
}

void gaussian_blur(Image* img) {
    Kernel3x3 kernel = {{1/16.0f, 2/16.0f, 1/16.0f, 2/16.0f, 4/16.0f, 2/16.0f, 1/16.0f, 2/16.0f, 1/16.0f}};
    apply_filter(img, &kernel);
}

void sharpen(Image* img) {
    Kernel3x3 kernel = {{0, -1, 0, -1, 5, -1, 0, -1, 0}};
    apply_filter(img, &kernel);
}

void edge_detect(Image* img) {
    Kernel3x3 sobel_x = {{-1, 0, 1, -2, 0, 2, -1, 0, 1}};
    Kernel3x3 sobel_y = {{-1, -2, -1, 0, 0, 0, 1, 2, 1}};
    uint8_t* output = malloc(img->width * img->height * 3);
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            float gx = apply_kernel_channel(img, x, y, 0, &sobel_x);
            float gy = apply_kernel_channel(img, x, y, 0, &sobel_y);
            float magnitude = sqrtf(gx*gx + gy*gy);
            if (magnitude > 255) magnitude = 255;
            int idx = (y * img->width + x) * 3;
            output[idx] = output[idx+1] = output[idx+2] = (uint8_t)magnitude;
        }
    }
    memcpy(img->pixels, output, img->width * img->height * 3);
    free(output);
}

void emboss(Image* img) {
    Kernel3x3 kernel = {{-2, -1, 0, -1, 1, 1, 0, 1, 2}};
    apply_filter(img, &kernel);
    for (int i = 0; i < img->width * img->height * 3; i++) {
        int val = img->pixels[i] + 128;
        img->pixels[i] = (val > 255) ? 255 : val;
    }
}

Image* copy_image(Image* src) {
    Image* img = malloc(sizeof(Image));
    img->width = src->width; img->height = src->height;
    img->pixels = malloc(img->width * img->height * 3);
    memcpy(img->pixels, src->pixels, img->width * img->height * 3);
    return img;
}

int main(int argc, char* argv[]) {
    printf("=== Image Filters ===\n\n");
    
    const char* input_file = (argc > 1) ? argv[1] : "assets/example-image.bmp";
    printf("Loading: %s\n", input_file);
    
    Image* original = read_bmp(input_file);
    if (!original) {
        fprintf(stderr, "Failed to load image. Make sure it's a 24-bit BMP file.\n");
        return 1;
    }
    printf("Loaded %dx%d image\n\n", original->width, original->height);
    
    printf("Applying filters...\n");
    
    printf("  1. Box blur...\n");
    Image* img = copy_image(original);
    blur(img);
    write_bmp("filter_01_blur.bmp", img);
    free(img->pixels); free(img);
    
    printf("  2. Gaussian blur...\n");
    img = copy_image(original);
    gaussian_blur(img);
    write_bmp("filter_02_gaussian.bmp", img);
    free(img->pixels); free(img);
    
    printf("  3. Sharpen...\n");
    img = copy_image(original);
    sharpen(img);
    write_bmp("filter_03_sharpen.bmp", img);
    free(img->pixels); free(img);
    
    printf("  4. Edge detection...\n");
    img = copy_image(original);
    edge_detect(img);
    write_bmp("filter_04_edges.bmp", img);
    free(img->pixels); free(img);
    
    printf("  5. Emboss...\n");
    img = copy_image(original);
    emboss(img);
    write_bmp("filter_05_emboss.bmp", img);
    free(img->pixels); free(img);
    
    free(original->pixels); free(original);
    
    printf("\n=== Summary ===\n");
    printf("Created 5 filtered images:\n");
    printf("  filter_01_blur.bmp     - Box blur\n");
    printf("  filter_02_gaussian.bmp - Gaussian blur\n");
    printf("  filter_03_sharpen.bmp  - Sharpen\n");
    printf("  filter_04_edges.bmp    - Edge detection\n");
    printf("  filter_05_emboss.bmp   - Emboss effect\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
