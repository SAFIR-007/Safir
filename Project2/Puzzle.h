#ifndef PUZZLE_H
#define PUZZLE_H

#include "GameShared.h"

struct GravityTile {
  int type;     // 0: Straight, 1: L-shape, 2: Cross
  int rotation; // 0: 0, 1: 90, 2: 180, 3: 270 degrees
  bool isActive;
};

struct PuzzleParticle {
  double x, y;
  double speed;
  double alpha;
  int size;
};

extern unsigned int chatgptImage;
extern unsigned int puzzleTransitionBG;

extern int puzzleTransitionPhase;
extern double puzzleTransitionTimer;
extern double puzzleTransitionAlpha;

extern GravityTile grid[4][4];
extern double surpriseTimer;
extern double puzzleTimer;
extern bool puzzleCleared;
extern bool puzzlePlayerInteracted;
extern double screenShake;
extern double intelTimer;
extern double warningTimer;
extern double secondBGTimer;
extern double puzzleEnergyTimer;
extern PuzzleParticle puzzleParticles[40];

extern unsigned int finalBG;

bool isTilePortOpen(int row, int col, int port);
bool isPuzzleAlreadySolved();
void initPuzzle();
void startPuzzleSequence();
void handlePuzzleMouseClick(int mx, int my);
void updatePuzzleTransition();
void updateSurpriseScreen();
void updatePuzzleScreen();
void drawSurpriseScreen();
void drawGravityNode(int x, int y, int size, GravityTile t);
void drawPuzzle();



inline void initPuzzle() {
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

inline void startPuzzleSequence() {
  gameState = SURPRISE_SCREEN;
  surpriseTimer = 5.0;
  initPuzzle();
}

inline void drawPuzzleTransition() {
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

inline void drawSurpriseScreen() {
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

inline void drawGravityNode(int x, int y, int size, GravityTile t) {
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

inline void drawPuzzle() {
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

inline bool isTilePortOpen(int row, int col, int port) {
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

inline bool isPuzzleAlreadySolved() {
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

inline void handlePuzzleMouseClick(int mx, int my) {
  int startX = 312, startY = 120;
  int tileSize = 100;
  if (mx >= startX && mx <= startX + 400 && my >= startY &&
      my <= startY + 400) {
    int col = (mx - startX) / tileSize;
    int row = 3 - (my - startY) / tileSize;
    if (row >= 0 && row < 4 && col >= 0 && col < 4) {
      grid[row][col].rotation = (grid[row][col].rotation + 1) % 4;
      puzzlePlayerInteracted = true;
    }
  }
}

inline void updatePuzzleTransition() {
  puzzleTransitionTimer -= 0.016;
  if (puzzleTransitionTimer <= 3.0 && puzzleTransitionPhase == 0) {
    puzzleTransitionPhase = 1;
  }
  if (puzzleTransitionPhase == 1) {
    puzzleTransitionAlpha += 0.01;
    if (puzzleTransitionAlpha > 1.0)
      puzzleTransitionAlpha = 1.0;
  }
  if (puzzleTransitionTimer <= 0) {
    gameState = PUZZLE_SCREEN;
    initPuzzle();
  }
}

inline void updateSurpriseScreen() {
  surpriseTimer -= 0.016;
  if (surpriseTimer <= 0) {
    gameState = PUZZLE_SCREEN;
    initPuzzle();
  }
}

inline void updatePuzzleScreen() {
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

  if (puzzlePlayerInteracted && isPuzzleAlreadySolved()) {
    puzzleCleared = true;
  }

  if (!puzzleCleared) {
    puzzleTimer -= 0.016;

    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        grid[i][j].isActive = false;

    bool visited[4][4] = {false};
    int stackR[16], stackC[16], top = -1;

    stackR[++top] = 0;
    stackC[top] = 0;
    visited[0][0] = true;

    while (top >= 0) {
      int r = stackR[top], c = stackC[top--];
      grid[r][c].isActive = true;
      if (r == 3 && c == 3)
        puzzleCleared = true;

      int dr[] = {-1, 1, 0, 0};
      int dc[] = {0, 0, -1, 1};
      int p[] = {0, 2, 3, 1};
      int opp[] = {2, 0, 1, 3};

      for (int i = 0; i < 4; i++) {
        int nr = r + dr[i], nc = c + dc[i];
        if (nr >= 0 && nr < 4 && nc >= 0 && nc < 4 && !visited[nr][nc]) {
          auto isOpen = [](int row, int col, int port) -> bool {
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
            if (type == 3) {
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
        gameState = MENU;
        surpriseTimer = 5.0;
      }
    }
  }

  if (puzzleCleared) {
    puzzleTimer = 30.0;
    static double winDelay = 3.0;
    winDelay -= 0.016;
    if (winDelay <= 0) {
      gameState = INTEL_SCREEN;
      intelTimer = 5.0;
      winDelay = 3.0;
    }
  }
}

#endif
