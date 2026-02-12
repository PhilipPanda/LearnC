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

Run any example:

```bash
bin/01_basic_shapes.exe
bin/02_3d_cube.exe
# etc.
```

## The Examples

### 01_basic_shapes.c

Basic 2D rendering and the game loop. Draws rectangles, circles, lines, triangles. Shows animation using sin/cos.

**Start here.** If this makes sense, move on.

### 02_3d_cube.c

3D wireframe cube with depth-based coloring. Shows how matrices transform 3D coordinates to 2D screen positions.

**Read with:** [docs/3D_GRAPHICS.md](../docs/3D_GRAPHICS.md)

### 03_3d_pyramid.c

Solid 3D pyramid with faces that brighten/darken based on angle to light. Introduction to filled triangles and basic lighting.

### 04_particles.c

Particle system with physics and gravity. Shows how to manage many moving objects efficiently.

### 05_textured_cube.c

Spinning cube with texture mapping. Works with or without a texture file in `assets/`.

### 06_simple_game.c

Simple shooter with collision detection and score tracking. Puts all the concepts together.

## Learning Path

Go in order. Try modifying each one before moving to the next:

1. Change colors and shapes in 01
2. Adjust rotation speed in 02, add more cubes
3. Change pyramid to a different shape in 03
4. Make particles bounce in 04
5. Add your own texture in 05
6. Add features to the game in 06

## Making Your Own

```c
#include "../src/renderer.h"
#include <stdio.h>

int main(void) {
    Renderer rend = {0};
    renderer_init(&rend, "My Program", 800, 600);

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

After these examples:
- Read the renderer source (`src/renderer.c`)
- Build something yourself
- Try optimizing slow parts
- Check out the other projects in LearnC
