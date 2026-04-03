#include "iGraphics.h"

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
#define BOSS_DEAD_IMAGE 12
#define LAB_TRANSITION 13
#define LAB_LEVEL 14
#define PUZZLE_TRANSITION 15

int gameState = MENU;
unsigned int bgImage;
unsigned int level1BG;
unsigned int playerSpritesheet;
unsigned int enemySpritesheet;
unsigned int finalBG;
unsigned int digitalRainBG;
unsigned int secondBG;
unsigned int chatgptImage;
unsigned int puzzleTransitionBG;

int puzzleTransitionPhase = 0;
double puzzleTransitionTimer = 0;
double puzzleTransitionAlpha = 0;

struct GravityTile {
  int type;     // 0: Straight, 1: L-shape, 2: Cross
  int rotation; // 0: 0, 1: 90, 2: 180, 3: 270 degrees
  bool isActive;
};

GravityTile grid[4][4];
double surpriseTimer = 5.0;
double puzzleTimer = 30.0;
bool puzzleCleared = false;
bool puzzlePlayerInteracted = false;
double screenShake = 0;
double intelTimer = 5.0;
double warningTimer = 5.0;
double secondBGTimer = 10.0;
double puzzleEnergyTimer = 0;

struct PuzzleParticle {
  double x, y;
  double speed;
  double alpha;
  int size;
};
PuzzleParticle puzzleParticles[40];

// ============================================================
// LAB LEVEL - "New Mecca Labs" Anti-Gravity Corridor
// ============================================================

struct LabTurret {
  double baseX, baseY; // Anchor point for movement
  double x, y;         // Current position
  int direction;       // 0=right, 1=left, 2=down
  double fireTimer;
  double fireInterval;
  double beamTimer;
  bool beamActive;

  // Evolution features
  int patternType;     // 0=Static, 1=Double, 2=Burst, 3=Alt
  int burstCount;      // Tracking for burst pattern
  double sweepRange;   // Range of horizontal/vertical sweep
  double sweepSpeed;   // Movement speed
  double sweepTime;    // Counter for oscillatory movement
  double warningTimer; // Glow/blink before fire
  bool isWarning;
};

struct LabPanel {
  double x, y;
  double w, h;
  bool triggered;
  bool fallen;
  double fallSpeed;
  double originalY;

  // Evolution features
  int sizeType;        // 0=Small, 1=Large
  double warningTimer; // Cracking/shaking phase
  bool isWarning;
};

struct AGZone {
  double startX, endX;
};

struct ElectricFloor {
  double x, w;
  double offTime, onTime; // Standard timing
  double timer;
  bool isActive;
};

struct SpeedZone {
  double startX, endX;
  double multiplier;
};

struct LabAmbientParticle {
  double x, y;
  double vx, vy;
  double size;
  double alpha;
  double life;
};
#define LAB_NUM_PARTICLES 60
LabAmbientParticle labParticles[LAB_NUM_PARTICLES];

double labScrollX = 0;
double labPlayerX = 150, labPlayerY = 105;
double labPlayerVY = 0;
bool labOnCeiling = false;
bool labIsJumping = false;
double labPlayerHealth = 100;
bool labPlayerFacingRight = true;
double labCorridorLength = 5120;
double labFloorY = 80;
double labCeilingY = 540;

double labTransitionTimer = 3.0;
double labTransitionAlpha = 0;
int labTransitionPhase = 0; // 0=fade out boss, 1=show title, 2=fade into lab
double labDamageFlash = 0;

// Survival Refinement
double labDashTimer = 0;
double labDashCooldown = 0;
double labInstructionTimer = 0;
const double LAB_MAX_HEALTH = 100;
const double LAB_MAX_HULL = 150;

#define LAB_NUM_TURRETS 8
#define LAB_NUM_PANELS 6
#define LAB_NUM_AGZONES 3
#define LAB_NUM_ELECTRIC 4
#define LAB_NUM_SPEED 2

#define LAB_NUM_HEALING 3

struct HealingZone {
  double startX, endX;
};

LabTurret labTurrets[LAB_NUM_TURRETS];
LabPanel labPanels[LAB_NUM_PANELS];
AGZone labAGZones[LAB_NUM_AGZONES];
ElectricFloor labElectricFloors[LAB_NUM_ELECTRIC];
SpeedZone labSpeedZones[LAB_NUM_SPEED];
HealingZone labHealingZones[LAB_NUM_HEALING];

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

enum BossState { P3_OBSERVE, P3_HUNT, P3_EXECUTE, P3_DUEL, P3_DEAD };
BossState phase3BossState = P3_OBSERVE;
bool strikeWindowActive = false;
double phase3SearchAngle = -1.5708;
double phase3SearchDir = 1.0;
double phase3BossLastSeenX = 512;
double phase3BossLastSeenY = 320;
double phase3BossMoveTimer = 0;
enum Phase3Attack { P3_ATK_NONE, P3_ATK_HEAVY, P3_ATK_FEINT, P3_ATK_SHOCKWAVE };
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
struct P3Shadow {
  double x, y, w, h;
};
P3Shadow phase3Shadows[6];

// Pillars
struct P3Pillar {
  double x, y, w, h;
};
P3Pillar phase3Pillars[4];

// Seals
struct P3Seal {
  double x, y;
  bool activated;
};
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
  if (type == 3) { // T-Junction
    if (rot == 0)
      return (port == 3 || port == 1 || port == 0); // L, R, T
    if (rot == 1)
      return (port == 0 || port == 1 || port == 2); // T, R, B
    if (rot == 2)
      return (port == 1 || port == 2 || port == 3); // R, B, L
    if (rot == 3)
      return (port == 2 || port == 3 || port == 0); // B, L, T
  }
  return false;
}

// Helper: check if the grid is already solved (path from (0,0) to (3,3))
bool isPuzzleAlreadySolved() {
  // Reset all active states first
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      grid[i][j].isActive = false;

  // Power starts at CORE (Left of (0,0) -> Port 3)
  if (!isTilePortOpen(0, 0, 3))
    return false;

  bool visited[4][4] = {false};
  int stackR[16], stackC[16], top = -1;
  stackR[++top] = 0;
  stackC[top] = 0;
  visited[0][0] = true;
  grid[0][0].isActive = true;

  int dr[] = {-1, 1, 0, 0}; // U, D, L, R
  int dc[] = {0, 0, -1, 1};
  int p[] = {0, 2, 3, 1};
  int opp[] = {2, 0, 1, 3};

  bool reachedLock = false;

  while (top >= 0) {
    int r = stackR[top], c = stackC[top--];

    // Check if reached LOCK at (3,3) and Output Port 1 (Right) is open
    if (r == 3 && c == 3 && isTilePortOpen(3, 3, 1))
      reachedLock = true;

    for (int i = 0; i < 4; i++) {
      int nr = r + dr[i], nc = c + dc[i];
      if (nr >= 0 && nr < 4 && nc >= 0 && nc < 4 && !visited[nr][nc]) {
        // Must have matching ports open
        if (isTilePortOpen(r, c, p[i]) && isTilePortOpen(nr, nc, opp[i])) {
          visited[nr][nc] = true;
          grid[nr][nc].isActive = true;
          stackR[++top] = nr;
          stackC[top] = nc;
        }
      }
    }
  }
  return reachedLock;
}

