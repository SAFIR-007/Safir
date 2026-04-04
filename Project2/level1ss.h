#ifndef LEVEL1SS_H
#define LEVEL1SS_H

#include "GameShared.h"

// ============================================================
// NEW SIDE-SCROLLING LEVEL DECLARATIONS
// ============================================================

extern unsigned int newLevelBG[3];
extern unsigned int newPlayerSpritesheet;
extern unsigned int npc1Spritesheet;
extern unsigned int npc2Spritesheet;
extern int puzzleTransitionPhase;
extern double puzzleTransitionTimer;
extern double puzzleTransitionAlpha;
extern double scrollX;               // world scroll offset
extern double newPlayerX;            // player screen X position
extern double newPlayerY;            // player Y (ground level)
extern double newPlayerVelY;         // vertical velocity for jump
extern bool newPlayerOnGround;       // is the player on the ground?
extern int newPlayerFrame;           // animation frame (0-3)
extern double newPlayerAnimTimer;    // animation timing
extern int newPlayerDir;             // 1 = right, -1 = left
extern bool newPlayerMoving;         // is the player walking?
extern bool newPlayerIsShooting;     // is the player currently shooting?
extern double newPlayerShootTimer;   // timer to revert to idle/walking
extern double newPlayerFireCooldown; // cooldown between shots
extern bool arrowKeyLeft;            // arrow key states
extern bool arrowKeyRight;
extern bool arrowKeyUp;

extern int newPlayerHealth;
extern int newPlayerMaxHealth;

#define MAX_NEW_BULLETS 20
struct NewLevelBullet {
  double x, y;
  double speed;
  int dir; // 1 or -1
  bool active;
  bool isPlayerBullet; // True if player shot it, false if enemy
};
extern NewLevelBullet newBullets[MAX_NEW_BULLETS];

#define MAX_NEW_ENEMIES 10
struct NewLevelEnemy {
  double x, y;
  bool active;
  int health;
  int type; // 1 for NPC.png, 2 for NPC2.png
  int dir; // 1 (right) or -1 (left)
  
  // Animation state
  int frame;
  double animTimer;
  bool isShooting;
  double shootTimer;
  double fireCooldown;
};
extern NewLevelEnemy newEnemies[MAX_NEW_ENEMIES];

#define GROUND_Y 50
#define GRAVITY 0.6
#define JUMP_FORCE 14.0
#define SCROLL_SPEED 4.0
#define BG_WIDTH 1024
#define TOTAL_SCROLL_WIDTH (BG_WIDTH * 3) // 3072

// Function prototypes for new level functionality
void initNewLevel();
void drawNewLevel();
void updateNewLevel();
void mouseNewLevel(int button, int state, int mx, int my);
void keyboardNewLevel(unsigned char key);
void keyUpNewLevel(unsigned char key);
void specialKeyboardNewLevel(unsigned char key);
void specialKeyUpNewLevel(unsigned char key);



inline void initNewLevel() {
  gameState = NEW_LEVEL;
  scrollX = 0;
  newPlayerX = 200;
  newPlayerY = GROUND_Y;
  newPlayerVelY = 0;
  newPlayerOnGround = true;
  newPlayerFrame = 0;
  newPlayerDir = 1;
  newPlayerMoving = false;
  newPlayerIsShooting = false;
  newPlayerShootTimer = 0;
  newPlayerFireCooldown = 0;
  arrowKeyLeft = false;
  arrowKeyRight = false;
  arrowKeyUp = false;
  newPlayerHealth = 100;
  for (int i = 0; i < MAX_NEW_BULLETS; i++) {
    newBullets[i].active = false;
  }

  // Initialize Enemies
  for (int i = 0; i < MAX_NEW_ENEMIES; i++) {
    newEnemies[i].active = false;
  }
  
  // Let's spawn a mix of NPC1 and NPC2 down the absolute right
  // The level width is 3072.
  int spawnXs[4] = {800, 1500, 2200, 2800};
  for (int i = 0; i < 4; i++) {
    newEnemies[i].active = true;
    newEnemies[i].x = spawnXs[i];
    newEnemies[i].y = GROUND_Y;
    newEnemies[i].health = 200;
    newEnemies[i].type = 1; // All enemies use NPC.png
    newEnemies[i].dir = -1; // Face left towards player initially
    newEnemies[i].frame = 0;
    newEnemies[i].animTimer = 0;
    newEnemies[i].isShooting = false;
    newEnemies[i].shootTimer = 0;
    newEnemies[i].fireCooldown = 0;
  }
}

