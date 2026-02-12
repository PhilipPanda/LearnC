#include "../src/renderer.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(void) {
    Renderer rend = {0};
    
    if (!renderer_init(&rend, "Example 02: Image Rendering", 800, 600)) {
        printf("Failed to initialize renderer!\n");
        return 1;
    }

    Image* img = image_load_bmp("assets/test.bmp");
    if (!img) {
        printf("Warning: Could not load test.bmp\n");
        printf("Create a 64x64 BMP image at assets/test.bmp to see image effects\n");
    }

    clock_t start = clock();

    while (rend.is_running) {
        renderer_handle_events(&rend);
        float time = (float)(clock() - start) / CLOCKS_PER_SEC;

        renderer_clear(&rend, color_make(20, 25, 35, 255));
        
        renderer_draw_text(&rend, "Image Rendering Demo", 10, 10, color_make(255, 255, 255, 255), 2);

        if (img) {
            renderer_draw_image(&rend, img, 50, 80);
            renderer_draw_text(&rend, "Normal", 50, 60, color_make(200, 200, 200, 255), 1);
            
            float scale = 1.0f + 0.5f * sin(time);
            renderer_draw_image_ex(&rend, img, 200, 80, scale, 255);
            renderer_draw_text(&rend, "Scaled", 200, 60, color_make(200, 200, 200, 255), 1);
            
            uint8_t alpha = (uint8_t)(128 + 127 * sin(time * 2));
            renderer_draw_image_ex(&rend, img, 370, 80, 1.5f, alpha);
            renderer_draw_text(&rend, "Fading", 370, 60, color_make(200, 200, 200, 255), 1);
            
            uint8_t r = (uint8_t)(128 + 127 * sin(time));
            uint8_t g = (uint8_t)(128 + 127 * cos(time * 1.5));
            uint8_t b = (uint8_t)(128 + 127 * sin(time * 2));
            renderer_apply_tint(&rend, img, 550, 80, color_make(r, g, b, 255));
            renderer_draw_text(&rend, "Tinted", 550, 60, color_make(200, 200, 200, 255), 1);
            
            for (int i = 0; i < 5; i++) {
                float s = 0.5f + i * 0.3f;
                uint8_t a = (uint8_t)(255 - i * 40);
                renderer_draw_image_ex(&rend, img, 50 + i * 70, 250, s, a);
            }
            renderer_draw_text(&rend, "Size & Alpha Variation", 50, 230, color_make(200, 200, 200, 255), 1);
        } else {
            renderer_draw_text(&rend, "No image loaded!", 300, 250, color_make(255, 100, 100, 255), 2);
            renderer_draw_text(&rend, "Create assets/test.bmp (64x64 recommended)", 200, 290, color_make(200, 200, 200, 255), 1);
        }

        renderer_present(&rend);
        renderer_sleep(16);
    }

    if (img) image_free(img);
    renderer_cleanup(&rend);
    return 0;
}
