# Game Loop

## The Big Picture

Most programs run, do something, then exit. Games are different - they run continuously until the player quits. The game loop is the heartbeat of every game: get input, update game state, render, repeat.

This pattern is fundamental. Master it and you understand how every game works.

## Basic Game Loop

```c
int running = 1;
while (running) {
    handle_input();   // Read keyboard/mouse
    update();         // Update game logic
    render();         // Draw everything
}
```

That's it. Three steps, repeated 60 times per second.

## Detailed Structure

```c
void game_loop() {
    int running = 1;
    
    while (running) {
        // 1. Handle events
        if (key_pressed(KEY_ESCAPE)) {
            running = 0;
        }
        
        // 2. Update game state
        player.x += player.velocity_x;
        player.y += player.velocity_y;
        
        // Check collisions
        if (check_collision(player, enemy)) {
            game_over = 1;
        }
        
        // 3. Render
        clear_screen(COLOR_BLACK);
        draw_player(&player);
        draw_enemy(&enemy);
        present();
    }
}
```

## Timing and Frame Rate

Problem: Different computers run at different speeds. On a fast computer, the loop runs 1000 times per second. On a slow one, maybe 30 times. Your game speed shouldn't vary!

### Fixed Time Step

Run game logic at fixed rate, render as fast as possible:

```c
#define TARGET_FPS 60
#define MS_PER_FRAME (1000 / TARGET_FPS)

void game_loop() {
    clock_t last_time = clock();
    double accumulator = 0.0;
    
    while (running) {
        clock_t current_time = clock();
        double delta_ms = ((double)(current_time - last_time) / CLOCKS_PER_SEC) * 1000.0;
        last_time = current_time;
        
        accumulator += delta_ms;
        
        // Update at fixed rate
        while (accumulator >= MS_PER_FRAME) {
            handle_input();
            update(MS_PER_FRAME);
            accumulator -= MS_PER_FRAME;
        }
        
        // Render as fast as possible
        render();
    }
}
```

This keeps physics deterministic regardless of frame rate.

### Delta Time

Alternative: pass elapsed time to update:

```c
void update(double delta_time) {
    // Move based on time, not frames
    player.x += player.velocity * delta_time;
}
```

**Fixed time step:** Consistent, deterministic, better for physics  
**Delta time:** Simpler, variable frame rate, can cause issues

Most games use fixed time step for game logic, delta time for rendering interpolation.

## Frame Rate Control

### V-Sync (Renderer handles it)

Let the renderer sync to monitor refresh rate (60Hz usually).

### Manual Limiting

```c
#define TARGET_FPS 60
#define FRAME_TIME (1000 / TARGET_FPS)

void game_loop() {
    while (running) {
        clock_t frame_start = clock();
        
        handle_input();
        update();
        render();
        
        // Delay to hit target FPS
        clock_t frame_end = clock();
        double elapsed = ((double)(frame_end - frame_start) / CLOCKS_PER_SEC) * 1000.0;
        
        if (elapsed < FRAME_TIME) {
            // Sleep or busy wait
            double sleep_time = FRAME_TIME - elapsed;
            // Sleep implementation depends on platform
        }
    }
}
```

## Game States

Games have different states: menu, playing, paused, game over.

```c
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER
} GameState;

GameState current_state = STATE_MENU;

void game_loop() {
    while (running) {
        switch (current_state) {
            case STATE_MENU:
                handle_input_menu();
                update_menu();
                render_menu();
                break;
                
            case STATE_PLAYING:
                handle_input_game();
                update_game();
                render_game();
                break;
                
            case STATE_PAUSED:
                handle_input_pause();
                render_game();  // Still show game
                render_pause_overlay();
                break;
                
            case STATE_GAME_OVER:
                handle_input_game_over();
                render_game_over();
                break;
        }
        
        present();
    }
}
```

## Common Patterns

### Separate Update and Render

```c
typedef struct {
    void (*update)(double dt);
    void (*render)(void);
} GameObject;

GameObject objects[100];
int object_count = 0;

void game_update(double dt) {
    for (int i = 0; i < object_count; i++) {
        objects[i].update(dt);
    }
}

void game_render() {
    for (int i = 0; i < object_count; i++) {
        objects[i].render();
    }
}
```

### Update Order Matters

```c
void update() {
    handle_input();        // 1. Get input
    update_player();       // 2. Move player
    update_enemies();      // 3. Move enemies
    update_projectiles();  // 4. Move bullets
    check_collisions();    // 5. Check everything
    remove_dead_objects(); // 6. Clean up
    spawn_new_objects();   // 7. Spawn new stuff
}
```

Order affects game feel. Moving player before enemies makes it feel more responsive.

## Performance

### Only Update What's Needed

```c
void update() {
    if (current_state == STATE_PLAYING) {
        // Only update game objects when playing
        update_game_objects();
    }
    
    // Always update animations
    update_animations();
}
```

### Spatial Partitioning

Don't check every object against every other:

```c
// BAD: O(n²)
for (int i = 0; i < enemy_count; i++) {
    for (int j = 0; j < bullet_count; j++) {
        check_collision(enemies[i], bullets[j]);
    }
}

// GOOD: Use grid/quadtree to only check nearby objects
Grid* grid = create_grid(world_width, world_height, cell_size);
check_collisions_in_grid(grid);
```

## Debugging Game Loop

### FPS Counter

```c
int frame_count = 0;
clock_t last_fps_time = clock();

void update_fps() {
    frame_count++;
    clock_t current = clock();
    double elapsed = (double)(current - last_fps_time) / CLOCKS_PER_SEC;
    
    if (elapsed >= 1.0) {
        printf("FPS: %d\n", frame_count);
        frame_count = 0;
        last_fps_time = current;
    }
}
```

### Timing Breakdown

```c
clock_t input_time, update_time, render_time;

input_time = measure_time(handle_input);
update_time = measure_time(update);
render_time = measure_time(render);

printf("Input: %.2fms, Update: %.2fms, Render: %.2fms\n",
       input_time, update_time, render_time);
```

## Platform Differences

### Windows

```c
#include <windows.h>

void sleep_ms(int ms) {
    Sleep(ms);
}

double get_time() {
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / freq.QuadPart;
}
```

### Linux

```c
#include <unistd.h>
#include <time.h>

void sleep_ms(int ms) {
    usleep(ms * 1000);
}

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
```

## Summary

The game loop is simple conceptually but crucial to get right. Handle input, update state, render. Repeat forever. Control timing to ensure consistent game speed across different hardware.

Key points:
- Fixed time step for deterministic updates
- Separate input, update, render
- State machines for game states
- Frame rate limiting
- Update order matters

Master the game loop and you can build any game.
