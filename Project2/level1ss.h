#ifndef LEVEL1SS_H
#define LEVEL1SS_H

// ============================================================
// NEW SIDE-SCROLLING LEVEL DECLARATIONS
// ============================================================

extern unsigned int newLevelBG[3];
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

#endif // LEVEL1SS_H
