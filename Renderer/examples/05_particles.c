#include "../src/renderer.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_PARTICLES 500

typedef struct {
    float x, y;
    float vx, vy;
    float life;
    Color color;
} Particle;

Particle particles[MAX_PARTICLES];

void reset_particle(Particle* p, int cx, int cy) {
    float angle = (rand() % 360) * M_PI / 180.0f;
    float speed = 1.0f + (rand() % 100) / 50.0f;
    p->x = cx;
    p->y = cy;
    p->vx = cos(angle) * speed;
    p->vy = sin(angle) * speed;
    p->life = 1.0f;
    p->color = color_make(
        150 + rand() % 105,
        100 + rand() % 155,
        50 + rand() % 100,
        255
    );
}

int main(void) {
    Renderer rend = {0};
    
    if (!renderer_init(&rend, "Example 05: Particle System", 800, 600)) {
        printf("Failed to initialize renderer!\n");
        return 1;
    }

    srand(time(NULL));
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        reset_particle(&particles[i], 400, 300);
    }

    clock_t start = clock();

    while (rend.is_running) {
        renderer_handle_events(&rend);
        float time = (float)(clock() - start) / CLOCKS_PER_SEC;

        renderer_clear(&rend, color_make(10, 10, 15, 255));
        
        renderer_draw_text(&rend, "Particle System", 10, 10, color_make(255, 255, 255, 255), 2);
        
        int emit_x = 400 + (int)(sin(time) * 200);
        int emit_y = 300 + (int)(cos(time * 1.5f) * 150);

        for (int i = 0; i < MAX_PARTICLES; i++) {
            Particle* p = &particles[i];
            
            p->vy += 0.05f;
            p->x += p->vx;
            p->y += p->vy;
            p->life -= 0.005f;
            
            if (p->life <= 0 || p->y > 600) {
                reset_particle(p, emit_x, emit_y);
            }
            
            int size = (int)(3 + p->life * 3);
            uint8_t alpha = (uint8_t)(p->life * 255);
            Color c = p->color;
            c.a = alpha;
            
            renderer_draw_filled_circle(&rend, (int)p->x, (int)p->y, size, c);
        }
        
        renderer_draw_filled_circle(&rend, emit_x, emit_y, 8, color_make(255, 255, 100, 255));
        renderer_draw_circle(&rend, emit_x, emit_y, 10, color_make(255, 255, 255, 200));

        renderer_present(&rend);
        renderer_sleep(16);
    }

    renderer_cleanup(&rend);
    return 0;
}
