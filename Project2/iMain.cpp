#include "iGraphics.h"
#include "level1ss.h"
#include "BounceShooter.h"
#include "Puzzle.h"
#include "Boss1.h"

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
#define NEW_LEVEL 12
#define GAME_OVER 13

int gameState = MENU;
unsigned int bgImage;
unsigned int level1BG;
unsigned int playerSpritesheet;
unsigned int enemySpritesheet;
unsigned int finalBG;
unsigned int digitalRainBG;
unsigned int secondBG;
unsigned int newPlayerSpritesheet;
unsigned int npc1Spritesheet;
unsigned int npc2Spritesheet;

// ============================================================
// NEW SIDE-SCROLLING LEVEL
// ============================================================
unsigned int newLevelBG[3];
double scrollX = 0;               // world scroll offset
double newPlayerX = 200;           // player screen X position
double newPlayerY = 50;            // player Y (ground level)
double newPlayerVelY = 0;          // vertical velocity for jump
bool newPlayerOnGround = true;     // is the player on the ground?
int newPlayerFrame = 0;            // animation frame (0-3)
double newPlayerAnimTimer = 0;     // animation timing
int newPlayerDir = 1;              // 1 = right, -1 = left
bool newPlayerMoving = false;      // is the player walking?
bool newPlayerIsShooting = false;  // is the player currently shooting?
double newPlayerShootTimer = 0;    // timer to revert to idle/walking
double newPlayerFireCooldown = 0;  // cooldown between shots
bool arrowKeyLeft = false;         // arrow key states
bool arrowKeyRight = false;
bool arrowKeyUp = false;

int newPlayerHealth = 100;
int newPlayerMaxHealth = 100;

// Instantiating variables declared in level1ss.h
NewLevelBullet newBullets[MAX_NEW_BULLETS];
NewLevelEnemy newEnemies[MAX_NEW_ENEMIES];





GravityTile grid[4][4];
double surpriseTimer = 5.0;
double puzzleTimer = 30.0;
bool puzzleCleared = false;
double screenShake = 0;
double intelTimer = 5.0;
double warningTimer = 5.0;
double secondBGTimer = 10.0;



Player player = {100, 32, 100, false, 0, 0, 0}; // y=32 means feet on ground
Bullet bullets[100];
Enemy enemies[3]; // Only 3 fixed enemies

// ============================================================
// BOSS LEVEL - "The Sentinel Judge"
// ============================================================

// Player variables
double phase3PlayerX = 100;
double phase3PlayerY = 100;
int phase3PlayerSpeed = 5;
bool phase3CrouchMode = false;
int phase3PlayerHealth = 100;
double phase3NoiseLevel = 0;
bool phase3CriticalStrikeReady = false;
double phase3DodgeTimer = 0;
bool phase3DodgeActive = false;
double phase3DodgeDX = 0, phase3DodgeDY = 0;
double phase3AtkCooldown = 0;

// Decoy variables
bool phase3DecoyActive = false;
double phase3DecoyX = 0;
double phase3DecoyY = 0;
double phase3DecoyTimer = 0;

// Boss variables
double phase3BossX = 512;
double phase3BossY = 520;
int phase3BossHealth = 250;
int phase3AwarenessLevel = 0;


BossState phase3BossState = P3_OBSERVE;
bool strikeWindowActive = false;
double phase3SearchAngle = -1.5708;
double phase3SearchDir = 1.0;
double phase3BossLastSeenX = 512;
double phase3BossLastSeenY = 320;
double phase3BossMoveTimer = 0;

Phase3Attack phase3BossAttack = P3_ATK_NONE;
double phase3BossAtkTimer = 0;
double phase3StrikeTimer = 0;
double phase3ShockRadius = 0;
bool phase3ShockActive = false;
double phase3BossVictoryTimer = 0;
double phase3BossChargeX = 0;
double phase3BossChargeY = 0;
int phase3SealsActivated = 0;

// Shadow zones

P3Shadow phase3Shadows[6];

// Pillars

P3Pillar phase3Pillars[4];

// Seals

P3Seal phase3Seals[3];

// Key state tracking - use the framework's keyPressed[] for reliability
bool keyStates[256] = {false};
bool p3IsKeyPressed(unsigned char k) { return keyPressed[k] || keyStates[k]; }


void iShowSubImage(int x, int y, int width, int height, unsigned int texture,
                   int sx, int sy, int sw, int sh, int tw, int th) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  // iGraphics uses negative Y for texture coords
  // Full image: (0,0) -> (1,0) -> (1,-1) -> (0,-1)
  double tx1 = (double)sx / tw;
  double tx2 = (double)(sx + sw) / tw;
  double ty1 = -(double)sy / th;
  double ty2 = -(double)(sy + sh) / th;

  glBegin(GL_QUADS);
  glTexCoord2f(tx1, ty1);
  glVertex2f(x, y);
  glTexCoord2f(tx2, ty1);
  glVertex2f(x + width, y);
  glTexCoord2f(tx2, ty2);
  glVertex2f(x + width, y + height);
  glTexCoord2f(tx1, ty2);
  glVertex2f(x, y + height);
  glEnd();

  glDisable(GL_TEXTURE_2D);
}

void initEnemies() {
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

// Helper: check if a port is open for a given tile
bool isTilePortOpen(int row, int col, int port) {
  int type = grid[row][col].type;
  int rot = grid[row][col].rotation;
  if (type == 0)
    return (port % 2 != rot % 2);
  if (type == 1) {
    if (rot == 0)
      return (port == 0 || port == 1);
    if (rot == 1)
      return (port == 1 || port == 2);
    if (rot == 2)
      return (port == 2 || port == 3);
    if (rot == 3)
      return (port == 3 || port == 0);
  }
  if (type == 2)
    return true;
  return false;
}

// Helper: check if the grid is already solved (path from (0,0) to (3,3))
bool isPuzzleAlreadySolved() {
  bool visited[4][4] = {false};
  int stackR[16], stackC[16], top = -1;
  stackR[++top] = 0;
  stackC[top] = 0;
  visited[0][0] = true;

  int dr[] = {-1, 1, 0, 0};
  int dc[] = {0, 0, -1, 1};
  int p[] = {0, 2, 3, 1};
  int opp[] = {2, 0, 1, 3};

  while (top >= 0) {
    int r = stackR[top], c = stackC[top--];
    if (r == 3 && c == 3)
      return true;

    for (int i = 0; i < 4; i++) {
      int nr = r + dr[i], nc = c + dc[i];
      if (nr >= 0 && nr < 4 && nc >= 0 && nc < 4 && !visited[nr][nc]) {
        if (isTilePortOpen(r, c, p[i]) && isTilePortOpen(nr, nc, opp[i])) {
          visited[nr][nc] = true;
          stackR[++top] = nr;
          stackC[top] = nc;
        }
      }
    }
  }
  return false;
}

void initPuzzle() {
  srand(time(NULL));
  // Randomize tile types once
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      grid[i][j].type = rand() % 3; // 0, 1, 2
      grid[i][j].rotation = rand() % 4;
      grid[i][j].isActive = false;
    }
  }

  // Re-randomize rotations until the puzzle is NOT already solved
  int maxAttempts = 100; // Safety limit to avoid infinite loop
  while (isPuzzleAlreadySolved() && maxAttempts-- > 0) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        grid[i][j].rotation = rand() % 4;
      }
    }
  }

  puzzleTimer = 30.0;
  puzzleCleared = false;
}

