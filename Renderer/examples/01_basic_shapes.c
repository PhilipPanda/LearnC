#include "../src/renderer.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(void) {
    Renderer rend = {0};
    
    if (!renderer_init(&rend, "Example 01: Basic Shapes", 800, 600)) {
        printf("Failed to initialize renderer!\n");
        return 1;
    }

    clock_t start = clock();

    // Main loop - runs until window is closed
    while (rend.is_running) {
        renderer_handle_events(&rend);  // Process window events (close button, etc)
        float time = (float)(clock() - start) / CLOCKS_PER_SEC;

        // Clear screen to dark background
        renderer_clear(&rend, color_make(30, 30, 40, 255));
        
        // Rectangle with outline
        renderer_draw_filled_rect(&rend, 50, 50, 100, 80, color_make(255, 100, 100, 255));
        renderer_draw_rect(&rend, 55, 55, 90, 70, color_make(255, 255, 255, 255));
        
        // Circle with outline
        renderer_draw_filled_circle(&rend, 250, 90, 40, color_make(100, 255, 100, 255));
        renderer_draw_circle(&rend, 250, 90, 45, color_make(255, 255, 255, 255));
        
        // Lines - thin and thick
        renderer_draw_line(&rend, 350, 50, 450, 130, color_make(100, 100, 255, 255));
        renderer_draw_thick_line(&rend, 350, 140, 450, 220, 3, color_make(255, 255, 100, 255));
        
        // Triangle with outline
        renderer_draw_filled_triangle(&rend, 550, 50, 650, 50, 600, 130, color_make(255, 150, 200, 255));
        renderer_draw_triangle(&rend, 545, 45, 655, 45, 600, 135, color_make(255, 255, 255, 255));
        
        // Pulsing circle - size varies with sin wave
        int pulse = (int)(20 + 10 * sin(time * 3));
        renderer_draw_filled_circle(&rend, 100, 300, 30 + pulse, color_make(255, 200, 100, 180));
        
        // Rotating circles - using sin/cos to calculate positions
        for (int i = 0; i < 8; i++) {
            float angle = time + i * M_PI / 4;  // Evenly spaced angles
            int x = 400 + (int)(cos(angle) * 100);  // X position on circle
            int y = 350 + (int)(sin(angle) * 100);  // Y position on circle
            renderer_draw_filled_circle(&rend, x, y, 15, color_make(150, 100, 255, 255));
        }
        
        // Text labels
        renderer_draw_text(&rend, "Basic 2D Shapes", 10, 10, color_make(255, 255, 255, 255), 2);
        renderer_draw_text(&rend, "This is the render loop - clear, draw, present, repeat", 10, 30, color_make(200, 200, 200, 255), 1);

        renderer_present(&rend);
        renderer_sleep(16);
    }

    renderer_cleanup(&rend);
    return 0;
}