void initPuzzle() {
  srand(time(NULL));
  puzzlePlayerInteracted = false;
  puzzleTimer = 25.0; // EXTREME Difficulty: 25s
  puzzleCleared = false;

  // Strategic T-junction layout (Medium-High difficulty)
  // Types: 0=Straight, 1=L, 2=Cross, 3=T
  struct LayoutTile {
    int type;
    int correctRot;
  };
  LayoutTile layout[4][4] = {
      {{3, 2}, {1, 0}, {3, 1}, {1, 1}}, // Row 0
      {{1, 3}, {3, 3}, {3, 1}, {1, 2}}, // Row 1
      {{3, 0}, {1, 2}, {3, 3}, {1, 1}}, // Row 2
      {{1, 1}, {3, 3}, {1, 3}, {3, 0}}  // Row 3
  };

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      grid[i][j].type = layout[i][j].type;
      grid[i][j].rotation = rand() % 4;
      grid[i][j].isActive = false;
    }
  }

  // Shuffling loop: Guaranteed UNSOLVED start
  int safety = 0;
  while (isPuzzleAlreadySolved() && safety < 100) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        grid[i][j].rotation = rand() % 4;
      }
    }
    safety++;
  }

  puzzleTimer = 35.0; // Medium-High duration

  // Clear isActive after initialization/shuffling
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      grid[i][j].isActive = false;

  // Initialize Particles
  for (int i = 0; i < 40; i++) {
    puzzleParticles[i].x = rand() % 1024;
    puzzleParticles[i].y = rand() % 640;
    puzzleParticles[i].speed = 1.0 + (rand() % 100) / 50.0;
    puzzleParticles[i].alpha = (rand() % 100) / 100.0;
    puzzleParticles[i].size = 2 + rand() % 4;
  }
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

  // Button 4: LAB LEVEL (NEW)
  if (iMouseX >= 98 && iMouseX <= 248 && iMouseY >= 150 && iMouseY <= 190) {
    iSetColor(0, 120, 180); // Neon blue on hover
    iFilledRectangle(98, 150, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(0, 60, 100);
    iFilledRectangle(98, 150, 150, 40);
    iSetColor(180, 240, 255);
  }
  iText(120, 165, "LAB LEVEL", GLUT_BITMAP_HELVETICA_12);

  // Button 5: EXIT
  if (iMouseX >= 98 && iMouseX <= 248 && iMouseY >= 100 && iMouseY <= 140) {
    iSetColor(100, 0, 0); // Darker red on hover
    iFilledRectangle(98, 100, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(30, 30, 30);
    iFilledRectangle(98, 100, 150, 40);
    iSetColor(160, 160, 160);
  }
  iText(135, 115, "EXIT", GLUT_BITMAP_HELVETICA_12);

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

  // Level selection (NEW)
  iSetColor(0, 150, 255);
  iText(350, 180, "LEVEL SELECT", GLUT_BITMAP_HELVETICA_18);
  if (iMouseX >= 350 && iMouseX <= 550 && iMouseY >= 130 && iMouseY <= 170) {
    iSetColor(0, 150, 100); // Hover green-blue
    iFilledRectangle(350, 130, 200, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(0, 60, 100);
    iFilledRectangle(350, 130, 200, 40);
    iSetColor(180, 240, 255);
  }
  iText(375, 145, "START LABORATORY", GLUT_BITMAP_HELVETICA_10);

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
  iText(20, 30, "WASD:Move C:Crouch SPACE:Dodge P:Attack F:Decoy E:Seal",
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
      ad -= 6.28318;
    while (ad < -3.14159)
      ad += 6.28318;
    phase3CriticalStrikeReady = (dist < 120 && (ad > 1.2 || ad < -1.2));
  } else
    phase3CriticalStrikeReady = false;

  if (phase3PlayerHealth <= 0) {
    phase3PlayerHealth = 0;
    phase3BossVictoryTimer = 3.0;
  }
}

void updatePhase3Boss() {
  if (phase3BossState == P3_DEAD || phase3PlayerHealth <= 0) {
    phase3BossVictoryTimer -= 0.016;
    if (phase3BossVictoryTimer <= 0) {
      if (phase3BossState == P3_DEAD) {
        gameState = BOSS_DEAD_IMAGE;
      } else {
        gameState = MENU; // Player died
      }
    }
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
      phase3BossX += (dx / dist) * 0.6;
      phase3BossY += (dy / dist) * 0.6;
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
      phase3BossX += (dx / dist) * 0.9;
      phase3BossY += (dy / dist) * 0.9;
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
      phase3BossX += (dx / dist) * 0.5;
      phase3BossY += (dy / dist) * 0.5;
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

// ============================================================
// LAB LEVEL FUNCTIONS
// ============================================================

void initLabLevel() {
  labScrollX = 0;
  labPlayerX = 150;
  labPlayerY = labFloorY + 25;
  labInstructionTimer = 7.0;
  labPlayerVY = 0;
  labOnCeiling = false;
  labIsJumping = false;
  labPlayerHealth = 100;
  labPlayerFacingRight = true;
  labDamageFlash = 0;

  // Anti-gravity zones (Strategically placed where verticality is needed)
  labAGZones[0] = {1200, 1600}; // Sector 1/2 transition
  labAGZones[1] = {2800, 3400}; // Sector 2/3 transition
  labAGZones[2] = {4200, 4800}; // Sector 3 final gauntlet

  // Sector 1: Early (Basic static/double)
  labTurrets[0] = {500,   200, 500, 200, 0, 0, 3.0, 0,
                   false, 0,   0,   0,   0, 0, 0,   false};
  labTurrets[1] = {1350,  labCeilingY, 1350, labCeilingY, 2, 0, 3.5, 0,
                   false, 1,           0,    0,           0, 0, 0,   false};

  // Sector 2: Mid (Burst and Sweep)
  labTurrets[2] = {2100,  150, 2100, 150, 0,   0, 3.0, 0,
                   false, 2,   0,    60,  1.5, 0, 0,   false};
  labTurrets[3] = {2600,  320, 2600, 320, 1, 0, 2.5, 0,
                   false, 1,   0,    0,   0, 0, 0,   false};
  labTurrets[4] = {3000,  labCeilingY, 3000, labCeilingY, 2,   0, 3.0, 0,
                   false, 2,           0,    120,         1.0, 0, 0,   false};

  // Sector 3: Late (Alternating and Fast Sweep)
  labTurrets[5] = {3800,  250, 3800, 250, 0,   0, 2.0, 0,
                   false, 3,   0,    100, 2.0, 0, 0,   false};
  labTurrets[6] = {4400,  labCeilingY, 4400, labCeilingY, 2,   0, 2.0, 0,
                   false, 3,           0,    180,         1.5, 0, 0,   false};
  labTurrets[7] = {4900,  labCeilingY, 4900, labCeilingY, 2, 0, 2.0, 0,
                   false, 2,           0,    0,           0, 0, 0,   false};

  // === HAZARD: FALLING PANELS ===
  // sizeType: 0=Small, 1=Large
  labPanels[0] = {650, labCeilingY - 20, 40, 40, false, false,
                  0,   labCeilingY - 20, 0,  0,  false}; // Small
  labPanels[1] = {1800, labCeilingY - 30, 140, 30, false, false,
                  0,    labCeilingY - 30, 1,   0,  false}; // Large
  labPanels[2] = {2600, labCeilingY - 20, 50, 50, false, false,
                  0,    labCeilingY - 20, 0,  0,  false}; // Small (Moved out of
                                                          // healing zone)
  labPanels[3] = {3250, labCeilingY - 30, 120, 35, false, false,
                  0,    labCeilingY - 30, 1,   0,  false}; // Large
  labPanels[4] = {3800, labCeilingY - 20, 45, 45, false, false,
                  0,    labCeilingY - 20, 0,  0,  false}; // Small
  labPanels[5] = {4600, labCeilingY - 40, 180, 40, false, false,
                  0,    labCeilingY - 40, 1,   0,  false}; // Large (Huge)

  // === HAZARD: ELECTRIC FLOORS ===
  labElectricFloors[0] = {1380, 200, 2.0, 1.0, 0, false};
  labElectricFloors[1] = {2100, 300, 1.5, 0.8, 0, false};
  labElectricFloors[2] = {3500, 250, 3.0, 1.5, 0, false};
  labElectricFloors[3] = {4450, 400, 1.0, 2.0, 0, false};

  // === HEALING ZONES (Green Zones) ===
  labHealingZones[0] = {600, 900};   // Sector 1 Safe (Clear of turret 0)
  labHealingZones[1] = {2200, 2500}; // Sector 2 Safe (Clear of panel 2)
  labHealingZones[2] = {4000, 4300}; // Sector 3 Safe

  // === SPEED ZONES ===
  labSpeedZones[0] = {1800, 2100, 1.3}; // 30% boost
  labSpeedZones[1] = {3400, 3800, 1.5}; // 50% boost

  // === AMBIENT PARTICLES ===
  for (int i = 0; i < LAB_NUM_PARTICLES; i++) {
    labParticles[i].life = 0; // Force respawn
  }
}

void drawBossDeadImage() {
  // Show the ChatGPT image full screen
  iShowImage(0, 0, 1024, 640, chatgptImage);

  // Overlay with slight vignette
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
  glBegin(GL_QUADS);
  glVertex2f(0, 0);
  glVertex2f(1024, 0);
  glVertex2f(1024, 80);
  glVertex2f(0, 80);
  glEnd();
  glDisable(GL_BLEND);

  // Text prompt
  iSetColor(255, 255, 255);
  iText(350, 50, "THE SENTINEL HAS FALLEN", GLUT_BITMAP_TIMES_ROMAN_24);
  iSetColor(180, 0, 0);
  iText(370, 20, "CLICK ANYWHERE TO CONTINUE", GLUT_BITMAP_HELVETICA_12);
}

void drawLabTransition() {
  // Cinematic transition: fade to black → show title → fade into lab
  iSetColor(0, 0, 0);
  iFilledRectangle(0, 0, 1024, 640);

  if (labTransitionPhase == 0) {
    // Phase 0: Fade to black (already black)
    // Just show fading text
    double alpha = 1.0 - (labTransitionTimer / 1.0);
    if (alpha > 1.0)
      alpha = 1.0;
    if (alpha < 0.0)
      alpha = 0.0;
    int brightness = (int)(255 * alpha);
    iSetColor(brightness, brightness, brightness);
    iText(380, 340, "ENTERING...", GLUT_BITMAP_TIMES_ROMAN_24);
  } else if (labTransitionPhase == 1) {
    // Phase 1: Show lab name with dramatic reveal
    // Red scanning line effect
    double scanY = 640 * (1.0 - labTransitionTimer / 2.0);
    iSetColor(180, 0, 0);
    iLine(100, scanY, 924, scanY);
    iLine(100, scanY + 1, 924, scanY + 1);

    // Lab name
    iSetColor(0, 200, 255);
    iText(320, 360, "NEW MECCA LABS", GLUT_BITMAP_TIMES_ROMAN_24);

    // Subtitle
    iSetColor(100, 150, 180);
    iText(340, 320, "ANTI-GRAVITY RESEARCH CORRIDOR", GLUT_BITMAP_HELVETICA_12);

    // Warning text
    iSetColor(255, 60, 60);
    iText(360, 260, ">> HAZARD LEVEL: CRITICAL <<", GLUT_BITMAP_HELVETICA_12);

    // Decorative corners
    iSetColor(0, 150, 255);
    iLine(100, 420, 140, 420);
    iLine(100, 420, 100, 380);
    iLine(924, 420, 884, 420);
    iLine(924, 420, 924, 380);
    iLine(100, 240, 140, 240);
    iLine(100, 240, 100, 280);
    iLine(924, 240, 884, 240);
    iLine(924, 240, 924, 280);

    // Progress bar
    double progress = 1.0 - (labTransitionTimer / 2.0);
    iSetColor(0, 80, 120);
    iFilledRectangle(300, 200, 400, 8);
    iSetColor(0, 200, 255);
    iFilledRectangle(300, 200, (int)(400 * progress), 8);
  } else {
    // Phase 2: Fade into lab (brief flash)
    double alpha = labTransitionTimer / 0.5;
    if (alpha > 1.0)
      alpha = 1.0;
    int brightness = (int)(255 * (1.0 - alpha));
    iSetColor(brightness, brightness, brightness);
    iFilledRectangle(0, 0, 1024, 640);
  }
}

void drawLabInstructions() {
  if (labInstructionTimer <= 0)
    return;

  // Calculate fade-in and fade-out
  double alpha = 1.0;
  if (labInstructionTimer > 6.0)
    alpha = (7.0 - labInstructionTimer); // Fade in over 1s
  else if (labInstructionTimer < 1.0)
    alpha = labInstructionTimer; // Fade out over 1s

  if (alpha > 1.0)
    alpha = 1.0;
  if (alpha < 0)
    alpha = 0;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Background box
  glColor4f(0.0f, 0.05f, 0.1f, (float)(0.8 * alpha));
  iFilledRectangle(250, 150, 524, 300);

  // Borders
  glColor4f(0.0f, 0.6f, 1.0f, (float)(alpha));
  iRectangle(250, 150, 524, 300);
  iRectangle(252, 152, 520, 296);

  // Header
  iSetColor((int)(0 * alpha), (int)(200 * alpha), (int)(255 * alpha));
  iText(380, 410, "MISSION PROTOCOLS", GLUT_BITMAP_TIMES_ROMAN_24);

  // Instructions
  iSetColor((int)(255 * alpha), (int)(255 * alpha), (int)(255 * alpha));
  iText(300, 360, "A / D  - MOVE LEFT / RIGHT", GLUT_BITMAP_HELVETICA_18);
  iText(300, 320, "W / SPACE - FLIP GRAVITY (IN AG ZONES)",
        GLUT_BITMAP_HELVETICA_18);
  iText(300, 280, "X - EMERGENCY DASH", GLUT_BITMAP_HELVETICA_18);

  // Zone info
  iSetColor((int)(0 * alpha), (int)(200 * alpha), (int)(255 * alpha));
  iText(300, 230, "[ BLUE ZONES ] : ANTI-GRAVITY ENABLED",
        GLUT_BITMAP_HELVETICA_12);
  iSetColor((int)(0 * alpha), (int)(255 * alpha), (int)(100 * alpha));
  iText(300, 210, "[ GREEN ZONES ] : HULL REGENERATION",
        GLUT_BITMAP_HELVETICA_12);

  // Warning
  iSetColor((int)(255 * alpha), (int)(50 * alpha), (int)(50 * alpha));
  iText(300, 175, "SYSTEM ADAPTATION REQUIRED FOR SURVIVAL",
        GLUT_BITMAP_HELVETICA_10);

  glDisable(GL_BLEND);
}

void drawLabLevel() {
  // Camera follows player
  double camX = labPlayerX - 300;
  if (camX < 0)
    camX = 0;
  if (camX > labCorridorLength - 1024)
    camX = labCorridorLength - 1024;

  // === BACKGROUND ===
  double bgW = 1024, bgH = 640;
  for (int i = -1; i < (int)(labCorridorLength / bgW) + 2; i++) {
    double bx = i * bgW - (camX * 0.5); // Parallax factor 0.5
    if (bx + bgW > 0 && bx < 1024) {
      iShowImage((int)bx, 0, (int)bgW, (int)bgH, chatgptImage);
    }
  }

  // Dark overlay for visibility
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
  iFilledRectangle(0, 0, 1024, 640);
  glDisable(GL_BLEND);

  glPushMatrix();
  glTranslatef(-camX, 0, 0);

  // === AMBIENT PARTICLES (Dust) ===
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for (int i = 0; i < LAB_NUM_PARTICLES; i++) {
    if (labParticles[i].life > 0) {
      glColor4f(0.8f, 0.9f, 1.0f, (float)labParticles[i].alpha);
      iFilledCircle(labParticles[i].x, labParticles[i].y, labParticles[i].size);
    }
  }
  glDisable(GL_BLEND);

  // Corridor Floor/Ceiling
  iSetColor(30, 30, 50);
  iFilledRectangle(camX, 0, 1024, labFloorY);
  iFilledRectangle(camX, labCeilingY, 1024, 640 - labCeilingY);
  iSetColor(100, 150, 255);
  iLine(camX, labFloorY, camX + 1024, labFloorY);
  iLine(camX, labCeilingY, camX + 1024, labCeilingY);

  // === SPEED ZONES ===
  for (int i = 0; i < LAB_NUM_SPEED; i++) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 1.0f, 1.0f, 0.15f);
    iFilledRectangle(labSpeedZones[i].startX, labFloorY,
                     labSpeedZones[i].endX - labSpeedZones[i].startX,
                     labCeilingY - labFloorY);
    glColor4f(0.0f, 1.0f, 1.0f, 0.4f);
    for (int s = 0; s < 5; s++) {
      double sx = labSpeedZones[i].startX +
                  (int)(secondBGTimer * 200 + s * 100) %
                      (int)(labSpeedZones[i].endX - labSpeedZones[i].startX);
      iLine(sx, labFloorY, sx + 20, labFloorY + 50);
    }
    glDisable(GL_BLEND);
  }

  // === SAFE ZONES (AG Fields) ===
  for (int i = 0; i < LAB_NUM_AGZONES; i++) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    double glow = 0.2 + 0.1 * sin(secondBGTimer * 4);
    glColor4f(0.0f, 0.6f, 1.0f, (float)glow);
    iFilledRectangle(labAGZones[i].startX, labFloorY,
                     labAGZones[i].endX - labAGZones[i].startX,
                     labCeilingY - labFloorY);

    // Border Glow
    glColor4f(0.4f, 0.8f, 1.0f, 0.5f);
    iRectangle(labAGZones[i].startX, labFloorY,
               labAGZones[i].endX - labAGZones[i].startX,
               labCeilingY - labFloorY);
    glDisable(GL_BLEND);

    // Safe Zone Label
    iSetColor(255, 255, 255);
    iText(labAGZones[i].startX + 10, labFloorY + 10, "ANTI-GRAVITY FIELD",
          GLUT_BITMAP_HELVETICA_12);

    // Ceiling Markings
    iSetColor(0, 100, 255);
    iLine(labAGZones[i].startX, labCeilingY, labAGZones[i].endX, labCeilingY);
    iLine(labAGZones[i].startX, labCeilingY - 2, labAGZones[i].endX,
          labCeilingY - 2);
  }

  // === HEALING ZONES ===
  for (int i = 0; i < LAB_NUM_HEALING; i++) {
    double healPulse = 0.15 + 0.1 * sin(secondBGTimer * 6);
    // Full height subtle glow (Works for walls and ceilings)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(0.0f, 1.0f, 0.4f, (float)(0.15 * healPulse));
    iFilledRectangle(labHealingZones[i].startX, 0,
                     labHealingZones[i].endX - labHealingZones[i].startX, 640);
    glDisable(GL_BLEND);

    // Hazard-free Border
    iSetColor(0, 255, 100);
    iRectangle(labHealingZones[i].startX, labFloorY,
               labHealingZones[i].endX - labHealingZones[i].startX,
               labCeilingY - labFloorY);

    // Healing Zone Header
    iSetColor(255, 255, 255);
    iText(labHealingZones[i].startX + 10, labFloorY + 10, "REPAIR HARMONICS",
          GLUT_BITMAP_HELVETICA_12);

    // Floating Plus symbols (Enhanced)
    glEnable(GL_BLEND);
    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA); // Ensure correct blend for plus symbols
    for (int p = 0; p < 8; p++) {
      double px = labHealingZones[i].startX + 15 + p * 35;
      double py = (int)(secondBGTimer * 80 + p * 40) % 640;
      glColor4f(0.5f, 1.0f, 0.6f, (float)(0.6 * healPulse));
      iFilledRectangle(px - 1, py - 6, 2, 12);
      iFilledRectangle(px - 6, py - 1, 12, 2);
    }
    glDisable(GL_BLEND);
  }

  // === ELECTRIC FLOORS ===
  for (int i = 0; i < LAB_NUM_ELECTRIC; i++) {
    if (labElectricFloors[i].isActive) {
      // Neon Yellow Shimmer
      glEnable(GL_BLEND);
      glColor4f(1.0f, 1.0f, 0.0f, 0.4f + (float)sin(secondBGTimer * 20) * 0.2f);
      iFilledRectangle(labElectricFloors[i].x, labFloorY - 5,
                       labElectricFloors[i].w, 5);
      glDisable(GL_BLEND);

      // Neon Yellow Arcs
      iSetColor(255, 255, 100);
      for (int a = 0; a < 6; a++) {
        double ax =
            labElectricFloors[i].x + rand() % (int)labElectricFloors[i].w;
        double ay = labFloorY;
        for (int seg = 0; seg < 3; seg++) {
          double nax = ax + (rand() % 16 - 8);
          double nay = ay + (rand() % 15 + 5);
          iLine(ax, ay, nax, nay);
          ax = nax;
          ay = nay;
        }
      }
    } else {
      iSetColor(40, 40, 80);
      iFilledRectangle(labElectricFloors[i].x, labFloorY - 5,
                       labElectricFloors[i].w, 5);
    }
  }

  // === TURRETS & LASERS ===
  for (int i = 0; i < LAB_NUM_TURRETS; i++) {
    // Hover animation
    double hoverY = sin(secondBGTimer * 3 + i) * 6;
    double tx = labTurrets[i].x, ty = labTurrets[i].y + hoverY;

    // Warning glow
    if (labTurrets[i].isWarning) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4f(1.0f, 0.0f, 0.0f, 0.5f + (float)sin(secondBGTimer * 15) * 0.4f);
      iFilledCircle(tx, ty, 18);
      glDisable(GL_BLEND);
    }
    // Emitter (Metallic Accent)
    iSetColor(60, 60, 75);
    iFilledRectangle(tx - 12, ty - 12, 24, 24);
    iSetColor(150, 150, 180); // Highlight
    iLine(tx - 12, ty + 12, tx + 12, ty + 12);

    iSetColor(255, 0, 0);
    iFilledCircle(tx, ty, 5);

    // Beam
    if (labTurrets[i].beamActive) {
      iSetColor(255, 0, 0);
      if (labTurrets[i].direction == 0)
        iFilledRectangle(tx + 12, ty - 2, 2000, 4);
      else if (labTurrets[i].direction == 1)
        iFilledRectangle(tx - 2000, ty - 2, 2000 - 12, 4);
      else if (labTurrets[i].direction == 2)
        iFilledRectangle(tx - 2, labFloorY, 4, ty - labFloorY - 12);

      // Beam glow
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4f(1.0f, 0.0f, 0.0f, 0.3f);
      if (labTurrets[i].direction == 2)
        iFilledRectangle(tx - 6, labFloorY, 12, ty - labFloorY - 12);
      glDisable(GL_BLEND);
    }
  }

  // === FALLING PANELS ===
  for (int i = 0; i < LAB_NUM_PANELS; i++) {
    if (!labPanels[i].fallen) {
      double px = labPanels[i].x, py = labPanels[i].y;

      // Hover animation for untriggered panels
      if (!labPanels[i].triggered) {
        py += sin(secondBGTimer * 2 + i) * 3;
      }

      if (labPanels[i].isWarning) {
        px += (rand() % 6 - 3);
        py += (rand() % 6 - 3);
      }

      // Panel Body (Metallic look)
      iSetColor(50, 50, 65);
      iFilledRectangle(px, py, labPanels[i].w, labPanels[i].h);

      // Top Highlight
      iSetColor(120, 120, 150);
      iLine(px, py + labPanels[i].h, px + labPanels[i].w, py + labPanels[i].h);

      iSetColor(20, 20, 30);
      iRectangle(px, py, labPanels[i].w, labPanels[i].h);

      if (labPanels[i].isWarning) {
        // Ceiling Crack Visual
        iSetColor(150, 150, 150);
        iLine(px - 10, labCeilingY, px + 5, labCeilingY - 10);
        iLine(px + 5, labCeilingY - 10, px + 20, labCeilingY);
        iLine(px + 30, labCeilingY, px + 50, labCeilingY - 15);

        // Warning Icon
        iSetColor(255, 100, 0);
        iText(px + labPanels[i].w / 2 - 5, py + labPanels[i].h / 2 - 5, "!",
              GLUT_BITMAP_HELVETICA_18);
      }
    }
  }

  // === PLAYER ===
  glPushMatrix();
  glTranslatef(labPlayerX, labPlayerY, 0);
  if (!labPlayerFacingRight)
    glScalef(-1, 1, 1);
  if (labOnCeiling)
    glScalef(1, -1, 1);

  // Restore player appearance
  int fw = 64, fh = 64;
  int frameIdx = 2; // Standard right-facing frame
  int sx = frameIdx * fw;
  int sy = 0;
  iShowSubImage(-30, -30, 60, 60, playerSpritesheet, sx, sy, fw, fh, 256, 64);
  glPopMatrix();

  // Damage flash
  if (labDamageFlash > 0) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.0f, 0.0f, (float)labDamageFlash * 0.4f);
    iFilledRectangle(camX, 0, 1024, 640);
    glDisable(GL_BLEND);
  }

  glPopMatrix();

  // === HUD ===
  iSetColor(100, 100, 100);
  iRectangle(20, 600, 200, 15); // Border

  if (labPlayerHealth > 100) {
    iSetColor(0, 200, 255); // Cyan for Hull Integrity
    iFilledRectangle(20, 600, (int)(200 * labPlayerHealth / 150.0), 15);
  } else {
    iSetColor(0, 255, 100); // Green for standard health
    iFilledRectangle(20, 600, (int)(200 * labPlayerHealth / 100.0), 15);
  }
  iSetColor(255, 255, 255);
  iText(20, 620, "HULL INTEGRITY", GLUT_BITMAP_HELVETICA_12);

  // Dash Indicator
  iSetColor(150, 150, 150);
  iRectangle(20, 580, 100, 8);
  if (labDashCooldown <= 0) {
    iSetColor(255, 255, 0); // Yellow for Ready
    iFilledRectangle(20, 580, 100, 8);
    iText(130, 580, "DASH READY [X]", GLUT_BITMAP_HELVETICA_10);
  } else {
    iSetColor(100, 50, 0); // Dull orange for charging
    iFilledRectangle(20, 580, (int)(100 * (1.0 - labDashCooldown / 3.0)), 8);
    iText(130, 580, "CHARGING...", GLUT_BITMAP_HELVETICA_10);
  }

  iSetColor(0, 150, 255);
  iText(800, 620, "MECCA LABS - SECTOR 7", GLUT_BITMAP_HELVETICA_12);

  double progress = labPlayerX / labCorridorLength;
  iSetColor(50, 50, 50);
  iFilledRectangle(300, 600, 400, 10);
  iSetColor(0, 200, 255);
  iFilledRectangle(300, 600, (int)(400 * progress), 10);

  // Instructions overlay
  drawLabInstructions();
}

