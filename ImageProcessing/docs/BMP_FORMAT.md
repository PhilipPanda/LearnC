# BMP Format

## The Big Picture

BMP (Bitmap) is one of the simplest image formats. It stores uncompressed pixel data, making it perfect for learning image processing in C. No compression algorithms needed - just read and write raw pixels!

## File Structure

A BMP file has three main parts:

```
┌─────────────────────┐
│  File Header        │ 14 bytes
├─────────────────────┤
│  Info Header        │ 40+ bytes (usually 40)
├─────────────────────┤
│  Pixel Data         │ Variable size
└─────────────────────┘
```

## File Header (14 bytes)

```c
#pragma pack(push, 1)  // No padding!

typedef struct {
    uint16_t type;          // "BM" (0x4D42)
    uint32_t file_size;     // Total file size in bytes
    uint16_t reserved1;     // Unused (0)
    uint16_t reserved2;     // Unused (0)
    uint32_t offset;        // Offset to pixel data (usually 54)
} BMPFileHeader;

#pragma pack(pop)
```

**Note:** The `type` field is the ASCII codes for "BM":
- 'B' = 0x42
- 'M' = 0x4D
- Combined (little-endian): 0x4D42

## Info Header (40 bytes for BITMAPINFOHEADER)

```c
#pragma pack(push, 1)

typedef struct {
    uint32_t header_size;      // Size of this header (40)
    int32_t  width;            // Image width in pixels
    int32_t  height;           // Image height in pixels
    uint16_t planes;           // Number of color planes (always 1)
    uint16_t bits_per_pixel;   // Bits per pixel (1, 4, 8, 24, 32)
    uint32_t compression;      // Compression type (0 = none)
    uint32_t image_size;       // Size of pixel data (can be 0 if uncompressed)
    int32_t  x_pixels_per_m;   // Horizontal resolution (pixels/meter)
    int32_t  y_pixels_per_m;   // Vertical resolution (pixels/meter)
    uint32_t colors_used;      // Number of colors in palette (0 = all)
    uint32_t colors_important; // Important colors (0 = all)
} BMPInfoHeader;

#pragma pack(pop)
```

## Complete Header Structure

```c
#pragma pack(push, 1)

typedef struct {
    // File Header (14 bytes)
    uint16_t type;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
    
    // Info Header (40 bytes)
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
```

## Pixel Data

### 24-bit RGB (Most Common)

Pixels are stored as **BGR** (Blue, Green, Red) - **not RGB**!

```c
typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} BMPPixel;  // 3 bytes
```

### Row Padding

**Critical:** Each row must be padded to a multiple of 4 bytes!

```c
// Calculate row size with padding
int row_size_bytes(int width) {
    int bytes_per_row = width * 3;  // 3 bytes per pixel (24-bit)
    return (bytes_per_row + 3) & ~3;  // Round up to multiple of 4
}

// Examples:
// Width 1: 3 bytes → padded to 4 bytes (1 padding byte)
// Width 2: 6 bytes → padded to 8 bytes (2 padding bytes)
// Width 3: 9 bytes → padded to 12 bytes (3 padding bytes)
// Width 4: 12 bytes → no padding needed
// Width 5: 15 bytes → padded to 16 bytes (1 padding byte)
```

### Bottom-Up Storage

**Important:** BMP stores pixels **bottom-to-top** by default!

```
File stores:
  Row (height-1)  ← Last row (top of image)
  Row (height-2)
  ...
  Row 1
  Row 0           ← First row (bottom of image)

If height is negative, it's top-to-bottom (less common).
```

## Reading a BMP File

```c
typedef struct {
    int width;
    int height;
    uint8_t* pixels;  // RGB data (R,G,B,R,G,B,...)
} Image;

Image* read_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }
    
    // Read header
    BMPHeader header;
    if (fread(&header, sizeof(header), 1, file) != 1) {
        fprintf(stderr, "Failed to read header\n");
        fclose(file);
        return NULL;
    }
    
    // Verify BMP signature
    if (header.type != 0x4D42) {  // "BM"
        fprintf(stderr, "Not a BMP file\n");
        fclose(file);
        return NULL;
    }
    
    // Only support 24-bit uncompressed
    if (header.bits_per_pixel != 24 || header.compression != 0) {
        fprintf(stderr, "Only 24-bit uncompressed BMP supported\n");
        fclose(file);
        return NULL;
    }
    
    // Allocate image
    Image* img = malloc(sizeof(Image));
    img->width = header.width;
    img->height = abs(header.height);
    img->pixels = malloc(img->width * img->height * 3);
    
    // Seek to pixel data
    fseek(file, header.offset, SEEK_SET);
    
    // Calculate row size with padding
    int row_size = ((img->width * 3 + 3) / 4) * 4;
    uint8_t* row = malloc(row_size);
    
    // Read pixels (bottom-up)
    for (int y = 0; y < img->height; y++) {
        // Read row with padding
        fread(row, row_size, 1, file);
        
        // Convert BGR to RGB and store
        int row_index = (img->height - 1 - y) * img->width * 3;  // Flip vertically
        for (int x = 0; x < img->width; x++) {
            img->pixels[row_index + x*3 + 0] = row[x*3 + 2];  // Red (from Blue)
            img->pixels[row_index + x*3 + 1] = row[x*3 + 1];  // Green
            img->pixels[row_index + x*3 + 2] = row[x*3 + 0];  // Blue (from Red)
        }
    }
    
    free(row);
    fclose(file);
    return img;
}
```

