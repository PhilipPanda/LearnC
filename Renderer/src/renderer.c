#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static Renderer* g_renderer = NULL;

#ifdef _WIN32

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_CLOSE:
            if (g_renderer) g_renderer->is_running = false;
            return 0;
        case WM_KEYDOWN:
            if (wparam == VK_ESCAPE && g_renderer) 
                g_renderer->is_running = false;
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool renderer_init(Renderer* rend, const char* title, int width, int height) {
    g_renderer = rend;
    rend->width = width;
    rend->height = height;
    rend->is_running = true;

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "CRendererClass";

    if (!RegisterClassEx(&wc)) return false;

    RECT rect = {0, 0, width, height};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    rend->hwnd = CreateWindowEx(0, "CRendererClass", title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!rend->hwnd) return false;

    rend->hdc = GetDC(rend->hwnd);
    rend->back_buffer_dc = CreateCompatibleDC(rend->hdc);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    rend->back_buffer = CreateDIBSection(rend->back_buffer_dc, &bmi, DIB_RGB_COLORS,
        (void**)&rend->framebuffer, NULL, 0);

    if (!rend->back_buffer) return false;

    SelectObject(rend->back_buffer_dc, rend->back_buffer);
    ShowWindow(rend->hwnd, SW_SHOW);
    UpdateWindow(rend->hwnd);

    return true;
}

void renderer_cleanup(Renderer* rend) {
    if (rend->back_buffer) DeleteObject(rend->back_buffer);
    if (rend->back_buffer_dc) DeleteDC(rend->back_buffer_dc);
    if (rend->hdc) ReleaseDC(rend->hwnd, rend->hdc);
    if (rend->hwnd) DestroyWindow(rend->hwnd);
    g_renderer = NULL;
}

void renderer_present(Renderer* rend) {
    BitBlt(rend->hdc, 0, 0, rend->width, rend->height, rend->back_buffer_dc, 0, 0, SRCCOPY);
}