void updateLabLevel() {
  if (labInstructionTimer > 0)
    labInstructionTimer -= 0.016;

  // === SURVIVAL MECHANICS UPDATE ===
  if (labDashTimer > 0)
    labDashTimer -= 0.016;
  if (labDashCooldown > 0)
    labDashCooldown -= 0.016;

  // Base speed + Dash multiplier
  double currentSpeed = (labDashTimer > 0) ? 12.0 : 4.5;
  // === SPEED ZONE LOGIC ===
  for (int i = 0; i < LAB_NUM_SPEED; i++) {
    if (labPlayerX >= labSpeedZones[i].startX &&
        labPlayerX <= labSpeedZones[i].endX) {
      currentSpeed *= labSpeedZones[i].multiplier;
      break;
    }
  }

  // Player movement
  if (keyPressed['d'] || keyPressed['D']) {
    labPlayerX += currentSpeed;
    labPlayerFacingRight = true;
  }
  if (keyPressed['a'] || keyPressed['A']) {
    labPlayerX -= currentSpeed;
    labPlayerFacingRight = false;
  }

  // Clamp player position
  if (labPlayerX < 30)
    labPlayerX = 30;
  if (labPlayerX > labCorridorLength - 30)
    labPlayerX = labCorridorLength - 30;

  // Check if player is in an anti-gravity zone
  bool inAGZone = false;
  for (int i = 0; i < LAB_NUM_AGZONES; i++) {
    if (labPlayerX >= labAGZones[i].startX &&
        labPlayerX <= labAGZones[i].endX) {
      inAGZone = true;
      break;
    }
  }

  // Auto-switch gravity when entering/leaving AG zones
  if (inAGZone && !labOnCeiling &&
      (keyPressed['w'] || keyPressed['W'] || keyPressed[' '])) {
    if (!labIsJumping) {
      labOnCeiling = true;
      labPlayerVY = 8;
      labIsJumping = true;
    }
  }
  if (!inAGZone && labOnCeiling) {
    labOnCeiling = false;
    labPlayerVY = 0;
  }

  // Gravity and jumping
  if (labOnCeiling) {
    labPlayerVY += 0.6;
    labPlayerY += labPlayerVY;
    if (labPlayerY >= labCeilingY - 25) {
      labPlayerY = labCeilingY - 25;
      labPlayerVY = 0;
      labIsJumping = false;
    }
    if (labPlayerY <= labFloorY + 25) {
      labPlayerY = labFloorY + 25;
      labPlayerVY = 0;
      labOnCeiling = false;
      labIsJumping = false;
    }
    if ((keyPressed['s'] || keyPressed['S'] || keyPressed[' ']) &&
        !labIsJumping) {
      labPlayerVY = -10;
      labIsJumping = true;
    }
  } else {
    labPlayerVY -= 0.6;
    labPlayerY += labPlayerVY;
    if (labPlayerY <= labFloorY + 25) {
      labPlayerY = labFloorY + 25;
      labPlayerVY = 0;
      labIsJumping = false;
    }
    if (labPlayerY >= labCeilingY - 25) {
      labPlayerY = labCeilingY - 25;
      labPlayerVY = 0;
      if (inAGZone) {
        labOnCeiling = true;
        labIsJumping = false;
      }
    }
    if ((keyPressed['w'] || keyPressed['W'] || keyPressed[' ']) &&
        !labIsJumping) {
      labPlayerVY = 12;
      labIsJumping = true;
    }
  }

  // Damage flash decay
  if (labDamageFlash > 0)
    labDamageFlash -= 0.05;

  // Over-health Decay (Hull integrity slowly stabilizes to 100)
  if (labPlayerHealth > LAB_MAX_HEALTH && labDashTimer <= 0) {
    labPlayerHealth -= 0.005; // Very slow decay
  }

  // === GLOBAL SAFETY CHECK ===
  bool isSafe = false;
  for (int z = 0; z < LAB_NUM_AGZONES; z++) {
    if (labPlayerX > labAGZones[z].startX && labPlayerX < labAGZones[z].endX) {
      isSafe = true;
      break;
    }
  }

  // === HEALING ZONE LOGIC (Green Zones) ===
  bool underAttack = (labDamageFlash > 0);

  // Check Laser Intersection
  for (int i = 0; i < LAB_NUM_TURRETS; i++) {
    if (labTurrets[i].beamActive) {
      if (labTurrets[i].direction == 2) { // Down-firing
        if (fabs(labPlayerX - labTurrets[i].x) < 40)
          underAttack = true;
      } else { // Horizontal
        if (fabs(labPlayerY - labTurrets[i].y) < 40)
          underAttack = true;
      }
    }
  }

  // Check Falling Panels Intersection
  for (int i = 0; i < LAB_NUM_PANELS; i++) {
    if (labPanels[i].triggered && !labPanels[i].fallen) {
      if (fabs(labPlayerX - (labPanels[i].x + labPanels[i].w / 2)) <
              (labPanels[i].w / 2 + 20) &&
          fabs(labPlayerY - labPanels[i].y) < 60) {
        underAttack = true;
      }
    }
  }

  // Check Electric Floors Intersection
  for (int i = 0; i < LAB_NUM_ELECTRIC; i++) {
    if (labElectricFloors[i].isActive && !labOnCeiling && !labIsJumping) {
      if (labPlayerX > labElectricFloors[i].x &&
          labPlayerX < labElectricFloors[i].x + labElectricFloors[i].w) {
        underAttack = true;
      }
    }
  }

  for (int i = 0; i < LAB_NUM_HEALING; i++) {
    if (labPlayerX > labHealingZones[i].startX &&
        labPlayerX < labHealingZones[i].endX) {
      // Heal only if fully inside and NOT under attack
      if (labPlayerHealth < LAB_MAX_HULL && !underAttack) {
        labPlayerHealth += 0.25; // Gradual constant rate (~15 HP/sec)
        if (labPlayerHealth > LAB_MAX_HULL)
          labPlayerHealth = LAB_MAX_HULL;
      }
    }
  }

  // === ELECTRIC FLOOR LOGIC ===
  for (int i = 0; i < LAB_NUM_ELECTRIC; i++) {
    labElectricFloors[i].timer -= 0.016;
    if (labElectricFloors[i].timer <= 0) {
      labElectricFloors[i].isActive = !labElectricFloors[i].isActive;
      // Burst patterns: 1.2s ON, 1.8s OFF
      labElectricFloors[i].timer = labElectricFloors[i].isActive ? 1.2 : 1.8;
    }

    // Static collision (damage if on floor and active)
    if (labElectricFloors[i].isActive && !labOnCeiling && !labIsJumping &&
        !isSafe) {
      if (labPlayerX > labElectricFloors[i].x &&
          labPlayerX < labElectricFloors[i].x + labElectricFloors[i].w) {
        labPlayerHealth -= 1;
        labDamageFlash = 0.5;
      }
    }
  }

  // === AMBIENT PARTICLES UPDATE ===
  for (int i = 0; i < LAB_NUM_PARTICLES; i++) {
    if (labParticles[i].life <= 0) {
      labParticles[i].x = labPlayerX + (rand() % 1200 - 400);
      labParticles[i].y = labFloorY + (rand() % (int)(labCeilingY - labFloorY));
      labParticles[i].vx = (rand() % 100 - 50) / 400.0;
      labParticles[i].vy = (rand() % 100 - 50) / 400.0;
      labParticles[i].size = 1 + rand() % 2;
      labParticles[i].alpha = 0.1 + (rand() % 30) / 100.0;
      labParticles[i].life = 2.0 + (rand() % 300) / 100.0;
    } else {
      labParticles[i].x += labParticles[i].vx;
      labParticles[i].y += labParticles[i].vy;
      labParticles[i].life -= 0.016;
      labParticles[i].alpha -= 0.0005;
    }
  }

  // === TURRET LOGIC ===
  for (int i = 0; i < LAB_NUM_TURRETS; i++) {
    // 1. SWEEP MOVEMENT
    if (labTurrets[i].sweepRange > 0) {
      labTurrets[i].sweepTime += 0.016 * labTurrets[i].sweepSpeed;
      double offset = sin(labTurrets[i].sweepTime) * labTurrets[i].sweepRange;
      if (labTurrets[i].direction == 2) { // Down-firing
        labTurrets[i].x = labTurrets[i].baseX + offset;
      } else { // Horizontal
        labTurrets[i].y = labTurrets[i].baseY + offset;
      }
    }

    // 2. FIRE PATTERNS & BEAM STATE
    if (labTurrets[i].beamActive) {
      labTurrets[i].beamTimer -= 0.016;
      if (labTurrets[i].beamTimer <= 0) {
        labTurrets[i].beamActive = false;
        labTurrets[i].fireTimer = labTurrets[i].fireInterval;

        // Handle Pattern Progression
        if (labTurrets[i].patternType == 2) { // Burst
          labTurrets[i].burstCount++;
          if (labTurrets[i].burstCount < 3)
            labTurrets[i].fireTimer = 0.2; // Rapid burst
          else
            labTurrets[i].burstCount = 0;
        } else if (labTurrets[i].patternType == 1) { // Double
          labTurrets[i].burstCount++;
          if (labTurrets[i].burstCount < 2)
            labTurrets[i].fireTimer = 0.4;
          else
            labTurrets[i].burstCount = 0;
        }
      }

      // Laser collision logic (shared)
      double tx = labTurrets[i].x, ty = labTurrets[i].y;
      bool hit = false;
      if (labTurrets[i].direction == 0 && labPlayerX > tx + 33 &&
          fabs(labPlayerY - ty) < 20)
        hit = true;
      else if (labTurrets[i].direction == 1 && labPlayerX < tx - 33 &&
               fabs(labPlayerY - ty) < 20)
        hit = true;
      else if (labTurrets[i].direction == 2 && fabs(labPlayerX - tx) < 25 &&
               labPlayerY < ty && labPlayerY > labFloorY - 50)
        hit = true;

      if (hit) {
        labPlayerHealth -= 1;
        labDamageFlash = 1.0;
      }
    } else {
      labTurrets[i].fireTimer -= 0.016;
      // Warning stage
      if (labTurrets[i].fireTimer < 0.6)
        labTurrets[i].isWarning = true;
      else
        labTurrets[i].isWarning = false;

      if (labTurrets[i].fireTimer <= 0) {
        labTurrets[i].beamActive = true;
        labTurrets[i].beamTimer = 1.0; // Slightly longer beam for learnability
        labTurrets[i].isWarning = false;
      }
    }
  }

  // === FALLING PANEL LOGIC ===
  for (int i = 0; i < LAB_NUM_PANELS; i++) {
    if (!labPanels[i].triggered && !labPanels[i].fallen) {
      // Detect player at fair distance
      if (fabs(labPlayerX - (labPanels[i].x + labPanels[i].w / 2)) < 250) {
        labPanels[i].triggered = true;
        labPanels[i].warningTimer = 1.8; // Long shake warning
      }
    }
    if (labPanels[i].triggered && !labPanels[i].fallen) {
      if (labPanels[i].warningTimer > 0) {
        labPanels[i].warningTimer -= 0.016;
        labPanels[i].isWarning = true;
      } else {
        labPanels[i].isWarning = false;
        double fallAccel = (labPanels[i].sizeType == 0) ? 0.6 : 0.3;
        labPanels[i].fallSpeed += fallAccel;
        labPanels[i].y -= labPanels[i].fallSpeed;
      }

      bool panelHit = (labPanels[i].y < labPlayerY + 25 &&
                       labPanels[i].y + labPanels[i].h > labPlayerY - 25 &&
                       labPlayerX > labPanels[i].x - 25 &&
                       labPlayerX < labPanels[i].x + labPanels[i].w + 25);

      if (panelHit) {
        labPlayerHealth -= (labPanels[i].sizeType == 1 ? 30 : 15);
        labDamageFlash = 1.0;
        labPanels[i].fallen = true;
      }
      if (labPanels[i].y <= labFloorY)
        labPanels[i].fallen = true;
    }
  }

  // Check player death
  if (labPlayerHealth <= 0) {
    labPlayerHealth = 0;
    gameState = MENU;
  }

  // Check level completion (Reached the Core)
  if (labPlayerX >= labCorridorLength - 50) {
    gameState = PUZZLE_TRANSITION;
    puzzleTransitionPhase = 0;
    puzzleTransitionTimer = 3.0; // Stay on black screen for 3 seconds
  }
}

