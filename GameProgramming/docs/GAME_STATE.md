# Game State Management

## The Big Picture

Games aren't just the gameplay. There's a menu, pause screen, game over, high scores. Each mode needs different logic, input, and rendering. Managing this cleanly keeps code organized and bugs away.

State machines are the answer. Simple concept: game is in one state at a time, transitions between states follow rules.

## Basic State Machine

```c
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER
} GameState;

GameState current_state = STATE_MENU;

void update() {
    switch (current_state) {
        case STATE_MENU:
            update_menu();
            break;
        case STATE_PLAYING:
            update_game();
            break;
        case STATE_PAUSED:
            // Don't update game logic when paused
            break;
        case STATE_GAME_OVER:
            update_game_over();
            break;
    }
}

void render() {
    switch (current_state) {
        case STATE_MENU:
            render_menu();
            break;
        case STATE_PLAYING:
            render_game();
            break;
        case STATE_PAUSED:
            render_game();  // Show game in background
            render_pause_overlay();
            break;
        case STATE_GAME_OVER:
            render_game_over();
            break;
    }
}
```

## State Transitions

Change state with function:

```c
void change_state(GameState new_state) {
    // Exit current state
    switch (current_state) {
        case STATE_MENU:
            cleanup_menu();
            break;
        case STATE_PLAYING:
            cleanup_game();
            break;
        // ...
    }
    
    current_state = new_state;
    
    // Enter new state
    switch (new_state) {
        case STATE_MENU:
            init_menu();
            break;
        case STATE_PLAYING:
            init_game();
            break;
        // ...
    }
}
```

Now use it:

```c
void handle_input_menu() {
    if (key_just_pressed(KEY_ENTER)) {
        change_state(STATE_PLAYING);
    }
}

void check_game_over() {
    if (player.health <= 0) {
        change_state(STATE_GAME_OVER);
    }
}
```

## State Data

Each state might need its own data:

```c
typedef struct {
    int cursor_position;
    int num_options;
} MenuState;

typedef struct {
    Player player;
    Enemy enemies[10];
    int enemy_count;
    int score;
} PlayingState;

typedef struct {
    int final_score;
    int high_score;
    char player_name[20];
} GameOverState;

MenuState menu_state = {0};
PlayingState playing_state = {0};
GameOverState gameover_state = {0};
```

Access based on current state:

```c
void update() {
    switch (current_state) {
        case STATE_MENU:
            if (key_just_pressed(KEY_UP)) {
                menu_state.cursor_position--;
                if (menu_state.cursor_position < 0) {
                    menu_state.cursor_position = menu_state.num_options - 1;
                }
            }
            break;
            
        case STATE_PLAYING:
            update_player(&playing_state.player);
            for (int i = 0; i < playing_state.enemy_count; i++) {
                update_enemy(&playing_state.enemies[i]);
            }
            break;
    }
}
```

## Menu System

### Simple Menu

```c
typedef struct {
    const char* text;
    void (*action)(void);
} MenuItem;

MenuItem menu_items[] = {
    {"Start Game", start_game},
    {"High Scores", show_high_scores},
    {"Quit", quit_game}
};

int menu_cursor = 0;
int menu_item_count = 3;

void update_menu() {
    if (key_just_pressed(KEY_UP)) {
        menu_cursor--;
        if (menu_cursor < 0) menu_cursor = menu_item_count - 1;
    }
    if (key_just_pressed(KEY_DOWN)) {
        menu_cursor++;
        if (menu_cursor >= menu_item_count) menu_cursor = 0;
    }
    if (key_just_pressed(KEY_ENTER)) {
        menu_items[menu_cursor].action();  // Call function
    }
}

void render_menu() {
    for (int i = 0; i < menu_item_count; i++) {
        int color = (i == menu_cursor) ? COLOR_YELLOW : COLOR_WHITE;
        draw_text(menu_items[i].text, 100, 100 + i * 30, color);
    }
}
```

### Nested Menus

```c
typedef enum {
    MENU_MAIN,
    MENU_SETTINGS,
    MENU_AUDIO,
    MENU_VIDEO
} MenuScreen;

MenuScreen current_menu = MENU_MAIN;

void handle_back_button() {
    switch (current_menu) {
        case MENU_AUDIO:
        case MENU_VIDEO:
            current_menu = MENU_SETTINGS;
            break;
        case MENU_SETTINGS:
            current_menu = MENU_MAIN;
            break;
    }
}
```

## Pause System

```c
int is_paused = 0;

void toggle_pause() {
    is_paused = !is_paused;
}

void update() {
    if (current_state == STATE_PLAYING) {
        if (key_just_pressed(KEY_ESCAPE)) {
            toggle_pause();
        }
        
        if (!is_paused) {
            update_game();
        }
    }
}

void render() {
    if (current_state == STATE_PLAYING) {
        render_game();
        
        if (is_paused) {
            // Darken background
            draw_rect_filled(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                           color_rgba(0, 0, 0, 128));
            draw_text("PAUSED", SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT/2, COLOR_WHITE);
            draw_text("Press ESC to resume", SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 30, COLOR_WHITE);
        }
    }
}
```