void renderer_handle_events(Renderer* rend) {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void renderer_sleep(int ms) {
    Sleep(ms);
}

#else

bool renderer_init(Renderer* rend, const char* title, int width, int height) {
    g_renderer = rend;
    rend->width = width;
    rend->height = height;
    rend->is_running = true;

    rend->display = XOpenDisplay(NULL);
    if (!rend->display) return false;

    int screen = DefaultScreen(rend->display);
    rend->window = XCreateSimpleWindow(rend->display, RootWindow(rend->display, screen),
        0, 0, width, height, 1, BlackPixel(rend->display, screen), 
        WhitePixel(rend->display, screen));

    XStoreName(rend->display, rend->window, title);
    XSelectInput(rend->display, rend->window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(rend->display, rend->window);

    rend->gc = XCreateGC(rend->display, rend->window, 0, NULL);

    rend->framebuffer = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    if (!rend->framebuffer) return false;

    Visual* visual = DefaultVisual(rend->display, screen);
    rend->ximage = XCreateImage(rend->display, visual, 24, ZPixmap, 0,
        (char*)rend->framebuffer, width, height, 32, 0);

    if (!rend->ximage) {
        free(rend->framebuffer);
        return false;
    }

    Atom wm_delete = XInternAtom(rend->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(rend->display, rend->window, &wm_delete, 1);

    return true;
}

void renderer_cleanup(Renderer* rend) {
    if (rend->ximage) {
        rend->ximage->data = NULL;
        XDestroyImage(rend->ximage);
    }
    if (rend->framebuffer) free(rend->framebuffer);
    if (rend->gc) XFreeGC(rend->display, rend->gc);
    if (rend->window) XDestroyWindow(rend->display, rend->window);
    if (rend->display) XCloseDisplay(rend->display);
    g_renderer = NULL;
}

void renderer_present(Renderer* rend) {
    XPutImage(rend->display, rend->window, rend->gc, rend->ximage, 
        0, 0, 0, 0, rend->width, rend->height);
    XFlush(rend->display);
}

void renderer_handle_events(Renderer* rend) {
    while (XPending(rend->display)) {
        XEvent event;
        XNextEvent(rend->display, &event);
        
        if (event.type == KeyPress) {
            KeySym key = XLookupKeysym(&event.xkey, 0);
            if (key == XK_Escape) rend->is_running = false;
        } else if (event.type == ClientMessage) {
            rend->is_running = false;
        }
    }
}

void renderer_sleep(int ms) {
    usleep(ms * 1000);
}

#endif

void renderer_clear(Renderer* rend, Color color) {
    uint32_t pixel = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
    for (int i = 0; i < rend->width * rend->height; i++) {
        rend->framebuffer[i] = pixel;
    }
}

void renderer_draw_pixel(Renderer* rend, int x, int y, Color color) {
    if (x < 0 || x >= rend->width || y < 0 || y >= rend->height) return;
    
    int index = y * rend->width + x;
    uint32_t* pixel = &rend->framebuffer[index];
    
    if (color.a == 255) {
        *pixel = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
    } else {
        uint8_t dst_r = (*pixel >> 16) & 0xFF;
        uint8_t dst_g = (*pixel >> 8) & 0xFF;
        uint8_t dst_b = *pixel & 0xFF;
        
        float alpha = color.a / 255.0f;
        uint8_t r = (uint8_t)(color.r * alpha + dst_r * (1 - alpha));
        uint8_t g = (uint8_t)(color.g * alpha + dst_g * (1 - alpha));
        uint8_t b = (uint8_t)(color.b * alpha + dst_b * (1 - alpha));
        
        *pixel = (255 << 24) | (r << 16) | (g << 8) | b;
    }
}

void renderer_draw_rect(Renderer* rend, int x, int y, int w, int h, Color color) {
    for (int i = 0; i < w; i++) {
        renderer_draw_pixel(rend, x + i, y, color);
        renderer_draw_pixel(rend, x + i, y + h - 1, color);
    }
    for (int i = 0; i < h; i++) {
        renderer_draw_pixel(rend, x, y + i, color);
        renderer_draw_pixel(rend, x + w - 1, y + i, color);
    }
}

void renderer_draw_filled_rect(Renderer* rend, int x, int y, int w, int h, Color color) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            renderer_draw_pixel(rend, x + i, y + j, color);
        }
    }
}

void renderer_draw_circle(Renderer* rend, int cx, int cy, int radius, Color color) {
    int x = radius, y = 0, err = 0;
    while (x >= y) {
        renderer_draw_pixel(rend, cx + x, cy + y, color);
        renderer_draw_pixel(rend, cx + y, cy + x, color);
        renderer_draw_pixel(rend, cx - y, cy + x, color);
        renderer_draw_pixel(rend, cx - x, cy + y, color);
        renderer_draw_pixel(rend, cx - x, cy - y, color);
        renderer_draw_pixel(rend, cx - y, cy - x, color);
        renderer_draw_pixel(rend, cx + y, cy - x, color);
        renderer_draw_pixel(rend, cx + x, cy - y, color);
        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void renderer_draw_filled_circle(Renderer* rend, int cx, int cy, int radius, Color color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                renderer_draw_pixel(rend, cx + x, cy + y, color);
            }
        }
    }
}

