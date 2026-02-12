# How the Renderer Works

## The Big Picture

The renderer does three things:
1. Gives you a window with a pixel buffer (framebuffer)
2. Lets you draw stuff into that buffer
3. Copies the buffer to the screen

That's it. Everything else is just different ways to set pixel colors.

## Memory Layout

### The Framebuffer

```c
uint32_t* framebuffer;  // Array of pixels, ARGB format
```

Each pixel is a 32-bit integer: `0xAARRGGBB`
- Byte 3 (high): Alpha (transparency)
- Byte 2: Red
- Byte 1: Green  
- Byte 0 (low): Blue

For an 800x600 window, you have 480,000 pixels in a flat array:
```
[pixel(0,0), pixel(1,0), pixel(2,0), ..., pixel(799,0),
 pixel(0,1), pixel(1,1), ..., pixel(799,599)]
```

To get pixel (x, y): `framebuffer[y * width + x]`

That's row-major order - first row left to right, then second row, etc.

## Drawing a Pixel

```c
void renderer_draw_pixel(Renderer* rend, int x, int y, Color color) {
    if (x < 0 || x >= rend->width || y < 0 || y >= rend->height) return;
    
    int index = y * rend->width + x;
    uint32_t pixel = (color.a << 24) | (color.r << 16) | 
                     (color.g << 8) | color.b;
    rend->framebuffer[index] = pixel;
}
```

Bounds check, calculate index, pack color into 32 bits, write it. That's all drawing is.

## Drawing a Line

Bresenham's algorithm - walk from point A to point B, deciding whether to step horizontally or diagonally at each pixel.

```c
// Simplified version
void draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        set_pixel(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}
```

The error term (`err`) tracks whether we're above or below the ideal line. When it gets too big in one direction, we step that way to correct it.

## Drawing a Circle

For each point around the circle, calculate where it should be using sin/cos:

```c
for (float angle = 0; angle < 2 * PI; angle += step) {
    int x = cx + (int)(cos(angle) * radius);
    int y = cy + (int)(sin(angle) * radius);
    set_pixel(x, y);
}
```

For filled circles, draw horizontal lines for each y coordinate:

```c
for (int y = -radius; y <= radius; y++) {
    int x_offset = (int)sqrt(radius*radius - y*y);
    draw_horizontal_line(cx - x_offset, cx + x_offset, cy + y);
}
```

## Drawing a Triangle

### Wireframe

Just draw three lines connecting the vertices.

### Filled

1. Find the bounding box (min/max x and y of the three vertices)
2. For every pixel in that box:
   - Check if it's inside the triangle using barycentric coordinates
   - If yes, draw it

Barycentric coordinates tell you where a point is relative to triangle vertices:
- If all three coordinates are positive, point is inside
- The coordinates also give you weights for interpolation (colors, texture coords, etc.)

```c
// Is point P inside triangle ABC?
barycentric(A, B, C, P) -> (u, v, w)
if (u >= 0 && v >= 0 && w >= 0) {
    // Inside! And we can interpolate:
    color = u * colorA + v * colorB + w * colorC;
}
```

## Alpha Blending

When drawing semi-transparent stuff, blend source and destination colors:

```c
Color blend(Color src, Color dst) {
    float alpha = src.a / 255.0f;
    return Color {
        .r = src.r * alpha + dst.r * (1 - alpha),
        .g = src.g * alpha + dst.g * (1 - alpha),
        .b = src.b * alpha + dst.b * (1 - alpha),
        .a = 255
    };
}
```

The alpha value says "how much of source vs destination." Alpha = 0 means all destination, alpha = 1 means all source.

## Platform Specifics

### Windows (GDI32)

1. Create a window with Win32 API
2. Create an off-screen bitmap (DIB section) for the framebuffer
3. Draw to the framebuffer
4. BitBlt the framebuffer to the window

### Linux (X11)

1. Create a window with XLib
2. Create an XImage pointing to the framebuffer array
3. Draw to the framebuffer
4. XPutImage to copy framebuffer to window

Both platforms give you the same interface - a pixel array you can write to. The renderer hides the platform differences.

## The Render Loop

```c
while (running) {
    handle_input();           // Process keyboard/mouse/close events
    clear_framebuffer();      // Fill with background color
    draw_everything();        // Your rendering code
    copy_to_screen();         // Show it
    sleep(16);                // Cap at ~60 FPS
}
```

This is the game loop. Same pattern used everywhere from AAA games to small demos.

## Why Software Rendering?

GPUs do the same thing, just massively parallel. Instead of one thread checking pixels in a triangle, GPUs have thousands of tiny cores doing it simultaneously.

By doing it yourself on the CPU:
- You understand what actually happens
- You appreciate why GPUs are designed the way they are
- You can implement custom algorithms that don't map to GPU hardware
- You have total control for debugging and experimentation

Once you understand software rendering, GPU programming makes way more sense.
