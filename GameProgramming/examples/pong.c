/*
 * pong.c
 * 
 * Classic Pong game - 2-player or vs AI.
 * Teaches: Continuous movement, ball physics, AI opponent, score tracking.
 */

#include "../../Renderer/src/renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#define KEY_W 'W'
#define KEY_S 'S'
#define KEY_UP VK_UP
#define KEY_DOWN VK_DOWN
#define KEY_ENTER VK_RETURN
#define KEY_ESCAPE VK_ESCAPE
#define KEY_P 'P'
#define KEY_1 '1'
#define KEY_2 '2'
#define is_key_pressed(k) (GetAsyncKeyState(k) & 0x8000)
#else
#define KEY_W 1
#define KEY_S 2
#define KEY_UP 3
#define KEY_DOWN 4
#define KEY_ENTER 5
#define KEY_ESCAPE 6
#define KEY_P 7
#define KEY_1 8
#define KEY_2 9
int is_key_pressed(int k) { return 0; }
#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_WIDTH 15
#define PADDLE_HEIGHT 100
#define PADDLE_SPEED 6
#define BALL_SIZE 15
#define BALL_SPEED 5

typedef struct {
    float x, y;
    float w, h;
    float vy;
    int score;
} Paddle;

typedef struct {
    float x, y;
    float vx, vy;
    float size;
} Ball;

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER
} GameState;

Paddle player1, player2;
Ball ball;
GameState state = STATE_MENU;
int ai_enabled = 1;
int winning_score = 5;
int last_key_state[256] = {0};

int key_just_pressed(int key) {
    int current = is_key_pressed(key);
    int previous = last_key_state[key];
    last_key_state[key] = current;
    return current && !previous;
}

void init_paddle(Paddle* p, float x, float y) {
    p->x = x;
    p->y = y;
    p->w = PADDLE_WIDTH;
    p->h = PADDLE_HEIGHT;
    p->vy = 0;
    p->score = 0;
}

void reset_ball() {
    ball.x = SCREEN_WIDTH / 2;
    ball.y = SCREEN_HEIGHT / 2;
    ball.size = BALL_SIZE;
    
    float angle = ((float)rand() / RAND_MAX) * 3.14159f * 2.0f;
    ball.vx = cosf(angle) * BALL_SPEED;
    ball.vy = sinf(angle) * BALL_SPEED;
    
    if (fabs(ball.vx) < 2.0f) {
        ball.vx = ball.vx > 0 ? 2.0f : -2.0f;
    }
}

void start_game() {
    init_paddle(&player1, 30, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2);
    init_paddle(&player2, SCREEN_WIDTH - 30 - PADDLE_WIDTH, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2);
    reset_ball();
    state = STATE_PLAYING;
}

void update_ai(Paddle* paddle, Ball* ball) {
    float paddle_center = paddle->y + paddle->h / 2;
    float ball_center = ball->y + ball->size / 2;
    
    if (ball_center < paddle_center - 10) {
        paddle->vy = -PADDLE_SPEED * 0.7f;
    } else if (ball_center > paddle_center + 10) {
        paddle->vy = PADDLE_SPEED * 0.7f;
    } else {
        paddle->vy = 0;
    }
}

void handle_input() {
    if (state == STATE_MENU) {
        if (key_just_pressed(KEY_ENTER)) {
            start_game();
        }
        if (key_just_pressed(KEY_1)) {
            ai_enabled = 0;
            start_game();
        }
        if (key_just_pressed(KEY_2)) {
            ai_enabled = 1;
            start_game();
        }
    } else if (state == STATE_PLAYING) {
        if (is_key_pressed(KEY_W)) {
            player1.vy = -PADDLE_SPEED;
        } else if (is_key_pressed(KEY_S)) {
            player1.vy = PADDLE_SPEED;
        } else {
            player1.vy = 0;
        }
        
        if (!ai_enabled) {
            if (is_key_pressed(KEY_UP)) {
                player2.vy = -PADDLE_SPEED;
            } else if (is_key_pressed(KEY_DOWN)) {
                player2.vy = PADDLE_SPEED;
            } else {
                player2.vy = 0;
            }
        }
        
        if (key_just_pressed(KEY_P)) {
            state = STATE_PAUSED;
        }
    } else if (state == STATE_PAUSED) {
        if (key_just_pressed(KEY_P)) {
            state = STATE_PLAYING;
        }
    } else if (state == STATE_GAME_OVER) {
        if (key_just_pressed(KEY_ENTER)) {
            player1.score = 0;
            player2.score = 0;
            start_game();
        }
        if (key_just_pressed(KEY_ESCAPE)) {
            state = STATE_MENU;
        }
    }
}

int check_paddle_ball_collision(Paddle* paddle, Ball* ball) {
    return ball->x < paddle->x + paddle->w &&
           ball->x + ball->size > paddle->x &&
           ball->y < paddle->y + paddle->h &&
           ball->y + ball->size > paddle->y;
}