void renderer_draw_line(Renderer* rend, int x1, int y1, int x2, int y2, Color color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        renderer_draw_pixel(rend, x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

Image* image_load_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    uint8_t header[54];
    fread(header, 1, 54, file);

    if (header[0] != 'B' || header[1] != 'M') {
        fclose(file);
        return NULL;
    }

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int bits_per_pixel = *(short*)&header[28];

    if (bits_per_pixel != 24 && bits_per_pixel != 32) {
        fclose(file);
        return NULL;
    }

    Image* img = (Image*)malloc(sizeof(Image));
    img->width = width;
    img->height = abs(height);
    img->pixels = (uint32_t*)malloc(img->width * img->height * sizeof(uint32_t));

    int row_size = ((bits_per_pixel * width + 31) / 32) * 4;
    uint8_t* row = (uint8_t*)malloc(row_size);

    for (int y = 0; y < img->height; y++) {
        fread(row, 1, row_size, file);
        for (int x = 0; x < img->width; x++) {
            int pixel_index = (height > 0) ? (img->height - 1 - y) * img->width + x : y * img->width + x;
            uint8_t b = row[x * (bits_per_pixel / 8) + 0];
            uint8_t g = row[x * (bits_per_pixel / 8) + 1];
            uint8_t r = row[x * (bits_per_pixel / 8) + 2];
            uint8_t a = (bits_per_pixel == 32) ? row[x * 4 + 3] : 255;
            img->pixels[pixel_index] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }

    free(row);
    fclose(file);
    return img;
}

void image_free(Image* img) {
    if (img) {
        free(img->pixels);
        free(img);
    }
}

void renderer_draw_image(Renderer* rend, Image* img, int x, int y) {
    if (!img) return;
    for (int j = 0; j < img->height; j++) {
        for (int i = 0; i < img->width; i++) {
            uint32_t pixel = img->pixels[j * img->width + i];
            Color color = {
                (pixel >> 16) & 0xFF,
                (pixel >> 8) & 0xFF,
                pixel & 0xFF,
                (pixel >> 24) & 0xFF
            };
            renderer_draw_pixel(rend, x + i, y + j, color);
        }
    }
}

void renderer_draw_image_ex(Renderer* rend, Image* img, int x, int y, float scale, uint8_t alpha) {
    if (!img) return;
    int scaled_w = (int)(img->width * scale);
    int scaled_h = (int)(img->height * scale);
    
    for (int j = 0; j < scaled_h; j++) {
        for (int i = 0; i < scaled_w; i++) {
            int src_x = (int)(i / scale);
            int src_y = (int)(j / scale);
            if (src_x >= img->width || src_y >= img->height) continue;
            
            uint32_t pixel = img->pixels[src_y * img->width + src_x];
            Color color = {
                (pixel >> 16) & 0xFF,
                (pixel >> 8) & 0xFF,
                pixel & 0xFF,
                (uint8_t)(((pixel >> 24) & 0xFF) * (alpha / 255.0f))
            };
            renderer_draw_pixel(rend, x + i, y + j, color);
        }
    }
}

void renderer_apply_tint(Renderer* rend, Image* img, int x, int y, Color tint) {
    if (!img) return;
    for (int j = 0; j < img->height; j++) {
        for (int i = 0; i < img->width; i++) {
            uint32_t pixel = img->pixels[j * img->width + i];
            Color color = {
                (uint8_t)(((pixel >> 16) & 0xFF) * (tint.r / 255.0f)),
                (uint8_t)(((pixel >> 8) & 0xFF) * (tint.g / 255.0f)),
                (uint8_t)((pixel & 0xFF) * (tint.b / 255.0f)),
                (pixel >> 24) & 0xFF
            };
            renderer_draw_pixel(rend, x + i, y + j, color);
        }
    }
}

void renderer_draw_gradient_rect(Renderer* rend, int x, int y, int w, int h, Color c1, Color c2, bool vertical) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            float t = vertical ? (float)j / h : (float)i / w;
            Color color = {
                (uint8_t)(c1.r + (c2.r - c1.r) * t),
                (uint8_t)(c1.g + (c2.g - c1.g) * t),
                (uint8_t)(c1.b + (c2.b - c1.b) * t),
                255
            };
            renderer_draw_pixel(rend, x + i, y + j, color);
        }
    }
}

Color color_make(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    Color c = {r, g, b, a};
    return c;
}

Color color_blend(Color src, Color dst) {
    float alpha = src.a / 255.0f;
    Color result = {
        (uint8_t)(src.r * alpha + dst.r * (1 - alpha)),
        (uint8_t)(src.g * alpha + dst.g * (1 - alpha)),
        (uint8_t)(src.b * alpha + dst.b * (1 - alpha)),
        255
    };
    return result;
}

