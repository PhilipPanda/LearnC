# Input Handling

## The Big Picture

Players interact with games through input: keyboard, mouse, gamepad. Reading input is easy. Making it feel good is hard. This is the difference between a responsive, fun game and a frustrating one.

## Keyboard Input

Our renderer provides basic keyboard state:

```c
int key_pressed(int key);  // Is key currently down?
```

### Continuous Movement

Hold key = continuous action:

```c
void update_player() {
    if (key_pressed(KEY_LEFT)) {
        player.x -= player.speed;
    }
    if (key_pressed(KEY_RIGHT)) {
        player.x += player.speed;
    }
    if (key_pressed(KEY_UP)) {
        player.y -= player.speed;
    }
    if (key_pressed(KEY_DOWN)) {
        player.y += player.speed;
    }
}
```

Call this every frame. Simple and smooth.

### Single Action (Press Once)

Problem: Holding space bar fires 60 bullets per second!

Solution: Track key state changes:

```c
typedef struct {
    int current;
    int previous;
} KeyState;

KeyState keys[256] = {0};

void update_input() {
    for (int i = 0; i < 256; i++) {
        keys[i].previous = keys[i].current;
        keys[i].current = key_pressed(i);
    }
}

int key_just_pressed(int key) {
    return keys[key].current && !keys[key].previous;
}

int key_just_released(int key) {
    return !keys[key].current && keys[key].previous;
}
```

Now detect press edges:

```c
void handle_input() {
    if (key_just_pressed(KEY_SPACE)) {
        fire_bullet();  // Only once per press
    }
    
    if (key_just_pressed(KEY_P)) {
        toggle_pause();
    }
}
```

## Input Patterns

### Four-Way Movement

```c
void update_player() {
    player.vx = 0;
    player.vy = 0;
    
    if (key_pressed(KEY_LEFT))  player.vx = -1;
    if (key_pressed(KEY_RIGHT)) player.vx = 1;
    if (key_pressed(KEY_UP))    player.vy = -1;
    if (key_pressed(KEY_DOWN))  player.vy = 1;
    
    player.x += player.vx * player.speed;
    player.y += player.vy * player.speed;
}
```

### Diagonal Movement (Normalized)

Problem: Diagonal is faster (sqrt(2) ≈ 1.4x)!

```c
void update_player() {
    float dx = 0, dy = 0;
    
    if (key_pressed(KEY_LEFT))  dx -= 1;
    if (key_pressed(KEY_RIGHT)) dx += 1;
    if (key_pressed(KEY_UP))    dy -= 1;
    if (key_pressed(KEY_DOWN))  dy += 1;
    
    // Normalize diagonal movement
    if (dx != 0 && dy != 0) {
        dx *= 0.707f;  // 1/sqrt(2)
        dy *= 0.707f;
    }
    
    player.x += dx * player.speed;
    player.y += dy * player.speed;
}
```

### Acceleration/Deceleration

More natural feel:

```c
void update_player() {
    float target_vx = 0;
    
    if (key_pressed(KEY_LEFT))  target_vx = -player.max_speed;
    if (key_pressed(KEY_RIGHT)) target_vx = player.max_speed;
    
    // Smoothly approach target velocity
    if (target_vx != 0) {
        // Accelerate
        player.vx += target_vx * player.acceleration;
        if (player.vx > player.max_speed) player.vx = player.max_speed;
        if (player.vx < -player.max_speed) player.vx = -player.max_speed;
    } else {
        // Decelerate (friction)
        player.vx *= player.friction;  // 0.9 for ice, 0.7 for normal
        if (fabs(player.vx) < 0.01) player.vx = 0;
    }
    
    player.x += player.vx;
}
```

### Buffered Input

Problem: Player presses jump slightly before landing - nothing happens. Frustrating!

Solution: Buffer recent inputs:

```c
#define BUFFER_SIZE 5
int input_buffer[BUFFER_SIZE] = {0};
int buffer_index = 0;

void handle_input() {
    if (key_just_pressed(KEY_SPACE)) {
        input_buffer[buffer_index] = KEY_SPACE;
        buffer_index = (buffer_index + 1) % BUFFER_SIZE;
    }
}

void update_player() {
    if (player.on_ground) {
        // Check if jump was pressed recently
        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (input_buffer[i] == KEY_SPACE) {
                player_jump();
                input_buffer[i] = 0;  // Consume input
                break;
            }
        }
    }
}
```

### Coyote Time

Allow jumping slightly after leaving platform:

```c
#define COYOTE_TIME 5  // frames

int frames_since_grounded = 0;

void update_player() {
    if (player.on_ground) {
        frames_since_grounded = 0;
    } else {
        frames_since_grounded++;
    }
    
    if (key_just_pressed(KEY_SPACE)) {
        // Can jump if just left ground
        if (frames_since_grounded < COYOTE_TIME) {
            player_jump();
        }
    }
}
```

These small details make games feel much better!

## Input Context

Different controls in different states:

