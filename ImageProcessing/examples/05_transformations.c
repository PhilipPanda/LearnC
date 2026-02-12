/*
 * Image Transformations
 * 
 * Learn geometric operations:
 * - Flip horizontal and vertical
 * - Rotate 90°, 180°, 270°
 * - Scale (resize)
 * - Crop
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

void flip_horizontal(Image* img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width / 2; x++) {
            int left = (y * img->width + x) * 3;
            int right = (y * img->width + (img->width - 1 - x)) * 3;
            for (int c = 0; c < 3; c++) {
                uint8_t temp = img->pixels[left + c];
                img->pixels[left + c] = img->pixels[right + c];
                img->pixels[right + c] = temp;
            }
        }
    }
}

void flip_vertical(Image* img) {
    for (int y = 0; y < img->height / 2; y++) {
        for (int x = 0; x < img->width; x++) {
            int top = (y * img->width + x) * 3;
            int bottom = ((img->height - 1 - y) * img->width + x) * 3;
            for (int c = 0; c < 3; c++) {
                uint8_t temp = img->pixels[top + c];
                img->pixels[top + c] = img->pixels[bottom + c];
                img->pixels[bottom + c] = temp;
            }
        }
    }
}

Image* rotate_90_cw(Image* img) {
    Image* rotated = malloc(sizeof(Image));
    rotated->width = img->height; rotated->height = img->width;
    rotated->pixels = malloc(rotated->width * rotated->height * 3);
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int src = (y * img->width + x) * 3;
            int dst = (x * rotated->width + (img->height - 1 - y)) * 3;
            rotated->pixels[dst] = img->pixels[src];
            rotated->pixels[dst+1] = img->pixels[src+1];
            rotated->pixels[dst+2] = img->pixels[src+2];
        }
    }
    return rotated;
}

Image* scale(Image* img, int new_width, int new_height) {
    Image* scaled = malloc(sizeof(Image));
    scaled->width = new_width; scaled->height = new_height;
    scaled->pixels = malloc(new_width * new_height * 3);
    
    float x_ratio = (float)img->width / new_width;
    float y_ratio = (float)img->height / new_height;
    
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            int src_x = (int)(x * x_ratio);
            int src_y = (int)(y * y_ratio);
            int src = (src_y * img->width + src_x) * 3;
            int dst = (y * new_width + x) * 3;
            scaled->pixels[dst] = img->pixels[src];
            scaled->pixels[dst+1] = img->pixels[src+1];
            scaled->pixels[dst+2] = img->pixels[src+2];
        }
    }
    return scaled;
}

Image* crop(Image* img, int x, int y, int width, int height) {
    if (x < 0) x = 0; if (y < 0) y = 0;
    if (x + width > img->width) width = img->width - x;
    if (y + height > img->height) height = img->height - y;
    
    Image* cropped = malloc(sizeof(Image));
    cropped->width = width; cropped->height = height;
    cropped->pixels = malloc(width * height * 3);
    
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int src = ((y + row) * img->width + (x + col)) * 3;
            int dst = (row * width + col) * 3;
            cropped->pixels[dst] = img->pixels[src];
            cropped->pixels[dst+1] = img->pixels[src+1];
            cropped->pixels[dst+2] = img->pixels[src+2];
        }
    }
    return cropped;
}

Image* copy_image(Image* src) {
    Image* img = malloc(sizeof(Image));
    img->width = src->width; img->height = src->height;
    img->pixels = malloc(img->width * img->height * 3);
    memcpy(img->pixels, src->pixels, img->width * img->height * 3);
    return img;
}

int main(int argc, char* argv[]) {
    printf("=== Image Transformations ===\n\n");
    
    const char* input_file = (argc > 1) ? argv[1] : "assets/example-image.bmp";
    printf("Loading: %s\n", input_file);
    
    Image* original = read_bmp(input_file);
    if (!original) {
        fprintf(stderr, "Failed to load image. Make sure it's a 24-bit BMP file.\n");
        return 1;
    }
    printf("Loaded %dx%d image\n\n", original->width, original->height);
    
    printf("Applying transformations...\n");
    
    printf("  1. Flip horizontal...\n");
    Image* img = copy_image(original);
    flip_horizontal(img);
    write_bmp("transform_01_flip_h.bmp", img);
    free(img->pixels); free(img);
    
    printf("  2. Flip vertical...\n");
    img = copy_image(original);
    flip_vertical(img);
    write_bmp("transform_02_flip_v.bmp", img);
    free(img->pixels); free(img);
    
    printf("  3. Rotate 90° CW...\n");
    img = rotate_90_cw(original);
    write_bmp("transform_03_rotate_90.bmp", img);
    free(img->pixels); free(img);
    
    printf("  4. Rotate 180°...\n");
    img = rotate_90_cw(original);
    Image* img2 = rotate_90_cw(img);
    write_bmp("transform_04_rotate_180.bmp", img2);
    free(img->pixels); free(img);
    free(img2->pixels); free(img2);
    
    printf("  5. Rotate 270° CW...\n");
    img = rotate_90_cw(original);
    img2 = rotate_90_cw(img);
    Image* img3 = rotate_90_cw(img2);
    write_bmp("transform_05_rotate_270.bmp", img3);
    free(img->pixels); free(img);
    free(img2->pixels); free(img2);
    free(img3->pixels); free(img3);
    
    printf("  6. Scale to 50%%...\n");
    img = scale(original, original->width / 2, original->height / 2);
    write_bmp("transform_06_scale_50.bmp", img);
    free(img->pixels); free(img);
    
    printf("  7. Scale to 200%%...\n");
    img = scale(original, original->width * 2, original->height * 2);
    write_bmp("transform_07_scale_200.bmp", img);
    free(img->pixels); free(img);
    
    printf("  8. Crop center 50%%...\n");
    int crop_w = original->width / 2;
    int crop_h = original->height / 2;
    img = crop(original, original->width / 4, original->height / 4, crop_w, crop_h);
    write_bmp("transform_08_crop.bmp", img);
    free(img->pixels); free(img);
    
    free(original->pixels); free(original);
    
    printf("\n=== Summary ===\n");
    printf("Created 8 transformed images:\n");
    printf("  transform_01_flip_h.bmp     - Flipped horizontal\n");
    printf("  transform_02_flip_v.bmp     - Flipped vertical\n");
    printf("  transform_03_rotate_90.bmp  - Rotated 90° CW\n");
    printf("  transform_04_rotate_180.bmp - Rotated 180°\n");
    printf("  transform_05_rotate_270.bmp - Rotated 270° CW\n");
    printf("  transform_06_scale_50.bmp   - Scaled to 50%%\n");
    printf("  transform_07_scale_200.bmp  - Scaled to 200%%\n");
    printf("  transform_08_crop.bmp       - Cropped center\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
