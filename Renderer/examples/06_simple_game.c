#include "../src/renderer.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_ENEMIES 10
#define MAX_BULLETS 20

typedef struct { float x, y, vx, vy; bool active; } Bullet;
typedef struct { float x, y; float angle; bool active; } Enemy;

Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
int player_x = 400, player_y = 500;
int score = 0;

void spawn_enemy() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].x = rand() % 700 + 50;
            enemies[i].y = -20;
            enemies[i].active = true;
            break;
        }
    }
}

void shoot(int x, int y) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].x = x;
            bullets[i].y = y;
            bullets[i].vx = 0;
            bullets[i].vy = -8;
            bullets[i].active = true;
            break;
        }
    }
}

int main(void) {
    Renderer rend = {0};
    
    if (!renderer_init(&rend, "Example 07: Simple Shooter Game", 800, 600)) {
        printf("Failed to initialize renderer!\n");
        return 1;
    }

    srand(time(NULL));
    
    clock_t start = clock();
    clock_t last_spawn = start;
    clock_t last_shot = start;

    while (rend.is_running) {
        renderer_handle_events(&rend);
        float time = (float)(clock() - start) / CLOCKS_PER_SEC;

        if (clock() - last_spawn > CLOCKS_PER_SEC / 2) {
            spawn_enemy();
            last_spawn = clock();
        }

        if (clock() - last_shot > CLOCKS_PER_SEC / 5) {
            shoot(player_x, player_y - 20);
            last_shot = clock();
        }

        renderer_clear(&rend, color_make(10, 10, 20, 255));
        
        for (int i = 0; i < 20; i++) {
            int sx = (rand() % 800);
            int sy = ((int)(time * 50 + i * 30)) % 600;
            renderer_draw_pixel(&rend, sx, sy, color_make(200, 200, 255, 255));
        }
        
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                bullets[i].x += bullets[i].vx;
                bullets[i].y += bullets[i].vy;
                if (bullets[i].y < 0) bullets[i].active = false;
                
                renderer_draw_filled_circle(&rend, (int)bullets[i].x, (int)bullets[i].y, 3, color_make(255, 255, 100, 255));
            }
        }
        
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].active) {
                enemies[i].y += 2;
                enemies[i].angle += 0.05f;
                
                if (enemies[i].y > 620) {
                    enemies[i].active = false;
                    continue;
                }
                
                for (int j = 0; j < MAX_BULLETS; j++) {
                    if (bullets[j].active) {
                        float dx = bullets[j].x - enemies[i].x;
                        float dy = bullets[j].y - enemies[i].y;
                        if (dx*dx + dy*dy < 400) {
                            enemies[i].active = false;
                            bullets[j].active = false;
                            score += 10;
                        }
                    }
                }
                
                int ex = (int)enemies[i].x;
                int ey = (int)enemies[i].y;
                renderer_draw_filled_triangle(&rend, ex, ey-15, ex-12, ey+10, ex+12, ey+10, color_make(255, 100, 100, 255));
                renderer_draw_triangle(&rend, ex, ey-15, ex-12, ey+10, ex+12, ey+10, color_make(255, 200, 200, 255));
            }
        }
        
        renderer_draw_filled_triangle(&rend, player_x, player_y-20, player_x-15, player_y+15, player_x+15, player_y+15, color_make(100, 200, 255, 255));
        renderer_draw_triangle(&rend, player_x, player_y-20, player_x-15, player_y+15, player_x+15, player_y+15, color_make(200, 230, 255, 255));
        
        player_x = 400 + (int)(sin(time * 2) * 150);
        
        char score_text[64];
        snprintf(score_text, sizeof(score_text), "SCORE: %d", score);
        renderer_draw_text(&rend, score_text, 10, 10, color_make(255, 255, 100, 255), 2);
        renderer_draw_text(&rend, "Auto-aim shooter!", 10, 30, color_make(200, 200, 200, 255), 1);

        renderer_present(&rend);
        renderer_sleep(16);
    }

    renderer_cleanup(&rend);
    return 0;
}
