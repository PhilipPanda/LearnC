#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(int argc, char* argv[]) {
    Renderer rend = {0};
    
    if (!renderer_init(&rend, "C Renderer - Pure C", SCREEN_WIDTH, SCREEN_HEIGHT)) {
        printf("Failed to initialize renderer!\n");
        return 1;
    }

    printf("C Renderer initialized successfully!\n");
    printf("Controls: ESC to exit\n");

    Image* test_image = image_load_bmp("assets/test.bmp");
    if (!test_image) {
        printf("Note: test.bmp not found - rendering primitives only\n");
    }

    clock_t start_time = clock();
    int frame_count = 0;

    while (rend.is_running) {
        renderer_handle_events(&rend);

        float time = (float)(clock() - start_time) / CLOCKS_PER_SEC;

        renderer_clear(&rend, color_make(20, 30, 50, 255));
        
        Color grad1 = color_make(40, 40, 80, 255);
        Color grad2 = color_make(20, 20, 40, 255);
        renderer_draw_gradient_rect(&rend, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, grad1, grad2, true);
        for (int i = 0; i < 5; i++) {
            float angle = time * 0.5 + i * (2 * M_PI / 5);
            int x = (int)(SCREEN_WIDTH / 2 + cos(angle) * 200);
            int y = (int)(SCREEN_HEIGHT / 2 + sin(angle) * 150);
            
            uint8_t r = (uint8_t)(128 + 127 * sin(time + i));
            uint8_t g = (uint8_t)(128 + 127 * cos(time * 0.7 + i));
            uint8_t b = (uint8_t)(128 + 127 * sin(time * 1.3 + i));
            
            renderer_draw_filled_rect(&rend, x - 20, y - 20, 40, 40, color_make(r, g, b, 200));
        }

        for (int i = 0; i < 3; i++) {
            float angle = time + i * (2 * M_PI / 3);
            int cx = (int)(SCREEN_WIDTH / 2 + cos(angle) * 180);
            int cy = (int)(SCREEN_HEIGHT / 2 + sin(angle) * 180);
            
            uint8_t r = (i == 0) ? 255 : 50;
            uint8_t g = (i == 1) ? 255 : 50;
            uint8_t b = (i == 2) ? 255 : 50;
            
            for (int j = 0; j < 10; j++) {
                float trail_angle = angle - j * 0.1f;
                int tx = (int)(SCREEN_WIDTH / 2 + cos(trail_angle) * 180);
                int ty = (int)(SCREEN_HEIGHT / 2 + sin(trail_angle) * 180);
                uint8_t alpha = (uint8_t)(150 - j * 15);
                renderer_draw_filled_circle(&rend, tx, ty, 15, color_make(r, g, b, alpha));
            }
            
            renderer_draw_filled_circle(&rend, cx, cy, 25, color_make(r, g, b, 255));
            renderer_draw_circle(&rend, cx, cy, 27, color_make(255, 255, 255, 255));
        }

        for (int i = 0; i < 12; i++) {
            float angle = time * 2 + i * (M_PI / 6);
            int x1 = SCREEN_WIDTH / 2;
            int y1 = SCREEN_HEIGHT / 2;
            int x2 = (int)(x1 + cos(angle) * 100);
            int y2 = (int)(y1 + sin(angle) * 100);
            
            uint8_t intensity = (uint8_t)(128 + 127 * sin(time + i));
            renderer_draw_line(&rend, x1, y1, x2, y2, color_make(intensity, intensity, 255, 180));
        }

        for (int i = 0; i < SCREEN_WIDTH; i += 50) {
            renderer_draw_line(&rend, i, 0, i, SCREEN_HEIGHT, color_make(60, 60, 80, 100));
        }
        for (int i = 0; i < SCREEN_HEIGHT; i += 50) {
            renderer_draw_line(&rend, 0, i, SCREEN_WIDTH, i, color_make(60, 60, 80, 100));
        }

        renderer_draw_rect(&rend, 5, 5, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10, color_make(150, 200, 255, 255));
        
        int pulse_size = (int)(20 + 10 * sin(time * 3));
        renderer_draw_filled_rect(&rend, 20, 20, pulse_size, pulse_size, color_make(255, 100, 100, 255));
        renderer_draw_filled_rect(&rend, SCREEN_WIDTH - 20 - pulse_size, 20, pulse_size, pulse_size, color_make(100, 255, 100, 255));
        renderer_draw_filled_rect(&rend, 20, SCREEN_HEIGHT - 20 - pulse_size, pulse_size, pulse_size, color_make(100, 100, 255, 255));
        renderer_draw_filled_rect(&rend, SCREEN_WIDTH - 20 - pulse_size, SCREEN_HEIGHT - 20 - pulse_size, pulse_size, pulse_size, color_make(255, 255, 100, 255));
        
        if (test_image) {
            renderer_draw_image(&rend, test_image, 50, 50);
            
            float scale = 1.0f + 0.5f * sin(time);
            uint8_t alpha = (uint8_t)(128 + 127 * sin(time * 2));
            renderer_draw_image_ex(&rend, test_image, SCREEN_WIDTH - 200, 50, scale, alpha);
            
            uint8_t tint_r = (uint8_t)(128 + 127 * sin(time));
            uint8_t tint_g = (uint8_t)(128 + 127 * cos(time * 1.5));
            uint8_t tint_b = (uint8_t)(128 + 127 * sin(time * 2));
            renderer_apply_tint(&rend, test_image, 50, SCREEN_HEIGHT - 200, color_make(tint_r, tint_g, tint_b, 255));
        }

        renderer_present(&rend);
        frame_count++;
        renderer_sleep(16);
    }

    if (test_image) image_free(test_image);
    renderer_cleanup(&rend);
    
    float total_time = (float)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("Rendered %d frames in %.2f seconds (%.1f FPS)\n", frame_count, total_time, frame_count / total_time);

    return 0;
}