// Forward declarations
void drawPuzzleTransition();
void drawSurpriseScreen();
void drawPuzzle();
void drawGravityNode(int x, int y, int size, GravityTile t);

void iDraw() {
  iClear();

  if (gameState == MENU) {
    drawMenu();
  } else if (gameState == LEVEL1) {
    drawLevel1();
  } else if (gameState == CREDITS_STATE) {
    drawCredits();
  } else if (gameState == OPTIONS_STATE) {
    drawOptions();
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
  } else if (gameState == BOSS_DEAD_IMAGE) {
    drawBossDeadImage();
  } else if (gameState == LAB_TRANSITION) {
    drawLabTransition();
  } else if (gameState == LAB_LEVEL) {
    drawLabLevel();
  } else if (gameState == PUZZLE_TRANSITION) {
    drawPuzzleTransition();
  }
}

void drawPuzzleTransition() {
  // Cinematic transition using 2nd bg.png
  iSetColor(0, 0, 0);
  iFilledRectangle(0, 0, 1024, 640);

  if (puzzleTransitionPhase >= 1) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, (float)puzzleTransitionAlpha);
    iShowImage(0, 0, 1024, 640, puzzleTransitionBG);
    glDisable(GL_BLEND);
  }

  // Draw message
  if (puzzleTransitionPhase == 0) {
    // Neon blue Level Cleared text (no jumping)
    static double clearPulse = 0;
    clearPulse += 0.05;
    int pulseIntensity = (int)(180 + 75 * sin(clearPulse));

    // Broader neon blue lines (frames)
    iSetColor(0, pulseIntensity, pulseIntensity);
    iFilledRectangle(250, 360, 524, 4); // Top broad line
    iFilledRectangle(250, 280, 524, 4); // Bottom broad line

    // Neon blue text
    iSetColor(80, 230, 255);
    iText(420, 315, "LEVEL CLEARED", GLUT_BITMAP_TIMES_ROMAN_24);
  } else if (puzzleTransitionPhase == 1) {
    // Mission Objective: Neon Amber/Cyan Glow
    static double objectivePulse = 0;
    objectivePulse += 0.05;
    double pulseAlpha = 0.5 + 0.5 * sin(objectivePulse);

    // Mission Frame (Neon Cyan/White)
    iSetColor(0, 200, 255);
    iRectangle(280, 290, 464, 60); // Main frame
    int val = (int)(150 + 105 * pulseAlpha);
    iSetColor(0, val, val);
    iRectangle(275, 285, 474, 70); // Outlier glow rectangle

    // Mission Header
    iSetColor(255, 255, 255);
    iText(290, 332, "MISSION OBJECTIVE:", GLUT_BITMAP_HELVETICA_10);

    // Glowing Objective Text
    iSetColor(255, 220, 0); // Amber/Gold for primary goal
    iText(340, 310, "STABILIZE THE GRAVITY CORE", GLUT_BITMAP_TIMES_ROMAN_24);

    // Sub-text or Scanline effect
    if (rand() % 10 < 2) { // Random flicker
      iSetColor(255, 255, 255);
      iLine(280, 320, 744, 320);
    }
  }

  // Final fade to black overlay if in Phase 2
  if (puzzleTransitionPhase == 2) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, (float)(1.0 - puzzleTransitionAlpha));
    iFilledRectangle(0, 0, 1024, 640);
    glDisable(GL_BLEND);
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
  // Holographic Frame
  iSetColor(0, 80, 100);
  iRectangle(x, y, size, size);
  iSetColor(0, 40, 60);
  iRectangle(x + 2, y + 2, size - 4, size - 4);

  int mid = size / 2;
  int conduitThick = 12;

  auto drawConduit = [&](int x1, int y1, int w, int h, bool vertical) {
    // Base Tube (Dark)
    iSetColor(20, 30, 40);
    iFilledRectangle(x1, y1, w, h);

    // Cyan Glow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (t.isActive)
      glColor4f(0.0f, 0.8f, 1.0f, 0.4f);
    else
      glColor4f(0.3f, 0.4f, 0.5f, 0.2f);

    if (vertical)
      iFilledRectangle(x1 + 2, y1, w - 4, h);
    else
      iFilledRectangle(x1, y1 + 2, w, h - 4);

    // Inner Core (White/Bright)
    if (t.isActive)
      iSetColor(200, 240, 255);
    else
      iSetColor(100, 110, 120);

    // Bloom effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.4f, 0.4f, 0.4f);
    // This part of the code edit seems misplaced. It refers to
    // `labTurrets[i].direction`, `tx`, `ty`, `labFloorY` which are not defined
    // in `drawGravityNode`. Assuming it was meant to be added elsewhere. For
    // now, commenting it out to avoid compilation errors. if
    // (labTurrets[i].direction == 2)
    //   iFilledRectangle(tx - 6, labFloorY, 12, ty - labFloorY - 15);
    glDisable(GL_BLEND);
    if (vertical)
      iFilledRectangle(x1 + 5, y1, 2, h);
    else
      iFilledRectangle(x1, y1 + 5, w, 2);

    // Energy Pulse (Animating)
    if (t.isActive) {
      iSetColor(255, 220, 100);
      double pSize = 15;
      if (vertical) {
        double py = y1 + (h - pSize) * puzzleEnergyTimer;
        iFilledRectangle(x1 + 2, py, w - 4, pSize);
      } else {
        double px = x1 + (w - pSize) * puzzleEnergyTimer;
        iFilledRectangle(px, y1 + 2, pSize, h - 4);
      }
    }
    glDisable(GL_BLEND);
  };

  if (t.type == 0) { // Straight
    if (t.rotation % 2 == 0)
      drawConduit(x, y + mid - 6, size, 12, false); // Horz
    else
      drawConduit(x + mid - 6, y, 12, size, true); // Vert
  } else if (t.type == 1) {                        // L-Shape
    if (t.rotation == 0) {                         // Right-Top
      drawConduit(x + mid - 6, y + mid - 6, mid + 6, 12, false);
      drawConduit(x + mid - 6, y + mid - 6, 12, mid + 6, true);
    } else if (t.rotation == 1) { // Right-Bottom
      drawConduit(x + mid - 6, y + mid - 6, mid + 6, 12, false);
      drawConduit(x + mid - 6, y, 12, mid + 6, true);
    } else if (t.rotation == 2) { // Left-Bottom
      drawConduit(x, y + mid - 6, mid + 6, 12, false);
      drawConduit(x + mid - 6, y, 12, mid + 6, true);
    } else if (t.rotation == 3) { // Left-Top
      drawConduit(x, y + mid - 6, mid + 6, 12, false);
      drawConduit(x + mid - 6, y + mid - 6, 12, mid + 6, true);
    }
  } else if (t.type == 2) { // Cross
    drawConduit(x, y + mid - 6, size, 12, false);
    drawConduit(x + mid - 6, y, 12, size, true);
  } else if (t.type == 3) { // T-Junction
    if (t.rotation == 0) {  // LTR (3,1,0)
      drawConduit(x, y + mid - 6, size, 12, false);
      drawConduit(x + mid - 6, y + mid - 6, 12, mid + 6, true);
    } else if (t.rotation == 1) { // TRB (0,1,2)
      drawConduit(x + mid - 6, y, 12, size, true);
      drawConduit(x + mid - 6, y + mid - 6, mid + 6, 12, false);
    } else if (t.rotation == 2) { // RBL (1,2,3)
      drawConduit(x, y + mid - 6, size, 12, false);
      drawConduit(x + mid - 6, y, 12, mid + 6, true);
    } else if (t.rotation == 3) { // BLT (2,3,0)
      drawConduit(x + mid - 6, y, 12, size, true);
      drawConduit(x, y + mid - 6, mid + 6, 12, false);
    }
  }
}

