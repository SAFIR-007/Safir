#ifndef BOUNCESHOOTER_H
#define BOUNCESHOOTER_H

#include "GameShared.h"

struct Bullet {
  double x, y;
  double dx, dy;
  bool active;
};

struct Enemy {
  double x, y;
  bool active;
  int health;
  int animFrame;
  double animTimer;
  int dir;           // 1 = right, -1 = left
  double minX, maxX; // patrol bounds
};

struct Player {
  double x, y;
  int health;
  bool isShooting;
  int shootFrame;
  double shootTimer;
  double reloadTimer; // Cooldown between shots
};

extern unsigned int level1BG;
extern unsigned int playerSpritesheet;
extern unsigned int enemySpritesheet;
extern Player player;
extern Bullet bullets[100];
extern Enemy enemies[3];

void initNewLevel();
void startLevel1();
void initEnemies();
void drawLevel1();
void handleLevel1MouseClick(int mx, int my);
void updateLevel1();



inline void initEnemies() {
  // Wall boundaries (for reference):
  // Left wall: x=430 to x=500, y=0 to y=340
  // T-wall vertical: x=700 to x=760, y=0 to ceiling
  // T-wall horizontal bar: x=760 to x=960, top surface at y=240

  // Enemy 1: On the ground, between the walls (Left End 515, T-Wall Start 580)
  enemies[0].x = 475;
  enemies[0].y = 40;
  enemies[0].active = true;
  enemies[0].health = 3;
  enemies[0].animFrame = 0;
  enemies[0].animTimer = 0;
  enemies[0].dir = 1;
  enemies[0].minX = 445;
  enemies[0].maxX = 550;

  // Enemy 2: On the ground, between the walls
  enemies[1].x = 540;
  enemies[1].y = 40;
  enemies[1].active = true;
  enemies[1].health = 3;
  enemies[1].animFrame = 0;
  enemies[1].animTimer = 0;
  enemies[1].dir = -1;
  enemies[1].minX = 445;
  enemies[1].maxX = 550;

  // Enemy 3: On the horizontal bar of the T-shaped wall (Shifted Right)
  // Horizontal bar is x=660 to 860
  enemies[2].x = 740;
  enemies[2].y = 190;
  enemies[2].active = true;
  enemies[2].health = 3;
  enemies[2].animFrame = 0;
  enemies[2].animTimer = 0;
  enemies[2].dir = -1;
  enemies[2].minX = 730;
  enemies[2].maxX = 910;
}

inline void drawLevel1() {
  iShowImage(0, 0, 1024, 640, level1BG);

  // Render Player (Stationary, Facing Right)
  // PC.png has 4 frames in a row, each approx 64x64
  // Total image: 256 x 64 (4 frames of 64x64)
  int fw = 64, fh = 64; // frame width/height
  int totalW = 256;     // total sheet width (4 * 64)
  int totalH = 64;      // total sheet height (1 row)

  // Frame 2 (index 2) = facing right
  int frameIdx = 2;
  if (player.isShooting) {
    frameIdx = 3; // Use frame 3 as the "shooting" pose
  }

  int sx = frameIdx * fw;
  int sy = 0;

  iShowSubImage(player.x - 40, player.y - 40, 80, 80, playerSpritesheet, sx, sy,
                fw, fh, totalW, totalH);

  // Draw Bullets
  iSetColor(255, 255, 0);
  for (int i = 0; i < 100; i++) {
    if (bullets[i].active) {
      iFilledCircle(bullets[i].x, bullets[i].y, 5);
    }
  }

  // Draw Enemies using NPC.png spritesheet
  // NPC.png: 4 frames, each 64x64, total 256x64
  for (int i = 0; i < 3; i++) {
    if (enemies[i].active) {
      int efw = 64, efh = 64;
      int esx = enemies[i].animFrame * efw;
      int esy = 0;
      iShowSubImage(enemies[i].x - 40, enemies[i].y - 40, 80, 80,
                    enemySpritesheet, esx, esy, efw, efh, 256, 64);
      // Health bar (Widened for larger sprite)
      iSetColor(0, 255, 0);
      iFilledRectangle(enemies[i].x - 25, enemies[i].y + 45,
                       enemies[i].health * 17, 6);
    }
  }

  // UI
  iSetColor(255, 255, 255);
  char scoreText[50];
  sprintf_s(scoreText, "HEALTH: %d", player.health);
  iText(20, 600, scoreText, GLUT_BITMAP_HELVETICA_18);
  // iText(20, 570, "PRESS [M] FOR MENU", GLUT_BITMAP_HELVETICA_12);
}

inline void startLevel1() {
  gameState = LEVEL1;
  player.health = 100;
  player.x = 120;
  player.y = 50;
  player.isShooting = false;
  player.reloadTimer = 0;
  initEnemies();
}