inline void drawNewLevel() {
  // --- Draw scrolling backgrounds ---
  // Each BG is 1024px wide. Total loop = 3072px.
  // We need to figure out which BG panels are visible and where to draw them.
  double offset = scrollX;
  // Normalize offset into [0, TOTAL_SCROLL_WIDTH)
  offset = offset - (int)(offset / TOTAL_SCROLL_WIDTH) * TOTAL_SCROLL_WIDTH;
  if (offset < 0) offset += TOTAL_SCROLL_WIDTH;

  // We may need to draw up to 2 panels to fill the 1024px screen
  for (int i = -1; i <= 1; i++) {
    // Calculate the world-x of panel start
    // Panel index in the sequence
    double panelWorldX = 0;
    int panelIdx = 0;

    // For each potential visible panel position
    double checkX = (int)(offset / BG_WIDTH) * BG_WIDTH + i * BG_WIDTH;
    double screenX = checkX - offset;

    // Only draw if it overlaps the screen [0, 1024)
    if (screenX + BG_WIDTH > 0 && screenX < 1024) {
      // Which BG image?
      double worldX = checkX;
      // Normalize worldX to [0, TOTAL_SCROLL_WIDTH)
      worldX = worldX - (int)(worldX / TOTAL_SCROLL_WIDTH) * TOTAL_SCROLL_WIDTH;
      if (worldX < 0) worldX += TOTAL_SCROLL_WIDTH;
      panelIdx = (int)(worldX / BG_WIDTH);
      if (panelIdx < 0) panelIdx = 0;
      if (panelIdx > 2) panelIdx = 2;

      iShowImage((int)screenX, 0, BG_WIDTH, 640, newLevelBG[panelIdx]);
    }
  }

  // --- Draw player character (using PC2.png) ---
  int fw = 63, fh = 61;       // approx frame size for 1011x247 grid (16 cols, 4 rows)
  int totalW = 1011, totalH = 247;

  // PC2.png layout:
  // Row 0 (top): Idle/Forward
  // Row 1: Walk Right cycle (16 frames)
  // Row 2: Walk Left cycle (16 frames)
  // Row 3 (bottom): Shooting/Action

  // PC2.png only has the walk cycle facing right on Row 2 (index 2).
  // For facing left, we use the same row but flip the texture horizontally.
  int frameX;
  int frameY = 2; // Always use Row 2 (walk right) by default
  bool flipX = false;

  if (newPlayerDir == 1) { // Facing Right
    flipX = false;
    if (newPlayerIsShooting) {
      frameY = 2; // Stay on the side-facing row
      frameX = 1; // Use a distinct action frame instead of idle (0) or late cycle (8)
    } else if (newPlayerMoving) {
      frameX = newPlayerFrame % 8; // Cycle frames
    } else {
      frameX = 0; // Idle frame
    }
  } else { // Facing Left
    flipX = true;
    if (newPlayerIsShooting) {
      frameY = 2; // Flipped row for left
      frameX = 1; 
    } else if (newPlayerMoving) {
      frameX = newPlayerFrame % 8; // Cycle frames
    } else {
      frameX = 0; // Idle frame
    }
  }

  int sx = frameX * fw;
  int sy = frameY * fh;
  int x = (int)newPlayerX - 40;
  int y = (int)newPlayerY - 40;
  int width = 80;
  int height = 80;

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, newPlayerSpritesheet);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  double tx1 = (double)sx / totalW;
  double tx2 = (double)(sx + fw) / totalW;
  double ty1 = -(double)sy / totalH;
  double ty2 = -(double)(sy + fh) / totalH;

  glBegin(GL_QUADS);
  if (flipX) {
    // Flipped horizontally
    glTexCoord2f(tx2, ty1); glVertex2f(x, y);
    glTexCoord2f(tx1, ty1); glVertex2f(x + width, y);
    glTexCoord2f(tx1, ty2); glVertex2f(x + width, y + height);
    glTexCoord2f(tx2, ty2); glVertex2f(x, y + height);
  } else {
    // Normal
    glTexCoord2f(tx1, ty1); glVertex2f(x, y);
    glTexCoord2f(tx2, ty1); glVertex2f(x + width, y);
    glTexCoord2f(tx2, ty2); glVertex2f(x + width, y + height);
    glTexCoord2f(tx1, ty2); glVertex2f(x, y + height);
  }
  glEnd();
  glDisable(GL_TEXTURE_2D);

  // --- Draw Enemies ---
  // All enemies use NPC.png: 4 frames x 1 row (256x64)
  // Frame 0: back, Frame 1: side-right, Frame 2: front/gun, Frame 3: side-right walk
  // Use a fixed side-facing frame to avoid rotation effect
  for (int i = 0; i < MAX_NEW_ENEMIES; i++) {
    if (newEnemies[i].active) {
      int eFrameX;
      bool eFlipX = false;

      // NPC.png frame 1 faces LEFT, frame 2 faces RIGHT
      if (newEnemies[i].dir == -1) { // Enemy facing left (toward player on left)
        if (newEnemies[i].isShooting) {
          eFrameX = 2; // Shooting frame faces right, flip it
          eFlipX = true;
        } else {
          eFrameX = 1; // Walk frame already faces left, no flip
          eFlipX = false;
        }
      } else { // Enemy facing right (toward player on right)
        if (newEnemies[i].isShooting) {
          eFrameX = 2; // Shooting frame already faces right, no flip
          eFlipX = false;
        } else {
          eFrameX = 1; // Walk frame faces left, flip it
          eFlipX = true;
        }
      }

      int eTotalW = 256, eTotalH = 64;
      int eFw = 64, eFh = 64;

      int esx = eFrameX * eFw;
      int esy = 0;
      // Enemy is positioned based on scrollX
      int ex = (int)newEnemies[i].x - (int)scrollX - 40;
      int ey = (int)newEnemies[i].y - 40;
      // Fixed size to match player: 80x80
      int eWidth = 80;
      int eHeight = 80;
      
      // Only draw if roughly on-screen
      if (ex > -100 && ex < (int)1124) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, npc1Spritesheet); // All enemies use NPC.png
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        double etx1 = (double)esx / eTotalW;
        double etx2 = (double)(esx + eFw) / eTotalW;
        double ety1 = -(double)esy / eTotalH;
        double ety2 = -(double)(esy + eFh) / eTotalH;

        glBegin(GL_QUADS);
        if (eFlipX) {
          glTexCoord2f(etx2, ety1); glVertex2f(ex, ey);
          glTexCoord2f(etx1, ety1); glVertex2f(ex + eWidth, ey);
          glTexCoord2f(etx1, ety2); glVertex2f(ex + eWidth, ey + eHeight);
          glTexCoord2f(etx2, ety2); glVertex2f(ex, ey + eHeight);
        } else {
          glTexCoord2f(etx1, ety1); glVertex2f(ex, ey);
          glTexCoord2f(etx2, ety1); glVertex2f(ex + eWidth, ey);
          glTexCoord2f(etx2, ety2); glVertex2f(ex + eWidth, ey + eHeight);
          glTexCoord2f(etx1, ety2); glVertex2f(ex, ey + eHeight);
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);

        // Enemy Health Bar
        iSetColor(100, 100, 100);
        iRectangle(ex + 15, ey + 80, 50, 5);
        iSetColor(220, 20, 20);
        double eHpPercent = (double)newEnemies[i].health / 200.0;
        if (eHpPercent < 0) eHpPercent = 0;
        iFilledRectangle(ex + 16, ey + 81, 48 * eHpPercent, 3);
      }
    }
  }

  // --- Draw Bullets ---
  for (int i = 0; i < MAX_NEW_BULLETS; i++) {
    if (newBullets[i].active) {
      if (newBullets[i].isPlayerBullet) {
        iSetColor(255, 200, 0); // Yellow-orange laser for player
      } else {
        iSetColor(255, 50, 50); // Red laser for enemies
      }
      
      // Calculate bullet screen position based on scrollX
      int bx = (int)newBullets[i].x - (int)scrollX;
      int by = (int)newBullets[i].y;
      
      iFilledRectangle(bx, by, 15, 4); // Thin laser line
    }
  }

  // --- Draw UI (Health Bar) ---
  int hpBarWidth = 200;
  int hpBarHeight = 20;
  int hpX = 20;
  int hpY = 640 - 40;
  
  // Grey background limit
  iSetColor(100, 100, 100);
  iRectangle(hpX, hpY, hpBarWidth, hpBarHeight);
  
  // Red current health
  iSetColor(220, 20, 20);
  double healthPercent = (double)newPlayerHealth / newPlayerMaxHealth;
  if (healthPercent < 0) healthPercent = 0;
  iFilledRectangle(hpX + 1, hpY + 1, (hpBarWidth - 2) * healthPercent, hpBarHeight - 2);

  char hpText[32];
  sprintf_s(hpText, "PLAYER HP: %d / %d", newPlayerHealth, newPlayerMaxHealth);
  iSetColor(255, 255, 255);
  iText(hpX + 5, hpY + 5, hpText, GLUT_BITMAP_HELVETICA_12);
}

