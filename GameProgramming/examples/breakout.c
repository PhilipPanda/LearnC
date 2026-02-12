/*
 * breakout.c
 * 
 * Classic Breakout/Arkanoid game - break bricks with ball and paddle.
 * Teaches: Multi-object collision, level design, power-ups, game juice.
 */

#include "../../Renderer/src/renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#define KEY_LEFT VK_LEFT
#define KEY_RIGHT VK_RIGHT
#define KEY_SPACE VK_SPACE
#define KEY_ENTER VK_RETURN
#define KEY_ESCAPE VK_ESCAPE
#define is_key_pressed(k) (GetAsyncKeyState(k) & 0x8000)
#else
#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_SPACE 3
#define KEY_ENTER 4
#define KEY_ESCAPE 5
int is_key_pressed(int k) { return 0; }
#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define PADDLE_SPEED 8
#define BALL_SIZE 12
#define BALL_SPEED 5
#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_WIDTH 70
#define BRICK_HEIGHT 25
#define BRICK_PADDING 5

typedef struct {
    float x, y;
    int active;
    int hits;
    Color color;
} Brick;

typedef struct {
    float x, y;
    float w, h;
} Paddle;

typedef struct {
    float x, y;
    float vx, vy;
    int size;
    int stuck;
} Ball;

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_WIN
} GameState;

Paddle paddle;
Ball ball;
Brick bricks[BRICK_ROWS * BRICK_COLS];
int score = 0;
int lives = 3;
GameState state = STATE_MENU;
int last_key_state[256] = {0};

int key_just_pressed(int key) {
    int current = is_key_pressed(key);
    int previous = last_key_state[key];
    last_key_state[key] = current;
    return current && !previous;
}

void init_bricks() {
    int index = 0;
    int start_y = 80;
    
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLS; col++) {
            Brick* b = &bricks[index++];
            b->x = col * (BRICK_WIDTH + BRICK_PADDING) + 20;
            b->y = row * (BRICK_HEIGHT + BRICK_PADDING) + start_y;
            b->active = 1;
            
            if (row == 0) {
                b->hits = 2;
                b->color = color_make(255, 50, 50, 255);
            } else if (row == 1) {
                b->hits = 1;
                b->color = color_make(255, 150, 50, 255);
            } else if (row == 2) {
                b->hits = 1;
                b->color = color_make(255, 255, 50, 255);
            } else if (row == 3) {
                b->hits = 1;
                b->color = color_make(50, 255, 50, 255);
            } else {
                b->hits = 1;
                b->color = color_make(50, 150, 255, 255);
            }
        }
    }
}

void reset_ball_and_paddle() {
    paddle.x = SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2;
    paddle.y = SCREEN_HEIGHT - 50;
    paddle.w = PADDLE_WIDTH;
    paddle.h = PADDLE_HEIGHT;
    
    ball.x = paddle.x + paddle.w / 2 - BALL_SIZE / 2;
    ball.y = paddle.y - BALL_SIZE;
    ball.size = BALL_SIZE;
    ball.vx = 0;
    ball.vy = 0;
    ball.stuck = 1;
}

void start_game() {
    init_bricks();
    reset_ball_and_paddle();
    score = 0;
    lives = 3;
    state = STATE_PLAYING;
}

int check_collision(float x1, float y1, float w1, float h1,
                    float x2, float y2, float w2, float h2) {
    return x1 < x2 + w2 &&
           x1 + w1 > x2 &&
           y1 < y2 + h2 &&
           y1 + h1 > y2;
}

void handle_input() {
    if (state == STATE_MENU) {
        if (key_just_pressed(KEY_ENTER)) {
            start_game();
        }
    } else if (state == STATE_PLAYING) {
        if (is_key_pressed(KEY_LEFT)) {
            paddle.x -= PADDLE_SPEED;
        }
        if (is_key_pressed(KEY_RIGHT)) {
            paddle.x += PADDLE_SPEED;
        }
        
        if (paddle.x < 0) paddle.x = 0;
        if (paddle.x + paddle.w > SCREEN_WIDTH) paddle.x = SCREEN_WIDTH - paddle.w;
        
        if (ball.stuck && key_just_pressed(KEY_SPACE)) {
            ball.stuck = 0;
            ball.vx = ((float)rand() / RAND_MAX - 0.5f) * 3.0f;
            ball.vy = -BALL_SPEED;
        }
    } else if (state == STATE_GAME_OVER || state == STATE_WIN) {
        if (key_just_pressed(KEY_ENTER)) {
            start_game();
        }
        if (key_just_pressed(KEY_ESCAPE)) {
            state = STATE_MENU;
        }
    }
}

