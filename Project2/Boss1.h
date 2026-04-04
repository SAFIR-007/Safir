#ifndef BOSS1_H
#define BOSS1_H

#include "GameShared.h"

enum BossState { P3_OBSERVE, P3_HUNT, P3_EXECUTE, P3_DUEL, P3_DEAD };
enum Phase3Attack { P3_ATK_NONE, P3_ATK_HEAVY, P3_ATK_FEINT, P3_ATK_SHOCKWAVE };

struct P3Shadow {
  double x, y, w, h;
};

struct P3Pillar {
  double x, y, w, h;
};

struct P3Seal {
  double x, y;
  bool activated;
};

extern double phase3PlayerX;
extern double phase3PlayerY;
extern int phase3PlayerSpeed;
extern bool phase3CrouchMode;
extern int phase3PlayerHealth;
extern double phase3NoiseLevel;
extern bool phase3CriticalStrikeReady;
extern double phase3DodgeTimer;
extern bool phase3DodgeActive;
extern double phase3DodgeDX, phase3DodgeDY;
extern double phase3AtkCooldown;

extern bool phase3DecoyActive;
extern double phase3DecoyX;
extern double phase3DecoyY;
extern double phase3DecoyTimer;

extern double phase3BossX;
extern double phase3BossY;
extern int phase3BossHealth;
extern int phase3AwarenessLevel;

extern BossState phase3BossState;
extern bool strikeWindowActive;
extern double phase3SearchAngle;
extern double phase3SearchDir;
extern double phase3BossLastSeenX;
extern double phase3BossLastSeenY;
extern double phase3BossMoveTimer;
extern Phase3Attack phase3BossAttack;
extern double phase3BossAtkTimer;
extern double phase3StrikeTimer;
extern double phase3ShockRadius;
extern bool phase3ShockActive;
extern double phase3BossVictoryTimer;
extern double phase3BossChargeX;
extern double phase3BossChargeY;
extern int phase3SealsActivated;

extern P3Shadow phase3Shadows[6];
extern P3Pillar phase3Pillars[4];
extern P3Seal phase3Seals[3];

extern unsigned int finalBG;
extern unsigned int digitalRainBG;
extern unsigned int secondBG;
extern double intelTimer;
extern double warningTimer;
extern double secondBGTimer;

bool p3IsKeyPressed(unsigned char k);
void startBossLevel();
void p3InitPhase3();
bool phase3PlayerInShadow();
bool phase3PlayerInSearchlight();
void drawPhase3Arena();
void drawPhase3Boss();
void drawPhase3Player();
void drawPhase3HUD();
void handlePhase3Stealth();
void handlePhase3Combat();
void updatePhase3Player();
void updatePhase3Boss();
void drawIntel();
void drawWarning();
void drawSecondBG();
void handlePhase3KeyPress(unsigned char key);
void handlePhase3MouseClick();
void updateWarningScreen();
void updateSecondBGScreen();
void updatePhase3Level();



inline void p3InitPhase3() {
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

inline void startBossLevel() {
  gameState = BOSS_LEVEL;
  p3InitPhase3();
}

inline bool phase3PlayerInShadow() {
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

inline bool phase3PlayerInSearchlight() {
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

inline void drawPhase3Arena() {
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

inline void drawPhase3Boss() {
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

inline void drawPhase3Player() {
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

inline void drawPhase3HUD() {
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

inline void handlePhase3Stealth() {
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

inline void handlePhase3Combat() {
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

inline void handlePhase3KeyPress(unsigned char key) {
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

inline void handlePhase3MouseClick() {
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

inline void updatePhase3Player() {
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

inline void updatePhase3Boss() {
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

inline void drawIntel() {
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

inline void drawWarning() {
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

inline void drawSecondBG() { iShowImage(0, 0, 1024, 640, secondBG); }

inline void drawFinalBG() { iShowImage(0, 0, 1024, 640, finalBG); }

inline bool p3IsKeyPressed(unsigned char k) { return keyPressed[k] || keyStates[k]; }

inline void updateWarningScreen() {
  warningTimer -= 0.016;
  if (warningTimer <= 0) {
    gameState = SECOND_BG;
    secondBGTimer = 10.0;
  }
}

inline void updateSecondBGScreen() {
  secondBGTimer -= 0.016;
  if (secondBGTimer <= 0) {
    startBossLevel();
  }
}

inline void updatePhase3Level() {
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

#endif
