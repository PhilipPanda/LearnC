# Image Processing Examples

## Building

### Windows
```bash
build_all.bat
```

### Linux / macOS
```bash
chmod +x build_all.sh
./build_all.sh
```

## Examples Overview

### 01 - BMP File Reader
Reads BMP files and displays detailed format information and pixel statistics.

```bash
# Read the included example image
bin\01_bmp_reader.exe

# Or use the batch file (Windows)
run_bmp_reader.bat

# Or read your own BMP file
bin\01_bmp_reader.exe path\to\image.bmp
```

**Analyzes:**
- File format and headers
- Image dimensions
- Bits per pixel
- Average colors and brightness

### 02 - BMP Writer (Coming Soon)
Creates BMP images from scratch, draws shapes and gradients.

### 03 - Image Filters (Coming Soon)
Applies convolution filters: blur, sharpen, edge detection.

### 04 - Color Operations (Coming Soon)
Brightness, contrast, grayscale, sepia tone adjustments.

### 05 - Transformations (Coming Soon)
Rotate, flip, scale, and crop images.

### 06 - Image Effects (Coming Soon)
Special effects: negative, posterize, pixelate, vignette.

## Example Image

An example BMP image is included in `assets/example-image.bmp` for testing the reader and filters. You can also:
- Create your own BMP images using any image editor
- Use online converters to convert JPG/PNG to BMP format
- BMP files must be 24-bit uncompressed for these examples

## Documentation

See the `../docs/` directory for detailed guides on:
- Image basics (pixels, RGB, color spaces)
- BMP file format
- Image filters and convolution
- Color operations
- Image transformations

## Note

The examples use 24-bit BMP format (uncompressed RGB) which is the simplest format to work with in C. Other formats like PNG and JPEG require external libraries for compression/decompression.