```c
void handle_input() {
    switch (game_state) {
        case STATE_MENU:
            if (key_just_pressed(KEY_ENTER)) {
                start_game();
            }
            if (key_just_pressed(KEY_UP)) {
                menu_cursor_up();
            }
            if (key_just_pressed(KEY_DOWN)) {
                menu_cursor_down();
            }
            break;
            
        case STATE_PLAYING:
            update_player_input();
            if (key_just_pressed(KEY_ESCAPE)) {
                pause_game();
            }
            break;
            
        case STATE_PAUSED:
            if (key_just_pressed(KEY_ESCAPE)) {
                resume_game();
            }
            if (key_just_pressed(KEY_Q)) {
                quit_to_menu();
            }
            break;
    }
}
```

## Remappable Controls

Let players customize:

```c
typedef struct {
    int up;
    int down;
    int left;
    int right;
    int action;
} Controls;

Controls controls = {
    .up = KEY_UP,
    .down = KEY_DOWN,
    .left = KEY_LEFT,
    .right = KEY_RIGHT,
    .action = KEY_SPACE
};

void update_player() {
    if (key_pressed(controls.up)) {
        // Use control mapping
    }
}
```

## Input Validation

### Debouncing

Prevent accidental double-presses:

```c
#define DEBOUNCE_TIME 100  // ms

clock_t last_press_time = 0;

void handle_input() {
    if (key_just_pressed(KEY_SPACE)) {
        clock_t now = clock();
        double elapsed = ((double)(now - last_press_time) / CLOCKS_PER_SEC) * 1000.0;
        
        if (elapsed > DEBOUNCE_TIME) {
            fire_bullet();
            last_press_time = now;
        }
    }
}
```

### Rapid Fire Limit

```c
#define FIRE_COOLDOWN 200  // ms

clock_t last_shot_time = 0;

void update_shooting() {
    if (key_pressed(KEY_SPACE)) {
        clock_t now = clock();
        double elapsed = ((double)(now - last_shot_time) / CLOCKS_PER_SEC) * 1000.0;
        
        if (elapsed > FIRE_COOLDOWN) {
            fire_bullet();
            last_shot_time = now;
        }
    }
}
```

## Mouse Input (if supported)

```c
typedef struct {
    int x, y;
    int left_button;
    int right_button;
    int left_pressed;   // Edge detection
    int right_pressed;
} Mouse;

Mouse mouse = {0};

void update_mouse() {
    int prev_left = mouse.left_button;
    int prev_right = mouse.right_button;
    
    // Get current state from renderer
    mouse.x = get_mouse_x();
    mouse.y = get_mouse_y();
    mouse.left_button = get_mouse_button(MOUSE_LEFT);
    mouse.right_button = get_mouse_button(MOUSE_RIGHT);
    
    // Detect presses
    mouse.left_pressed = mouse.left_button && !prev_left;
    mouse.right_pressed = mouse.right_button && !prev_right;
}

void handle_mouse() {
    if (mouse.left_pressed) {
        // Click at mouse.x, mouse.y
        handle_click(mouse.x, mouse.y);
    }
}
```

## Testing Input

### Print Input State

```c
void debug_input() {
    printf("Keys: ");
    if (key_pressed(KEY_LEFT)) printf("LEFT ");
    if (key_pressed(KEY_RIGHT)) printf("RIGHT ");
    if (key_pressed(KEY_UP)) printf("UP ");
    if (key_pressed(KEY_DOWN)) printf("DOWN ");
    printf("\n");
}
```

### Input Recording

Record and replay for testing:

```c
typedef struct {
    int frame;
    int key;
    int pressed;
} InputEvent;

InputEvent recording[1000];
int record_count = 0;

void record_input() {
    for (int i = 0; i < 256; i++) {
        if (key_just_pressed(i)) {
            recording[record_count++] = (InputEvent){
                .frame = current_frame,
                .key = i,
                .pressed = 1
            };
        }
    }
}
```

## Common Mistakes

**1. Polling in wrong place**
```c
// BAD: Only checks once per second
void slow_function() {
    sleep(1000);
    if (key_pressed(KEY_SPACE)) {  // Missed most presses!
        shoot();
    }
}

// GOOD: Check every frame
void game_loop() {
    handle_input();  // Every frame
    update();
    render();
}
```

**2. Frame-dependent movement**
```c
// BAD: Faster on high FPS
player.x += 5;  // Moves 5 pixels per frame

// GOOD: Time-independent
player.x += player.speed * delta_time;
```

**3. No input buffering**
Players will notice and complain about "missed" inputs.

## Summary

Good input handling is invisible. Bad input handling ruins the game. Key principles:

- Poll input every frame
- Separate continuous (hold) from discrete (press) actions
- Buffer recent inputs for forgiveness
- Use acceleration/deceleration for feel
- Context-sensitive controls
- Test with different input timings

The goal: player thinks "I pressed jump and I jumped" not "I pressed jump but the game didn't register it."