void update() {
    if (state != STATE_PLAYING) return;
    
    if (ai_enabled) {
        update_ai(&player2, &ball);
    }
    
    player1.y += player1.vy;
    player2.y += player2.vy;
    
    if (player1.y < 0) player1.y = 0;
    if (player1.y + player1.h > SCREEN_HEIGHT) player1.y = SCREEN_HEIGHT - player1.h;
    if (player2.y < 0) player2.y = 0;
    if (player2.y + player2.h > SCREEN_HEIGHT) player2.y = SCREEN_HEIGHT - player2.h;
    
    ball.x += ball.vx;
    ball.y += ball.vy;
    
    if (ball.y <= 0 || ball.y + ball.size >= SCREEN_HEIGHT) {
        ball.vy = -ball.vy;
    }
    
    if (check_paddle_ball_collision(&player1, &ball)) {
        ball.vx = fabs(ball.vx);
        float hit_pos = (ball.y + ball.size / 2) - (player1.y + player1.h / 2);
        ball.vy += hit_pos * 0.1f;
        ball.vx *= 1.05f;
        ball.vy *= 1.05f;
    }
    
    if (check_paddle_ball_collision(&player2, &ball)) {
        ball.vx = -fabs(ball.vx);
        float hit_pos = (ball.y + ball.size / 2) - (player2.y + player2.h / 2);
        ball.vy += hit_pos * 0.1f;
        ball.vx *= 1.05f;
        ball.vy *= 1.05f;
    }
    
    if (ball.x < 0) {
        player2.score++;
        reset_ball();
        if (player2.score >= winning_score) {
            state = STATE_GAME_OVER;
        }
    } else if (ball.x > SCREEN_WIDTH) {
        player1.score++;
        reset_ball();
        if (player1.score >= winning_score) {
            state = STATE_GAME_OVER;
        }
    }
}

void render(Renderer* rend) {
    renderer_clear(rend, color_make(20, 20, 30, 255));
    
    if (state == STATE_MENU) {
        renderer_draw_text(rend, "PONG", SCREEN_WIDTH/2 - 40, 100, color_make(255, 255, 255, 255), 2);
        renderer_draw_text(rend, "Press 1 for 2 Player", SCREEN_WIDTH/2 - 100, 250, color_make(200, 200, 200, 255), 1);
        renderer_draw_text(rend, "Press 2 for vs AI", SCREEN_WIDTH/2 - 80, 290, color_make(200, 200, 200, 255), 1);
        renderer_draw_text(rend, "Controls:", SCREEN_WIDTH/2 - 50, 350, color_make(150, 150, 150, 255), 1);
        renderer_draw_text(rend, "Player 1: W/S", SCREEN_WIDTH/2 - 60, 380, color_make(100, 100, 100, 255), 1);
        renderer_draw_text(rend, "Player 2: UP/DOWN", SCREEN_WIDTH/2 - 80, 410, color_make(100, 100, 100, 255), 1);
    } else {
        for (int y = 0; y < SCREEN_HEIGHT; y += 20) {
            renderer_draw_filled_rect(rend, SCREEN_WIDTH/2 - 2, y, 4, 10, color_make(100, 100, 100, 255));
        }
        
        renderer_draw_filled_rect(rend, (int)player1.x, (int)player1.y, (int)player1.w, (int)player1.h,
                          color_make(100, 150, 255, 255));
        renderer_draw_filled_rect(rend, (int)player2.x, (int)player2.y, (int)player2.w, (int)player2.h,
                          color_make(255, 100, 100, 255));
        
        renderer_draw_filled_rect(rend, (int)ball.x, (int)ball.y, (int)ball.size, (int)ball.size,
                          color_make(255, 255, 255, 255));
        
        char score_text[32];
        sprintf(score_text, "%d", player1.score);
        renderer_draw_text(rend, score_text, SCREEN_WIDTH/2 - 100, 30, color_make(100, 150, 255, 255), 2);
        sprintf(score_text, "%d", player2.score);
        renderer_draw_text(rend, score_text, SCREEN_WIDTH/2 + 80, 30, color_make(255, 100, 100, 255), 2);
        
        if (state == STATE_PAUSED) {
            renderer_draw_text(rend, "PAUSED", SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2, color_make(255, 255, 255, 255), 2);
            renderer_draw_text(rend, "Press P to resume", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 40, color_make(200, 200, 200, 255), 1);
        }
    }
    
    if (state == STATE_GAME_OVER) {
        const char* winner = (player1.score >= winning_score) ? "Player 1 Wins!" : "Player 2 Wins!";
        Color winner_color = (player1.score >= winning_score) ? 
            color_make(100, 150, 255, 255) : color_make(255, 100, 100, 255);
        
        renderer_draw_text(rend, winner, SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 40, winner_color, 2);
        renderer_draw_text(rend, "Press ENTER to play again", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 20, color_make(200, 200, 200, 255), 1);
        renderer_draw_text(rend, "Press ESC for menu", SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2 + 50, color_make(200, 200, 200, 255), 1);
    }
    
    renderer_present(rend);
}

int main(void) {
    srand(time(NULL));
    
    Renderer rend = {0};
    if (!renderer_init(&rend, "Pong", SCREEN_WIDTH, SCREEN_HEIGHT)) {
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
