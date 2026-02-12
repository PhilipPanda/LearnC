# Image Basics

## The Big Picture

Digital images are grids of colored dots (pixels). Understanding how images are stored and manipulated in memory is fundamental to all image processing.

## What is a Pixel?

A **pixel** (picture element) is the smallest unit of an image. Each pixel has a color value.

```
Image (4x4 pixels):
┌─────┬─────┬─────┬─────┐
│ R,G,B│ R,G,B│ R,G,B│ R,G,B│
├─────┼─────┼─────┼─────┤
│ R,G,B│ R,G,B│ R,G,B│ R,G,B│
├─────┼─────┼─────┼─────┤
│ R,G,B│ R,G,B│ R,G,B│ R,G,B│
├─────┼─────┼─────┼─────┤
│ R,G,B│ R,G,B│ R,G,B│ R,G,B│
└─────┴─────┴─────┴─────┘

Each pixel stores Red, Green, Blue values.
```

## Color Representations

### RGB (Red, Green, Blue)

Most common color model. Each channel: 0-255 (8 bits).

```c
typedef struct {
    uint8_t red;    // 0-255
    uint8_t green;  // 0-255
    uint8_t blue;   // 0-255
} RGB;

// Common colors:
RGB black   = {0,   0,   0};
RGB white   = {255, 255, 255};
RGB red     = {255, 0,   0};
RGB green   = {0,   255, 0};
RGB blue    = {0,   0,   255};
RGB yellow  = {255, 255, 0};
RGB cyan    = {0,   255, 255};
RGB magenta = {255, 0,   255};
```

### Grayscale

Single channel (intensity). 0 = black, 255 = white.

```c
// Convert RGB to grayscale (luminance method)
uint8_t rgb_to_gray(RGB color) {
    // Human eye is more sensitive to green
    return (uint8_t)(0.299f * color.red + 
                     0.587f * color.green + 
                     0.114f * color.blue);
}
```

### RGBA (with Alpha)

RGB + transparency channel.

```c
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;  // 0 = transparent, 255 = opaque
} RGBA;
```

## Image Dimensions

```c
typedef struct {
    int width;     // Pixels per row
    int height;    // Rows
    RGB* pixels;   // Pixel data
} Image;

// Total pixels = width * height
// Memory needed = width * height * 3 bytes (RGB)
// Memory needed = width * height * 4 bytes (RGBA)
```

## Pixel Coordinates

Images use **row-major** order with origin typically at **top-left**.

```
(0,0) ────► X (width)
  │
  │
  ▼
  Y (height)

Pixel at (x, y):
index = y * width + x
```

### Accessing Pixels

```c
// Get pixel at (x, y)
RGB get_pixel(Image* img, int x, int y) {
    if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
        RGB black = {0, 0, 0};
        return black;  // Out of bounds
    }
    return img->pixels[y * img->width + x];
}

// Set pixel at (x, y)
void set_pixel(Image* img, int x, int y, RGB color) {
    if (x >= 0 && x < img->width && y >= 0 && y < img->height) {
        img->pixels[y * img->width + x] = color;
    }
}
```

## Bit Depth

**Bits per pixel** determines color range:

- **1-bit:** Black and white only (2 colors)
- **8-bit:** 256 colors or grayscale
- **16-bit:** 65,536 colors (5-6-5 RGB)
- **24-bit:** 16.7 million colors (8-8-8 RGB) **[Most common]**
- **32-bit:** 16.7 million colors + alpha (8-8-8-8 RGBA)

### True Color (24-bit RGB)

```c
// Each color channel: 8 bits = 256 values
// Total colors: 256 × 256 × 256 = 16,777,216

RGB color;
color.red = 128;    // 50% red
color.green = 64;   // 25% green
color.blue = 192;   // 75% blue
```

## Image Formats

### Uncompressed
- **BMP** - Windows bitmap (easiest to work with in C)
- **PPM** - Portable PixMap (simple text/binary format)

### Compressed
- **PNG** - Lossless compression
- **JPEG** - Lossy compression (photos)
- **GIF** - Lossless (256 colors max, animation)
- **WebP** - Modern, efficient compression

We'll focus on **BMP** because it's uncompressed and simple to parse.

## Memory Layout

### Row-Major (Most Common)

```c
// Image: 3×2 pixels
// Memory: R₀G₀B₀ R₁G₁B₁ R₂G₂B₂ R₃G₃B₃ R₄G₄B₄ R₅G₅B₅
//         ↑ row 0 (3 pixels)  ↑ row 1 (3 pixels)

Image img;
img.width = 3;
img.height = 2;
img.pixels = malloc(3 * 2 * sizeof(RGB));

// Pixel (x=1, y=1) is at index: 1*3 + 1 = 4
```

