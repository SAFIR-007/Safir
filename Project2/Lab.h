#ifndef LAB_H
#define LAB_H

#include "GameShared.h"

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
#define LAB_NUM_TURRETS 8
#define LAB_NUM_PANELS 6
#define LAB_NUM_AGZONES 3
#define LAB_NUM_ELECTRIC 4
#define LAB_NUM_SPEED 2
#define LAB_NUM_HEALING 3

struct HealingZone {
  double startX, endX;
};

extern LabAmbientParticle labParticles[LAB_NUM_PARTICLES];
extern double labScrollX;
extern double labPlayerX, labPlayerY;
extern double labPlayerVY;
extern bool labOnCeiling;
extern bool labIsJumping;
extern double labPlayerHealth;
extern bool labPlayerFacingRight;
extern double labCorridorLength;
extern double labFloorY;
extern double labCeilingY;

extern double labTransitionTimer;
extern double labTransitionAlpha;
extern int labTransitionPhase; // 0=fade out boss, 1=show title, 2=fade into lab
extern double labDamageFlash;

// Survival Refinement
extern double labDashTimer;
extern double labDashCooldown;
extern double labInstructionTimer;
extern double labWalkAnimTimer;
extern int labWalkFrame;
extern bool labPlayerMoving;
extern const double LAB_MAX_HEALTH;
extern const double LAB_MAX_HULL;
extern double warningTimer;
extern unsigned int playerSpritesheet;
extern unsigned int newPlayerSpritesheet;

extern LabTurret labTurrets[LAB_NUM_TURRETS];
extern LabPanel labPanels[LAB_NUM_PANELS];
extern AGZone labAGZones[LAB_NUM_AGZONES];
extern ElectricFloor labElectricFloors[LAB_NUM_ELECTRIC];
extern SpeedZone labSpeedZones[LAB_NUM_SPEED];
extern HealingZone labHealingZones[LAB_NUM_HEALING];

void startLabLevel();
void startLabTransition();
void initLabLevel();
void handleLabKeyboard(unsigned char key);
void drawLabTransition();
void drawLabInstructions();
void drawLabLevel();
void updateLabTransition();
void updateLabLevel();



inline void initLabLevel() {
  labScrollX = 0;
  labPlayerX = 150;
  labPlayerY = labFloorY + 25;
  labInstructionTimer = 7.0;
  labWalkAnimTimer = 0;
  labWalkFrame = 0;
  labPlayerMoving = false;
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

inline void startLabLevel() {
  gameState = LAB_LEVEL;
  initLabLevel();
}

inline void startLabTransition() {
  gameState = LAB_TRANSITION;
  labTransitionTimer = 1.0;
  labTransitionPhase = 0;
}

inline void handleLabKeyboard(unsigned char key) {
  if ((key == 'x' || key == 'X') && labDashCooldown <= 0) {
    labDashTimer = 0.4;
    labDashCooldown = 3.0;
  }
}

inline void drawLabTransition() {
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

inline void drawLabInstructions() {
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

inline void drawLabLevel() {
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

  if (labPlayerMoving && !labIsJumping) {
    int fw = 63, fh = 61;
    int sx = (labWalkFrame % 8) * fw;
    int sy = 2 * fh;
    iShowSubImage(-36, -38, 72, 72, newPlayerSpritesheet, sx, sy, fw, fh,
                  1011, 247);
  } else {
    int fw = 64, fh = 64;
    int frameIdx = 2;
    int sx = frameIdx * fw;
    int sy = 0;
    iShowSubImage(-30, -30, 60, 60, playerSpritesheet, sx, sy, fw, fh, 256,
                  64);
  }
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

inline void updateLabTransition() {
  labTransitionTimer -= 0.016;
  if (labTransitionPhase == 0 && labTransitionTimer <= 0) {
    labTransitionPhase = 1;
    labTransitionTimer = 3.5;
  } else if (labTransitionPhase == 1 && labTransitionTimer <= 0) {
    labTransitionPhase = 2;
    labTransitionTimer = 0.5;
  } else if (labTransitionPhase == 2 && labTransitionTimer <= 0) {
    startLabLevel();
  }
}

inline void updateLabLevel() {
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
  labPlayerMoving = false;
  if (keyPressed['d'] || keyPressed['D']) {
    labPlayerX += currentSpeed;
    labPlayerFacingRight = true;
    labPlayerMoving = true;
  }
  if (keyPressed['a'] || keyPressed['A']) {
    labPlayerX -= currentSpeed;
    labPlayerFacingRight = false;
    labPlayerMoving = true;
  }

  if (labPlayerMoving && !labIsJumping) {
    labWalkAnimTimer += (labDashTimer > 0) ? 0.35 : 0.18;
    if (labWalkAnimTimer >= 1.0) {
      labWalkAnimTimer = 0.0;
      labWalkFrame = (labWalkFrame + 1) % 8;
    }
  } else {
    labWalkAnimTimer = 0.0;
    labWalkFrame = 0;
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
    gameState = WARNING_SCREEN;
    warningTimer = 5.0;
  }
}

#endif
