# Collision Detection

## The Big Picture

Games need to know when things touch. Player hits wall, bullet hits enemy, coin collected. Collision detection is checking if two objects overlap.

Simple concept, but performance matters. Checking every object against every other is O(n²) - gets slow fast. Smart approaches keep it fast.

## Axis-Aligned Bounding Box (AABB)

Simplest and fastest for rectangles.

### Rectangle Collision

```c
typedef struct {
    float x, y;      // Position
    float w, h;      // Size
} Rect;

int rect_collision(Rect a, Rect b) {
    return a.x < b.x + b.w &&
           a.x + a.w > b.x &&
           a.y < b.y + b.h &&
           a.y + a.h > b.y;
}
```

Visual:
```
   a.x          a.x+a.w
    |------------|
    |     A      |
    |------------|
          |------------|
          |     B      |
          |------------|
         b.x          b.x+b.w

Overlaps if:
- A's left < B's right
- A's right > B's left  
- A's top < B's bottom
- A's bottom > B's top
```

### Point-Rectangle Collision

Click detection, cursor-over checks:

```c
int point_in_rect(float px, float py, Rect r) {
    return px >= r.x &&
           px <= r.x + r.w &&
           py >= r.y &&
           py <= r.y + r.h;
}
```

## Circle Collision

Better for round objects (balls, explosions):

```c
typedef struct {
    float x, y;      // Center
    float radius;
} Circle;

int circle_collision(Circle a, Circle b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float distance_squared = dx * dx + dy * dy;
    float radius_sum = a.radius + b.radius;
    
    return distance_squared < radius_sum * radius_sum;
}
```

Why squared? Avoid expensive sqrt():
```c
// SLOW
float distance = sqrt(dx*dx + dy*dy);
return distance < radius_sum;

// FAST (same result)
float distance_squared = dx*dx + dy*dy;
return distance_squared < radius_sum * radius_sum;
```

## Grid-Based Collision

For tile-based games (Snake, Pac-Man, platformers):

```c
#define GRID_WIDTH 20
#define GRID_HEIGHT 15
#define TILE_SIZE 32

int grid[GRID_HEIGHT][GRID_WIDTH];  // 0 = empty, 1 = wall

int tile_collision(float x, float y) {
    int tile_x = (int)(x / TILE_SIZE);
    int tile_y = (int)(y / TILE_SIZE);
    
    // Check bounds
    if (tile_x < 0 || tile_x >= GRID_WIDTH ||
        tile_y < 0 || tile_y >= GRID_HEIGHT) {
        return 1;  // Out of bounds = collision
    }
    
    return grid[tile_y][tile_x] == 1;  // 1 = solid tile
}
```

Check multiple points for better accuracy:

```c
int player_collision(Player* p) {
    // Check four corners
    int tl = tile_collision(p->x, p->y);                    // Top-left
    int tr = tile_collision(p->x + p->w, p->y);             // Top-right
    int bl = tile_collision(p->x, p->y + p->h);             // Bottom-left
    int br = tile_collision(p->x + p->w, p->y + p->h);      // Bottom-right
    
    return tl || tr || bl || br;
}
```

## Collision Response

### Simple Blocking

Stop movement:

```c
void update_player() {
    float new_x = player.x + player.vx;
    float new_y = player.y + player.vy;
    
    // Check new position
    if (!tile_collision(new_x, player.y)) {
        player.x = new_x;
    }
    
    if (!tile_collision(player.x, new_y)) {
        player.y = new_y;
    }
}
```

Note: Check X and Y separately for smooth wall sliding.

### Pushback

Move objects apart:

```c
void resolve_collision(Rect* a, Rect* b) {
    // Calculate overlap
    float overlap_x = (a->x + a->w / 2) - (b->x + b->w / 2);
    float overlap_y = (a->y + a->h / 2) - (b->y + b->h / 2);
    
    // Push back based on overlap
    if (fabs(overlap_x) > fabs(overlap_y)) {
        // Horizontal separation
        if (overlap_x > 0) {
            a->x += overlap_x;
        } else {
            a->x -= overlap_x;
        }
    } else {
        // Vertical separation
        if (overlap_y > 0) {
            a->y += overlap_y;
        } else {
            a->y -= overlap_y;
        }
    }
}
```

### Bounce

Reverse velocity:

```c
void ball_wall_bounce(Ball* ball) {
    if (ball->x < 0 || ball->x + ball->w > SCREEN_WIDTH) {
        ball->vx = -ball->vx;  // Reverse horizontal
    }
    
    if (ball->y < 0 || ball->y + ball->h > SCREEN_HEIGHT) {
        ball->vy = -ball->vy;  // Reverse vertical
    }
}
```

With energy loss:

```c
ball->vx *= -0.8;  // 80% bounce
ball->vy *= -0.8;
```

## Optimization Strategies

### Spatial Partitioning

Don't check every object against every other:

```c
// BAD: O(n²)
for (int i = 0; i < bullet_count; i++) {
    for (int j = 0; j < enemy_count; j++) {
        if (collides(bullets[i], enemies[j])) {
            hit(enemies[j]);
        }
    }
}
```

Use a grid to only check nearby objects:

```c
#define GRID_CELLS 10

typedef struct {
    Rect* objects[100];
    int count;
} GridCell;

GridCell spatial_grid[GRID_CELLS][GRID_CELLS];

void add_to_grid(Rect* obj) {
    int cell_x = (int)(obj->x / (SCREEN_WIDTH / GRID_CELLS));
    int cell_y = (int)(obj->y / (SCREEN_HEIGHT / GRID_CELLS));
    
    if (cell_x >= 0 && cell_x < GRID_CELLS &&
        cell_y >= 0 && cell_y < GRID_CELLS) {
        GridCell* cell = &spatial_grid[cell_y][cell_x];
        cell->objects[cell->count++] = obj;
    }
}

void check_collisions_in_grid() {
    for (int y = 0; y < GRID_CELLS; y++) {
        for (int x = 0; x < GRID_CELLS; x++) {
            GridCell* cell = &spatial_grid[y][x];
            
            // Only check objects in same cell
            for (int i = 0; i < cell->count; i++) {
                for (int j = i + 1; j < cell->count; j++) {
                    if (collides(cell->objects[i], cell->objects[j])) {
                        handle_collision(cell->objects[i], cell->objects[j]);
                    }
                }
            }
        }
    }
}
```

### Broad Phase / Narrow Phase

Two-step detection:

1. **Broad phase:** Quick rough check (circles)
2. **Narrow phase:** Precise check (pixel-perfect)

```c
int check_collision(Sprite* a, Sprite* b) {
    // Broad phase: Circle check (fast)
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dist_sq = dx * dx + dy * dy;
    float radius_sum = a->radius + b->radius;
    
    if (dist_sq > radius_sum * radius_sum) {
        return 0;  // Definitely not colliding
    }
    
    // Narrow phase: Rectangle check (slower but accurate)
    return rect_collision(a->rect, b->rect);
}
```

### Early Exit

Stop checking once collision found:

```c
int player_hit_enemy() {
    for (int i = 0; i < enemy_count; i++) {
        if (collides(player, enemies[i])) {
            return 1;  // Found one, stop checking
        }
    }
    return 0;
}
```

## Advanced Techniques

### Swept AABB

Continuous collision (for fast-moving objects):

```c
float swept_aabb(Rect a, float vx, float vy, Rect b) {
    // Find time of first contact
    // Returns value between 0 (start) and 1 (end)
    // Implementation complex, but prevents tunneling
}
```

Without this, fast bullets can pass through thin walls.

### Collision Layers

Not everything collides with everything:

```c
typedef enum {
    LAYER_PLAYER   = 1 << 0,  // 0001
    LAYER_ENEMY    = 1 << 1,  // 0010
    LAYER_WALL     = 1 << 2,  // 0100
    LAYER_PICKUP   = 1 << 3   // 1000
} CollisionLayer;

typedef struct {
    Rect rect;
    CollisionLayer layer;
    CollisionLayer mask;  // What can it collide with?
} GameObject;

int can_collide(GameObject* a, GameObject* b) {
    return (a->layer & b->mask) || (b->layer & a->mask);
}
```

Example:
```c
player.layer = LAYER_PLAYER;
player.mask = LAYER_ENEMY | LAYER_WALL | LAYER_PICKUP;

enemy.layer = LAYER_ENEMY;
enemy.mask = LAYER_PLAYER | LAYER_WALL;

// Enemies don't collide with each other
// Players don't collide with pickups they already collected
```

## Collision Types

### Trigger vs Solid

```c
typedef struct {
    Rect rect;
    int is_trigger;  // If true, detect but don't block
} Collider;

void handle_collision(GameObject* a, GameObject* b) {
    if (b->collider.is_trigger) {
        // Just notify
        on_trigger_enter(a, b);
    } else {
        // Block and push back
        resolve_solid_collision(a, b);
    }
}
```

**Trigger:** Coin collection, death zones, checkpoints  
**Solid:** Walls, platforms, obstacles

## Testing Collisions

### Visual Debug

Draw collision boxes:

```c
void debug_draw_colliders() {
    for (int i = 0; i < object_count; i++) {
        Rect r = objects[i].collider;
        
        // Draw green if no collision, red if colliding
        int color = objects[i].is_colliding ? COLOR_RED : COLOR_GREEN;
        draw_rect_outline(r.x, r.y, r.w, r.h, color);
    }
}
```

### Collision Count

```c
int collision_checks = 0;

void check_all_collisions() {
    collision_checks = 0;
    
    for (int i = 0; i < object_count; i++) {
        for (int j = i + 1; j < object_count; j++) {
            collision_checks++;
            if (collides(objects[i], objects[j])) {
                handle_collision(objects[i], objects[j]);
            }
        }
    }
    
    printf("Collision checks: %d\n", collision_checks);
}
```

## Common Mistakes

**1. Tunneling (missed collisions)**
```c
// BAD: Fast bullet skips over thin wall
bullet.x += bullet.vx;  // vx = 100, wall width = 5

// GOOD: Raycast or swept collision
check_path(bullet.x, bullet.y, bullet.x + bullet.vx, bullet.y + bullet.vy);
```

**2. Double collision**
```c
// BAD: Both objects push each other back = fighting
resolve_collision(&a, &b);
resolve_collision(&b, &a);

// GOOD: One resolution
resolve_collision(&a, &b);
```

**3. Floating point precision**
```c
// BAD: Might not be exactly 0
if (player.x == target.x) {
    // Rarely true!
}

// GOOD: Use tolerance
if (fabs(player.x - target.x) < 0.01) {
    // Close enough
}
```

## Summary

Collision detection is checking if shapes overlap. Start simple (AABB), optimize later (spatial partitioning). Response determines what happens: block, bounce, trigger event.

Key points:
- AABB for rectangles (fast)
- Circle for round objects
- Grid for tile-based games
- Spatial partitioning for many objects
- Separate X and Y checks for smooth sliding
- Visual debug drawing helps immensely

Most bugs in games are collision-related. Test thoroughly!