void drawLevel1() {
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

void drawMenu() {
  iShowImage(0, 0, 1024, 640, bgImage);

  // Title: WRATH OF GOD
  // Vertical red bar
  iSetColor(180, 0, 0);
  iFilledRectangle(100, 360, 4, 100);

  // Main Title Text
  iSetColor(50, 0, 0); // Shadow
  iText(132, 408, "WRATH OF GOD", GLUT_BITMAP_TIMES_ROMAN_24);
  iSetColor(180, 0, 0); // Front
  iText(130, 410, "WRATH OF GOD", GLUT_BITMAP_TIMES_ROMAN_24);

  // Subtitle
  iSetColor(160, 160, 160);
  iText(130, 380, "SILENT PULSES, SHIFTING EYES", GLUT_BITMAP_HELVETICA_12);

  // Menu Buttons
  // Button 1: START CAMPAIGN
  if (iMouseX >= 98 && iMouseX <= 248 && iMouseY >= 300 && iMouseY <= 340) {
    iSetColor(220, 0, 0); // Lighter red on hover
    iFilledRectangle(98, 300, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(180, 0, 0);
    iFilledRectangle(98, 300, 150, 40);
    iSetColor(255, 255, 255);
  }
  iText(120, 315, "START CAMPAIGN", GLUT_BITMAP_HELVETICA_12);

  // Button 2: OPTIONS
  if (iMouseX >= 98 && iMouseX <= 248 && iMouseY >= 250 && iMouseY <= 290) {
    iSetColor(60, 60, 60); // Lighter grey on hover
    iFilledRectangle(98, 250, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(30, 30, 30);
    iFilledRectangle(98, 250, 150, 40);
    iSetColor(160, 160, 160);
  }
  iText(120, 265, "OPTIONS", GLUT_BITMAP_HELVETICA_12);

  // Button 3: CREDITS
  if (iMouseX >= 98 && iMouseX <= 248 && iMouseY >= 200 && iMouseY <= 240) {
    iSetColor(60, 60, 60); // Lighter grey on hover
    iFilledRectangle(98, 200, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(30, 30, 30);
    iFilledRectangle(98, 200, 150, 40);
    iSetColor(160, 160, 160);
  }
  iText(120, 215, "CREDITS", GLUT_BITMAP_HELVETICA_12);

  // Button 4: EXIT
  if (iMouseX >= 98 && iMouseX <= 248 && iMouseY >= 150 && iMouseY <= 190) {
    iSetColor(100, 0, 0); // Darker red on hover
    iFilledRectangle(98, 150, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(30, 30, 30);
    iFilledRectangle(98, 150, 150, 40);
    iSetColor(160, 160, 160);
  }
  iText(120, 165, "EXIT", GLUT_BITMAP_HELVETICA_12);

  // Decorative Brackets (Top Left)
  iSetColor(180, 0, 0);
  iLine(30, 610, 50, 610);
  iLine(30, 610, 30, 590);

  // Decorative Brackets (Bottom Left)
  iLine(30, 30, 50, 30);
  iLine(30, 30, 30, 50);

  // Info text (Bottom Right)
  iSetColor(80, 80, 80);
  iText(850, 60, "ALPHA BUILD V0.9.4", GLUT_BITMAP_8_BY_13);
  iText(840, 40, "ONLINE SERVICES: ACTIVE", GLUT_BITMAP_8_BY_13);
  iText(880, 20, "REGION: EU-WEST", GLUT_BITMAP_8_BY_13);
}

void drawCredits() {
  iShowImage(0, 0, 1024, 640, bgImage);

  // Dim background for readability
  iSetColor(0, 0, 0);
  iSetColor(0, 0, 0);
  iFilledRectangle(0, 0, 1024, 640);

  // Credits Title
  iSetColor(180, 0, 0);
  iText(400, 500, "DEVELOPMENT TEAM", GLUT_BITMAP_TIMES_ROMAN_24);

  // Team Members
  iSetColor(255, 255, 255);

  // Member 1
  iText(350, 400, "SAFIR RAHMAN", GLUT_BITMAP_HELVETICA_18);
  iSetColor(160, 160, 160);
  iText(350, 375, "00724205101010", GLUT_BITMAP_HELVETICA_12);

  // Member 2
  iSetColor(255, 255, 255);
  iText(350, 300, "TASEEN EHFAAZ", GLUT_BITMAP_HELVETICA_18);
  iSetColor(160, 160, 160);
  iText(350, 275, "00724205101016", GLUT_BITMAP_HELVETICA_12);

  // Member 3
  iSetColor(255, 255, 255);
  iText(350, 200, "RAISA JAHAN", GLUT_BITMAP_HELVETICA_18);
  iSetColor(160, 160, 160);
  iText(350, 175, "00724205101017", GLUT_BITMAP_HELVETICA_12);

  // Back Button Instruction
  iSetColor(180, 0, 0);
  iText(100, 50, "PRESS [ESC] OR CLICK TO RETURN", GLUT_BITMAP_HELVETICA_12);
}

void drawOptions() {
  iShowImage(0, 0, 1024, 640, bgImage);

  // Dim background for readability
  iSetColor(0, 0, 0);
  iFilledRectangle(0, 0, 1024, 640);

  // Title
  iSetColor(180, 0, 0);
  iText(380, 500, "HOW TO PLAY", GLUT_BITMAP_TIMES_ROMAN_24);

  // Instructions
  iSetColor(255, 255, 255);
  iText(350, 400, "- AIM: MOVE MOUSE", GLUT_BITMAP_HELVETICA_18);
  iText(350, 350, "- SHOOT: LEFT CLICK", GLUT_BITMAP_HELVETICA_18);
  iText(350, 300, "- ENEMIES: DEFEAT TO CLEAR STAGE", GLUT_BITMAP_HELVETICA_18);
  iText(350, 250, "- WALLS: BULLETS REFLECT OFF STONE",
        GLUT_BITMAP_HELVETICA_18);

  // Back Button Instruction
  iSetColor(180, 0, 0);
  iText(100, 50, "PRESS [ESC] OR CLICK TO RETURN", GLUT_BITMAP_HELVETICA_12);
}

void drawWinScreen() {
  iShowImage(0, 0, 1024, 640, level1BG);

  // Dim background
  iSetColor(0, 0, 0);
  iFilledRectangle(0, 0, 1024, 640);

  iSetColor(255, 255, 255);
  iText(400, 350, "LEVEL CLEAR!", GLUT_BITMAP_TIMES_ROMAN_24);
  iSetColor(180, 0, 0);
  iText(380, 300, "CLICK ANYWHERE TO CONTINUE", GLUT_BITMAP_HELVETICA_18);
}

void drawFinalBG() { iShowImage(0, 0, 1024, 640, finalBG); }

void drawWarning() {
  // Black background
  iSetColor(0, 0, 0);
  iFilledRectangle(0, 0, 1024, 640);

  // Ominous red border
  iSetColor(180, 0, 0);
  iRectangle(40, 40, 944, 560);
  iRectangle(42, 42, 940, 556);

  // Warning header
  iSetColor(255, 0, 0);
  iText(380, 480, "!! WARNING !!", GLUT_BITMAP_TIMES_ROMAN_24);

  // Warning text lines
  iSetColor(255, 80, 80);
  iText(250, 400, "The real surprise is still there...",
        GLUT_BITMAP_HELVETICA_18);

  iSetColor(255, 50, 50);
  iText(250, 350, "The Sentinel Judge has awakened...",
        GLUT_BITMAP_HELVETICA_18);

  iSetColor(255, 0, 0);
  iText(300, 290, "You are into the trap.", GLUT_BITMAP_HELVETICA_18);

  // Countdown number
  int countdown = (int)ceil(warningTimer);
  if (countdown < 0)
    countdown = 0;
  char buf[10];
  sprintf_s(buf, "%d", countdown);
  iSetColor(255, 255, 255);
  iText(500, 200, buf, GLUT_BITMAP_TIMES_ROMAN_24);

  // Progress bar
  iSetColor(180, 0, 0);
  iFilledRectangle(312, 160, (int)(400 * (warningTimer / 5.0)), 8);
}

void drawSecondBG() { iShowImage(0, 0, 1024, 640, secondBG); }

// ============================================================
// BOSS LEVEL FUNCTIONS
// ============================================================

void p3InitPhase3() {
  phase3BossX = 512;
  phase3BossY = 520;
  phase3BossHealth = 250;
  phase3AwarenessLevel = 0;
  phase3BossState = P3_OBSERVE;
  strikeWindowActive = false;
  phase3SearchAngle = -1.5708; // -PI/2
  phase3SearchDir = 1.0;
  phase3BossLastSeenX = 512;
  phase3BossLastSeenY = 320;
  phase3BossMoveTimer = 0;
  phase3BossAttack = P3_ATK_NONE;
  phase3BossAtkTimer = 0;
  phase3StrikeTimer = 0;
  phase3ShockRadius = 0;
  phase3ShockActive = false;
  phase3BossVictoryTimer = 0;
  phase3BossChargeX = 0;
  phase3BossChargeY = 0;

  phase3PlayerX = 100;
  phase3PlayerY = 100;
  phase3PlayerHealth = 100;
  phase3CrouchMode = false;
  phase3NoiseLevel = 0;
  phase3CriticalStrikeReady = false;
  phase3DodgeTimer = 0;
  phase3DodgeActive = false;
  phase3AtkCooldown = 0;

  phase3DecoyActive = false;
  phase3DecoyTimer = 0;
  phase3SealsActivated = 0;

  phase3Shadows[0] = {50, 50, 160, 100};
  phase3Shadows[1] = {300, 200, 120, 80};
  phase3Shadows[2] = {620, 80, 140, 90};
  phase3Shadows[3] = {820, 300, 150, 100};
  phase3Shadows[4] = {140, 350, 100, 80};
  phase3Shadows[5] = {480, 300, 100, 70};

  phase3Pillars[0] = {250, 0, 30, 250};
  phase3Pillars[1] = {500, 0, 30, 180};
  phase3Pillars[2] = {700, 0, 30, 280};
  phase3Pillars[3] = {400, 300, 30, 150};

  phase3Seals[0] = {180, 80, false};
  phase3Seals[1] = {750, 150, false};
  phase3Seals[2] = {900, 400, false};
}

bool phase3PlayerInShadow() {
  for (int i = 0; i < 6; i++) {
    if (phase3PlayerX >= phase3Shadows[i].x &&
        phase3PlayerX <= phase3Shadows[i].x + phase3Shadows[i].w &&
        phase3PlayerY >= phase3Shadows[i].y &&
        phase3PlayerY <= phase3Shadows[i].y + phase3Shadows[i].h) {
      return true;
    }
  }
  return false;
}

bool phase3PlayerInSearchlight() {
  double dx = phase3PlayerX - phase3BossX;
  double dy = phase3PlayerY - phase3BossY;
  double dist = sqrt(dx * dx + dy * dy);
  if (dist > 350)
    return false;
  double angleToPlayer = atan2(dy, dx);
  double diff = angleToPlayer - phase3SearchAngle;
  while (diff > 3.14159)
    diff -= 6.28318;
  while (diff < -3.14159)
    diff += 6.28318;
  return (diff > -0.35 && diff < 0.35);
}

void drawPhase3Arena() {
  iSetColor(25, 25, 30);
  iFilledRectangle(0, 0, 1024, 640);
  iSetColor(40, 38, 35);
  iFilledRectangle(0, 0, 1024, 460);
  iSetColor(50, 48, 45);
  for (int i = 0; i < 1024; i += 64)
    iLine(i, 0, i, 460);
  for (int j = 0; j < 460; j += 64)
    iLine(0, j, 1024, j);

  for (int i = 0; i < 6; i++) {
    iSetColor(10, 10, 15);
    iFilledRectangle(phase3Shadows[i].x, phase3Shadows[i].y, phase3Shadows[i].w,
                     phase3Shadows[i].h);
    iSetColor(20, 20, 30);
    iRectangle(phase3Shadows[i].x, phase3Shadows[i].y, phase3Shadows[i].w,
               phase3Shadows[i].h);
  }
  for (int i = 0; i < 4; i++) {
    iSetColor(70, 65, 55);
    iFilledRectangle(phase3Pillars[i].x, phase3Pillars[i].y, phase3Pillars[i].w,
                     phase3Pillars[i].h);
    iSetColor(90, 85, 75);
    iRectangle(phase3Pillars[i].x, phase3Pillars[i].y, phase3Pillars[i].w,
               phase3Pillars[i].h);
  }
  iSetColor(60, 55, 50);
  iFilledRectangle(362, 470, 300, 25);
  iSetColor(80, 75, 70);
  iRectangle(362, 470, 300, 25);

  for (int i = 0; i < 3; i++) {
    if (phase3Seals[i].activated)
      iSetColor(0, 180, 0);
    else
      iSetColor(0, 200, 255);
    iFilledCircle(phase3Seals[i].x, phase3Seals[i].y, 12);
    iSetColor(255, 255, 255);
    iCircle(phase3Seals[i].x, phase3Seals[i].y, 14);
    char sb[10];
    sprintf_s(sb, "S%d", i + 1);
    iText(phase3Seals[i].x - 6, phase3Seals[i].y - 4, sb,
          GLUT_BITMAP_HELVETICA_12);
  }
  if (phase3DecoyActive) {
    iSetColor(255, 200, 0);
    iFilledCircle(phase3DecoyX, phase3DecoyY, 8);
    iSetColor(255, 255, 100);
    iCircle(phase3DecoyX, phase3DecoyY, 12);
  }
}

void drawPhase3Boss() {
  if (phase3BossState == P3_OBSERVE || phase3BossState == P3_HUNT) {
    double range = 350;
    int seg = 20;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 0.6f, 0.08f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f((float)phase3BossX, (float)phase3BossY);
    for (int i = 0; i <= seg; i++) {
      double a = phase3SearchAngle - 0.35 + (0.7 * i / seg);
      glVertex2f((float)(phase3BossX + range * cos(a)),
                 (float)(phase3BossY + range * sin(a)));
    }
    glEnd();
    glDisable(GL_BLEND);
    iSetColor(255, 255, 100);
    iLine(phase3BossX, phase3BossY,
          phase3BossX + range * cos(phase3SearchAngle - 0.35),
          phase3BossY + range * sin(phase3SearchAngle - 0.35));
    iLine(phase3BossX, phase3BossY,
          phase3BossX + range * cos(phase3SearchAngle + 0.35),
          phase3BossY + range * sin(phase3SearchAngle + 0.35));
  }
  if (phase3ShockActive) {
    iSetColor(255, 50, 50);
    iCircle(phase3BossX, phase3BossY, phase3ShockRadius);
    if (phase3ShockRadius > 20)
      iCircle(phase3BossX, phase3BossY, phase3ShockRadius - 10);
  }
  if (phase3BossState == P3_DEAD)
    iSetColor(80, 30, 30);
  else if (strikeWindowActive)
    iSetColor(255, 150, 50);
  else
    iSetColor(200, 30, 30);
  iFilledCircle(phase3BossX, phase3BossY, 30);
  double eyeA = phase3SearchAngle;
  if (phase3BossState >= P3_HUNT)
    eyeA = atan2(phase3PlayerY - phase3BossY, phase3PlayerX - phase3BossX);
  iSetColor(255, 255, 0);
  iFilledCircle(phase3BossX + 12 * cos(eyeA + 0.4),
                phase3BossY + 12 * sin(eyeA + 0.4), 4);
  iFilledCircle(phase3BossX + 12 * cos(eyeA - 0.4),
                phase3BossY + 12 * sin(eyeA - 0.4), 4);
  if (phase3BossState != P3_DEAD) {
    iSetColor(255, 80, 80);
    iText(phase3BossX - 45, phase3BossY + 38, "SENTINEL JUDGE",
          GLUT_BITMAP_HELVETICA_12);
  }
  if (phase3BossAttack == P3_ATK_HEAVY && phase3BossAtkTimer > 0) {
    iSetColor(255, 0, 0);
    iCircle(phase3BossChargeX, phase3BossChargeY, 40);
  }
}

void drawPhase3Player() {
  if (phase3PlayerHealth <= 0)
    return;
  double r = phase3CrouchMode ? 10 : 15;
  if (phase3DodgeActive)
    iSetColor(100, 255, 100);
  else if (phase3CrouchMode)
    iSetColor(30, 150, 30);
  else
    iSetColor(50, 220, 50);
  iFilledCircle(phase3PlayerX, phase3PlayerY, r);
  iSetColor(100, 255, 100);
  iCircle(phase3PlayerX, phase3PlayerY, r + 1);
  if (phase3CrouchMode) {
    iSetColor(200, 200, 200);
    iText(phase3PlayerX - 15, phase3PlayerY + 18, "CROUCH",
          GLUT_BITMAP_HELVETICA_12);
  }
  if (phase3CriticalStrikeReady) {
    iSetColor(255, 255, 0);
    iCircle(phase3PlayerX, phase3PlayerY, r + 5);
  }
}

void drawPhase3HUD() {
  char buf[80];
  iSetColor(60, 60, 60);
  iFilledRectangle(312, 610, 400, 16);
  if (phase3AwarenessLevel < 50)
    iSetColor(0, 200, 255);
  else if (phase3AwarenessLevel < 80)
    iSetColor(255, 200, 0);
  else
    iSetColor(255, 50, 50);
  iFilledRectangle(312, 610, (int)(400.0 * phase3AwarenessLevel / 100), 16);
  iSetColor(255, 255, 255);
  sprintf_s(buf, "AWARENESS: %d%%", phase3AwarenessLevel);
  iText(440, 613, buf, GLUT_BITMAP_HELVETICA_12);

  iSetColor(60, 60, 60);
  iFilledRectangle(20, 610, 250, 16);
  iSetColor(200, 30, 30);
  int bw = (int)(250.0 * phase3BossHealth / 250);
  if (bw < 0)
    bw = 0;
  iFilledRectangle(20, 610, bw, 16);
  iSetColor(255, 255, 255);
  sprintf_s(buf, "BOSS: %d/250", phase3BossHealth);
  iText(80, 613, buf, GLUT_BITMAP_HELVETICA_12);

  iSetColor(60, 60, 60);
  iFilledRectangle(20, 10, 200, 14);
  iSetColor(50, 220, 50);
  int pw = (int)(200.0 * phase3PlayerHealth / 100);
  if (pw < 0)
    pw = 0;
  iFilledRectangle(20, 10, pw, 14);
  iSetColor(255, 255, 255);
  sprintf_s(buf, "HP: %d/100", phase3PlayerHealth);
  iText(70, 12, buf, GLUT_BITMAP_HELVETICA_12);

  iSetColor(200, 200, 200);
  if (phase3BossState == P3_OBSERVE)
    iText(750, 613, "STATE: OBSERVE", GLUT_BITMAP_HELVETICA_12);
  else if (phase3BossState == P3_HUNT)
    iText(750, 613, "STATE: HUNT", GLUT_BITMAP_HELVETICA_12);
  else if (phase3BossState == P3_EXECUTE)
    iText(750, 613, "STATE: EXECUTE!", GLUT_BITMAP_HELVETICA_12);
  else if (phase3BossState == P3_DUEL)
    iText(750, 613, "STATE: DUEL", GLUT_BITMAP_HELVETICA_12);
  else if (phase3BossState == P3_DEAD)
    iText(750, 613, "DEFEATED", GLUT_BITMAP_HELVETICA_12);

  if (strikeWindowActive) {
    iSetColor(255, 255, 0);
    iText(420, 580, ">>> STRIKE NOW! <<<", GLUT_BITMAP_HELVETICA_18);
  }
  sprintf_s(buf, "SEALS: %d/3", phase3SealsActivated);
  iSetColor(0, 200, 255);
  iText(20, 590, buf, GLUT_BITMAP_HELVETICA_12);
  iSetColor(100, 100, 100);
  iText(20, 30, "WASD:Move C:Crouch SPACE:Dodge CLICK:Attack F:Decoy E:Seal",
        GLUT_BITMAP_HELVETICA_12);

  if (phase3BossState == P3_DEAD) {
    iSetColor(255, 255, 0);
    iText(350, 360, "THE SENTINEL JUDGE HAS FALLEN!",
          GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(200, 200, 200);
    iText(410, 310, "Returning to menu...", GLUT_BITMAP_HELVETICA_12);
  }
  if (phase3PlayerHealth <= 0) {
    iSetColor(255, 0, 0);
    iText(450, 360, "YOU DIED", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(200, 200, 200);
    iText(410, 310, "Returning to menu...", GLUT_BITMAP_HELVETICA_12);
  }
}

void handlePhase3Stealth() {
  if (phase3BossState != P3_OBSERVE)
    return;
  bool inLight = phase3PlayerInSearchlight();
  bool inShadow = phase3PlayerInShadow();
  if (inLight && !inShadow && !phase3CrouchMode) {
    phase3AwarenessLevel += 1;
    phase3BossLastSeenX = phase3PlayerX;
    phase3BossLastSeenY = phase3PlayerY;
  } else if (inLight && !inShadow && phase3CrouchMode) {
    // Crouching in light raises awareness very slowly
  } else {
    // Player is hidden: awareness decays naturally
    phase3AwarenessLevel -= 1;
  }
  if (phase3NoiseLevel > 50 && !inShadow)
    phase3AwarenessLevel += 1;
  if (phase3AwarenessLevel > 100)
    phase3AwarenessLevel = 100;
  if (phase3AwarenessLevel < 0)
    phase3AwarenessLevel = 0;
}

void handlePhase3Combat() {
  if (phase3AtkCooldown > 0)
    phase3AtkCooldown -= 0.016;
  if (phase3DecoyActive) {
    phase3DecoyTimer -= 0.016;
    if (phase3DecoyTimer <= 0)
      phase3DecoyActive = false;
  }
  if (phase3BossState != P3_DUEL && phase3BossState != P3_EXECUTE)
    return;
  if (strikeWindowActive) {
    phase3StrikeTimer -= 0.016;
    if (phase3StrikeTimer <= 0) {
      strikeWindowActive = false;
      phase3BossAtkTimer = 2.0;
    }
    return;
  }
  phase3BossAtkTimer -= 0.016;
  if (phase3BossAtkTimer <= 0 && phase3BossAttack == P3_ATK_NONE) {
    int atk = rand() % 3;
    if (atk == 0)
      phase3BossAttack = P3_ATK_HEAVY;
    else if (atk == 1)
      phase3BossAttack = P3_ATK_FEINT;
    else
      phase3BossAttack = P3_ATK_SHOCKWAVE;
    phase3BossAtkTimer = 2.5;
    if (phase3BossAttack == P3_ATK_HEAVY) {
      phase3BossChargeX = phase3PlayerX;
      phase3BossChargeY = phase3PlayerY;
    }
    if (phase3BossAttack == P3_ATK_SHOCKWAVE) {
      phase3ShockActive = true;
      phase3ShockRadius = 0;
    }
  }
  if (phase3BossAttack == P3_ATK_HEAVY && phase3BossAtkTimer < 1.5 &&
      phase3BossAtkTimer > 0.8) {
    double dx = phase3PlayerX - phase3BossChargeX;
    double dy = phase3PlayerY - phase3BossChargeY;
    if (sqrt(dx * dx + dy * dy) < 50 && !phase3DodgeActive) {
      phase3PlayerHealth -= 20;
      phase3BossAttack = P3_ATK_NONE;
    }
  }
  if (phase3BossAttack == P3_ATK_FEINT && phase3BossAtkTimer < 1.8 &&
      phase3BossAtkTimer > 1.2) {
    double dx = phase3PlayerX - phase3BossX;
    double dy = phase3PlayerY - phase3BossY;
    if (sqrt(dx * dx + dy * dy) < 65 && !phase3DodgeActive) {
      phase3PlayerHealth -= 12;
      phase3BossAttack = P3_ATK_NONE;
    }
  }
  if (phase3BossAttack == P3_ATK_SHOCKWAVE && phase3ShockActive) {
    phase3ShockRadius += 4;
    double dx = phase3PlayerX - phase3BossX;
    double dy = phase3PlayerY - phase3BossY;
    double dist = sqrt(dx * dx + dy * dy);
    if (dist < phase3ShockRadius + 20 && dist > phase3ShockRadius - 20 &&
        !phase3DodgeActive) {
      phase3PlayerHealth -= 8;
      phase3ShockActive = false;
    }
    if (phase3ShockRadius > 400)
      phase3ShockActive = false;
  }
  if (phase3BossAtkTimer <= 0 && phase3BossAttack != P3_ATK_NONE) {
    strikeWindowActive = true;
    phase3StrikeTimer = 3.0;
    phase3BossAttack = P3_ATK_NONE;
    phase3ShockActive = false;
  }
}

void updatePhase3Player() {
  if (phase3PlayerHealth <= 0)
    return;
  if (phase3DodgeActive) {
    phase3PlayerX += phase3DodgeDX * 8;
    phase3PlayerY += phase3DodgeDY * 8;
    phase3DodgeTimer -= 0.016;
    if (phase3DodgeTimer <= 0)
      phase3DodgeActive = false;
  } else {
    double speed = phase3CrouchMode ? 2.0 : 4.0;
    double moved = 0;
    if (p3IsKeyPressed('w') || p3IsKeyPressed('W')) {
      phase3PlayerY += speed;
      moved = 1;
    }
    if (p3IsKeyPressed('s') || p3IsKeyPressed('S')) {
      phase3PlayerY -= speed;
      moved = 1;
    }
    if (p3IsKeyPressed('a') || p3IsKeyPressed('A')) {
      phase3PlayerX -= speed;
      moved = 1;
    }
    if (p3IsKeyPressed('d') || p3IsKeyPressed('D')) {
      phase3PlayerX += speed;
      moved = 1;
    }
    if (moved > 0)
      phase3NoiseLevel += phase3CrouchMode ? 0.5 : 2.0;
    else
      phase3NoiseLevel -= 1.0;
    if (phase3NoiseLevel < 0)
      phase3NoiseLevel = 0;
    if (phase3NoiseLevel > 100)
      phase3NoiseLevel = 100;
  }
  if (phase3PlayerX < 15)
    phase3PlayerX = 15;
  if (phase3PlayerX > 1009)
    phase3PlayerX = 1009;
  if (phase3PlayerY < 15)
    phase3PlayerY = 15;
  if (phase3PlayerY > 455)
    phase3PlayerY = 455;

  for (int i = 0; i < 4; i++) {
    double px = phase3Pillars[i].x, py = phase3Pillars[i].y;
    double pw = phase3Pillars[i].w, ph = phase3Pillars[i].h;
    if (phase3PlayerX > px && phase3PlayerX < px + pw && phase3PlayerY > py &&
        phase3PlayerY < py + ph) {
      double dL = phase3PlayerX - px, dR = (px + pw) - phase3PlayerX;
      double dB = phase3PlayerY - py, dT = (py + ph) - phase3PlayerY;
      double m = dL;
      int s = 0;
      if (dR < m) {
        m = dR;
        s = 1;
      }
      if (dB < m) {
        m = dB;
        s = 2;
      }
      if (dT < m) {
        m = dT;
        s = 3;
      }
      if (s == 0)
        phase3PlayerX = px - 1;
      else if (s == 1)
        phase3PlayerX = px + pw + 1;
      else if (s == 2)
        phase3PlayerY = py - 1;
      else
        phase3PlayerY = py + ph + 1;
    }
  }
  if (phase3BossState == P3_HUNT) {
    double dx = phase3PlayerX - phase3BossX, dy = phase3PlayerY - phase3BossY;
    double dist = sqrt(dx * dx + dy * dy);
    double ap = atan2(dy, dx);
    double ba = atan2(phase3BossLastSeenY - phase3BossY,
                      phase3BossLastSeenX - phase3BossX);
    double ad = ap - ba;
    while (ad > 3.14159)
      ad -= 3.28318;
    while (ad < -3.14159)
      ad += 3.28318;
    phase3CriticalStrikeReady = (dist < 120 && (ad > 1.2 || ad < -1.2));
  } else
    phase3CriticalStrikeReady = false;

  if (phase3PlayerHealth <= 0) {
    phase3PlayerHealth = 0;
    phase3BossVictoryTimer = 3.0;
  }
}

void updatePhase3Boss() {
  if (phase3BossState == P3_DEAD) {
    phase3BossVictoryTimer -= 0.016;
    if (phase3BossVictoryTimer <= 0) {
      gameState = MENU; // Transition to Menu
    }
    return;
  }
  if (phase3PlayerHealth <= 0) {
    phase3BossVictoryTimer -= 0.016;
    if (phase3BossVictoryTimer <= 0)
      gameState = MENU;
    return;
  }
  if (phase3BossState == P3_OBSERVE) {
    phase3SearchAngle += 0.02 * phase3SearchDir;
    if (phase3SearchAngle > -0.785)
      phase3SearchDir = -1;
    if (phase3SearchAngle < -2.356)
      phase3SearchDir = 1;
    if (phase3SealsActivated >= 3 || phase3AwarenessLevel >= 100) {
      phase3BossState = P3_HUNT;
      phase3BossAtkTimer = 3.0;
    }
  } else if (phase3BossState == P3_HUNT) {
    double ta = atan2(phase3BossLastSeenY - phase3BossY,
                      phase3BossLastSeenX - phase3BossX);
    double diff = ta - phase3SearchAngle;
    while (diff > 3.14159)
      diff -= 6.28318;
    while (diff < -3.14159)
      diff += 6.28318;
    phase3SearchAngle += diff * 0.05;
    double tX = phase3DecoyActive ? phase3DecoyX : phase3BossLastSeenX;
    double tY = phase3DecoyActive ? phase3DecoyY : phase3BossLastSeenY;
    double dx = tX - phase3BossX, dy = tY - phase3BossY;
    double dist = sqrt(dx * dx + dy * dy);
    if (dist > 5) {
      phase3BossX += (dx / dist) * 2.5;
      phase3BossY += (dy / dist) * 2.5;
    }
    if (sqrt(pow(phase3PlayerX - phase3BossX, 2) +
             pow(phase3PlayerY - phase3BossY, 2)) < 80) {
      phase3BossLastSeenX = phase3PlayerX;
      phase3BossLastSeenY = phase3PlayerY;
      phase3AwarenessLevel += 1;
    }
    if (phase3BossX < 30)
      phase3BossX = 30;
    if (phase3BossX > 994)
      phase3BossX = 994;
    if (phase3BossY < 30)
      phase3BossY = 30;
    if (phase3BossY > 600)
      phase3BossY = 600;
    if (phase3BossHealth <= 125) {
      phase3BossState = P3_DUEL;
      phase3BossAtkTimer = 2.0;
      strikeWindowActive = false;
    }
    if (phase3AwarenessLevel >= 100 && phase3BossState == P3_HUNT) {
      phase3BossState = P3_EXECUTE;
      phase3BossAtkTimer = 1.5;
    }
  } else if (phase3BossState == P3_EXECUTE) {
    double dx = phase3PlayerX - phase3BossX, dy = phase3PlayerY - phase3BossY;
    double dist = sqrt(dx * dx + dy * dy);
    if (dist > 5) {
      phase3BossX += (dx / dist) * 4.0;
      phase3BossY += (dy / dist) * 4.0;
    }
    phase3SearchAngle = atan2(dy, dx);
    if (dist < 40 && !phase3DodgeActive)
      phase3PlayerHealth -= 1;
    if (phase3BossX < 30)
      phase3BossX = 30;
    if (phase3BossX > 994)
      phase3BossX = 994;
    if (phase3BossY < 30)
      phase3BossY = 30;
    if (phase3BossY > 600)
      phase3BossY = 600;
    if (phase3BossHealth <= 125) {
      phase3BossState = P3_DUEL;
      phase3BossAtkTimer = 2.0;
      strikeWindowActive = false;
    }
  } else if (phase3BossState == P3_DUEL) {
    double dx = phase3PlayerX - phase3BossX, dy = phase3PlayerY - phase3BossY;
    double dist = sqrt(dx * dx + dy * dy);
    if (dist > 80) {
      phase3BossX += (dx / dist) * 2.0;
      phase3BossY += (dy / dist) * 2.0;
    }
    if (phase3BossX < 30)
      phase3BossX = 30;
    if (phase3BossX > 994)
      phase3BossX = 994;
    if (phase3BossY < 30)
      phase3BossY = 30;
    if (phase3BossY > 600)
      phase3BossY = 600;
  }
  if (phase3BossHealth <= 0) {
    phase3BossHealth = 0;
    phase3BossState = P3_DEAD;
    phase3BossVictoryTimer = 4.0;
    strikeWindowActive = false;
    phase3ShockActive = false;
  }
}

void drawIntel() {
  iShowImage(0, 0, 1024, 640, digitalRainBG);

  // Semi-transparent dark overlay for readability
  iSetColor(0, 0, 0);
  iFilledRectangle(80, 100, 864, 440);
  iSetColor(0, 180, 0);
  iRectangle(80, 100, 864, 440);

  // Header
  iSetColor(0, 255, 0);
  iText(350, 490, "[ CLASSIFIED INTEL ]", GLUT_BITMAP_TIMES_ROMAN_24);

  // Divider line
  iSetColor(0, 120, 0);
  iLine(120, 470, 904, 470);

  // Intel text
  iSetColor(0, 220, 0);
  iText(120, 420, ">> Gravity periodically flips, pulling everything",
        GLUT_BITMAP_HELVETICA_18);
  iText(140, 390, "to ceiling or floor.", GLUT_BITMAP_HELVETICA_18);

  iText(120, 340, ">> Boss reloads every 20s and is vulnerable for 3s.",
        GLUT_BITMAP_HELVETICA_18);

  iText(120, 290, ">> High chaos triggers Rage Mode, increasing",
        GLUT_BITMAP_HELVETICA_18);
  iText(140, 260, "boss aggression.", GLUT_BITMAP_HELVETICA_18);

  iSetColor(0, 255, 100);
  iText(120, 200, ">> Adapt to shifting gravity, exploit reload windows,",
        GLUT_BITMAP_HELVETICA_18);
  iText(140, 170, "and survive the storm.", GLUT_BITMAP_HELVETICA_18);

  // Timer bar at bottom
  iSetColor(0, 100, 0);
  iFilledRectangle(120, 120, (int)(760 * (intelTimer / 5.0)), 10);

  // Auto-dismiss text
  char buf[30];
  sprintf_s(buf, "Auto-closing in %.0fs...", intelTimer > 0 ? intelTimer : 0);
  iSetColor(0, 140, 0);
  iText(400, 130, buf, GLUT_BITMAP_HELVETICA_12);
}

// Forward declarations
void drawSurpriseScreen();
void drawPuzzle();
void drawGravityNode(int x, int y, int size, GravityTile t);

// ============================================================
// DRAW NEW SIDE-SCROLLING LEVEL
// ============================================================
void drawNewLevel() {
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

void iDraw() {
  iClear();

  if (gameState == MENU) {
    drawMenu();
  } else if (gameState == NEW_LEVEL) {
    drawNewLevel();
  } else if (gameState == GAME_OVER) {
    // Game Over Screen
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, 1024, 640);
    iSetColor(180, 0, 0);
    iText(380, 380, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(160, 160, 160);
    iText(350, 300, "CLICK ANYWHERE TO RETURN TO MENU", GLUT_BITMAP_HELVETICA_18);
  } else if (gameState == CREDITS_STATE) {
    drawCredits();
  } else if (gameState == OPTIONS_STATE) {
    drawOptions();
  }

  else if (gameState == LEVEL1) {
    drawLevel1();
  } else if (gameState == WIN_SCREEN) {
    drawWinScreen();
  } else if (gameState == FINAL_BG) {
    drawFinalBG();
  } else if (gameState == SURPRISE_SCREEN) {
    drawSurpriseScreen();
  } else if (gameState == PUZZLE_SCREEN) {
    drawPuzzle();
  } else if (gameState == INTEL_SCREEN) {
    drawIntel();
  } else if (gameState == WARNING_SCREEN) {
    drawWarning();
  } else if (gameState == SECOND_BG) {
    drawSecondBG();
  } else if (gameState == BOSS_LEVEL) {
    drawPhase3Arena();
    drawPhase3Player();
    drawPhase3Boss();
    drawPhase3HUD();
  }

}

void drawSurpriseScreen() {
  iShowImage(0, 0, 1024, 640, finalBG);
  iSetColor(0, 0, 0); // Darken
  iFilledRectangle(0, 0, 1024, 640);

  iSetColor(255, 255, 255);
  iText(350, 400, "WAIT FOR A SURPRISE...", GLUT_BITMAP_TIMES_ROMAN_24);

  char buf[10];
  sprintf_s(buf, "%d", (int)ceil(surpriseTimer));
  iSetColor(180, 0, 0);
  iText(480, 300, buf, GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawGravityNode(int x, int y, int size, GravityTile t) {
  iSetColor(50, 50, 50);
  iRectangle(x, y, size, size);

  if (t.isActive)
    iSetColor(0, 255, 255); // Glowing blue
  else
    iSetColor(100, 100, 100);

  int mid = size / 2;
  int thick = 10;

  // Rotate logic within drawing
  // Straight: Left-Right (default rot 0)
  // L-Shape: Bottom-Right (default rot 0)
  // Cross: All (rot irrelevent)

  if (t.type == 0) { // Straight
    if (t.rotation % 2 == 0)
      iFilledRectangle(x, y + mid - 5, size, 10); // Horz
    else
      iFilledRectangle(x + mid - 5, y, 10, size); // Vert
  } else if (t.type == 1) {                       // L-Shape
    // Simplified: draw based on rot
    if (t.rotation == 0) { // Right-Top
      iFilledRectangle(x + mid - 5, y + mid - 5, mid + 5, 10);
      iFilledRectangle(x + mid - 5, y + mid - 5, 10, mid + 5);
    } else if (t.rotation == 1) { // Right-Bottom
      iFilledRectangle(x + mid - 5, y + mid - 5, mid + 5, 10);
      iFilledRectangle(x + mid - 5, y, 10, mid + 5);
    } else if (t.rotation == 2) { // Left-Bottom
      iFilledRectangle(x, y + mid - 5, mid + 5, 10);
      iFilledRectangle(x + mid - 5, y, 10, mid + 5);
    } else if (t.rotation == 3) { // Left-Top
      iFilledRectangle(x, y + mid - 5, mid + 5, 10);
      iFilledRectangle(x + mid - 5, y + mid - 5, 10, mid + 5);
    }
  } else if (t.type == 2) { // Cross
    iFilledRectangle(x, y + mid - 5, size, 10);
    iFilledRectangle(x + mid - 5, y, 10, size);
  }
}

void drawPuzzle() {
  iShowImage(0, 0, 1024, 640, finalBG); // Use level background for atmosphere

  // Screen Shake if any
  double dx = (rand() % 10 - 5) * screenShake;
  double dy = (rand() % 10 - 5) * screenShake;

  // UI
  iSetColor(255, 255, 255);
  iText(400 + dx, 600 + dy, "STABILIZE GRAVITY CORE", GLUT_BITMAP_HELVETICA_18);

  char timeBuf[20];
  sprintf_s(timeBuf, "TIME: %.1fs", puzzleTimer);
  if (puzzleTimer < 5.0)
    iSetColor(255, 0, 0);
  iText(850 + dx, 600 + dy, timeBuf, GLUT_BITMAP_TIMES_ROMAN_24);

  // Grid
  int startX = 312, startY = 120;
  int tileSize = 100;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      drawGravityNode(startX + j * tileSize + dx,
                      startY + (3 - i) * tileSize + dy, tileSize, grid[i][j]);
    }
  }

  // Terminals
  iSetColor(0, 255, 0);
  iText(200 + dx, 450 + dy, "CORE", GLUT_BITMAP_HELVETICA_18); // Top-Left
  iSetColor(255, 0, 0);
  iText(750 + dx, 150 + dy, "LOCK", GLUT_BITMAP_HELVETICA_18); // Bottom-Right

  if (puzzleCleared) {
    iSetColor(0, 255, 0);
    iText(400, 300, "SYSTEM STABILIZED!", GLUT_BITMAP_TIMES_ROMAN_24);
  }

  // Chaos Indicator (Visual Only for impact)
  if (puzzleTimer < 10.0) {
    iSetColor(255, 0, 0);
    iText(20, 20, "!!! GRAVITY SPIKE DETECTED !!!", GLUT_BITMAP_HELVETICA_12);
  }
}

void iMouseMove(int mx, int my) {}

void iPassiveMouseMove(int mx, int my) {}

void iKeyboard(unsigned char key) {
  if (key == 'b' || key == 'B' || key == '\b' ||
      key == 27) // 'b', Backspace, or ESC (27)
  {
    if (gameState == CREDITS_STATE || gameState == OPTIONS_STATE) {
      gameState = MENU;
    }
  }
  // ESC returns to menu from NEW_LEVEL
  if (key == 27) {
    if (gameState == NEW_LEVEL) {
      gameState = MENU;
    }
  }
  if (key == 'm' || key == 'M') {
    if (gameState != LEVEL1 && gameState != BOSS_LEVEL && gameState != NEW_LEVEL) {
      gameState = MENU;
    }
  }

  // New level WASD key handling
  if (gameState == NEW_LEVEL) {
    keyboardNewLevel(key);
  }


  // Quick test: press T on menu to jump to boss level
  if ((key == 't' || key == 'T') && gameState == MENU) {
    gameState = BOSS_LEVEL;
    p3InitPhase3();
  }

  // Boss level controls
  if (gameState == BOSS_LEVEL) {
    keyStates[key] = true;
    if (key == 'c' || key == 'C') {
      phase3CrouchMode = !phase3CrouchMode;
    }
    if (key == ' ') {
      if (!phase3DodgeActive) {
        phase3DodgeActive = true;
        phase3DodgeTimer = 0.25;
        phase3DodgeDX = 0;
        phase3DodgeDY = 0;
        if (p3IsKeyPressed('w') || p3IsKeyPressed('W'))
          phase3DodgeDY = 1;
        if (p3IsKeyPressed('s') || p3IsKeyPressed('S'))
          phase3DodgeDY = -1;
        if (p3IsKeyPressed('a') || p3IsKeyPressed('A'))
          phase3DodgeDX = -1;
        if (p3IsKeyPressed('d') || p3IsKeyPressed('D'))
          phase3DodgeDX = 1;
        if (phase3DodgeDX == 0 && phase3DodgeDY == 0)
          phase3DodgeDY = -1;
      }
    }
    if (key == 'f' || key == 'F') {
      if (!phase3DecoyActive) {
        phase3DecoyActive = true;
        phase3DecoyX = phase3PlayerX;
        phase3DecoyY = phase3PlayerY;
        phase3DecoyTimer = 5.0;
      }
    }
    if (key == 'e' || key == 'E') {
      for (int i = 0; i < 3; i++) {
        double dx = phase3PlayerX - phase3Seals[i].x;
        double dy = phase3PlayerY - phase3Seals[i].y;
        if (sqrt(dx * dx + dy * dy) < 40 && !phase3Seals[i].activated) {
          phase3Seals[i].activated = true;
          phase3SealsActivated++;
        }
      }
    }
  }

}

void iKeyUp(unsigned char key) { 
  keyStates[key] = false; 

  // New level WASD key release
  if (gameState == NEW_LEVEL) {
    keyUpNewLevel(key);
  }
}

void iSpecialKeyboard(unsigned char key) {
  if (gameState == BOSS_LEVEL) {
    keyStates[key] = true;
  }
  if (key == 27) // ESC
  {
    if (gameState == CREDITS_STATE || gameState == OPTIONS_STATE) {
      gameState = MENU;
    }
  }
  // New level arrow key handling
  if (gameState == NEW_LEVEL) {
    specialKeyboardNewLevel(key);
  }
}

void iSpecialKeyUp(unsigned char key) {
  if (gameState == BOSS_LEVEL) {
    keyStates[key] = false;
  }
  // New level arrow key release
  if (gameState == NEW_LEVEL) {
    specialKeyUpNewLevel(key);
  }
}

void iMouse(int button, int state, int mx, int my) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    if (gameState == MENU) {
      // ... existing checks ...
      // (I'll keep the full block for clarity in the tool call)
      // Check START CAMPAIGN button
      if (mx >= 98 && mx <= 248 && my >= 300 && my <= 340) {
        gameState = LEVEL1;
        initEnemies(); // Place enemies at fixed positions
      }
      // Check OPTIONS button
      else if (mx >= 98 && mx <= 248 && my >= 250 && my <= 290) {
        gameState = OPTIONS_STATE;
      }
      // Check CREDITS button
      else if (mx >= 98 && mx <= 248 && my >= 200 && my <= 240) {
        gameState = CREDITS_STATE;
      }
      // Check EXIT button
      else if (mx >= 98 && mx <= 248 && my >= 150 && my <= 190) {
        exit(0);
      }

    } else if (gameState == GAME_OVER) {
      gameState = MENU;
    } else if (gameState == NEW_LEVEL) {
      mouseNewLevel(button, state, mx, my);
    } else if (gameState == CREDITS_STATE || gameState == OPTIONS_STATE) {
      // Click anywhere to go back (Fallback for keyboard issues)
      gameState = MENU;
    }

    else if (gameState == LEVEL1) {
      // Trigger Shooting Animation
      player.isShooting = true;
      player.shootFrame = 0;
      player.shootTimer = 0;

      // Shooting logic
      if (player.reloadTimer <= 0) {
        for (int i = 0; i < 100; i++) {
          if (!bullets[i].active) {
            bullets[i].x = player.x + 20;
            bullets[i].y = player.y + 10;

            // Calculate direction towards mouse
            double angle = atan2(my - bullets[i].y, mx - bullets[i].x);
            bullets[i].dx = 7 * cos(angle);
            bullets[i].dy = 7 * sin(angle);

            bullets[i].active = true;
            player.reloadTimer = 30.0; // Wait ~0.5s (30 * 16ms)
            break;
          }
        }
      }
    } else if (gameState == FINAL_BG) {
      gameState = SURPRISE_SCREEN;
      surpriseTimer = 5.0;
    } else if (gameState == SURPRISE_SCREEN) {
      // Skip countdown? User didn't ask, but good to have
    } else if (gameState == PUZZLE_SCREEN) {
      // Check if clicked a tile
      int startX = 312, startY = 120;
      int tileSize = 100;
      if (mx >= startX && mx <= startX + 400 && my >= startY &&
          my <= startY + 400) {
        int col = (mx - startX) / tileSize;
        int row = 3 - (my - startY) / tileSize; // Grid is drawn (3-i)
        if (row >= 0 && row < 4 && col >= 0 && col < 4) {
          grid[row][col].rotation = (grid[row][col].rotation + 1) % 4;
        }
      }
    } else if (gameState == WIN_SCREEN) {
      gameState = FINAL_BG;
    } else if (gameState == BOSS_LEVEL) {
      double dx = phase3PlayerX - phase3BossX;
      double dy = phase3PlayerY - phase3BossY;
      double dist = sqrt(dx * dx + dy * dy);
      if (strikeWindowActive) {
        if (dist < 120) {
          phase3BossHealth -= 25;
          strikeWindowActive = false;
          phase3BossAtkTimer = 2.0;
        }
      } else if (phase3CriticalStrikeReady) {
        phase3BossHealth -= 60;
        phase3CriticalStrikeReady = false;
        phase3AwarenessLevel = 0;
        phase3BossState = P3_OBSERVE;
      } else if (dist < 100 && phase3AtkCooldown <= 0) {
        // Basic melee attack - always available when close enough
        phase3BossHealth -= 10;
        phase3AtkCooldown = 1.0;
      }
    }

  }

  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
  }
}

void fixedUpdate() {
  // Menu Navigation (Back to Home)
  if (gameState == CREDITS_STATE || gameState == OPTIONS_STATE) {
    if (p3IsKeyPressed(27) || p3IsKeyPressed('b') || p3IsKeyPressed('B')) {
      gameState = MENU;
    }
  }

  // ============================================================
  // NEW SIDE-SCROLLING LEVEL UPDATE
  // ============================================================
  if (gameState == NEW_LEVEL) {
    updateNewLevel();
  }

  if (gameState == LEVEL1) {
    // WASD Player Movement
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
    // Clamp player within screen
    if (player.x < 0)
      player.x = 0;
    if (player.x > 1004)
      player.x = 1004;
    if (player.y < 0)
      player.y = 0;
    if (player.y > 600)
      player.y = 600;

    // Implement shooting reload/cooldown
    if (player.reloadTimer > 0) {
      player.reloadTimer -= 1.0;
    }

    // Shooting Animation Update
    if (player.isShooting) {
      player.shootTimer += 0.2;
      if (player.shootTimer > 1.0) {
        player.shootTimer = 0;
        player.shootFrame++;
        if (player.shootFrame >= 3) { // 3 frame animation
          player.isShooting = false;
          player.shootFrame = 0;
        }
      }
    }

    // Bullet Movement
    for (int i = 0; i < 100; i++) {
      if (bullets[i].active) {
        bullets[i].x += bullets[i].dx;
        bullets[i].y += bullets[i].dy;

        // Deactivate bullets off-screen (left/right edges)
        if (bullets[i].x < 0 || bullets[i].x > 1024) {
          bullets[i].active = false;
        }

        // Bounce off ceiling (y >= 600)
        if (bullets[i].y >= 600) {
          bullets[i].y = 599;
          bullets[i].dy = -bullets[i].dy;
        }

        // Bounce off ground (y <= 0)
        if (bullets[i].y <= 0) {
          bullets[i].y = 1;
          bullets[i].dy = -bullets[i].dy;
        }

        // Bounce off left vertical wall (Thicker: x=300 to x=515)
        // Updated logic: bounce off the face it hits
        if (bullets[i].x >= 300 && bullets[i].x <= 370 && bullets[i].y <= 200) {
          // Moving right -> Hit Left Face
          if (bullets[i].dx > 0) {
            bullets[i].dx = -bullets[i].dx;
            bullets[i].x = 299; // Bounce left
          }
          // Moving left -> Hit Right Face
          else if (bullets[i].dx < 0) {
            bullets[i].dx = -bullets[i].dx;
            bullets[i].x = 371; // Bounce right
          }
        }

        // Bounce off T-wall vertical part (shifted LEFT again: x=580 to x=660,
        // y<=320)
        if (bullets[i].x >= 610 && bullets[i].x <= 660 && bullets[i].y <= 310) {
          // Moving right -> Hit Left Face
          if (bullets[i].dx > 0) {
            bullets[i].dx = -bullets[i].dx;
            bullets[i].x = 609; // Bounce left
          }
          // Moving left -> Hit Right Face
          else if (bullets[i].dx < 0) {
            bullets[i].dx = -bullets[i].dx;
            bullets[i].x = 661; // Bounce right
          }
        }

        // Bounce off T-wall horizontal bar (Shifted Right: x=660 to x=860)
        // y-range remains lifted: 180-200
        if (bullets[i].x >= 660 && bullets[i].x <= 860 && bullets[i].y >= 180 &&
            bullets[i].y <= 200) {

          // Horizontal hit (from left)
          if (bullets[i].x < 670 && bullets[i].dx > 0) {
            bullets[i].dx = -bullets[i].dx;
            bullets[i].x = 659;
          }
          // Horizontal hit (from right - unlikely)
          else if (bullets[i].x > 850 && bullets[i].dx < 0) {
            bullets[i].dx = -bullets[i].dx;
            bullets[i].x = 861;
          }
          // Vertical hit (top/bottom)
          else {
            bullets[i].dy = -bullets[i].dy;
            if (bullets[i].dy > 0)
              bullets[i].y = 201; // Hit bottom
            else
              bullets[i].y = 179; // Hit top
          }
        }

        // Collision with enemies
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

    // Enemy Patrol Movement (left-right within bounds)
    for (int i = 0; i < 3; i++) {
      if (enemies[i].active) {
        // Patrol horizontally
        enemies[i].x += enemies[i].dir * 2;

        // Reverse direction at patrol bounds
        if (enemies[i].x >= enemies[i].maxX) {
          enemies[i].dir = -1;
        }
        if (enemies[i].x <= enemies[i].minX) {
          enemies[i].dir = 1;
        }

        // Walk animation (cycle through 4 frames)
        enemies[i].animTimer += 0.15;
        if (enemies[i].animTimer > 1.0) {
          enemies[i].animTimer = 0;
          enemies[i].animFrame = (enemies[i].animFrame + 1) % 4;
        }

        // Collision with player (Increased threshold for larger sprites)
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

    // Check for Level Completion
    bool allDead = true;
    for (int i = 0; i < 3; i++) {
      if (enemies[i].active) {
        allDead = false;
        break;
      }
    }
    if (allDead) {
      initNewLevel(); // Transition to Side-Scrolling Level
    }
  }

  if (gameState == SURPRISE_SCREEN) {
    surpriseTimer -= 0.016; // Approx 60fps
    if (surpriseTimer <= 0) {
      gameState = PUZZLE_SCREEN;
      initPuzzle();
    }
  }

  if (gameState == PUZZLE_SCREEN) {
    if (!puzzleCleared) {
      puzzleTimer -= 0.016;

      // Reset active status
      for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
          grid[i][j].isActive = false;

      // Connectivity Check (Simplified DFS)
      // We'll tag connected tiles starting from (0,0) if (0,0) has a port open
      // to the "CORE" Port 3 is Left, Port 1 is Right, Port 0 is Top, Port 2 is
      // Bottom

      // (hasPort removed - using isOpen lambda below instead)

      // I'll implement the actual logic here carefully
      bool visited[4][4] = {false};
      int stackR[16], stackC[16], top = -1;

      // Start Tile (0,0) - must have Top or Left port to connect to Core
      // (Visual: Core is Top-Left) Let's say Core connects to grid[0][0] from
      // any side for simplicity, but it must be a valid path
      stackR[++top] = 0;
      stackC[top] = 0;
      visited[0][0] = true;

      while (top >= 0) {
        int r = stackR[top], c = stackC[top--];
        grid[r][c].isActive = true;
        if (r == 3 && c == 3)
          puzzleCleared = true; // Reached bottom-right

        // Neighbors: Top (r-1, c), Bottom (r+1, c), Left (r, c-1), Right (r,
        // c+1) (Using row major: 0 is top row, 3 is bottom row)

        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};
        int p[] = {0, 2, 3, 1};   // Port for side
        int opp[] = {2, 0, 1, 3}; // Port for neighbor side

        for (int i = 0; i < 4; i++) {
          int nr = r + dr[i], nc = c + dc[i];
          if (nr >= 0 && nr < 4 && nc >= 0 && nc < 4 && !visited[nr][nc]) {
            // Check if port p[i] is open in current and port opp[i] is open in
            // neighbor
            auto isOpen = [](int row, int col, int port) -> bool {
              int type = grid[row][col].type;
              int rot = grid[row][col].rotation;
              if (type == 0)
                return (
                    port % 2 !=
                    rot % 2);  // Straight: horz(rot%2==0) opens L/R(port%2==1),
                               // vert(rot%2==1) opens T/B(port%2==0)
              if (type == 1) { // L-Shape
                if (rot == 0)
                  return (port == 0 || port == 1);
                if (rot == 1)
                  return (port == 1 || port == 2);
                if (rot == 2)
                  return (port == 2 || port == 3);
                if (rot == 3)
                  return (port == 3 || port == 0);
              }
              if (type == 2)
                return true; // Cross
              return false;
            };

            if (isOpen(r, c, p[i]) && isOpen(nr, nc, opp[i])) {
              visited[nr][nc] = true;
              stackR[++top] = nr;
              stackC[top] = nc;
            }
          }
        }
      }

      if (puzzleTimer <= 0) {
        screenShake = 1.0;
        if (screenShake > 0)
          screenShake -= 0.05;
        if (puzzleTimer < -2.0) {
          gameState = MENU; // Failure: Pulse Alarm and reset
          surpriseTimer = 5.0;
        }
      }
    }

    if (puzzleCleared) {
      // Success: Stabilize and wait, then show intel
      puzzleTimer = 30.0; // Stop timer
      static double winDelay = 3.0;
      winDelay -= 0.016;
      if (winDelay <= 0) {
        gameState = INTEL_SCREEN;
        intelTimer = 5.0;
        winDelay = 3.0;
      }
    }
  }

  // Intel screen auto-dismiss after 5 seconds -> go to WARNING
  if (gameState == INTEL_SCREEN) {
    intelTimer -= 0.016;
    if (intelTimer <= 0) {
      gameState = WARNING_SCREEN;
      warningTimer = 5.0;
    }
  }

  // Warning screen countdown -> show 2nd bg
  if (gameState == WARNING_SCREEN) {
    warningTimer -= 0.016;
    if (warningTimer <= 0) {
      gameState = SECOND_BG;
      secondBGTimer = 10.0;
    }
  }

  // 2nd BG stays for 10 seconds then go to BOSS LEVEL
  if (gameState == SECOND_BG) {
    secondBGTimer -= 0.016;
    if (secondBGTimer <= 0) {
      gameState = BOSS_LEVEL;
      p3InitPhase3();
    }
  }

  // Boss level updates
  if (gameState == BOSS_LEVEL) {
    // Edge-detected toggle actions using framework's keyPressed[]
    static bool prevC = false, prevF = false, prevE = false, prevSpace = false;

    bool curC = (keyPressed['c'] || keyPressed['C']) ? true : false;
    if (curC && !prevC) {
      phase3CrouchMode = !phase3CrouchMode;
    }
    prevC = curC;

    bool curSpace = keyPressed[' '] ? true : false;
    if (curSpace && !prevSpace) {
      if (!phase3DodgeActive) {
        phase3DodgeActive = true;
        phase3DodgeTimer = 0.25;
        phase3DodgeDX = 0;
        phase3DodgeDY = 0;
        if (p3IsKeyPressed('w') || p3IsKeyPressed('W'))
          phase3DodgeDY = 1;
        if (p3IsKeyPressed('s') || p3IsKeyPressed('S'))
          phase3DodgeDY = -1;
        if (p3IsKeyPressed('a') || p3IsKeyPressed('A'))
          phase3DodgeDX = -1;
        if (p3IsKeyPressed('d') || p3IsKeyPressed('D'))
          phase3DodgeDX = 1;
        if (phase3DodgeDX == 0 && phase3DodgeDY == 0)
          phase3DodgeDY = -1;
      }
    }
    prevSpace = curSpace;

    bool curF = (keyPressed['f'] || keyPressed['F']) ? true : false;
    if (curF && !prevF) {
      if (!phase3DecoyActive) {
        phase3DecoyActive = true;
        phase3DecoyX = phase3PlayerX;
        phase3DecoyY = phase3PlayerY;
        phase3DecoyTimer = 5.0;
      }
    }
    prevF = curF;

    bool curE = (keyPressed['e'] || keyPressed['E']) ? true : false;
    if (curE && !prevE) {
      for (int i = 0; i < 3; i++) {
        double dx = phase3PlayerX - phase3Seals[i].x;
        double dy = phase3PlayerY - phase3Seals[i].y;
        if (sqrt(dx * dx + dy * dy) < 40 && !phase3Seals[i].activated) {
          phase3Seals[i].activated = true;
          phase3SealsActivated++;
        }
      }
    }
    prevE = curE;

    updatePhase3Player();
    updatePhase3Boss();
    handlePhase3Stealth();
    handlePhase3Combat();
  }
}

int main() {
  iInitialize(1024, 640, "Wrath of God");

  // Load Images (MUST occur after iInitialize)
  bgImage = iLoadImage("Assets/menu_bg.png");
  level1BG = iLoadImage("Assets/Stage.png");
  playerSpritesheet = iLoadImage("Assets/PC.png");
  enemySpritesheet = iLoadImage("Assets/NPC.png");
  finalBG = iLoadImage("Assets/1st bg.png");
  digitalRainBG = iLoadImage("Assets/Digital Rain.png");
  secondBG = iLoadImage("Assets/vault room.png");

  // Load new level backgrounds
  newLevelBG[0] = iLoadImage("Assets/BG_1.png");
  newLevelBG[1] = iLoadImage("Assets/BG_2.png");
  newLevelBG[2] = iLoadImage("Assets/BG_3.png");

  // Load new player spritesheet
  newPlayerSpritesheet = iLoadImage("Assets/PC2.png");
  npc1Spritesheet = iLoadImage("Assets/NPC.png");
  npc2Spritesheet = iLoadImage("Assets/NPC2.png");

  // Opening/Loading the audio files
  // mciSendString("open \"Audios/background.mp3\" alias bgsong", NULL, 0,
  // NULL); mciSendString("open \"Audios/gameover.mp3\" alias ggsong", NULL, 0,
  // NULL);

  // Playing the background audio on repeat
  // mciSendString("play bgsong repeat", NULL, 0, NULL);

  iStart();
  return 0;
}

// ============================================================
// NEW SIDE-SCROLLING LEVEL IMPLEMENTATIONS
// ============================================================

void initNewLevel() {
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

void mouseNewLevel(int button, int state, int mx, int my) {
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

void keyboardNewLevel(unsigned char key) {
  if (key == 'A' || key == 'a') arrowKeyLeft = true;
  if (key == 'D' || key == 'd') arrowKeyRight = true;
  if (key == 'W' || key == 'w') arrowKeyUp = true;
}

void keyUpNewLevel(unsigned char key) {
  if (key == 'A' || key == 'a') arrowKeyLeft = false;
  if (key == 'D' || key == 'd') arrowKeyRight = false;
  if (key == 'W' || key == 'w') arrowKeyUp = false;
}

void specialKeyboardNewLevel(unsigned char key) {
  if (key == GLUT_KEY_LEFT) arrowKeyLeft = true;
  if (key == GLUT_KEY_RIGHT) arrowKeyRight = true;
  if (key == GLUT_KEY_UP) arrowKeyUp = true;
}

void specialKeyUpNewLevel(unsigned char key) {
  if (key == GLUT_KEY_LEFT) arrowKeyLeft = false;
  if (key == GLUT_KEY_RIGHT) arrowKeyRight = false;
  if (key == GLUT_KEY_UP) arrowKeyUp = false;
}

void updateNewLevel() {
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
      double enX = newEnemies[i].x - scrollX; // Enemy position on screen
      
      // Decrease timers
      if (newEnemies[i].fireCooldown > 0) newEnemies[i].fireCooldown -= 0.05;
      if (newEnemies[i].isShooting) {
        newEnemies[i].shootTimer -= 0.05;
        if (newEnemies[i].shootTimer <= 0) newEnemies[i].isShooting = false;
      }

      // Basic AI Logic: only act if roughly visible
      if (enX > -200 && enX < 1200) {
        // Distance to player
        double distToPlayerX = newPlayerX - enX;
        
        if (abs(distToPlayerX) > 250) { // Walk towards player if far
          // Move towards player
          newEnemies[i].dir = (distToPlayerX > 0) ? 1 : -1;
          newEnemies[i].x += 4.0 * newEnemies[i].dir; // Speed 4.0 (Synchronized with player)
          
          // No frame cycling needed - using fixed side-facing frame from NPC.png
        } else { // Stop and shoot if in range
           // Stop and shoot
           newEnemies[i].dir = (distToPlayerX > 0) ? 1 : -1; // face player
           newEnemies[i].frame = 0; // idle frame while shooting
           
           if (newEnemies[i].fireCooldown <= 0) {
             // Fire!
             for (int b = 0; b < MAX_NEW_BULLETS; b++) {
               if (!newBullets[b].active) {
                 newBullets[b].active = true;
                 newBullets[b].isPlayerBullet = false; // Enemy bullet
                 // Fix: Enemy X is already in world space
                 newBullets[b].x = newEnemies[i].x + (newEnemies[i].dir == 1 ? 40 : -40);
                 newBullets[b].y = newEnemies[i].y + 15;
                 newBullets[b].dir = newEnemies[i].dir;
                 newBullets[b].speed = 5.0; // Enemy bullet is slightly slower
                 
                 newEnemies[i].isShooting = true;
                 newEnemies[i].shootTimer = 0.2;
                 newEnemies[i].fireCooldown = 1.5; // Enemy shoots every 1.5s
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
    gameState = WIN_SCREEN; // Continue to the rest of the game
  }
}