void renderer_draw_triangle(Renderer* rend, int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
    renderer_draw_line(rend, x1, y1, x2, y2, color);
    renderer_draw_line(rend, x2, y2, x3, y3, color);
    renderer_draw_line(rend, x3, y3, x1, y1, color);
}

void renderer_draw_filled_triangle(Renderer* rend, int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
    if (y1 > y2) { int tx = x1, ty = y1; x1 = x2; y1 = y2; x2 = tx; y2 = ty; }
    if (y1 > y3) { int tx = x1, ty = y1; x1 = x3; y1 = y3; x3 = tx; y3 = ty; }
    if (y2 > y3) { int tx = x2, ty = y2; x2 = x3; y2 = y3; x3 = tx; y3 = ty; }

    int total_height = y3 - y1;
    if (total_height == 0) return;

    for (int y = y1; y <= y3; y++) {
        bool second_half = y > y2 || y2 == y1;
        int segment_height = second_half ? y3 - y2 : y2 - y1;
        if (segment_height == 0) continue;

        float alpha = (float)(y - y1) / total_height;
        float beta = (float)(y - (second_half ? y2 : y1)) / segment_height;

        int xa = x1 + (x3 - x1) * alpha;
        int xb = second_half ? x2 + (x3 - x2) * beta : x1 + (x2 - x1) * beta;

        if (xa > xb) { int tmp = xa; xa = xb; xb = tmp; }
        for (int x = xa; x <= xb; x++) {
            renderer_draw_pixel(rend, x, y, color);
        }
    }
}

void renderer_draw_thick_line(Renderer* rend, int x1, int y1, int x2, int y2, int thickness, Color color) {
    for (int t = -thickness/2; t <= thickness/2; t++) {
        int dx = y2 - y1;
        int dy = x2 - x1;
        float len = sqrtf(dx*dx + dy*dy);
        if (len == 0) continue;
        int ox = (int)(t * dx / len);
        int oy = (int)(-t * dy / len);
        renderer_draw_line(rend, x1 + ox, y1 + oy, x2 + ox, y2 + oy, color);
    }
}

void renderer_draw_textured_triangle(Renderer* rend, int x1, int y1, int x2, int y2, int x3, int y3,
    float u1, float v1, float u2, float v2, float u3, float v3, Image* texture) {
    if (!texture) return;
    
    if (y1 > y2) { 
        int tx = x1, ty = y1; x1 = x2; y1 = y2; x2 = tx; y2 = ty;
        float tu = u1, tv = v1; u1 = u2; v1 = v2; u2 = tu; v2 = tv;
    }
    if (y1 > y3) { 
        int tx = x1, ty = y1; x1 = x3; y1 = y3; x3 = tx; y3 = ty;
        float tu = u1, tv = v1; u1 = u3; v1 = v3; u3 = tu; v3 = tv;
    }
    if (y2 > y3) { 
        int tx = x2, ty = y2; x2 = x3; y2 = y3; x3 = tx; y3 = ty;
        float tu = u2, tv = v2; u2 = u3; v2 = v3; u3 = tu; v3 = tv;
    }

    int total_height = y3 - y1;
    if (total_height == 0) return;

    for (int y = y1; y <= y3; y++) {
        bool second_half = y > y2 || y2 == y1;
        int segment_height = second_half ? y3 - y2 : y2 - y1;
        if (segment_height == 0) continue;

        float alpha = (float)(y - y1) / total_height;
        float beta = (float)(y - (second_half ? y2 : y1)) / segment_height;

        int xa = x1 + (x3 - x1) * alpha;
        int xb = second_half ? x2 + (x3 - x2) * beta : x1 + (x2 - x1) * beta;
        
        float ua = u1 + (u3 - u1) * alpha;
        float va = v1 + (v3 - v1) * alpha;
        float ub = second_half ? u2 + (u3 - u2) * beta : u1 + (u2 - u1) * beta;
        float vb = second_half ? v2 + (v3 - v2) * beta : v1 + (v2 - v1) * beta;

        if (xa > xb) { 
            int tmp = xa; xa = xb; xb = tmp;
            float tmp_u = ua; ua = ub; ub = tmp_u;
            float tmp_v = va; va = vb; vb = tmp_v;
        }

        for (int x = xa; x <= xb; x++) {
            float t = (xb - xa) > 0 ? (float)(x - xa) / (xb - xa) : 0;
            float u = ua + (ub - ua) * t;
            float v = va + (vb - va) * t;
            
            int tex_x = (int)(u * texture->width) % texture->width;
            int tex_y = (int)(v * texture->height) % texture->height;
            if (tex_x < 0) tex_x += texture->width;
            if (tex_y < 0) tex_y += texture->height;
            
            uint32_t pixel = texture->pixels[tex_y * texture->width + tex_x];
            Color color = {
                (pixel >> 16) & 0xFF,
                (pixel >> 8) & 0xFF,
                pixel & 0xFF,
                (pixel >> 24) & 0xFF
            };
            renderer_draw_pixel(rend, x, y, color);
        }
    }
}

