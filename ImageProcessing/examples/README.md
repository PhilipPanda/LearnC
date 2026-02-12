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

### 02 - BMP Writer
Creates BMP images from scratch, draws shapes and gradients.

```bash
bin\02_bmp_writer.exe
```

**Creates:**
- Solid color images (red, green, blue)
- Color gradients
- Shapes (rectangles, circles, lines)
- Checkerboard pattern
- HSV color wheel

### 03 - Image Filters
Applies convolution filters to images.

```bash
bin\03_image_filters.exe
```

**Filters:**
- Box blur
- Gaussian blur
- Sharpen
- Edge detection (Sobel)
- Emboss

### 04 - Color Operations
Adjusts colors and brightness.

```bash
bin\04_color_operations.exe
```

**Operations:**
- Brightness adjustment (+/-)
- Contrast (high/low)
- Grayscale conversion
- Sepia tone
- Color inversion
- Binary threshold
- Gamma correction

### 05 - Transformations
Geometric image transformations.

```bash
bin\05_transformations.exe
```

**Transforms:**
- Flip horizontal/vertical
- Rotate 90°, 180°, 270°
- Scale (50%, 200%)
- Crop center

### 06 - Image Effects
Special artistic effects.

```bash
bin\06_image_effects.exe
```

**Effects:**
- Pixelate (mosaic)
- Posterize (reduced colors)
- Vignette (dark edges)
- Mirror effect
- Oil painting style
- Solarize

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