void drawPuzzle() {
  // 1. Deep Blue Gradient Background
  for (int i = 0; i < 640; i++) {
    int r = 5 * (640 - i) / 640;
    int g = 10 * (640 - i) / 640;
    int b = 30 * (640 - i) / 640;
    iSetColor(r, g, b);
    iLine(0, i, 1024, i);
  }

  // 2. Holographic Grid Layer
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(0.0f, 0.5f, 0.8f, 0.15f);
  for (int i = 0; i < 1024; i += 40)
    iLine(i, 0, i, 640);
  for (int i = 0; i < 640; i += 40)
    iLine(0, i, 1024, i);

  // 3. Environmental Side Panels (Monitors)
  iSetColor(10, 20, 30);
  iFilledRectangle(20, 100, 250, 440);  // Left Panel
  iFilledRectangle(754, 100, 250, 440); // Right Panel

  iSetColor(0, 100, 150);
  iRectangle(25, 105, 240, 430);
  iRectangle(759, 105, 240, 430);

  iSetColor(0, 200, 255);
  iText(35, 510, "GRAVITY CORE STATUS", GLUT_BITMAP_HELVETICA_12);
  iText(35, 490, "INSTABILITY: 87.4%", GLUT_BITMAP_HELVETICA_10);
  iText(770, 510, "DIAGNOSTIC FEED", GLUT_BITMAP_HELVETICA_12);
  iText(770, 490, "0xAF32 0100 1101", GLUT_BITMAP_HELVETICA_10);

  // 4. Floating Particles
  for (int i = 0; i < 40; i++) {
    glColor4f(0.6f, 0.9f, 1.0f, (float)puzzleParticles[i].alpha);
    iFilledCircle(puzzleParticles[i].x, puzzleParticles[i].y,
                  puzzleParticles[i].size);
  }
  glDisable(GL_BLEND);

  double dx = 0, dy = 0;
  if (!puzzleCleared && puzzleTimer < 10.0) {
    // EXTREME STRESS: Camera shake increases as time drops
    double stress = (10.0 - puzzleTimer) / 10.0;
    dx = (rand() % 9 - 4) * stress; // Up to 4px shake
    dy = (rand() % 9 - 4) * stress;

    // MELTDOWN ALERTS
    iSetColor(255, 0, 0);
    iText(770 + (int)dx, 470 + (int)dy, "CRITICAL: MELTDOWN",
          GLUT_BITMAP_HELVETICA_10);
    iText(770 + (int)dx, 455 + (int)dy, "STABILIZATION REQUIRED",
          GLUT_BITMAP_HELVETICA_10);
  } else if (!puzzleCleared && puzzleTimer < 25.0) {
    dx = (rand() % 3) - 1;
    dy = (rand() % 3) - 1;
  }

  // UI - Top Banner
  iSetColor(255, 255, 255);
  iText(400 + dx, 600 + dy, "STABILIZE GRAVITY CORE", GLUT_BITMAP_HELVETICA_18);

  char timeBuf[20];
  sprintf_s(timeBuf, "TIME: %.1fs", puzzleTimer);
  if (puzzleTimer < 10.0)
    iSetColor(255, 50, 50); // Warning Red
  else
    iSetColor(0, 200, 255); // Docile Cyan
  iText(850 + dx, 600 + dy, timeBuf, GLUT_BITMAP_TIMES_ROMAN_24);

  // 5. CORE and LOCK Terminals (Hexagonal Style)
  auto drawHexNode = [&](int x, int y, const char *label, bool active) {
    if (active)
      iSetColor(0, 255, 150);
    else
      iSetColor(200, 50, 50);

    // Outer Glow
    glEnable(GL_BLEND);
    if (active)
      glColor4f(0.0f, 1.0f, 0.6f, 0.3f);
    else
      glColor4f(0.8f, 0.2f, 0.2f, 0.3f);
    iFilledCircle(x, y, 45);
    glDisable(GL_BLEND);

    // Base
    iSetColor(10, 15, 20);
    iFilledCircle(x, y, 35);
    if (active)
      iSetColor(0, 255, 150);
    else
      iSetColor(200, 50, 50);
    iCircle(x, y, 35);
    iCircle(x, y, 32);

    iText(x - 22, y - 5, (char *)label, GLUT_BITMAP_HELVETICA_18);
  };

  drawHexNode(200 + dx, 470 + dy, "CORE", true);
  drawHexNode(824 + dx, 150 + dy, "LOCK", puzzleCleared);

  // 6. Connector Cables (Visual decoration)
  iSetColor(30, 40, 50);
  iLine(235 + dx, 470 + dy, 312, 470); // To Start
  iLine(712, 170, 789 + dx, 150 + dy); // From End (Bottom 120+50=170 -> 150)

  // 7. Grid of Tiles
  int startX = 312, startY = 120;
  int tileSize = 100;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      drawGravityNode(startX + j * tileSize + dx,
                      startY + (3 - i) * tileSize + dy, tileSize, grid[i][j]);
    }
  }

  // 8. Success Message
  if (puzzleCleared) {
    glEnable(GL_BLEND);
    glColor4f(0.0f, 1.0f, 0.5f, 0.2f);
    iFilledRectangle(0, 260, 1024, 80);
    glDisable(GL_BLEND);

    iSetColor(0, 255, 150);
    iText(startX + 65, 300, "SYSTEM STABILIZED!", GLUT_BITMAP_TIMES_ROMAN_24);
  }

  // 9. Chaos Warning
  if (puzzleTimer < 10.0 && !puzzleCleared) {
    static double alertAlpha = 0;
    alertAlpha += 0.1;
    glEnable(GL_BLEND);
    glColor4f(1.0f, 0.0f, 0.0f, (float)(0.15 + 0.1 * sin(alertAlpha)));
    iFilledRectangle(0, 0, 1024, 640);
    glDisable(GL_BLEND);

    iSetColor(255, 50, 50);
    iText(412, 580, "!!! CRITICAL CORE SPIKE !!!", GLUT_BITMAP_HELVETICA_18);
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
  if (key == 'm' || key == 'M') {
    if (gameState != LEVEL1 && gameState != BOSS_LEVEL) {
      gameState = MENU;
    }
  }

  // Quick test: press T on menu to jump to boss level
  if ((key == 't' || key == 'T') && gameState == MENU) {
    gameState = BOSS_LEVEL;
    p3InitPhase3();
  }

  // Quick test: press L on menu to jump to lab level
  if ((key == 'l' || key == 'L') && gameState == MENU) {
    gameState = LAB_LEVEL;
    initLabLevel();
  }

  // Laboratory shortcut: Dash
  if (gameState == LAB_LEVEL) {
    if ((key == 'x' || key == 'X') && labDashCooldown <= 0) {
      labDashTimer = 0.4;
      labDashCooldown = 3.0;
    }
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
    if (key == 'p' || key == 'P') {
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
        phase3BossHealth -= 10;
        phase3AtkCooldown = 1.0;
      }
    }
  }
}

