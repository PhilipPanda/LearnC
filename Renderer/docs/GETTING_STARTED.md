# Getting Started

## First Steps

1. **Build the examples**
```bash
cd examples
./build_examples.bat    # Windows
./build_examples.sh     # Linux
```

2. **Run the first example**
```bash
bin/01_basic_shapes.exe
```

You should see a window with colored shapes. If not, check requirements below.

## Requirements

### Windows
- GCC (MinGW or MSYS2)
- That's it - GDI32 comes with Windows

### Linux
- GCC
- X11 development files

Install X11 on Ubuntu/Debian:
```bash
sudo apt install libx11-dev build-essential
```

## Understanding the Code

Start with `01_basic_shapes.c`. The structure is:

```c
// 1. Initialize
Renderer rend = {0};
renderer_init(&rend, "Title", width, height);

// 2. Main loop
while (rend.is_running) {
    renderer_handle_events(&rend);  // Process input
    renderer_clear(&rend, bg_color);  // Clear screen
    
    // Draw stuff here
    renderer_draw_filled_circle(...);
    
    renderer_present(&rend);  // Show it
    renderer_sleep(16);  // ~60 FPS
}

// 3. Cleanup
renderer_cleanup(&rend);
```

That's the pattern for every program.

## How Drawing Works

Everything is pixels. The renderer gives you a framebuffer (array of pixels), you write colors to it, then display it.

```c
// Simple example - just one function call
renderer_draw_filled_rect(&rend, 100, 100, 50, 50, color_make(255, 0, 0, 255));
```

That draws a red square at position (100, 100) with size 50x50.

Behind the scenes:
1. Function calculates which pixels are inside the rectangle
2. Sets those pixels to red in the framebuffer
3. `renderer_present()` copies framebuffer to window

## Colors

Colors are RGBA - red, green, blue, alpha. Each value 0-255.

```c
Color red = color_make(255, 0, 0, 255);     // Solid red
Color green = color_make(0, 255, 0, 255);   // Solid green
Color transparent_blue = color_make(0, 0, 255, 128);  // Half-transparent blue
```

Alpha controls transparency:
- 255 = opaque (can't see through it)
- 0 = transparent (invisible)
- 128 = half-transparent (blend with background)

## Animation

Use time to create movement:

```c
float time = get_seconds();
float x = 100 + sin(time) * 50;  // Oscillates left/right
renderer_draw_filled_circle(&rend, (int)x, 200, 20, color);
```

Common patterns:
- `sin(time)` - smooth back and forth (-1 to 1)
- `time * speed` - linear movement
- `sin(time * 5)` - faster oscillation

## What to Learn

### Example 01 teaches:
- The render loop pattern
- Drawing 2D shapes
- Using sin/cos for circular motion
- Basic animation

### Example 02 teaches:
- Loading image files
- Drawing images vs primitives
- Alpha blending
- Image transformations

### Example 03 teaches:
- 3D coordinates
- Matrix transformations
- Perspective projection
- Wireframe rendering

### Examples 04-07:
Build on these basics - solid 3D, particles, textures, game logic.

## Common Mistakes

### Window closes immediately
Check you have the game loop. Without `while (rend.is_running)` the program exits right away.

### Nothing shows up
Did you call `renderer_present()`? The framebuffer doesn't display automatically.

### Drawing order matters
```c
renderer_draw_filled_rect(...);  // Drawn first
renderer_draw_filled_circle(...);  // Drawn on top
```

Later draws appear in front of earlier ones. Like painting layers.

### Coordinate system
- (0, 0) is top-left corner
- X increases to the right
- Y increases downward
- (width-1, height-1) is bottom-right

## Next Steps

1. Run all the examples in order
2. Modify them - change colors, sizes, speeds
3. Read the code while it's running
4. Check [docs/HOW_IT_WORKS.md](HOW_IT_WORKS.md) for internals
5. Try building something simple yourself

Start small - a bouncing ball, a moving rectangle, etc. Once that works, add more.

## Getting Help

If something doesn't work:
1. Check you ran the build script successfully
2. Make sure GCC and dependencies are installed
3. Read the error messages - they usually tell you what's wrong
4. Look at the working examples for reference

The code is simple C. If you get compiler errors, you're probably missing a semicolon or have mismatched braces.
