#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <unistd.h>
#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef struct {
    uint8_t r, g, b, a;
} Color;

typedef struct {
    int width;
    int height;
    uint32_t* pixels;
} Image;

typedef struct {
#ifdef _WIN32
    HWND hwnd;
    HDC hdc;
    HDC back_buffer_dc;
    HBITMAP back_buffer;
#else
    Display* display;
    Window window;
    GC gc;
    XImage* ximage;
#endif
    uint32_t* framebuffer;
    int width;
    int height;
    bool is_running;
} Renderer;

bool renderer_init(Renderer* rend, const char* title, int width, int height);
void renderer_cleanup(Renderer* rend);
void renderer_clear(Renderer* rend, Color color);
void renderer_present(Renderer* rend);
void renderer_handle_events(Renderer* rend);
void renderer_sleep(int ms);

void renderer_draw_pixel(Renderer* rend, int x, int y, Color color);
void renderer_draw_rect(Renderer* rend, int x, int y, int w, int h, Color color);
void renderer_draw_filled_rect(Renderer* rend, int x, int y, int w, int h, Color color);
void renderer_draw_circle(Renderer* rend, int cx, int cy, int radius, Color color);
void renderer_draw_filled_circle(Renderer* rend, int cx, int cy, int radius, Color color);
void renderer_draw_line(Renderer* rend, int x1, int y1, int x2, int y2, Color color);
void renderer_draw_triangle(Renderer* rend, int x1, int y1, int x2, int y2, int x3, int y3, Color color);
void renderer_draw_filled_triangle(Renderer* rend, int x1, int y1, int x2, int y2, int x3, int y3, Color color);
void renderer_draw_thick_line(Renderer* rend, int x1, int y1, int x2, int y2, int thickness, Color color);

Image* image_load_bmp(const char* filename);
void image_free(Image* img);
void renderer_draw_image(Renderer* rend, Image* img, int x, int y);
void renderer_draw_image_ex(Renderer* rend, Image* img, int x, int y, float scale, uint8_t alpha);

void renderer_apply_tint(Renderer* rend, Image* img, int x, int y, Color tint);
void renderer_draw_gradient_rect(Renderer* rend, int x, int y, int w, int h, Color c1, Color c2, bool vertical);
void renderer_draw_textured_triangle(Renderer* rend, int x1, int y1, int x2, int y2, int x3, int y3,
    float u1, float v1, float u2, float v2, float u3, float v3, Image* texture);

Color color_make(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
Color color_blend(Color src, Color dst);

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    float m[4][4];
} Mat4;

Vec3 vec3_make(float x, float y, float z);
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec3 vec3_mul(Vec3 v, float s);
Vec3 vec3_cross(Vec3 a, Vec3 b);
float vec3_dot(Vec3 a, Vec3 b);
Vec3 vec3_normalize(Vec3 v);

Mat4 mat4_identity(void);
Mat4 mat4_perspective(float fov, float aspect, float near_plane, float far_plane);
Mat4 mat4_look_at(Vec3 eye, Vec3 target, Vec3 up);
Mat4 mat4_translate(float x, float y, float z);
Mat4 mat4_rotate_x(float angle);
Mat4 mat4_rotate_y(float angle);
Mat4 mat4_rotate_z(float angle);
Mat4 mat4_scale(float x, float y, float z);
Mat4 mat4_mul(Mat4 a, Mat4 b);
Vec3 mat4_mul_vec3(Mat4 m, Vec3 v, float w);

void renderer_project_point(Renderer* rend, Vec3 point, Mat4 transform, int* x, int* y, float* depth);
void renderer_draw_line_3d(Renderer* rend, Vec3 p1, Vec3 p2, Mat4 transform, Color color);
void renderer_draw_triangle_3d(Renderer* rend, Vec3 p1, Vec3 p2, Vec3 p3, Mat4 transform, Color color);

void renderer_draw_text(Renderer* rend, const char* text, int x, int y, Color color, int scale);

#endif