Vec3 vec3_make(float x, float y, float z) {
    Vec3 v = {x, y, z};
    return v;
}

Vec3 vec3_add(Vec3 a, Vec3 b) {
    return vec3_make(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return vec3_make(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 vec3_mul(Vec3 v, float s) {
    return vec3_make(v.x * s, v.y * s, v.z * s);
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    return vec3_make(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float vec3_dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 vec3_normalize(Vec3 v) {
    float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len == 0) return vec3_make(0, 0, 0);
    return vec3_make(v.x / len, v.y / len, v.z / len);
}

Mat4 mat4_identity(void) {
    Mat4 m = {0};
    m.m[0][0] = m.m[1][1] = m.m[2][2] = m.m[3][3] = 1.0f;
    return m;
}

Mat4 mat4_perspective(float fov, float aspect, float near_plane, float far_plane) {
    Mat4 m = {0};
    float tan_half_fov = tanf(fov / 2.0f);
    m.m[0][0] = 1.0f / (aspect * tan_half_fov);
    m.m[1][1] = 1.0f / tan_half_fov;
    m.m[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
    m.m[2][3] = -1.0f;
    m.m[3][2] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
    return m;
}

Mat4 mat4_look_at(Vec3 eye, Vec3 target, Vec3 up) {
    Vec3 f = vec3_normalize(vec3_sub(target, eye));
    Vec3 s = vec3_normalize(vec3_cross(f, up));
    Vec3 u = vec3_cross(s, f);
    
    Mat4 m = mat4_identity();
    m.m[0][0] = s.x;
    m.m[1][0] = s.y;
    m.m[2][0] = s.z;
    m.m[0][1] = u.x;
    m.m[1][1] = u.y;
    m.m[2][1] = u.z;
    m.m[0][2] = -f.x;
    m.m[1][2] = -f.y;
    m.m[2][2] = -f.z;
    m.m[3][0] = -vec3_dot(s, eye);
    m.m[3][1] = -vec3_dot(u, eye);
    m.m[3][2] = vec3_dot(f, eye);
    return m;
}

Mat4 mat4_translate(float x, float y, float z) {
    Mat4 m = mat4_identity();
    m.m[3][0] = x;
    m.m[3][1] = y;
    m.m[3][2] = z;
    return m;
}

Mat4 mat4_rotate_x(float angle) {
    Mat4 m = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    m.m[1][1] = c;
    m.m[1][2] = s;
    m.m[2][1] = -s;
    m.m[2][2] = c;
    return m;
}

Mat4 mat4_rotate_y(float angle) {
    Mat4 m = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    m.m[0][0] = c;
    m.m[0][2] = -s;
    m.m[2][0] = s;
    m.m[2][2] = c;
    return m;
}

Mat4 mat4_rotate_z(float angle) {
    Mat4 m = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    m.m[0][0] = c;
    m.m[0][1] = s;
    m.m[1][0] = -s;
    m.m[1][1] = c;
    return m;
}

Mat4 mat4_scale(float x, float y, float z) {
    Mat4 m = mat4_identity();
    m.m[0][0] = x;
    m.m[1][1] = y;
    m.m[2][2] = z;
    return m;
}

Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 result = {0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}

Vec3 mat4_mul_vec3(Mat4 m, Vec3 v, float w) {
    Vec3 result;
    result.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0] * w;
    result.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1] * w;
    result.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2] * w;
    float ww = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3] * w;
    if (ww != 0) {
        result.x /= ww;
        result.y /= ww;
        result.z /= ww;
    }
    return result;
}

void renderer_project_point(Renderer* rend, Vec3 point, Mat4 transform, int* x, int* y, float* depth) {
    Vec3 projected = mat4_mul_vec3(transform, point, 1.0f);
    *x = (int)((projected.x + 1.0f) * 0.5f * rend->width);
    *y = (int)((1.0f - projected.y) * 0.5f * rend->height);
    if (depth) *depth = projected.z;
}

void renderer_draw_line_3d(Renderer* rend, Vec3 p1, Vec3 p2, Mat4 transform, Color color) {
    int x1, y1, x2, y2;
    renderer_project_point(rend, p1, transform, &x1, &y1, NULL);
    renderer_project_point(rend, p2, transform, &x2, &y2, NULL);
    renderer_draw_line(rend, x1, y1, x2, y2, color);
}

void renderer_draw_triangle_3d(Renderer* rend, Vec3 p1, Vec3 p2, Vec3 p3, Mat4 transform, Color color) {
    int x1, y1, x2, y2, x3, y3;
    renderer_project_point(rend, p1, transform, &x1, &y1, NULL);
    renderer_project_point(rend, p2, transform, &x2, &y2, NULL);
    renderer_project_point(rend, p3, transform, &x3, &y3, NULL);
    renderer_draw_filled_triangle(rend, x1, y1, x2, y2, x3, y3, color);
}

static const uint8_t font_data[128][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00},
    {0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00},
    {0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00},
    {0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00},
    {0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00},
    {0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00},
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},
    {0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06},
    {0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00},
    {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00},
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00},
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00},
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00},
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00},
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00},
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00},
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00},
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00},
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00},
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00},
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06},
    {0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00},
    {0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00},
    {0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00},
    {0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00},
    {0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00},
    {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00},
    {0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00},
    {0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00},
    {0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00},
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00},
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00},
    {0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00},
    {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00},
    {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},
    {0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00},
    {0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00},
    {0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00},
    {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00},
    {0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00},
    {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00},
    {0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00},
    {0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00},
    {0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00},
    {0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00},
    {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00},
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},
    {0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00},
    {0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00},
    {0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00},
    {0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00},
    {0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00},
    {0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00},
    {0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},
    {0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00},
    {0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00},
    {0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00},
    {0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00},
    {0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00},
    {0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00},
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F},
    {0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00},
    {0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},
    {0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E},
    {0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00},
    {0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},
    {0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00},
    {0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00},
    {0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00},
    {0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F},
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78},
    {0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00},
    {0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00},
    {0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00},
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00},
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},
    {0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00},
    {0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00},
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F},
    {0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00},
    {0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00},
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},
    {0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00},
    {0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

void renderer_draw_text(Renderer* rend, const char* text, int x, int y, Color color, int scale) {
    int start_x = x;
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\n') {
            y += 8 * scale;
            x = start_x;
            continue;
        }
        
        uint8_t ch = (uint8_t)text[i];
        if (ch >= 128) ch = 0;
        
        for (int row = 0; row < 8; row++) {
            uint8_t byte = font_data[ch][row];
            for (int col = 0; col < 8; col++) {
                if (byte & (1 << col)) {
                    for (int sy = 0; sy < scale; sy++) {
                        for (int sx = 0; sx < scale; sx++) {
                            renderer_draw_pixel(rend, x + col * scale + sx, y + row * scale + sy, color);
                        }
                    }
                }
            }
        }
        x += 8 * scale;
    }
}
