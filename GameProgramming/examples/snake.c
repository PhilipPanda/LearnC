/*
 * snake.c
 * 
 * Classic Snake game - eat food, grow longer, don't hit yourself or walls.
 * Teaches: Grid-based movement, self-collision, growing mechanics, game loop.
 */

#include "../../Renderer/src/renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define KEY_UP VK_UP
#define KEY_DOWN VK_DOWN
#define KEY_LEFT VK_LEFT
#define KEY_RIGHT VK_RIGHT
#define KEY_ENTER VK_RETURN
#define KEY_ESCAPE VK_ESCAPE
#define is_key_pressed(k) (GetAsyncKeyState(k) & 0x8000)
#else
// Linux placeholder - would need proper X11 input handling
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_RIGHT 4
#define KEY_ENTER 5
#define KEY_ESCAPE 6
int is_key_pressed(int k) { return 0; }
#endif

#define GRID_SIZE 20
#define CELL_SIZE 30
#define GAME_WIDTH (GRID_SIZE * CELL_SIZE)
#define GAME_HEIGHT (GRID_SIZE * CELL_SIZE)
#define MAX_SNAKE_LENGTH (GRID_SIZE * GRID_SIZE)

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[MAX_SNAKE_LENGTH];
    int length;
    Direction direction;
    Direction next_direction;
} Snake;

typedef struct {
    Point position;
} Food;

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAME_OVER
} GameState;

// Global game state
Snake snake;
Food food;
int score = 0;
GameState state = STATE_MENU;
int move_timer = 0;
int move_delay = 10;
int last_key_state[256] = {0};

// Key press detection
int key_just_pressed(int key) {
    int current = is_key_pressed(key);
    int previous = last_key_state[key];
    last_key_state[key] = current;
    return current && !previous;
}

// Initialize snake
void init_snake() {
    snake.length = 3;
    snake.direction = DIR_RIGHT;
    snake.next_direction = DIR_RIGHT;
    
    snake.body[0].x = GRID_SIZE / 2;
    snake.body[0].y = GRID_SIZE / 2;
    snake.body[1].x = snake.body[0].x - 1;
    snake.body[1].y = snake.body[0].y;
    snake.body[2].x = snake.body[1].x - 1;
    snake.body[2].y = snake.body[1].y;
}

// Spawn food
void spawn_food() {
    do {
        food.position.x = rand() % GRID_SIZE;
        food.position.y = rand() % GRID_SIZE;
        
        int on_snake = 0;
        for (int i = 0; i < snake.length; i++) {
            if (snake.body[i].x == food.position.x &&
                snake.body[i].y == food.position.y) {
                on_snake = 1;
                break;
            }
        }
        
        if (!on_snake) break;
    } while (1);
}

// Check collision with snake
int collides_with_snake(int x, int y, int skip_head) {
    int start = skip_head ? 1 : 0;
    for (int i = start; i < snake.length; i++) {
        if (snake.body[i].x == x && snake.body[i].y == y) {
            return 1;
        }
    }
    return 0;
}

// Start new game
void start_game() {
    init_snake();
    spawn_food();
    score = 0;
    move_timer = 0;
    state = STATE_PLAYING;
}

// Handle input
void handle_input() {
    if (state == STATE_MENU) {
        if (key_just_pressed(KEY_ENTER)) {
            start_game();
        }
    } else if (state == STATE_PLAYING) {
        if (is_key_pressed(KEY_UP) && snake.direction != DIR_DOWN) {
            snake.next_direction = DIR_UP;
        }
        if (is_key_pressed(KEY_DOWN) && snake.direction != DIR_UP) {
            snake.next_direction = DIR_DOWN;
        }
        if (is_key_pressed(KEY_LEFT) && snake.direction != DIR_RIGHT) {
            snake.next_direction = DIR_LEFT;
        }
        if (is_key_pressed(KEY_RIGHT) && snake.direction != DIR_LEFT) {
            snake.next_direction = DIR_RIGHT;
        }
    } else if (state == STATE_GAME_OVER) {
        if (key_just_pressed(KEY_ENTER)) {
            start_game();
        }
        if (key_just_pressed(KEY_ESCAPE)) {
            state = STATE_MENU;
        }
    }
}

