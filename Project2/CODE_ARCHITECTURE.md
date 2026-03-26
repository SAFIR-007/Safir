# Wrath of God — Code Architecture & Function Walkthrough

## File: `iMain.cpp` (1924 lines)

---

## 1. GLOBAL DECLARATIONS (Lines 1–137)

### Game States (Lines 3–14)
```cpp
#define MENU 0
#define LEVEL1 1
#define OPTIONS_STATE 2
#define CREDITS_STATE 3
#define WIN_SCREEN 4
#define FINAL_BG 5
#define SURPRISE_SCREEN 6
#define PUZZLE_SCREEN 7
#define INTEL_SCREEN 8
#define WARNING_SCREEN 9
#define SECOND_BG 10
#define BOSS_LEVEL 11
```
The entire game is driven by `int gameState`. Every draw, update, and input function checks this variable to decide what to do.

### Image Variables (Lines 17–23)
Stores texture IDs for backgrounds and spritesheets loaded in `main()`.

### Structs & Data

| Struct | Lines | Purpose |
|--------|-------|---------|
| `GravityTile` | 25–29 | Puzzle tile (type, rotation, active) |
| `Bullet` | 40–44 | Bullet position, velocity, active flag |
| `Enemy` | 46–54 | Enemy position, health, patrol, animation |
| `Player` | 55–62 | Player position, health, shooting state |
| `P3Shadow` | 117–119 | Boss level shadow zone rectangle |
| `P3Pillar` | 123–125 | Boss level obstacle pillar |
| `P3Seal` | 129–132 | Boss level activatable seal |

### Boss Level Variables (Lines 68–136)
All variables prefixed with `phase3` — boss position, health, states, attack timers, searchlight angle, awareness level, player position/health, dodge, decoy, noise, etc.

---

## 2. GAME FLOW — How Stages Connect

```
main() [L1901]
  ├── iInitialize() — creates window
  ├── iLoadImage() — loads all textures
  └── iStart() — enters GLUT main loop
        ├── iDraw() [L1188] — called every frame to render
        ├── fixedUpdate() [L1508] — called every 16ms for game logic
        ├── iKeyboard() [L1327] — called on key press
        ├── iKeyUp() [L1392] — called on key release
        └── iMouse() [L1412] — called on mouse click
```

---

## 3. STAGE: MAIN MENU (gameState = MENU)

### Drawing
| Function | Lines | What it does |
|----------|-------|-------------|
| `drawMenu()` | 343–424 | Draws background image, title text, and 4 buttons: START CAMPAIGN, OPTIONS, CREDITS, EXIT |

### Input — `iMouse()` (Lines 1415–1431)
Clicking buttons changes `gameState`:
- **START CAMPAIGN** → `gameState = LEVEL1`, calls `initEnemies()`
- **OPTIONS** → `gameState = OPTIONS_STATE`
- **CREDITS** → `gameState = CREDITS_STATE`
- **EXIT** → `exit(0)`

### Sub-screens
| Function | Lines | Triggered by |
|----------|-------|-------------|
| `drawOptions()` | 463–485 | OPTIONS button |
| `drawCredits()` | 426–461 | CREDITS button |

Both return to MENU via `B`/`Backspace`/`ESC` keys or clicking anywhere.

---

## 4. STAGE: LEVEL 1 — SHOOTING GAME (gameState = LEVEL1)

### Initialization
| Function | Lines | What it does |
|----------|-------|-------------|
| `initEnemies()` | 170–209 | Sets up 3 enemies at fixed positions with patrol bounds and health |

### Drawing
| Function | Lines | What it does |
|----------|-------|-------------|
| `drawLevel1()` | 289–341 | Draws stage background, walls, player sprite, enemy sprites, bullets, HP bar |
| `iShowSubImage()` | 139–168 | Helper to render a sub-region of a spritesheet |

### Game Logic — `fixedUpdate()` (Lines 1508–1668)
Called every 16ms. For LEVEL1:

1. **Player Movement** (L1511–1531) — WASD moves player, clamped to screen
2. **Reload Cooldown** (L1533–1535) — Decrements shot cooldown timer
3. **Shoot Animation** (L1537–1547) — Cycles through 3-frame animation
4. **Bullet Movement** (L1550–1643) — Moves bullets, bounces off walls/ceiling/floor, checks enemy collision
5. **Enemy Patrol** (L1645–1675) — Enemies walk left/right within patrol bounds, animate, damage player on touch
6. **Level Completion** (L1678–1688) — When all 3 enemies dead → `gameState = WIN_SCREEN`

### Input — `iMouse()` (Lines 1434–1457)
Left click:
- Triggers shooting animation
- Creates a bullet aimed at mouse position (with cooldown)

---

## 5. TRANSITION SCREENS (Between Level 1 → Boss Level)

After defeating all enemies, the game flows through a sequence of timed screens:

```
WIN_SCREEN → (click) → FINAL_BG → (click) → SURPRISE_SCREEN
→ (5s timer) → PUZZLE_SCREEN → (solve puzzle) → INTEL_SCREEN
→ (5s timer) → WARNING_SCREEN → (5s timer) → SECOND_BG
→ (10s timer) → BOSS_LEVEL
```

| Screen | Draw Function | Lines | Logic in fixedUpdate() | Duration |
|--------|--------------|-------|----------------------|----------|
| WIN_SCREEN | `drawWinScreen()` | 487–498 | Click → FINAL_BG | Manual |
| FINAL_BG | `drawFinalBG()` | 500 | Click → SURPRISE | Manual |
| SURPRISE_SCREEN | `drawSurpriseScreen()` | 1221–1233 | L1691–1696 | 5 seconds |
| PUZZLE_SCREEN | `drawPuzzle()` | 1278–1321 | L1699–1776 | 30s timer |
| INTEL_SCREEN | `drawIntel()` | 1137–1181 | L1802–1807 | 5 seconds |
| WARNING_SCREEN | `drawWarning()` | 502–540 | L1810–1816 | 5 seconds |
| SECOND_BG | `drawSecondBG()` | 542 | L1819–1825 | 10 seconds |

### Puzzle Screen Logic (Lines 1699–1776)
- `initPuzzle()` (L264–287) — Randomizes a 4×4 grid of pipe tiles
- `drawGravityNode()` (L1235–1276) — Draws each tile with rotation
- `isTilePortOpen()` (L211–230) — Checks if a tile side is open
- `isPuzzleAlreadySolved()` (L232–262) — Validates path from corner to corner
- Click a tile in `iMouse()` (L1466–1477) to rotate it
- DFS connectivity check runs every frame (L1716–1775) to detect solution

---

## 6. STAGE: BOSS LEVEL (gameState = BOSS_LEVEL)

### Initialization
| Function | Lines | What it does |
|----------|-------|-------------|
| `p3InitPhase3()` | 548–598 | Sets up boss at center (512,320), player at (100,400), creates 6 shadow zones, 4 pillars, 3 seals, resets all boss/player variables |

### Drawing (called every frame from `iDraw()` L1213–1218)
| Function | Lines | What it does |
|----------|-------|-------------|
| `drawPhase3Arena()` | 627–678 | Draws dark background, shadow zones (dark blue), pillars (gray), seals (glow), decoy, searchlight cone |
| `drawPhase3Player()` | 736–758 | Draws player circle (green/yellow when crouching), dodge afterimage, critical strike indicator |
| `drawPhase3Boss()` | 680–734 | Draws boss body (red), eye, attack indicators (charge line, feint ring, shockwave), death animation |
| `drawPhase3HUD()` | 760–833 | Draws player HP bar, boss HP bar, awareness bar, noise bar, seal count, boss state text, crouch indicator, controls legend |

### Game Logic (called every 16ms from `fixedUpdate()` L1832–1899)
| Function | Lines | What it does |
|----------|-------|-------------|
| Edge-detected keys | L1834–1889 | Handles C (crouch toggle), Space (dodge), F (decoy), E (seal activation) using framework's keyPressed[] |
| `updatePhase3Player()` | 934–1030 | WASD movement, dodge physics, pillar collision, critical strike angle check, boundary clamping |
| `updatePhase3Boss()` | 1032–1135 | Boss AI state machine: OBSERVE→HUNT→EXECUTE→DUEL→DEAD |
| `handlePhase3Stealth()` | 835–856 | Searchlight + shadow detection → increases/decreases awareness |
| `handlePhase3Combat()` | 858–932 | Boss attacks (heavy/feint/shockwave), strike window timing, decoy timer |

