# Image Processing in C

Learn digital image processing fundamentals in C. BMP file handling, image filters, color manipulation, transformations, and effects - everything you need to build image processing software.

## What you get

- BMP file reading and writing
- Image filters (blur, sharpen, edge detection)
- Color operations (brightness, contrast, saturation)
- Image transformations (rotate, flip, scale, crop)
- Special effects (sepia, negative, emboss, pixelate)
- Convolution and kernel operations
- Histogram analysis

All examples work with standard BMP files. No external image libraries - pure C implementation.

## Building

```bash
# Windows
cd examples
build_all.bat

# Linux / MSYS2
cd examples
./build_all.sh
```

## Documentation

- **[Image Basics](docs/IMAGE_BASICS.md)** - Pixels, color spaces, RGB, image formats, bit depth
- **[BMP Format](docs/BMP_FORMAT.md)** - Understanding and parsing BMP file structure
- **[Image Filters](docs/IMAGE_FILTERS.md)** - Convolution, blur, sharpen, edge detection
- **[Color Operations](docs/COLOR_OPERATIONS.md)** - Brightness, contrast, saturation, color transforms
- **[Transformations](docs/TRANSFORMATIONS.md)** - Rotate, flip, scale, crop operations

## Examples

| Example | What It Teaches |
|---------|----------------|
| 01_bmp_reader | Reading BMP files, parsing headers, pixel data extraction |
| 02_bmp_writer | Creating BMP files, drawing shapes, gradients |
| 03_image_filters | Blur, sharpen, edge detection, emboss with convolution |
| 04_color_operations | Brightness, contrast, grayscale, sepia tone |
| 05_transformations | Rotate, flip, scale, crop operations |
| 06_image_effects | Negative, posterize, pixelate, vignette |

An example BMP image is included in `examples/assets/example-image.bmp` for testing.

## What this teaches

- Digital image fundamentals (pixels, RGB, color spaces)
- BMP file format and bitmap structures
- Convolution and kernel operations
- Color space conversions
- Filtering techniques (spatial domain)
- Geometric transformations
- Image manipulation algorithms
- Memory-efficient image processing

After this, you'll understand how image editors work and can build photo filters, image converters, computer vision preprocessing, and more.

## Quick Start

```bash
cd examples
build_all.bat

# Create test images
bin\02_bmp_writer.exe

# Apply filters
bin\03_image_filters.exe

# Try color effects
bin\04_color_operations.exe
```

Digital images are everywhere - photos, games, web, medical imaging. Master these fundamentals and you can process any image format and build powerful image manipulation tools.
