# C Game Programming

Build actual games from scratch using the renderer. Learn game loops, input handling, collision detection, and game state management by making real playable games.

## What you get

- Classic arcade games (Snake, Pong, Breakout, Tetris)
- Game loop fundamentals
- Input handling and player control
- Collision detection algorithms
- Game state management
- Score tracking and UI
- AI opponents

Everything built with the custom renderer. No game engines, no libraries - just C and the rendering code.

## Building

Each game is standalone:

```bash
# Windows (PowerShell or cmd)
cd examples
build_all.bat

# Linux / MSYS2
cd examples
./build_all.sh
```

Games use the renderer from `../Renderer/src/renderer.c`

## Playing

Run any game:
```bash
# Windows
bin\snake.exe
bin\pong.exe

# Linux
./bin/snake
./bin/pong
```

Controls shown on startup screen for each game.

## Documentation

- **[Game Loop](docs/GAME_LOOP.md)** - Core game structure, timing, frame rate
- **[Input Handling](docs/INPUT.md)** - Keyboard input, player control patterns
- **[Collision Detection](docs/COLLISION.md)** - AABB, circle, grid-based collision
- **[Game State](docs/GAME_STATE.md)** - Menus, pause, game over, state machines

## Games

| Game | What It Teaches |
|------|----------------|
| Snake | Grid-based movement, self-collision, growing mechanics |
| Pong | Continuous movement, ball physics, AI opponent |
| Breakout | Brick collision, power-ups, level design |
| Tetris | Rotation, grid locking, line clearing, game feel |
| Space Shooter | Projectiles, spawning, scrolling, wave system |

Each game builds on concepts from previous ones.

## What this teaches

- Game loop architecture (update, render, timing)
- Input handling and responsiveness
- Collision detection algorithms
- Game state management
- Score and UI systems
- AI for opponents
- Game feel and polish
- Debugging game logic

After working through these, you'll understand how games work internally and can build your own from scratch.

## Quick Start

```bash
cd examples
build_all.bat      # Build all games
bin\snake.exe      # Play Snake!
```

Arrow keys to move, ESC to quit. Have fun!
