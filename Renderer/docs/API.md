# API Reference

## Setup and Lifecycle

### `renderer_init`
```c
bool renderer_init(Renderer* rend, const char* title, int width, int height);
```
Creates a window and initializes the framebuffer. Returns false if it fails.

**Example:**
```c
Renderer rend = {0};
if (!renderer_init(&rend, "My Program", 800, 600)) {
    // Handle error
}
```

### `renderer_cleanup`
```c
void renderer_cleanup(Renderer* rend);
```
Closes the window and frees resources. Call this before exiting.

### `renderer_handle_events`
```c
void renderer_handle_events(Renderer* rend);
```
Processes window events (close button, keyboard, etc). Sets `rend->is_running` to false when user closes window.

### `renderer_present`
```c
void renderer_present(Renderer* rend);
```
Copies the framebuffer to the window. Call once per frame after drawing.

### `renderer_sleep`
```c
void renderer_sleep(int ms);
```
Sleep for milliseconds. Useful for frame rate limiting (16ms ≈ 60 FPS).

## Drawing - 2D

### `renderer_clear`
```c
void renderer_clear(Renderer* rend, Color color);
```
Fills the entire framebuffer with a color. Call at start of each frame.

### `renderer_draw_pixel`
```c
void renderer_draw_pixel(Renderer* rend, int x, int y, Color color);
```
Sets a single pixel. Slow if you call it a lot - use higher-level functions when possible.

### `renderer_draw_line`
```c
void renderer_draw_line(Renderer* rend, int x1, int y1, int x2, int y2, Color color);
```
Draws a 1-pixel wide line from (x1, y1) to (x2, y2).

### `renderer_draw_thick_line`
```c
void renderer_draw_thick_line(Renderer* rend, int x1, int y1, int x2, int y2, 
                              int thickness, Color color);
```
Like draw_line but thicker.

### `renderer_draw_rect`
```c
void renderer_draw_rect(Renderer* rend, int x, int y, int w, int h, Color color);
```
Draws a rectangle outline. (x, y) is top-left corner.

### `renderer_draw_filled_rect`
```c
void renderer_draw_filled_rect(Renderer* rend, int x, int y, int w, int h, Color color);
```
Draws a filled rectangle.

### `renderer_draw_circle`
```c
void renderer_draw_circle(Renderer* rend, int cx, int cy, int radius, Color color);
```
Draws a circle outline. (cx, cy) is center.

### `renderer_draw_filled_circle`
```c
void renderer_draw_filled_circle(Renderer* rend, int cx, int cy, int radius, Color color);
```
Draws a filled circle.

### `renderer_draw_triangle`
```c
void renderer_draw_triangle(Renderer* rend, int x1, int y1, int x2, int y2, 
                           int x3, int y3, Color color);
```
Draws a triangle outline.

### `renderer_draw_filled_triangle`
```c
void renderer_draw_filled_triangle(Renderer* rend, int x1, int y1, int x2, int y2, 
                                   int x3, int y3, Color color);
```
Draws a filled triangle.

### `renderer_draw_gradient_rect`
```c
void renderer_draw_gradient_rect(Renderer* rend, int x, int y, int w, int h, 
                                 Color c1, Color c2, bool vertical);
```
Draws a rectangle with a gradient from c1 to c2. Set `vertical` to true for top-to-bottom, false for left-to-right.

### `renderer_draw_text`
```c
void renderer_draw_text(Renderer* rend, const char* text, int x, int y, 
                        Color color, int scale);
```
Draws text at (x, y). Scale 1 is normal, 2 is double size, etc. Basic bitmap font only.

## Drawing - Images

### `image_load_bmp`
```c
Image* image_load_bmp(const char* filename);
```
Loads a BMP file. Returns NULL if it fails. Only supports 24-bit and 32-bit BMPs.

### `image_free`
```c
void image_free(Image* img);
```
Frees an image loaded with `image_load_bmp`.

### `renderer_draw_image`
```c
void renderer_draw_image(Renderer* rend, Image* img, int x, int y);
```
Draws an image at (x, y) at its original size.

### `renderer_draw_image_ex`
```c
void renderer_draw_image_ex(Renderer* rend, Image* img, int x, int y, 
                            float scale, uint8_t alpha);
```
Draws an image scaled and with alpha transparency. Scale 1.0 is normal, 2.0 is double size. Alpha 0-255.

### `renderer_apply_tint`
```c
void renderer_apply_tint(Renderer* rend, Image* img, int x, int y, Color tint);
```
Draws an image tinted with a color. Multiplies image RGB by tint RGB.

## Drawing - 3D

### `renderer_project_point`
```c
void renderer_project_point(Renderer* rend, Vec3 point, Mat4 transform, 
                            int* x, int* y, float* depth);
```
Projects a 3D point to screen coordinates. Pass NULL for depth if you don't need it.