## Writing a BMP File

```c
int write_bmp(const char* filename, Image* img) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to create file");
        return -1;
    }
    
    // Calculate padding
    int row_size = ((img->width * 3 + 3) / 4) * 4;
    int padding = row_size - img->width * 3;
    int image_size = row_size * img->height;
    
    // Fill header
    BMPHeader header = {0};
    header.type = 0x4D42;  // "BM"
    header.file_size = 54 + image_size;
    header.offset = 54;
    header.header_size = 40;
    header.width = img->width;
    header.height = img->height;
    header.planes = 1;
    header.bits_per_pixel = 24;
    header.compression = 0;
    header.image_size = image_size;
    header.x_pixels_per_m = 2835;  // 72 DPI
    header.y_pixels_per_m = 2835;
    
    // Write header
    fwrite(&header, sizeof(header), 1, file);
    
    // Write pixels (bottom-up, BGR format)
    uint8_t pad[3] = {0, 0, 0};
    for (int y = img->height - 1; y >= 0; y--) {
        for (int x = 0; x < img->width; x++) {
            int idx = (y * img->width + x) * 3;
            // Write BGR (swap R and B)
            fputc(img->pixels[idx + 2], file);  // Blue
            fputc(img->pixels[idx + 1], file);  // Green
            fputc(img->pixels[idx + 0], file);  // Red
        }
        // Write padding
        fwrite(pad, padding, 1, file);
    }
    
    fclose(file);
    return 0;
}
```

## Quick Reference

### Header Values for Common Cases

```c
// 24-bit RGB, 100×100 pixels, uncompressed
BMPHeader header = {
    .type = 0x4D42,           // "BM"
    .file_size = 30054,       // 54 + (100*100*3)
    .reserved1 = 0,
    .reserved2 = 0,
    .offset = 54,
    .header_size = 40,
    .width = 100,
    .height = 100,
    .planes = 1,
    .bits_per_pixel = 24,
    .compression = 0,         // No compression
    .image_size = 30000,      // 100*100*3
    .x_pixels_per_m = 2835,   // ~72 DPI
    .y_pixels_per_m = 2835,
    .colors_used = 0,
    .colors_important = 0
};
```

### Bits Per Pixel

- **1-bit:** Monochrome (black & white)
- **4-bit:** 16 colors (with palette)
- **8-bit:** 256 colors (with palette)
- **24-bit:** True color RGB (16.7M colors) **[Most common]**
- **32-bit:** RGB + Alpha

We focus on 24-bit (no palette needed).

### Compression Types

- **0:** No compression (BI_RGB)
- **1:** RLE 8-bit
- **2:** RLE 4-bit
- **3:** Bitfields

We use type 0 (uncompressed).

## Helper Functions

### Validate BMP

```c
int is_valid_bmp(BMPHeader* header) {
    return header->type == 0x4D42 &&           // "BM"
           header->planes == 1 &&
           header->bits_per_pixel == 24 &&
           header->compression == 0 &&
           header->width > 0 &&
           header->height != 0;
}
```

### Calculate File Size

```c
size_t bmp_file_size(int width, int height) {
    int row_size = ((width * 3 + 3) / 4) * 4;
    return 54 + (row_size * height);
}
```

### Print BMP Info

```c
void print_bmp_info(BMPHeader* header) {
    printf("BMP File Information:\n");
    printf("  Signature: %c%c\n", header->type & 0xFF, (header->type >> 8) & 0xFF);
    printf("  File Size: %u bytes\n", header->file_size);
    printf("  Width: %d pixels\n", header->width);
    printf("  Height: %d pixels\n", abs(header->height));
    printf("  Bits Per Pixel: %u\n", header->bits_per_pixel);
    printf("  Compression: %u (%s)\n", header->compression,
           header->compression == 0 ? "None" : "Compressed");
}
```

## Common Pitfalls

1. **Forgetting BGR order** - BMP uses BGR, not RGB!
2. **Ignoring padding** - Rows must be 4-byte aligned
3. **Wrong vertical order** - Pixels stored bottom-to-top
4. **Missing #pragma pack** - Struct padding breaks headers
5. **Assuming width is multiple of 4** - Always calculate padding

## Summary

- BMP = File Header (14) + Info Header (40) + Pixel Data
- Signature: "BM" (0x4D42)
- 24-bit RGB is **stored as BGR**
- Rows padded to 4-byte boundaries
- Pixels stored **bottom-to-top** (usually)
- No compression needed
- Simple to parse - perfect for learning!

BMP is the easiest format to work with in C. Master BMP and you understand the fundamentals needed for any image format!
