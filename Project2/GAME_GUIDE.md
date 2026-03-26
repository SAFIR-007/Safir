# Wrath of God — Game Guide

## Controls

| Key | Action |
|-----|--------|
| **W / A / S / D** | Move Up / Left / Down / Right |
| **Left Click** | Shoot (Level 1) / Attack (Boss Level) |
| **C** | Toggle Crouch (Boss Level — reduces detection) |
| **Space** | Dodge Roll (Boss Level — press with WASD for direction) |
| **F** | Place Decoy (Boss Level — distracts boss for 5 seconds) |
| **E** | Activate Seal (Boss Level — when near a glowing seal) |
| **T** | Jump to Boss Level (from Menu — debug shortcut) |
| **M** | Return to Menu (from non-gameplay screens) |
| **B / Backspace / ESC** | Back (from Credits or Options) |

---

## Game Flow

```
Menu → Level 1 → Win Screen → Final BG → Surprise Screen
→ Puzzle Screen → Intel Screen → Warning Screen
→ Second BG → Boss Level
```

---

## Level 1 — Shooter

- Move with **WASD**, aim and shoot with **Left Click**.
- Defeat all **3 enemies** to complete the level.
- Bullets bounce off walls, ceiling, and floor.
- Avoid enemy contact — they drain your health on touch.
- Player HP: **100**

---

## Puzzle Screen — Gravity Core Stabilizer

- A 4×4 grid of pipe tiles connects "CORE" (top-left) to "LOCK" (bottom-right).
- **Left click** a tile to rotate it 90°.
- Connect a path from corner to corner before the **30-second timer** runs out.
- Tile types: Straight, L-Shape, Cross.

---

## Boss Level — "The Sentinel Judge"

### Overview
A top-down stealth/combat encounter with a multi-phase boss. The boss has **250 HP**, the player has **150 HP**.

### Boss Phases

#### Phase 1: OBSERVE (Stealth)
- The boss stands at center with a **rotating searchlight**.
- Stay in **shadow zones** (dark rectangles) to hide.
- **Crouch (C)** to reduce detection in the searchlight.
- If the **Awareness bar** reaches 100%, the boss enters Hunt → Execute mode.
- Awareness **decays naturally** when you are hidden.
- **Goal**: Activate **3 seals** by walking near them and pressing **E**. This forces the boss into Hunt mode.

#### Phase 2: HUNT
- The boss moves toward your last known position.
- Use **Decoy (F)** to distract the boss.
- **Critical Strike**: Get within 120 units and position yourself so the boss is facing away from you, then **Left Click** for **60 damage**. This resets the boss back to Observe.
- If awareness hits 100%, boss enters Execute mode.

#### Phase 3: EXECUTE
- The boss charges directly at you at high speed.
- **Dodge (Space + direction)** to avoid damage.
- Triggered when awareness reaches 100% during Hunt.

#### Phase 4: DUEL (Boss HP ≤ 125)
- The boss enters close combat and uses random attacks:
  - **Heavy Charge** — charges to your position (20 dmg)
  - **Feint** — close-range strike (12 dmg)
  - **Shockwave** — expanding ring from boss (8 dmg)
- After each attack, a **Strike Window** opens for 3 seconds — get within 120 units and **Left Click** for **25 damage**.
- **Dodge (Space)** to avoid boss attacks.

### Attack Summary

| Attack | Condition | Damage | Cooldown |
|--------|-----------|--------|----------|
| **Basic Melee** | Within 100 units, any phase | 10 | 1 second |
| **Critical Strike** | Behind boss in Hunt mode, within 120 units | 60 | Resets boss to Observe |
| **Strike Window** | After boss attack in Duel, within 120 units | 25 | Until next window |

### Tips
- Use shadows and crouching to keep Awareness low.
- Place decoys to control where the boss moves.
- Critical strikes are the fastest way to deal damage.
- In Duel phase, dodge boss attacks then counter during Strike Windows.
- Basic melee always works when close — use it as a fallback.

---

## Win / Lose Conditions

- **Boss dies (0 HP)**: Victory — returns to Menu after 4 seconds.
- **Player dies (0 HP)**: Defeat — returns to Menu after 3 seconds.