### Boss AI State Machine (`updatePhase3Boss()` L1032–1135)

```
OBSERVE (L1038–1047)
  │ Searchlight sweeps. If 3 seals activated OR awareness >= 100:
  ▼
HUNT (L1048–1087)
  │ Boss moves toward last known position.
  │ If boss HP <= 125 → DUEL
  │ If awareness >= 100 → EXECUTE
  ▼
EXECUTE (L1088–1110)
  │ Boss charges player at high speed.
  │ If boss HP <= 125 → DUEL
  ▼
DUEL (L1111–1126)
  │ Boss stays close, uses attacks.
  │ Strike windows open after each attack.
  ▼
DEAD (L1127–1134)
  │ Boss HP reaches 0. Victory timer → MENU.
```

### Attack Input — `iMouse()` (Lines 1481–1499)
Left click in BOSS_LEVEL:
1. **Strike Window active + within 120 units** → 25 damage to boss
2. **Critical Strike ready** (behind boss in HUNT) → 60 damage, resets to OBSERVE
3. **Basic Melee** (within 100 units, cooldown ready) → 10 damage

---

## 7. CORE ENGINE FUNCTIONS

### `iDraw()` — Lines 1188–1219
Master render function. Checks `gameState` and calls the appropriate `draw*()` function.

### `fixedUpdate()` — Lines 1508–1899
Master game logic function. Called every 16ms via timer. Handles ALL game state updates.

### `iKeyboard()` — Lines 1327–1390
Key press handler. Handles B/ESC (back), M (menu), T (boss test), boss level key states.

### `iMouse()` — Lines 1412–1506
Mouse click handler. Handles menu buttons, Level 1 shooting, puzzle tile rotation, boss level attacks.

### `main()` — Lines 1901–1923
Entry point. Initializes window (1024×640), loads all images, starts the GLUT loop.

---

## 8. QUICK REFERENCE: All Functions by Line

| Line | Function | Stage |
|------|----------|-------|
| 137 | `p3IsKeyPressed()` | Boss Level |
| 139 | `iShowSubImage()` | Utility |
| 170 | `initEnemies()` | Level 1 |
| 211 | `isTilePortOpen()` | Puzzle |
| 232 | `isPuzzleAlreadySolved()` | Puzzle |
| 264 | `initPuzzle()` | Puzzle |
| 289 | `drawLevel1()` | Level 1 |
| 343 | `drawMenu()` | Menu |
| 426 | `drawCredits()` | Menu |
| 463 | `drawOptions()` | Menu |
| 487 | `drawWinScreen()` | Transition |
| 500 | `drawFinalBG()` | Transition |
| 502 | `drawWarning()` | Transition |
| 542 | `drawSecondBG()` | Transition |
| 548 | `p3InitPhase3()` | Boss Level |
| 600 | `phase3PlayerInShadow()` | Boss Level |
| 612 | `phase3PlayerInSearchlight()` | Boss Level |
| 627 | `drawPhase3Arena()` | Boss Level |
| 680 | `drawPhase3Boss()` | Boss Level |
| 736 | `drawPhase3Player()` | Boss Level |
| 760 | `drawPhase3HUD()` | Boss Level |
| 835 | `handlePhase3Stealth()` | Boss Level |
| 858 | `handlePhase3Combat()` | Boss Level |
| 934 | `updatePhase3Player()` | Boss Level |
| 1032 | `updatePhase3Boss()` | Boss Level |
| 1137 | `drawIntel()` | Transition |
| 1221 | `drawSurpriseScreen()` | Transition |
| 1235 | `drawGravityNode()` | Puzzle |
| 1278 | `drawPuzzle()` | Puzzle |
| 1327 | `iKeyboard()` | Input |
| 1392 | `iKeyUp()` | Input |
| 1394 | `iSpecialKeyboard()` | Input |
| 1406 | `iSpecialKeyUp()` | Input |
| 1412 | `iMouse()` | Input |
| 1508 | `fixedUpdate()` | Engine |
| 1901 | `main()` | Engine |