// Update game logic
void update() {
    if (state != STATE_PLAYING) return;
    
    move_timer++;
    if (move_timer < move_delay) return;
    move_timer = 0;
    
    snake.direction = snake.next_direction;
    
    Point new_head = snake.body[0];
    switch (snake.direction) {
        case DIR_UP:    new_head.y--; break;
        case DIR_DOWN:  new_head.y++; break;
        case DIR_LEFT:  new_head.x--; break;
        case DIR_RIGHT: new_head.x++; break;
    }
    
    if (new_head.x < 0 || new_head.x >= GRID_SIZE ||
        new_head.y < 0 || new_head.y >= GRID_SIZE) {
        state = STATE_GAME_OVER;
        return;
    }
    
    if (collides_with_snake(new_head.x, new_head.y, 0)) {
        state = STATE_GAME_OVER;
        return;
    }
    
    int ate_food = (new_head.x == food.position.x &&
                    new_head.y == food.position.y);
    
    if (ate_food) {
        score += 10;
        spawn_food();
        if (move_delay > 3) move_delay--;
    }
    
    if (!ate_food) {
        for (int i = snake.length - 1; i > 0; i--) {
            snake.body[i] = snake.body[i - 1];
        }
    } else {
        if (snake.length < MAX_SNAKE_LENGTH) {
            for (int i = snake.length; i > 0; i--) {
                snake.body[i] = snake.body[i - 1];
            }
            snake.length++;
        }
    }
    
    snake.body[0] = new_head;
}

// Render game
void render(Renderer* rend) {
    renderer_clear(rend, color_make(20, 20, 20, 255));
    
    if (state == STATE_MENU) {
        renderer_draw_text(rend, "SNAKE", GAME_WIDTH/2 - 50, GAME_HEIGHT/2 - 80,
                          color_make(0, 255, 0, 255), 2);
        renderer_draw_text(rend, "Press ENTER to start", GAME_WIDTH/2 - 100, GAME_HEIGHT/2,
                          color_make(200, 200, 200, 255), 1);
        renderer_draw_text(rend, "Arrow keys to move", GAME_WIDTH/2 - 90, GAME_HEIGHT/2 + 30,
                          color_make(200, 200, 200, 255), 1);
    } else if (state == STATE_PLAYING) {
        // Draw grid
        for (int x = 0; x <= GRID_SIZE; x++) {
            int px = x * CELL_SIZE;
            renderer_draw_line(rend, px, 0, px, GAME_HEIGHT, color_make(40, 40, 40, 255));
        }
        for (int y = 0; y <= GRID_SIZE; y++) {
            int py = y * CELL_SIZE;
            renderer_draw_line(rend, 0, py, GAME_WIDTH, py, color_make(40, 40, 40, 255));
        }
        
        // Draw snake
        for (int i = 0; i < snake.length; i++) {
            int x = snake.body[i].x * CELL_SIZE;
            int y = snake.body[i].y * CELL_SIZE;
            
            Color c = (i == 0) ? color_make(100, 255, 100, 255) : color_make(0, 200, 0, 255);
            renderer_draw_filled_rect(rend, x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, c);
        }
        
        // Draw food
        int fx = food.position.x * CELL_SIZE;
        int fy = food.position.y * CELL_SIZE;
        renderer_draw_filled_rect(rend, fx + 1, fy + 1, CELL_SIZE - 2, CELL_SIZE - 2,
                                 color_make(255, 50, 50, 255));
        
        // Draw score
        char score_text[32];
        sprintf(score_text, "Score: %d", score);
        renderer_draw_text(rend, score_text, 10, 10, color_make(255, 255, 255, 255), 1);
    } else if (state == STATE_GAME_OVER) {
        renderer_draw_text(rend, "GAME OVER", GAME_WIDTH/2 - 80, GAME_HEIGHT/2 - 80,
                          color_make(255, 50, 50, 255), 2);
        
        char score_text[32];
        sprintf(score_text, "Final Score: %d", score);
        renderer_draw_text(rend, score_text, GAME_WIDTH/2 - 80, GAME_HEIGHT/2 - 20,
                          color_make(255, 255, 255, 255), 1);
        
        renderer_draw_text(rend, "Press ENTER to play again", GAME_WIDTH/2 - 120, GAME_HEIGHT/2 + 20,
                          color_make(200, 200, 200, 255), 1);
        renderer_draw_text(rend, "Press ESC for menu", GAME_WIDTH/2 - 90, GAME_HEIGHT/2 + 50,
                          color_make(200, 200, 200, 255), 1);
    }
    
    renderer_present(rend);
}

int main(void) {
    srand(time(NULL));
    
    Renderer rend = {0};
    if (!renderer_init(&rend, "Snake Game", GAME_WIDTH, GAME_HEIGHT)) {
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