## Game Over Screen

```c
void init_game_over() {
    gameover_state.final_score = playing_state.score;
    
    // Check high score
    if (gameover_state.final_score > save_data.high_score) {
        save_data.high_score = gameover_state.final_score;
        save_game();
    }
}

void update_game_over() {
    if (key_just_pressed(KEY_ENTER)) {
        change_state(STATE_MENU);
    }
    if (key_just_pressed(KEY_R)) {
        change_state(STATE_PLAYING);
    }
}

void render_game_over() {
    draw_text("GAME OVER", SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2 - 60, COLOR_RED);
    
    char score_text[50];
    sprintf(score_text, "Score: %d", gameover_state.final_score);
    draw_text(score_text, SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 20, COLOR_WHITE);
    
    sprintf(score_text, "High Score: %d", save_data.high_score);
    draw_text(score_text, SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 10, COLOR_YELLOW);
    
    draw_text("Press ENTER for menu", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 50, COLOR_WHITE);
    draw_text("Press R to retry", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 80, COLOR_WHITE);
}
```

## State Stack

For complex games, use stack of states:

```c
#define MAX_STATE_STACK 10

GameState state_stack[MAX_STATE_STACK];
int stack_size = 0;

void push_state(GameState state) {
    if (stack_size < MAX_STATE_STACK) {
        state_stack[stack_size++] = state;
        init_state(state);
    }
}

void pop_state() {
    if (stack_size > 0) {
        cleanup_state(state_stack[--stack_size]);
    }
}

GameState current_state() {
    return state_stack[stack_size - 1];
}
```

Usage:
```c
// Start at menu
push_state(STATE_MENU);

// Start game (push, don't replace)
push_state(STATE_PLAYING);

// Open pause menu
push_state(STATE_PAUSED);

// Close pause menu (pop back to playing)
pop_state();

// Back to main menu (pop back to menu)
pop_state();
```

This allows overlapping states (inventory over gameplay, pause over game, etc).

## Save/Load System

```c
typedef struct {
    int high_score;
    int unlocked_levels;
    float volume;
    int fullscreen;
} SaveData;

SaveData save_data = {0};

void save_game() {
    FILE* file = fopen("savegame.dat", "wb");
    if (file) {
        fwrite(&save_data, sizeof(SaveData), 1, file);
        fclose(file);
    }
}

void load_game() {
    FILE* file = fopen("savegame.dat", "rb");
    if (file) {
        fread(&save_data, sizeof(SaveData), 1, file);
        fclose(file);
    } else {
        // First time, set defaults
        save_data.high_score = 0;
        save_data.volume = 1.0f;
        save_data.fullscreen = 0;
    }
}
```

Call `load_game()` on startup, `save_game()` when needed.

## Transition Effects

Fade between states:

```c
typedef enum {
    TRANSITION_NONE,
    TRANSITION_FADE_OUT,
    TRANSITION_FADE_IN
} Transition;

Transition current_transition = TRANSITION_NONE;
float transition_alpha = 0.0f;
GameState pending_state = STATE_MENU;

void start_transition(GameState new_state) {
    current_transition = TRANSITION_FADE_OUT;
    transition_alpha = 0.0f;
    pending_state = new_state;
}

void update_transition() {
    if (current_transition == TRANSITION_FADE_OUT) {
        transition_alpha += 0.05f;
        if (transition_alpha >= 1.0f) {
            change_state(pending_state);
            current_transition = TRANSITION_FADE_IN;
        }
    } else if (current_transition == TRANSITION_FADE_IN) {
        transition_alpha -= 0.05f;
        if (transition_alpha <= 0.0f) {
            current_transition = TRANSITION_NONE;
        }
    }
}

void render_transition() {
    if (current_transition != TRANSITION_NONE) {
        int alpha = (int)(transition_alpha * 255);
        draw_rect_filled(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                       color_rgba(0, 0, 0, alpha));
    }
}
```

## Debug State Display

```c
void debug_draw_state() {
    const char* state_names[] = {
        "MENU", "PLAYING", "PAUSED", "GAME_OVER"
    };
    
    char debug_text[50];
    sprintf(debug_text, "State: %s", state_names[current_state]);
    draw_text(debug_text, 10, 10, COLOR_WHITE);
}
```

## Common Patterns

### Timed State

Auto-transition after delay:

```c
float state_timer = 0.0f;

void update_splash_screen() {
    state_timer += delta_time;
    
    if (state_timer > 3.0f || key_just_pressed(KEY_ENTER)) {
        change_state(STATE_MENU);
    }
}
```

### State History

Track previous state:

```c
GameState previous_state = STATE_MENU;

void change_state(GameState new_state) {
    previous_state = current_state;
    current_state = new_state;
}

void return_to_previous() {
    change_state(previous_state);
}
```

## Summary

State machines organize game flow. One state active at a time, clean transitions between states. Keep state data separate, handle input/update/render per state.

Key principles:
- Enum for state types
- Switch on current state
- Separate init/update/render per state
- Transition function for cleanup/setup
- State-specific data structures
- Stack for nested states

Good state management makes games maintainable. Bad state management creates spaghetti code full of global flags and boolean hell.