**Example:**
```c
Vec3 point_3d = vec3_make(0, 0, -5);
int x, y;
float depth;
renderer_project_point(&rend, point_3d, mvp_matrix, &x, &y, &depth);
// Now draw something at (x, y)
```

### `renderer_draw_line_3d`
```c
void renderer_draw_line_3d(Renderer* rend, Vec3 p1, Vec3 p2, Mat4 transform, Color color);
```
Draws a line between two 3D points. Transform should be model-view-projection matrix.

### `renderer_draw_triangle_3d`
```c
void renderer_draw_triangle_3d(Renderer* rend, Vec3 p1, Vec3 p2, Vec3 p3, 
                               Mat4 transform, Color color);
```
Draws a filled 3D triangle. Does backface culling.

### `renderer_draw_textured_triangle`
```c
void renderer_draw_textured_triangle(Renderer* rend, int x1, int y1, int x2, int y2, 
                                     int x3, int y3, float u1, float v1, float u2, 
                                     float v2, float u3, float v3, Image* texture);
```
Draws a textured triangle. (x, y) are screen coords, (u, v) are texture coordinates (0-1 range).

## Math - Vectors

### `vec3_make`
```c
Vec3 vec3_make(float x, float y, float z);
```
Creates a vector.

### `vec3_add`, `vec3_sub`
```c
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
```
Vector addition and subtraction.

### `vec3_mul`
```c
Vec3 vec3_mul(Vec3 v, float s);
```
Multiplies vector by scalar.

### `vec3_dot`
```c
float vec3_dot(Vec3 a, Vec3 b);
```
Dot product. Returns scalar. Useful for angle calculations.

### `vec3_cross`
```c
Vec3 vec3_cross(Vec3 a, Vec3 b);
```
Cross product. Returns perpendicular vector.

### `vec3_normalize`
```c
Vec3 vec3_normalize(Vec3 v);
```
Returns vector with same direction but length 1.

## Math - Matrices

### `mat4_identity`
```c
Mat4 mat4_identity(void);
```
Returns identity matrix (no transformation).

### `mat4_translate`
```c
Mat4 mat4_translate(float x, float y, float z);
```
Creates translation matrix.

### `mat4_rotate_x`, `mat4_rotate_y`, `mat4_rotate_z`
```c
Mat4 mat4_rotate_x(float angle);
Mat4 mat4_rotate_y(float angle);
Mat4 mat4_rotate_z(float angle);
```
Creates rotation matrices. Angle in radians.

### `mat4_scale`
```c
Mat4 mat4_scale(float x, float y, float z);
```
Creates scale matrix.

### `mat4_perspective`
```c
Mat4 mat4_perspective(float fov, float aspect, float near, float far);
```
Creates perspective projection matrix. FOV in radians.

**Example:**
```c
Mat4 proj = mat4_perspective(60.0f * (M_PI / 180.0f), 800.0f/600.0f, 0.1f, 100.0f);
```

### `mat4_look_at`
```c
Mat4 mat4_look_at(Vec3 eye, Vec3 target, Vec3 up);
```
Creates view matrix from camera position, look target, and up direction.

### `mat4_mul`
```c
Mat4 mat4_mul(Mat4 a, Mat4 b);
```
Multiplies two matrices. Order matters: `mat4_mul(A, B)` applies B then A.

### `mat4_mul_vec3`
```c
Vec3 mat4_mul_vec3(Mat4 m, Vec3 v, float w);
```
Transforms a vector by a matrix. w is usually 1.0 for points, 0.0 for directions.

## Colors

### `color_make`
```c
Color color_make(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
```
Creates a color. RGB values 0-255. Alpha 255 is opaque, 0 is transparent.

### `color_blend`
```c
Color color_blend(Color src, Color dst);
```
Alpha blends source over destination. Uses src.a for blend factor.

## Common Patterns

### Basic render loop
```c
while (rend.is_running) {
    renderer_handle_events(&rend);
    renderer_clear(&rend, color_make(0, 0, 0, 255));
    
    // Draw here
    
    renderer_present(&rend);
    renderer_sleep(16);
}
```

### 3D transform chain
```c
Mat4 model = mat4_identity();
model = mat4_mul(model, mat4_rotate_y(angle));
model = mat4_mul(model, mat4_translate(x, y, z));

Mat4 view = mat4_look_at(camera_pos, look_target, up_vec);
Mat4 proj = mat4_perspective(fov, aspect, near, far);

Mat4 mvp = mat4_mul(proj, mat4_mul(view, model));
```

### Animation
```c
float time = get_time_in_seconds();
float angle = time * 2.0f;  // Rotates over time
float pulse = sin(time * 3.0f);  // Oscillates -1 to 1
```