inline void handleLevel1MouseClick(int mx, int my) {
  player.isShooting = true;
  player.shootFrame = 0;
  player.shootTimer = 0;

  if (player.reloadTimer <= 0) {
    for (int i = 0; i < 100; i++) {
      if (!bullets[i].active) {
        bullets[i].x = player.x + 20;
        bullets[i].y = player.y + 10;

        double angle = atan2(my - bullets[i].y, mx - bullets[i].x);
        bullets[i].dx = 7 * cos(angle);
        bullets[i].dy = 7 * sin(angle);

        bullets[i].active = true;
        player.reloadTimer = 30.0;
        break;
      }
    }
  }
}

inline void updateLevel1() {
  double playerSpeed = 4.0;
  if (keyPressed['w'] || keyPressed['W']) {
    player.y += playerSpeed;
  }
  if (keyPressed['s'] || keyPressed['S']) {
    player.y -= playerSpeed;
  }
  if (keyPressed['a'] || keyPressed['A']) {
    player.x -= playerSpeed;
  }
  if (keyPressed['d'] || keyPressed['D']) {
    player.x += playerSpeed;
  }

  if (player.x < 0)
    player.x = 0;
  if (player.x > 1004)
    player.x = 1004;
  if (player.y < 0)
    player.y = 0;
  if (player.y > 600)
    player.y = 600;

  if (player.reloadTimer > 0) {
    player.reloadTimer -= 1.0;
  }

  if (player.isShooting) {
    player.shootTimer += 0.2;
    if (player.shootTimer > 1.0) {
      player.shootTimer = 0;
      player.shootFrame++;
      if (player.shootFrame >= 3) {
        player.isShooting = false;
        player.shootFrame = 0;
      }
    }
  }

  for (int i = 0; i < 100; i++) {
    if (bullets[i].active) {
      bullets[i].x += bullets[i].dx;
      bullets[i].y += bullets[i].dy;

      if (bullets[i].x < 0 || bullets[i].x > 1024) {
        bullets[i].active = false;
      }

      if (bullets[i].y >= 600) {
        bullets[i].y = 599;
        bullets[i].dy = -bullets[i].dy;
      }

      if (bullets[i].y <= 0) {
        bullets[i].y = 1;
        bullets[i].dy = -bullets[i].dy;
      }

      if (bullets[i].x >= 300 && bullets[i].x <= 370 && bullets[i].y <= 200) {
        if (bullets[i].dx > 0) {
          bullets[i].dx = -bullets[i].dx;
          bullets[i].x = 299;
        } else if (bullets[i].dx < 0) {
          bullets[i].dx = -bullets[i].dx;
          bullets[i].x = 371;
        }
      }

      if (bullets[i].x >= 610 && bullets[i].x <= 660 && bullets[i].y <= 310) {
        if (bullets[i].dx > 0) {
          bullets[i].dx = -bullets[i].dx;
          bullets[i].x = 609;
        } else if (bullets[i].dx < 0) {
          bullets[i].dx = -bullets[i].dx;
          bullets[i].x = 661;
        }
      }

      if (bullets[i].x >= 660 && bullets[i].x <= 860 && bullets[i].y >= 180 &&
          bullets[i].y <= 200) {
        if (bullets[i].x < 670 && bullets[i].dx > 0) {
          bullets[i].dx = -bullets[i].dx;
          bullets[i].x = 659;
        } else if (bullets[i].x > 850 && bullets[i].dx < 0) {
          bullets[i].dx = -bullets[i].dx;
          bullets[i].x = 861;
        } else {
          bullets[i].dy = -bullets[i].dy;
          if (bullets[i].dy > 0)
            bullets[i].y = 201;
          else
            bullets[i].y = 179;
        }
      }

      for (int j = 0; j < 3; j++) {
        if (enemies[j].active) {
          if (bullets[i].x > enemies[j].x - 20 &&
              bullets[i].x < enemies[j].x + 20 &&
              bullets[i].y > enemies[j].y - 25 &&
              bullets[i].y < enemies[j].y + 25) {
            bullets[i].active = false;
            enemies[j].health--;
            if (enemies[j].health <= 0) {
              enemies[j].active = false;
            }
          }
        }
      }
    }
  }

  for (int i = 0; i < 3; i++) {
    if (enemies[i].active) {
      enemies[i].x += enemies[i].dir * 2;

      if (enemies[i].x >= enemies[i].maxX) {
        enemies[i].dir = -1;
      }
      if (enemies[i].x <= enemies[i].minX) {
        enemies[i].dir = 1;
      }

      enemies[i].animTimer += 0.15;
      if (enemies[i].animTimer > 1.0) {
        enemies[i].animTimer = 0;
        enemies[i].animFrame = (enemies[i].animFrame + 1) % 4;
      }

      if (abs(enemies[i].x - player.x) < 40 &&
          abs(enemies[i].y - player.y) < 50) {
        player.health -= 1;
        if (player.health <= 0) {
          gameState = MENU;
          player.health = 100;
        }
      }
    }
  }

  bool allDead = true;
  for (int i = 0; i < 3; i++) {
    if (enemies[i].active) {
      allDead = false;
      break;
    }
  }
  if (allDead) {
    initNewLevel();
  }
}

#endif
