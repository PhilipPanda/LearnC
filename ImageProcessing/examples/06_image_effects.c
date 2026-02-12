/*
 * Image Effects
 * 
 * Learn special effects:
 * - Pixelate
 * - Posterize
 * - Vignette
 * - Mirror effect
 * - Oil painting effect
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

void pixelate(Image* img, int block_size) {
    for (int y = 0; y < img->height; y += block_size) {
        for (int x = 0; x < img->width; x += block_size) {
            int sum_r = 0, sum_g = 0, sum_b = 0, count = 0;
            
            for (int by = 0; by < block_size && y + by < img->height; by++) {
                for (int bx = 0; bx < block_size && x + bx < img->width; bx++) {
                    int idx = ((y + by) * img->width + (x + bx)) * 3;
                    sum_r += img->pixels[idx]; sum_g += img->pixels[idx+1]; sum_b += img->pixels[idx+2];
                    count++;
                }
            }
            
            uint8_t avg_r = sum_r / count, avg_g = sum_g / count, avg_b = sum_b / count;
            
            for (int by = 0; by < block_size && y + by < img->height; by++) {
                for (int bx = 0; bx < block_size && x + bx < img->width; bx++) {
                    int idx = ((y + by) * img->width + (x + bx)) * 3;
                    img->pixels[idx] = avg_r; img->pixels[idx+1] = avg_g; img->pixels[idx+2] = avg_b;
                }
            }
        }
    }
}

void posterize(Image* img, int levels) {
    int step = 256 / levels;
    for (int i = 0; i < img->width * img->height * 3; i++) {
        img->pixels[i] = (img->pixels[i] / step) * step;
    }
}

void vignette(Image* img, float strength) {
    int cx = img->width / 2;
    int cy = img->height / 2;
    float max_dist = sqrtf(cx*cx + cy*cy);
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int dx = x - cx, dy = y - cy;
            float dist = sqrtf(dx*dx + dy*dy);
            float factor = 1.0f - (dist / max_dist) * strength;
            if (factor < 0) factor = 0;
            
            int idx = (y * img->width + x) * 3;
            img->pixels[idx] = (uint8_t)(img->pixels[idx] * factor);
            img->pixels[idx+1] = (uint8_t)(img->pixels[idx+1] * factor);
            img->pixels[idx+2] = (uint8_t)(img->pixels[idx+2] * factor);
        }
    }
}

void mirror_horizontal(Image* img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = img->width / 2; x < img->width; x++) {
            int left = (y * img->width + (img->width - 1 - x)) * 3;
            int right = (y * img->width + x) * 3;
            img->pixels[right] = img->pixels[left];
            img->pixels[right+1] = img->pixels[left+1];
            img->pixels[right+2] = img->pixels[left+2];
        }
    }
}

void oil_painting(Image* img, int radius) {
    uint8_t* output = malloc(img->width * img->height * 3);
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int intensity_count[256] = {0};
            int sum_r[256] = {0}, sum_g[256] = {0}, sum_b[256] = {0};
            
            for (int ky = -radius; ky <= radius; ky++) {
                for (int kx = -radius; kx <= radius; kx++) {
                    int px = x + kx, py = y + ky;
                    if (px >= 0 && px < img->width && py >= 0 && py < img->height) {
                        int idx = (py * img->width + px) * 3;
                        int intensity = (img->pixels[idx] + img->pixels[idx+1] + img->pixels[idx+2]) / 3;
                        intensity_count[intensity]++;
                        sum_r[intensity] += img->pixels[idx];
                        sum_g[intensity] += img->pixels[idx+1];
                        sum_b[intensity] += img->pixels[idx+2];
                    }
                }
            }
            
            int max_intensity = 0, max_count = 0;
            for (int i = 0; i < 256; i++) {
                if (intensity_count[i] > max_count) {
                    max_count = intensity_count[i];
                    max_intensity = i;
                }
            }
            
            int idx = (y * img->width + x) * 3;
            output[idx] = sum_r[max_intensity] / max_count;
            output[idx+1] = sum_g[max_intensity] / max_count;
            output[idx+2] = sum_b[max_intensity] / max_count;
        }
    }
    memcpy(img->pixels, output, img->width * img->height * 3);
    free(output);
}

void solarize(Image* img, uint8_t threshold) {
    for (int i = 0; i < img->width * img->height * 3; i++) {
        if (img->pixels[i] > threshold) {
            img->pixels[i] = 255 - img->pixels[i];
        }
    }
}

int main(int argc, char* argv[]) {
    printf("=== Image Effects ===\n\n");
    
    const char* input_file = (argc > 1) ? argv[1] : "assets/example-image.bmp";
    printf("Loading: %s\n", input_file);
    
    Image* original = read_bmp(input_file);
    if (!original) {
        fprintf(stderr, "Failed to load image. Make sure it's a 24-bit BMP file.\n");
        return 1;
    }
    printf("Loaded %dx%d image\n\n", original->width, original->height);
    
    printf("Applying effects...\n");
    
    printf("  1. Pixelate (16px blocks)...\n");
    Image* img = copy_image(original);
    pixelate(img, 16);
    write_bmp("effect_01_pixelate.bmp", img);
    free(img->pixels); free(img);
    
    printf("  2. Posterize (8 levels)...\n");
    img = copy_image(original);
    posterize(img, 8);
    write_bmp("effect_02_posterize.bmp", img);
    free(img->pixels); free(img);
    
    printf("  3. Vignette...\n");
    img = copy_image(original);
    vignette(img, 0.7f);
    write_bmp("effect_03_vignette.bmp", img);
    free(img->pixels); free(img);
    
    printf("  4. Mirror horizontal...\n");
    img = copy_image(original);
    mirror_horizontal(img);
    write_bmp("effect_04_mirror.bmp", img);
    free(img->pixels); free(img);
    
    printf("  5. Oil painting...\n");
    img = copy_image(original);
    oil_painting(img, 3);
    write_bmp("effect_05_oil_painting.bmp", img);
    free(img->pixels); free(img);
    
    printf("  6. Solarize...\n");
    img = copy_image(original);
    solarize(img, 128);
    write_bmp("effect_06_solarize.bmp", img);
    free(img->pixels); free(img);
    
    free(original->pixels); free(original);
    
    printf("\n=== Summary ===\n");
    printf("Created 6 effect images:\n");
    printf("  effect_01_pixelate.bmp     - Pixelated (mosaic)\n");
    printf("  effect_02_posterize.bmp    - Reduced colors\n");
    printf("  effect_03_vignette.bmp     - Dark edges\n");
    printf("  effect_04_mirror.bmp       - Mirrored halves\n");
    printf("  effect_05_oil_painting.bmp - Oil painting style\n");
    printf("  effect_06_solarize.bmp     - Solarized\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
