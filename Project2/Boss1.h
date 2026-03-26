#ifndef BOSS1_H
#define BOSS1_H

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

extern unsigned int digitalRainBG;
extern unsigned int secondBG;
extern double intelTimer;
extern double warningTimer;
extern double secondBGTimer;

bool p3IsKeyPressed(unsigned char k);
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

#endif
