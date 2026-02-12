<p align="center">
  <img src="https://github.com/PhilipPanda/LearnC/blob/main/github/images/banner.png" alt="LearnC banner" width="500"/>
</p>

# LearnC

Learning C by building real stuff.

## What is this?

This isn't another "Hello World" tutorial. These are actual programs you can run, modify, and learn from - graphics renderers, games, parsers, whatever's useful.

C still runs most of the world (Linux, Windows kernel, databases, game engines, embedded systems). Worth learning properly.

## What's here

**Renderer/** - Software renderer that draws to a window
- 2D primitives (lines, circles, rectangles, triangles)
- 3D wireframe and solid rendering
- Image loading and display
- Basic particle systems
- Simple game example

More projects coming.

## Requirements

- GCC (MinGW on Windows, or MSYS2)
- Windows: GDI32 (already installed)
- Linux: X11 development files (`sudo apt install libx11-dev`)

## Quick start

```bash
cd Renderer/examples
./build_examples.bat    # Windows
./build_examples.sh     # Linux

# Run any example
bin/01_basic_shapes.exe
bin/03_3d_cube.exe
bin/07_simple_game.exe
```

## Learning approach

Each example builds on concepts from previous ones:
1. Basic shapes - drawing pixels, lines, circles
2. Images - loading from files, alpha blending
3. 3D wireframe - matrices, projection, rotation
4. 3D solid - triangle rasterization, simple lighting
5. Particles - simple physics simulation
6. Textured cube - texture mapping basics
7. Simple game - putting it all together

Read the code. Change things. Break stuff and fix it. That's how you learn.

## Documentation

Check `Renderer/docs/` for explanations of how everything works - the renderer internals, 3D math, rasterization, etc.

Each example has comments explaining the important parts.

## License

MIT - do whatever you want with it