inline void updateNewLevel() {
  // Check for game over
  if (newPlayerHealth <= 0) {
    gameState = GAME_OVER;
    return;
  }
  newPlayerMoving = false;

  // Horizontal movement: scroll the world
  if (arrowKeyRight) {
    scrollX += SCROLL_SPEED;
    newPlayerDir = 1;
    newPlayerMoving = true;
  }
  if (arrowKeyLeft) {
    scrollX -= SCROLL_SPEED;
    newPlayerDir = -1;
    newPlayerMoving = true;
  }

  // Wrap scrollX to stay in positive range
  if (scrollX < 0) scrollX += TOTAL_SCROLL_WIDTH;
  if (scrollX >= TOTAL_SCROLL_WIDTH) scrollX -= TOTAL_SCROLL_WIDTH;

  // Jump
  if (arrowKeyUp && newPlayerOnGround) {
    newPlayerVelY = JUMP_FORCE;
    newPlayerOnGround = false;
  }

  // Apply gravity
  if (!newPlayerOnGround) {
    newPlayerVelY -= GRAVITY;
    newPlayerY += newPlayerVelY;
    if (newPlayerY <= GROUND_Y) {
      newPlayerY = GROUND_Y;
      newPlayerVelY = 0;
      newPlayerOnGround = true;
    }
  }

  // Walk animation (cycle through frames when moving)
  if (newPlayerMoving) {
    newPlayerAnimTimer += 0.3; // Speed up animation slightly for 8-frame cycle
    if (newPlayerAnimTimer > 1.0) {
      newPlayerAnimTimer = 0;
      newPlayerFrame = (newPlayerFrame + 1) % 8; // 8-frame cycle
    }
  } else {
    newPlayerAnimTimer = 0;
    newPlayerFrame = 0;
  }

  // Shooting animation timer
  if (newPlayerIsShooting) {
    newPlayerShootTimer -= 0.05; // fixed update tick rate approx
    if (newPlayerShootTimer <= 0) {
      newPlayerIsShooting = false;
    }
  }

  if (newPlayerFireCooldown > 0) {
    newPlayerFireCooldown -= 0.05;
  }

  // Update bullets (all bullet positions are in WORLD space)
  for (int i = 0; i < MAX_NEW_BULLETS; i++) {
    if (newBullets[i].active) {
      newBullets[i].x += newBullets[i].speed * newBullets[i].dir;
      
      // Check collisions - all in world space
      if (newBullets[i].isPlayerBullet) { // Check collision against enemies
        for (int e = 0; e < MAX_NEW_ENEMIES; e++) {
          if (newEnemies[e].active) {
            // Both bullet and enemy are in world space
            double dist = sqrt(pow(newBullets[i].x - newEnemies[e].x, 2) + pow(newBullets[i].y - newEnemies[e].y, 2));
            if (dist < 40) { // hit radius
              newEnemies[e].health -= 25;
              newBullets[i].active = false;
              if (newEnemies[e].health <= 0) {
                newEnemies[e].active = false;
              }
              break;
            }
          }
        }
      } else { // Enemy bullet: Check collision against player
        // Convert player screen position to world space for comparison
        double playerWorldX = newPlayerX + scrollX;
        double dist = sqrt(pow(newBullets[i].x - playerWorldX, 2) + pow(newBullets[i].y - newPlayerY, 2));
           if (dist < 40) { // hit radius
             newPlayerHealth -= 10;
             newBullets[i].active = false;
             if (newPlayerHealth < 0) newPlayerHealth = 0;
           }
      }
      
      // Deactivate if off-screen (world-space bounds relative to current scroll)
      double bulletScreenX = newBullets[i].x - scrollX;
      if (bulletScreenX < -50 || bulletScreenX > 1050) {
        newBullets[i].active = false;
      }
    }
  }

  // Update Enemies (AI and Animation)
  for (int i = 0; i < MAX_NEW_ENEMIES; i++) {
    if (newEnemies[i].active) {
      double playerWorldX = newPlayerX + scrollX;
      double enX = newEnemies[i].x - scrollX; // Enemy position on screen
      double distToPlayerWorldX = playerWorldX - newEnemies[i].x;
      bool playerPassedEnemy = (playerWorldX > newEnemies[i].x + 30);
      
      // Decrease timers
      if (newEnemies[i].fireCooldown > 0) newEnemies[i].fireCooldown -= 0.05;
      if (newEnemies[i].isShooting) {
        newEnemies[i].shootTimer -= 0.05;
        if (newEnemies[i].shootTimer <= 0) newEnemies[i].isShooting = false;
      }

      // If the player has already gone past an enemy, keep chasing even from
      // behind/off-screen so the level stays active.
      if (playerPassedEnemy && distToPlayerWorldX > 80) {
        newEnemies[i].dir = 1;
        newEnemies[i].x += 3.5;
      }

      // Basic AI Logic: visible enemies can still engage normally.
      if (enX > -200 && enX < 1200) {
        if (!playerPassedEnemy) {
          double distToPlayerX = newPlayerX - enX;

          if (abs(distToPlayerX) > 250) { // Walk towards player if far
            newEnemies[i].dir = (distToPlayerX > 0) ? 1 : -1;
            newEnemies[i].x += 4.0 * newEnemies[i].dir;
          } else { // Stop and shoot if in range
            newEnemies[i].dir = (distToPlayerX > 0) ? 1 : -1;
            newEnemies[i].frame = 0;

            if (newEnemies[i].fireCooldown <= 0) {
              for (int b = 0; b < MAX_NEW_BULLETS; b++) {
                if (!newBullets[b].active) {
                  newBullets[b].active = true;
                  newBullets[b].isPlayerBullet = false; // Enemy bullet
                  newBullets[b].x = newEnemies[i].x + (newEnemies[i].dir == 1 ? 40 : -40);
                  newBullets[b].y = newEnemies[i].y + 15;
                  newBullets[b].dir = newEnemies[i].dir;
                  newBullets[b].speed = 5.0;

                  newEnemies[i].isShooting = true;
                  newEnemies[i].shootTimer = 0.2;
                  newEnemies[i].fireCooldown = 1.5;
                  break;
                }
              }
            }
          }
        } else if (distToPlayerWorldX <= 250) {
          newEnemies[i].dir = 1;
          newEnemies[i].frame = 0;

          if (newEnemies[i].fireCooldown <= 0) {
            for (int b = 0; b < MAX_NEW_BULLETS; b++) {
              if (!newBullets[b].active) {
                newBullets[b].active = true;
                newBullets[b].isPlayerBullet = false;
                newBullets[b].x = newEnemies[i].x + 40;
                newBullets[b].y = newEnemies[i].y + 15;
                newBullets[b].dir = 1;
                newBullets[b].speed = 5.0;

                newEnemies[i].isShooting = true;
                newEnemies[i].shootTimer = 0.2;
                newEnemies[i].fireCooldown = 1.5;
                break;
              }
            }
          }
        }
      }
    }
  }

  // Check for Level Completion (Side-Scrolling)
  bool anyAlive = false;
  for (int i = 0; i < MAX_NEW_ENEMIES; i++) {
    if (newEnemies[i].active) {
      anyAlive = true;
      break;
    }
  }
  if (!anyAlive) {
    gameState = PUZZLE_TRANSITION;
    puzzleTransitionTimer = 5.0; // 5-second transition
    puzzleTransitionPhase = 0;
    puzzleTransitionAlpha = 0;
  }
}

