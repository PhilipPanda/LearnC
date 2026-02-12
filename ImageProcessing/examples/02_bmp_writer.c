/*
 * BMP File Writer
 * 
 * Learn to create BMP files:
 * - Building BMP headers
 * - Writing pixel data
 * - Creating test images
 * - Drawing shapes and gradients
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265358979323846

#pragma pack(push, 1)

typedef struct {
    uint16_t type;
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

typedef struct {
    int width;
    int height;
    uint8_t* pixels;  // RGB data
} Image;

// Create image
Image* create_image(int width, int height) {
    Image* img = malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->pixels = calloc(width * height * 3, 1);
    return img;
}

void free_image(Image* img) {
    if (img) {
        free(img->pixels);
        free(img);
    }
}

// Set pixel
void set_pixel(Image* img, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (x >= 0 && x < img->width && y >= 0 && y < img->height) {
        int idx = (y * img->width + x) * 3;
        img->pixels[idx] = r;
        img->pixels[idx+1] = g;
        img->pixels[idx+2] = b;
    }
}

// Write BMP file
int write_bmp(const char* filename, Image* img) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to create file");
        return -1;
    }
    
    int row_size = ((img->width * 3 + 3) / 4) * 4;
    int padding = row_size - img->width * 3;
    int image_size = row_size * img->height;
    
    BMPHeader header = {0};
    header.type = 0x4D42;
    header.file_size = 54 + image_size;
    header.offset = 54;
    header.header_size = 40;
    header.width = img->width;
    header.height = img->height;
    header.planes = 1;
    header.bits_per_pixel = 24;
    header.compression = 0;
    header.image_size = image_size;
    header.x_pixels_per_m = 2835;
    header.y_pixels_per_m = 2835;
    
    fwrite(&header, sizeof(header), 1, file);
    
    uint8_t pad[3] = {0};
    for (int y = img->height - 1; y >= 0; y--) {
        for (int x = 0; x < img->width; x++) {
            int idx = (y * img->width + x) * 3;
            fputc(img->pixels[idx+2], file);  // B
            fputc(img->pixels[idx+1], file);  // G
            fputc(img->pixels[idx], file);    // R
        }
        fwrite(pad, padding, 1, file);
    }
    
    fclose(file);
    return 0;
}

// Draw rectangle
void draw_rect(Image* img, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            set_pixel(img, px, py, r, g, b);
        }
    }
}

// Draw circle
void draw_circle(Image* img, int cx, int cy, int radius, uint8_t r, uint8_t g, uint8_t b) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                set_pixel(img, cx + x, cy + y, r, g, b);
            }
        }
    }
}

// Draw line (Bresenham)
void draw_line(Image* img, int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        set_pixel(img, x0, y0, r, g, b);
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Create gradient
void create_gradient(Image* img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            uint8_t r = (uint8_t)(255 * x / img->width);
            uint8_t g = (uint8_t)(255 * y / img->height);
            uint8_t b = 128;
            set_pixel(img, x, y, r, g, b);
        }
    }
}

// Create color wheel
void create_color_wheel(Image* img) {
    int cx = img->width / 2;
    int cy = img->height / 2;
    int radius = (img->width < img->height ? img->width : img->height) / 2 - 10;
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int dx = x - cx;
            int dy = y - cy;
            float dist = sqrtf(dx*dx + dy*dy);
            
            if (dist <= radius) {
                float angle = atan2f(dy, dx);
                float hue = (angle + PI) / (2 * PI);
                
                // HSV to RGB (simplified)
                float h = hue * 6.0f;
                float s = dist / radius;
                float v = 1.0f;
                
                int i = (int)h;
                float f = h - i;
                float q = v * (1 - s * f);
                float t = v * (1 - s * (1 - f));
                
                float r, g, b;
                switch (i % 6) {
                    case 0: r = v; g = t; b = 0; break;
                    case 1: r = q; g = v; b = 0; break;
                    case 2: r = 0; g = v; b = t; break;
                    case 3: r = 0; g = q; b = v; break;
                    case 4: r = t; g = 0; b = v; break;
                    case 5: r = v; g = 0; b = q; break;
                    default: r = g = b = 0; break;
                }
                
                set_pixel(img, x, y, (uint8_t)(r*255), (uint8_t)(g*255), (uint8_t)(b*255));
            }
        }
    }
}

// Create checkerboard
void create_checkerboard(Image* img, int square_size) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int checker = ((x / square_size) + (y / square_size)) % 2;
            uint8_t color = checker ? 255 : 0;
            set_pixel(img, x, y, color, color, color);
        }
    }
}

int main(void) {
    printf("=== BMP File Writer ===\n\n");
    
    int width = 512;
    int height = 512;
    
    // 1. Solid colors
    printf("Creating solid color images...\n");
    Image* img = create_image(width, height);
    
    draw_rect(img, 0, 0, width, height, 255, 0, 0);
    write_bmp("01_red.bmp", img);
    printf("  Created: 01_red.bmp\n");
    
    draw_rect(img, 0, 0, width, height, 0, 255, 0);
    write_bmp("02_green.bmp", img);
    printf("  Created: 02_green.bmp\n");
    
    draw_rect(img, 0, 0, width, height, 0, 0, 255);
    write_bmp("03_blue.bmp", img);
    printf("  Created: 03_blue.bmp\n");
    
    // 2. Gradient
    printf("\nCreating gradient...\n");
    create_gradient(img);
    write_bmp("04_gradient.bmp", img);
    printf("  Created: 04_gradient.bmp\n");
    
    // 3. Shapes
    printf("\nCreating shapes...\n");
    draw_rect(img, 0, 0, width, height, 255, 255, 255);  // White background
    
    draw_rect(img, 50, 50, 150, 100, 255, 0, 0);
    draw_circle(img, 300, 100, 60, 0, 255, 0);
    draw_line(img, 50, 250, 450, 250, 0, 0, 255);
    draw_line(img, 250, 50, 250, 450, 255, 165, 0);
    
    write_bmp("05_shapes.bmp", img);
    printf("  Created: 05_shapes.bmp\n");
    
    // 4. Checkerboard
    printf("\nCreating checkerboard...\n");
    create_checkerboard(img, 32);
    write_bmp("06_checkerboard.bmp", img);
    printf("  Created: 06_checkerboard.bmp\n");
    
    // 5. Color wheel
    printf("\nCreating color wheel...\n");
    draw_rect(img, 0, 0, width, height, 50, 50, 50);
    create_color_wheel(img);
    write_bmp("07_color_wheel.bmp", img);
    printf("  Created: 07_color_wheel.bmp\n");
    
    free_image(img);
    
    printf("\n=== Summary ===\n");
    printf("Created 7 BMP files (%dx%d each):\n", width, height);
    printf("  01_red.bmp          - Solid red\n");
    printf("  02_green.bmp        - Solid green\n");
    printf("  03_blue.bmp         - Solid blue\n");
    printf("  04_gradient.bmp     - Color gradient\n");
    printf("  05_shapes.bmp       - Rectangles, circles, lines\n");
    printf("  06_checkerboard.bmp - Black and white pattern\n");
    printf("  07_color_wheel.bmp  - HSV color wheel\n");
    printf("\nAll files are 24-bit BMP format.\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