void iKeyUp(unsigned char key) { keyStates[key] = false; }

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
}

void iSpecialKeyUp(unsigned char key) {
  if (gameState == BOSS_LEVEL) {
    keyStates[key] = false;
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
      // Check LAB LEVEL button (NEW)
      else if (mx >= 98 && mx <= 248 && my >= 150 && my <= 190) {
        gameState = LAB_LEVEL;
        initLabLevel();
      }
      // Check EXIT button
      else if (mx >= 98 && mx <= 248 && my >= 100 && my <= 140) {
        exit(0);
      }

    } else if (gameState == LEVEL1) {
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
    } else if (gameState == OPTIONS_STATE) {
      if (mx >= 350 && mx <= 550 && my >= 130 && my <= 170) {
        gameState = LAB_LEVEL;
        initLabLevel();
      } else {
        gameState = MENU;
      }
    } else if (gameState == CREDITS_STATE) {
      // Click anywhere to go back
      gameState = MENU;
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
          puzzlePlayerInteracted = true;
        }
      }
    } else if (gameState == WIN_SCREEN) {
      gameState = FINAL_BG;
    } else if (gameState == BOSS_DEAD_IMAGE) {
      gameState = LAB_TRANSITION;
      labTransitionTimer = 3.0;
      labTransitionPhase = 0;
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
      gameState = PUZZLE_TRANSITION;
      puzzleTransitionTimer = 6.0;
      puzzleTransitionPhase = 0;
      puzzleTransitionAlpha = 0;
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
    puzzleEnergyTimer += 0.05;
    if (puzzleEnergyTimer > 1.0)
      puzzleEnergyTimer -= 1.0;

    for (int i = 0; i < 40; i++) {
      puzzleParticles[i].y += puzzleParticles[i].speed;
      if (puzzleParticles[i].y > 640) {
        puzzleParticles[i].y = -10;
        puzzleParticles[i].x = rand() % 1024;
      }
    }
    // Check for completion ONLY after interaction
    if (puzzlePlayerInteracted && isPuzzleAlreadySolved()) {
      puzzleCleared = true;
    }

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
                return true;
              if (type == 3) { // T-Junction
                if (rot == 0)
                  return (port == 3 || port == 1 || port == 0);
                if (rot == 1)
                  return (port == 0 || port == 1 || port == 2);
                if (rot == 2)
                  return (port == 1 || port == 2 || port == 3);
                if (rot == 3)
                  return (port == 2 || port == 3 || port == 0);
              }
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
  } else if (gameState == LAB_TRANSITION) {
    labTransitionTimer -= 0.016;
    if (labTransitionPhase == 0 && labTransitionTimer <= 0) {
      labTransitionPhase = 1;
      labTransitionTimer = 2.0;
    } else if (labTransitionPhase == 1 && labTransitionTimer <= 0) {
      labTransitionPhase = 2;
      labTransitionTimer = 0.5;
    } else if (labTransitionPhase == 2 && labTransitionTimer <= 0) {
      gameState = LAB_LEVEL;
      initLabLevel();
    }
  } else if (gameState == LAB_LEVEL) {
    updateLabLevel();
  } else if (gameState == PUZZLE_TRANSITION) {
    puzzleTransitionTimer -= 0.016;
    if (puzzleTransitionPhase == 0) {
      // Phase 0: Show Level Cleared (6.0 to 3.0s)
      puzzleTransitionAlpha = 0;
      if (puzzleTransitionTimer <= 3.0) {
        puzzleTransitionPhase = 1;
      }
    } else if (puzzleTransitionPhase == 1) {
      // Phase 1: Show background and message (3.0 to 1.0s)
      puzzleTransitionAlpha += 0.02; // Fade in BG
      if (puzzleTransitionAlpha > 1.0)
        puzzleTransitionAlpha = 1.0;

      if (puzzleTransitionTimer <= 1.0) {
        puzzleTransitionPhase = 2;
        puzzleTransitionAlpha = 1.0;
      }
    } else if (puzzleTransitionPhase == 2) {
      // Phase 2: Fade out to puzzle (3 to 4s)
      puzzleTransitionAlpha -= 0.02; // Fade out UI/BG
      if (puzzleTransitionAlpha < 0)
        puzzleTransitionAlpha = 0;

      if (puzzleTransitionTimer <= 0) {
        gameState = PUZZLE_SCREEN;
        initPuzzle();
      }
    }
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
  chatgptImage =
      iLoadImage("Assets/ChatGPT Image Feb 28, 2026, 01_02_51 AM.png");
  puzzleTransitionBG = iLoadImage("Assets/2nd bg.png");

  // Opening/Loading the audio files
  // mciSendString("open \"Audios/background.mp3\" alias bgsong", NULL, 0,
  // NULL); mciSendString("open \"Audios/gameover.mp3\" alias ggsong", NULL, 0,
  // NULL);

  // Playing the background audio on repeat
  // mciSendString("play bgsong repeat", NULL, 0, NULL);

  iStart();
  return 0;
}