inline void mouseNewLevel(int button, int state, int mx, int my) {
  if (newPlayerFireCooldown <= 0) {
    // Find an inactive bullet and fire it
    for (int i = 0; i < MAX_NEW_BULLETS; i++) {
      if (!newBullets[i].active) {
        newBullets[i].active = true;
        newBullets[i].isPlayerBullet = true; // Player fired this bullet
        // Fix: World X = Player Screen X + Current Scroll Offset
        newBullets[i].x = newPlayerX + scrollX + (newPlayerDir == 1 ? 40 : -40);
        newBullets[i].y = newPlayerY + 15; // Gun height
        newBullets[i].dir = newPlayerDir;
        newBullets[i].speed = 6.0; // Reduced laser speed from 10.0 to 6.0

        newPlayerIsShooting = true;
        newPlayerShootTimer = 0.2; // Show shooting pose for 0.2 seconds
        newPlayerFireCooldown = 0.5; // Cooldown limit: Wait 0.5 seconds before firing again
        break;
      }
    }
  }
}

inline void keyboardNewLevel(unsigned char key) {
  if (key == 'A' || key == 'a') arrowKeyLeft = true;
  if (key == 'D' || key == 'd') arrowKeyRight = true;
  if (key == 'W' || key == 'w') arrowKeyUp = true;
}

inline void keyUpNewLevel(unsigned char key) {
  if (key == 'A' || key == 'a') arrowKeyLeft = false;
  if (key == 'D' || key == 'd') arrowKeyRight = false;
  if (key == 'W' || key == 'w') arrowKeyUp = false;
}

inline void specialKeyboardNewLevel(unsigned char key) {
  if (key == GLUT_KEY_LEFT) arrowKeyLeft = true;
  if (key == GLUT_KEY_RIGHT) arrowKeyRight = true;
  if (key == GLUT_KEY_UP) arrowKeyUp = true;
}

inline void specialKeyUpNewLevel(unsigned char key) {
  if (key == GLUT_KEY_LEFT) arrowKeyLeft = false;
  if (key == GLUT_KEY_RIGHT) arrowKeyRight = false;
  if (key == GLUT_KEY_UP) arrowKeyUp = false;
}

#endif // LEVEL1SS_H
