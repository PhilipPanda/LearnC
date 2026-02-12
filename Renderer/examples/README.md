# Renderer Examples

Learn by doing. Each example demonstrates specific concepts, building on previous ones.

## Building

```bash
# Windows
build_examples.bat

# Linux  
chmod +x build_examples.sh
./build_examples.sh
```

This creates a `bin/` folder with all the executables. Run any of them:

```bash
bin/01_basic_shapes.exe
bin/03_3d_cube.exe
# etc.
```

## The Examples

### 01_basic_shapes.c

**What it teaches:** Basic 2D rendering, the game loop, drawing primitives

Draws rectangles, circles, lines, and triangles. Shows a pulsing circle and rotating objects. This is your first look at the render loop - clear, draw, present, repeat.

**Key concepts:**
- `renderer_init` / `renderer_cleanup`
- The game loop pattern
- `renderer_clear`, `renderer_present`
- 2D drawing functions
- Simple animation with `sin()` and `cos()`

**Start here.** If this makes sense, move on. If not, read it until it does.

### 02_image_rendering.c

**What it teaches:** Loading external assets, alpha blending, transformations

Loads a BMP file and displays it in different ways - scaled, faded, tinted. Shows alpha blending by drawing transparent circles over it.

**Key concepts:**
- `image_load_bmp` / `image_free`
- Drawing images vs drawing primitives
- Alpha transparency
- Image scaling
- Color tinting

**You need:** A `test.bmp` file in the `assets/` folder (64x64 recommended). Create one with any paint program.

### 03_3d_cube.c

**What it teaches:** 3D projection, matrices, wireframe rendering

Draws a spinning wireframe cube. This is where 3D starts. You'll see how matrices transform 3D coordinates to 2D screen positions.

**Key concepts:**
- `Vec3` and 3D coordinates
- `Mat4` transformations
- `mat4_perspective`, `mat4_look_at`
- Matrix multiplication order
- `renderer_draw_line_3d`
- Rotating objects over time

**Read this with** [docs/3D_GRAPHICS.md](../docs/3D_GRAPHICS.md) open.

### 04_3d_pyramid.c

**What it teaches:** Solid 3D objects, triangle rasterization, basic lighting

Draws a solid colored pyramid with faces that get brighter/darker based on their angle. Introduction to filled triangles and depth.

**Key concepts:**
- `renderer_draw_triangle_3d`
- Drawing filled vs wireframe
- Normal vectors for lighting
- Backface culling
- Simple diffuse lighting

### 05_particles.c

**What it teaches:** Managing multiple objects, simple physics

Spawns particles that move, fall due to gravity, and fade out. Shows how to handle many moving objects efficiently.

**Key concepts:**
- Arrays of structs
- Update/render separation
- Simple physics (velocity, acceleration)
- Lifetime management
- Alpha fade-out

### 06_textured_cube.c

**What it teaches:** Texture mapping, UV coordinates

Draws a spinning cube with a texture on each face (or colored faces if no texture). Shows how 2D images wrap onto 3D surfaces.

**Key concepts:**
- UV coordinates (texture space)
- `renderer_draw_textured_triangle`
- Mapping textures to 3D faces
- Optional asset loading

**Optional:** Add a `test.bmp` to `assets/` to see textures. Works without it too.

### 07_simple_game.c

**What it teaches:** Putting it all together in an interactive program

A simple shooter where you move and auto-aim at targets. Shows collision detection, score tracking, and basic game structure.

**Key concepts:**
- Game state management
- Collision detection (circle vs circle)
- Player input/controls
- Spawning and removing entities
- Win/lose conditions

## Learning Path

Work through them in order. Each one assumes you understand the previous examples.

1. Can you modify 01 to draw different shapes? Different colors?
2. In 02, can you make the image fade in and out?
3. In 03, can you change the rotation speed or add more cubes?
4. In 04, can you change the pyramid to a different shape?
5. In 05, can you make particles bounce off the ground?
6. In 06, can you create your own texture?
7. In 07, can you add more gameplay features?

## Common Issues

**"Failed to initialize"** - Probably missing GDI32 (Windows) or X11 (Linux). Install build tools properly.

**"test.bmp not found"** - Examples 02 and 06 need this. Create a small BMP file or skip those examples.

**Slow performance** - Software rendering is CPU-intensive. Complex scenes will slow down. That's expected.

**Crashes** - Check you're not accessing out-of-bounds arrays. Add bounds checks if needed.

## Making Your Own

Copy the structure:

```c
#include "../src/renderer.h"
#include <stdio.h>

int main(void) {
    Renderer rend = {0};
    if (!renderer_init(&rend, "My Program", 800, 600)) {
        return 1;
    }

    while (rend.is_running) {
        renderer_handle_events(&rend);
        renderer_clear(&rend, color_make(0, 0, 0, 255));
        
        // Your code here
        
        renderer_present(&rend);
        renderer_sleep(16);
    }

    renderer_cleanup(&rend);
    return 0;
}
```

Compile:
```bash
# Windows
gcc your_program.c ../src/renderer.c -I ../src -lgdi32 -luser32 -lm -o your_program.exe

# Linux
gcc your_program.c ../src/renderer.c -I ../src -lX11 -lm -o your_program
```

## What Next?

After working through these, you should understand:
- How rendering works at a low level
- The 3D graphics pipeline
- Basic game structure
- Managing state and resources in C

Next steps:
- Read the renderer source code (`src/renderer.c`)
- Implement your own features
- Try optimizing something that's slow
- Build a small game or demo

Or move on to other C projects in this repo.
