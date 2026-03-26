#ifndef BOUNCESHOOTER_H
#define BOUNCESHOOTER_H

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

void initEnemies();
void drawLevel1();

#endif