void update() {
    if (state != STATE_PLAYING) return;
    
    if (ball.stuck) {
        ball.x = paddle.x + paddle.w / 2 - ball.size / 2;
        ball.y = paddle.y - ball.size;
        return;
    }
    
    ball.x += ball.vx;
    ball.y += ball.vy;
    
    if (ball.x <= 0 || ball.x + ball.size >= SCREEN_WIDTH) {
        ball.vx = -ball.vx;
    }
    if (ball.y <= 0) {
        ball.vy = -ball.vy;
    }
    
    if (ball.y > SCREEN_HEIGHT) {
        lives--;
        if (lives <= 0) {
            state = STATE_GAME_OVER;
        } else {
            reset_ball_and_paddle();
        }
        return;
    }
    
    if (check_collision(ball.x, ball.y, ball.size, ball.size,
                       paddle.x, paddle.y, paddle.w, paddle.h)) {
        ball.vy = -fabs(ball.vy);
        
        float paddle_center = paddle.x + paddle.w / 2;
        float ball_center = ball.x + ball.size / 2;
        float offset = (ball_center - paddle_center) / (paddle.w / 2);
        ball.vx = offset * 5.0f;
    }
    
    for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
        Brick* b = &bricks[i];
        if (!b->active) continue;
        
        if (check_collision(ball.x, ball.y, ball.size, ball.size,
                           b->x, b->y, BRICK_WIDTH, BRICK_HEIGHT)) {
            b->hits--;
            if (b->hits <= 0) {
                b->active = 0;
                score += 10;
            } else {
                score += 5;
                b->color = color_make(150, 50, 50, 255);
            }
            
            ball.vy = -ball.vy;
            
            int all_destroyed = 1;
            for (int j = 0; j < BRICK_ROWS * BRICK_COLS; j++) {
                if (bricks[j].active) {
                    all_destroyed = 0;
                    break;
                }
            }
            if (all_destroyed) {
                state = STATE_WIN;
            }
            
            break;
        }
    }
}

void render(Renderer* rend) {
    renderer_clear(rend, color_make(20, 20, 30, 255));
    
    if (state == STATE_MENU) {
        renderer_draw_text(rend, "BREAKOUT", SCREEN_WIDTH/2 - 80, 150, color_make(255, 255, 255, 255), 2);
        renderer_draw_text(rend, "Press ENTER to start", SCREEN_WIDTH/2 - 100, 300, color_make(200, 200, 200, 255), 1);
        renderer_draw_text(rend, "LEFT/RIGHT to move paddle", SCREEN_WIDTH/2 - 120, 340, color_make(150, 150, 150, 255), 1);
        renderer_draw_text(rend, "SPACE to launch ball", SCREEN_WIDTH/2 - 90, 370, color_make(150, 150, 150, 255), 1);
    } else {
        for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
            Brick* b = &bricks[i];
            if (b->active) {
                renderer_draw_filled_rect(rend, (int)b->x, (int)b->y, BRICK_WIDTH, BRICK_HEIGHT, b->color);
                renderer_draw_rect(rend, (int)b->x, (int)b->y, BRICK_WIDTH, BRICK_HEIGHT,
                                          color_make(0, 0, 0, 255));
            }
        }
        
        renderer_draw_filled_rect(rend, (int)paddle.x, (int)paddle.y, (int)paddle.w, (int)paddle.h,
                          color_make(100, 150, 255, 255));
        
        renderer_draw_filled_rect(rend, (int)ball.x, (int)ball.y, ball.size, ball.size,
                          color_make(255, 255, 255, 255));
        
        char text[64];
        sprintf(text, "Score: %d", score);
        renderer_draw_text(rend, text, 10, 10, color_make(255, 255, 255, 255), 1);
        
        sprintf(text, "Lives: %d", lives);
        renderer_draw_text(rend, text, SCREEN_WIDTH - 100, 10, color_make(255, 100, 100, 255), 1);
        
        if (ball.stuck) {
            renderer_draw_text(rend, "Press SPACE to launch", SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT - 100,
                              color_make(200, 200, 200, 255), 1);
        }
    }
    
    if (state == STATE_GAME_OVER) {
        renderer_draw_text(rend, "GAME OVER", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 60, color_make(255, 50, 50, 255), 2);
        
        char text[64];
        sprintf(text, "Final Score: %d", score);
        renderer_draw_text(rend, text, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2, color_make(255, 255, 255, 255), 1);
        
        renderer_draw_text(rend, "Press ENTER to play again", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 40,
                          color_make(200, 200, 200, 255), 1);
    } else if (state == STATE_WIN) {
        renderer_draw_text(rend, "YOU WIN!", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 - 60, color_make(100, 255, 100, 255), 2);
        
        char text[64];
        sprintf(text, "Final Score: %d", score);
        renderer_draw_text(rend, text, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2, color_make(255, 255, 255, 255), 1);
        
        renderer_draw_text(rend, "Press ENTER to play again", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 40,
                          color_make(200, 200, 200, 255), 1);
    }
    
    renderer_present(rend);
}

int main(void) {
    srand(time(NULL));
    
    Renderer rend = {0};
    if (!renderer_init(&rend, "Breakout", SCREEN_WIDTH, SCREEN_HEIGHT)) {
        printf("Failed to initialize renderer\n");
        return 1;
    }
    
    while (rend.is_running) {
        renderer_handle_events(&rend);
        
        handle_input();
        update();
        render(&rend);
        
        renderer_sleep(16);
    }
    
    renderer_cleanup(&rend);
    return 0;
}