### Padding (BMP Files)

BMP rows must be padded to 4-byte boundaries:

```c
// Calculate row size with padding
int row_size = ((img.width * 3 + 3) / 4) * 4;

// Example: 5 pixels wide (15 bytes)
// 15 bytes → needs 1 padding byte → 16 bytes per row
```

## Color Operations

### Invert Colors (Negative)

```c
RGB invert(RGB color) {
    RGB result;
    result.red = 255 - color.red;
    result.green = 255 - color.green;
    result.blue = 255 - color.blue;
    return result;
}
```

### Brightness

```c
RGB adjust_brightness(RGB color, int amount) {
    RGB result;
    result.red = clamp(color.red + amount, 0, 255);
    result.green = clamp(color.green + amount, 0, 255);
    result.blue = clamp(color.blue + amount, 0, 255);
    return result;
}

int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
```

### Blend Two Colors

```c
RGB blend(RGB a, RGB b, float alpha) {
    // alpha: 0.0 = all a, 1.0 = all b
    RGB result;
    result.red = (uint8_t)(a.red * (1-alpha) + b.red * alpha);
    result.green = (uint8_t)(a.green * (1-alpha) + b.green * alpha);
    result.blue = (uint8_t)(a.blue * (1-alpha) + b.blue * alpha);
    return result;
}
```

## Image Size Calculations

```c
// Memory for RGB image
size_t image_size_bytes(int width, int height) {
    return width * height * sizeof(RGB);
}

// Examples:
// 1920×1080 (Full HD): 1920 * 1080 * 3 = 6,220,800 bytes ≈ 6 MB
// 3840×2160 (4K):      3840 * 2160 * 3 = 24,883,200 bytes ≈ 24 MB
// 640×480 (VGA):       640 * 480 * 3 = 921,600 bytes ≈ 900 KB
```

## Common Image Resolutions

```
320×240   - QVGA (old mobile)
640×480   - VGA (old standard)
800×600   - SVGA
1024×768  - XGA
1280×720  - HD (720p)
1920×1080 - Full HD (1080p)
2560×1440 - QHD (1440p)
3840×2160 - 4K UHD
7680×4320 - 8K UHD
```

## Working with Neighborhoods

Many operations need neighboring pixels:

```c
// Get 3×3 neighborhood around (x, y)
void get_neighborhood_3x3(Image* img, int x, int y, RGB neighbors[9]) {
    int idx = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            neighbors[idx++] = get_pixel(img, x + dx, y + dy);
        }
    }
}

// Average of neighborhood (blur)
RGB average_3x3(RGB neighbors[9]) {
    int r = 0, g = 0, b = 0;
    for (int i = 0; i < 9; i++) {
        r += neighbors[i].red;
        g += neighbors[i].green;
        b += neighbors[i].blue;
    }
    RGB result = {r/9, g/9, b/9};
    return result;
}
```

## Creating an Image

```c
Image* create_image(int width, int height) {
    Image* img = malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->pixels = calloc(width * height, sizeof(RGB));
    return img;
}

void free_image(Image* img) {
    if (img) {
        free(img->pixels);
        free(img);
    }
}

// Fill with solid color
void fill(Image* img, RGB color) {
    for (int i = 0; i < img->width * img->height; i++) {
        img->pixels[i] = color;
    }
}
```

## Drawing Basic Shapes

### Rectangle

```c
void draw_rect(Image* img, int x, int y, int w, int h, RGB color) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            set_pixel(img, px, py, color);
        }
    }
}
```

### Line (Bresenham's Algorithm)

```c
void draw_line(Image* img, int x0, int y0, int x1, int y1, RGB color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        set_pixel(img, x0, y0, color);
        
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
```

### Circle

```c
void draw_circle(Image* img, int cx, int cy, int radius, RGB color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                set_pixel(img, cx + x, cy + y, color);
            }
        }
    }
}
```

## Color Distance

Measure similarity between colors:

```c
// Euclidean distance in RGB space
float color_distance(RGB a, RGB b) {
    int dr = a.red - b.red;
    int dg = a.green - b.green;
    int db = a.blue - b.blue;
    return sqrtf(dr*dr + dg*dg + db*db);
}
```

## Summary

- **Pixel:** Smallest image unit, stores color (RGB)
- **Dimensions:** Width × Height determines total pixels
- **Coordinates:** (x, y) with (0,0) at top-left
- **Index:** `y * width + x` for pixel position
- **RGB:** 24-bit true color (8 bits per channel)
- **Grayscale:** Single intensity value (0-255)
- **Memory:** Width × Height × 3 bytes (RGB)
- **Operations:** Access, modify, blend, draw on pixels

Understanding these fundamentals is essential for all image processing - from simple filters to computer vision!
