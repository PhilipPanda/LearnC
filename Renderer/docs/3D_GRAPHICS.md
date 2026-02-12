# 3D Graphics

## The Problem

You have 3D coordinates (x, y, z) but a 2D screen. How do you go from 3D world to 2D pixels?

## The Pipeline

```
3D Model Space  ->  World Space  ->  Camera Space  ->  Clip Space  ->  Screen Space
     (Model          (World          (View             (Projection      (Viewport
     Matrix)         Matrix)         Matrix)           Matrix)          Transform)
```

Each step is a matrix multiplication. Let's break it down.

## Vectors

A 3D point or direction:

```c
typedef struct {
    float x, y, z;
} Vec3;
```

Common operations:
- **Add**: `(1,2,3) + (4,5,6) = (5,7,9)` - component-wise
- **Scale**: `(1,2,3) * 2 = (2,4,6)` - multiply each component
- **Dot product**: `(1,2,3) · (4,5,6) = 1*4 + 2*5 + 3*6 = 32` - measures alignment
- **Cross product**: `(1,0,0) × (0,1,0) = (0,0,1)` - perpendicular vector
- **Length**: `|(1,2,3)| = sqrt(1² + 2² + 3²) = 3.74`
- **Normalize**: `(3,4,0) -> (0.6, 0.8, 0)` - same direction, length 1

## Matrices

A 4x4 grid of numbers that transforms vectors:

```c
typedef struct {
    float m[4][4];
} Mat4;
```

Why 4x4 for 3D? Because we need to represent translation, and 3x3 matrices can't do that. The fourth coordinate (w) is homogeneous coordinates - trust me, it works.

### Identity Matrix

Doesn't change anything:
```
[1 0 0 0]
[0 1 0 0]
[0 0 1 0]
[0 0 0 1]
```

### Translation Matrix

Moves things:
```
[1 0 0 tx]
[0 1 0 ty]
[0 0 1 tz]
[0 0 0 1]
```

### Rotation Matrix (around Y axis)

Spins things:
```
[cos(θ)  0  sin(θ)  0]
[0       1  0       0]
[-sin(θ) 0  cos(θ)  0]
[0       0  0       1]
```

X and Z rotations are similar but rotate around those axes.

### Scale Matrix

Makes things bigger/smaller:
```
[sx 0  0  0]
[0  sy 0  0]
[0  0  sz 0]
[0  0  0  1]
```

## Applying Transformations

Multiply the matrix by the vector:

```c
Vec3 transform(Mat4 m, Vec3 v) {
    Vec3 result;
    result.x = m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z + m.m[0][3];
    result.y = m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z + m.m[1][3];
    result.z = m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3];
    float w  = m.m[3][0]*v.x + m.m[3][1]*v.y + m.m[3][2]*v.z + m.m[3][3];
    
    // Perspective divide
    if (w != 1.0f) {
        result.x /= w;
        result.y /= w;
        result.z /= w;
    }
    
    return result;
}
```

## Combining Transformations

Want to rotate then translate? Multiply the matrices:

```c
Mat4 model = mat4_identity();
model = mat4_mul(model, mat4_translate(2, 0, 0));  // Move right
model = mat4_mul(model, mat4_rotate_y(angle));     // Then spin

Vec3 transformed = mat4_mul_vec3(model, original_point);
```

**Order matters!** Rotating then translating is different from translating then rotating.

## The View Matrix

Camera position and orientation. Usually built with look-at:

```c
Mat4 view = mat4_look_at(
    eye,     // Camera position
    target,  // Where it's looking
    up       // Which way is up
);
```

This transforms world coordinates into camera-relative coordinates (camera at origin, looking down -Z axis).

## The Projection Matrix

Makes distant things appear smaller - perspective projection.

```c
Mat4 proj = mat4_perspective(
    fov,    // Field of view (like 60 degrees)
    aspect, // width/height ratio
    near,   // Near clipping plane
    far     // Far clipping plane
);
```

This is the magic that makes 3D look 3D. It does two things:
1. Creates a perspective divide (divide by depth)
2. Maps the visible volume to a cube from -1 to 1 in all directions

After projection, coordinates are in "normalized device coordinates" (NDC) from -1 to 1.

## Screen Space

Final step - map from NDC to actual pixels:

```c
int screen_x = (ndc.x + 1.0) * 0.5 * screen_width;
int screen_y = (1.0 - ndc.y) * 0.5 * screen_height;  // Flip Y
```

Now you have pixel coordinates you can draw.

## Putting It Together

```c
// For each vertex in your 3D model:
Vec3 vertex = {0, 0, -5};  // Original position

// Transform chain
Mat4 mvp = mat4_mul(projection, mat4_mul(view, model));
Vec3 clip_space = mat4_mul_vec3(mvp, vertex);

// Already did perspective divide in mat4_mul_vec3
// clip_space is now NDC coordinates

// To screen
int x = (int)((clip_space.x + 1.0f) * 0.5f * width);
int y = (int)((1.0f - clip_space.y) * 0.5f * height);

// Draw at (x, y)
```

## Triangle Rasterization

Once you have screen-space coordinates for three vertices, you need to fill in the pixels:

1. Find the bounding box
2. For each pixel in the box:
   - Calculate barycentric coordinates
   - If inside triangle (all barycentric coords positive):
     - Use barycentric coords to interpolate color/depth/texture
     - Draw the pixel

This is scan conversion - turning vector data (triangles) into raster data (pixels).

## Depth Testing

Problem: Multiple triangles might draw to the same pixel. Which one is in front?

Solution: Keep a depth buffer (Z-buffer) - one depth value per pixel. When drawing:

```c
for each pixel {
    float new_depth = interpolate_depth(triangle);
    if (new_depth < depth_buffer[pixel]) {
        depth_buffer[pixel] = new_depth;
        framebuffer[pixel] = color;
    }
}
```

Only draw if this triangle is closer than what's already there.

## Lighting (Basic)

How bright should a surface be? Depends on angle to light:

```c
// Normal = surface direction
// Light_dir = direction to light
float brightness = dot(normal, light_dir);
if (brightness < 0) brightness = 0;  // Back-facing

Color final_color = base_color * brightness;
```

This is lambertian/diffuse lighting. More complex lighting adds specular highlights, ambient light, etc.

## Why This Matters

Every 3D engine does exactly this, just faster and with more features. OpenGL, DirectX, game engines - same pipeline. Understanding it in software helps you use GPU APIs effectively, debug rendering issues, and write shaders.

The GPU is just hardware that does these matrix multiplies and triangle rasterizations really, really fast in parallel.
