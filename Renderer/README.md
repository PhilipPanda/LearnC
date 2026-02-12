# C Renderer

A software renderer written in pure C. No OpenGL, no DirectX - just a framebuffer and some math.

## What you get

- Window creation and event handling (Windows/Linux)
- 2D drawing (lines, circles, rectangles, triangles)
- 3D math (vectors, matrices, transformations)
- 3D projection and rendering
- Image loading (BMP files)
- Texture mapping
- Simple text rendering

Everything runs on the CPU. You control every pixel.

## Building

The renderer is just two files: `renderer.h` and `renderer.c`. Copy them to your project or build as a library.

### Build the demo

```bash
./build.bat    # Windows
./build.sh     # Linux
```

### Build the examples

```bash
cd examples
./build_examples.bat    # Windows
./build_examples.sh     # Linux
```

## Using it

```c
#include "renderer.h"

int main(void) {
    Renderer rend = {0};
    renderer_init(&rend, "Window Title", 800, 600);

    while (rend.is_running) {
        renderer_handle_events(&rend);
        renderer_clear(&rend, color_make(0, 0, 0, 255));
        
        // Draw stuff here
        renderer_draw_filled_circle(&rend, 400, 300, 50,
            color_make(255, 0, 0, 255));
        
        renderer_present(&rend);
        renderer_sleep(16);  // ~60 FPS
    }

    renderer_cleanup(&rend);
    return 0;
}
```

Compile:
```bash
# Windows
gcc your_program.c renderer.c -lgdi32 -luser32 -lm

# Linux
gcc your_program.c renderer.c -lX11 -lm
```

## Documentation

- **[API Reference](docs/API.md)** - Every function explained
- **[How It Works](docs/HOW_IT_WORKS.md)** - Renderer internals
- **[3D Graphics](docs/3D_GRAPHICS.md)** - Matrices, projection, rasterization
- **[Examples Guide](examples/README.md)** - What each example does

## Examples

Each example teaches something specific:

| Example | What It Teaches |
|---------|----------------|
| 01_basic_shapes | Drawing 2D primitives, basic rendering loop |
| 02_image_rendering | Loading images, alpha blending, transformations |
| 03_3d_cube | 3D projection, matrices, wireframe rendering |
| 04_3d_pyramid | Triangle rasterization, depth, basic lighting |
| 05_particles | Simple physics, managing many objects |
| 06_textured_cube | Texture mapping, UV coordinates |
| 07_simple_game | Collision detection, game loop, player input |

Start with 01, work your way through. Each one builds on previous concepts.

## Performance

This is software rendering - it's slow compared to GPU hardware. That's the point. You can see exactly how everything works.

On a modern CPU you'll get 60 FPS for simple scenes, but complex scenes with many triangles will slow down. That's normal. This is for learning, not shipping games.

## Platform support

- **Windows**: Uses GDI32 for the window and backbuffer
- **Linux**: Uses X11 for the window, XImage for the backbuffer

Same API on both platforms. The renderer abstracts the platform differences.

## What this teaches

- How graphics actually work under the hood
- Memory layouts and pointer manipulation
- Coordinate systems and transformations
- Basic linear algebra (vectors, matrices)
- Rasterization algorithms
- Cross-platform C programming
- Managing state and resources

After working through this, you'll understand what GPUs actually do when you call OpenGL or DirectX functions.
