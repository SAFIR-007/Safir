#ifndef SILENTFOUNDRY_H
#define SILENTFOUNDRY_H

#include "GameShared.h"

enum SFBossState {
  SF_PATROL,
  SF_SUSPICIOUS,
  SF_HUNT,
  SF_STUNNED,
  SF_COMBAT,
  SF_DEAD
};

struct SFPlayer {
  double x, y;
  int health;
  bool running, crouching;
  double dashTimer, dashCooldown, dashDX, dashDY;
  bool dashActive;
  double atkCooldown;
  double focusSerumTimer, silentBootsTimer, slowMindTimer, empPulseTimer;
  // Phase 2/3 additions
  int decoyCount;
  int momentumHits;     // consecutive hits without taking damage
  double momentumTimer; // boost duration
  bool momentumActive;
};

struct SFBoss {
  double x, y;
  int hp, shieldLayers;
  SFBossState state;
  double visionAngle, visionDir;
  double patrolX, patrolY;
  double suspicion;
  double huntTimer;
  double atkTimer, stunTimer, victoryTimer;
  double slamRadius;
  bool slamActive, slamHitDealt;
  double chargeX, chargeY;
  bool chargeActive;
  double chargeTimer;
  bool chargeHitDealt;
  double shockRadius;
  bool shockActive;
  int weakSpotHits;
  // Phase 3 micro-puzzle reaction
  bool microPuzzleActive;
  double microPuzzleBar;
  double microPuzzleTimer;
  double microPuzzleTarget;
  bool microPuzzleHit;
  // Phase 2/3 additions
  bool shieldActive; // adaptive: blocks damage
  double shieldTimer;
  bool adaptiveMode;           // Phase 3 adaptive AI on
  double lastSeenX, lastSeenY; // last known player position
  double pulseTimer;           // counter-stealth pulse cooldown
  double pulseRadius;          // expanding ring
  bool pulseActive;
  bool pulseHitDealt;     // New: prevents multiple hits per pulse
  int playerAggression;   // tracks melee attacks
  int playerDodges;       // tracks dash usage
  double aggressionDecay; // reset timer
  bool desperationMode;   // boss <25% HP
  double slowTimer;       // slowed by control panel
};

struct SFTower {
  double x, y;
  bool hacked, locked;
  int attemptsLeft;
  double lockTimer;
};

struct SFDrone {
  double x, y, angle, speed, radius;
  bool active;
};

struct SFPuzzleSystem {
  bool active;
  int towerIndex;
  double sliderPos; // 0.0 to 1.0
  double sliderSpeed;
  int sliderDir;      // 1 or -1
  double targetStart; // 0.0 to 0.8
  double targetWidth; // 0.2 approx
  char feedback[80];
  double feedbackTimer;
  double timer;
  bool timerEnabled;
  double closeTimer;
  bool completed;
};

struct SFPowerup {
  double x, y;
  int type;
  bool active;
  double timer, floatOffset;
};

struct SFShadowZone {
  double x, y, w, h;
};

struct SFSteamVent {
  double x, y, radius;
  bool active;
  double timer, interval;
};

// Phase 2/3 new structs
struct SFControlPanel {
  double x, y;
  int type; // 0=lights, 1=vent, 2=slow
  bool available;
  double cooldown;
};
struct SFDecoy {
  double x, y;
  bool active;
  double timer;
};
struct SFArenaHazard {
  double x, y, w, h;
  bool active;
  double timer;
  bool collapsing; // true=warning, then active=damage
};
struct SFSignalField {
  double x, y, w, h;
};

extern unsigned int chatgptImage;
extern double level2ScreenTimer;

extern SFPlayer sfPlayer;
extern SFBoss sfBoss;
extern SFTower sfTowers[4];
extern SFDrone sfDrones[3];
extern SFPuzzleSystem sfPuzzle;
extern SFPowerup sfPowerups[3];
extern SFShadowZone sfShadows[3];
extern SFSteamVent sfVents[3];
extern double sfScreenShake, sfDamageFlash, sfFlickerTimer;
extern double sfIntroTimer, sfExplosionTimer;
extern bool sfExplosionActive;
extern double sfPowerupSpawnTimer;
extern int sfTowersHacked;
extern int sfPhase; // 1,2,3

extern SFControlPanel sfPanels[3];
extern SFDecoy sfDecoys[2];
extern SFArenaHazard sfHazards[6];
extern SFSignalField sfSignalFields[2];
extern bool sfLightsOff;
extern double sfLightsOffTimer;
extern double sfHazardSpawnTimer;
extern double sfShadowStayTimer;

void sfGeneratePuzzle(int towerIdx);
void sfInit();
bool sfPlayerInShadow();
bool sfPlayerInVisionCone();
int sfPlayerNearTower();
bool sfPlayerInDroneSpotlight();
bool sfPlayerInSignalField();
void activateControlPanel(int i);
void deployDecoy();
void sfUpdatePanels();
void sfUpdateDecoys();
void sfUpdatePulse();
void sfUpdateHazards();
void sfUpdateMomentum();
void sfUpdateAdaptiveAI();
void updateSuspicion();
void updateSFDrones();
void updateSFPlayer();
void updateSFBoss();
void checkPuzzleAnswer();
void sfCheckCollisions();
void spawnSFPowerup();
void drawSFArena();
void drawSFBoss();
void drawSFPlayer();
void renderPuzzleUI();
void renderSFHUD();
void sfUpdateEffects();
void startSilentFoundryLevel();
void drawLevel2Screen();
void handleSilentFoundryKeyPress(unsigned char key);
void handleSilentFoundryMouseClick();
void updateLevel2Screen();
void updateSilentFoundryLevel();



inline void sfGeneratePuzzle(int towerIdx) {
  sfPuzzle.active = true;
  sfPuzzle.towerIndex = towerIdx;
  sfPuzzle.sliderPos = 0.0;
  sfPuzzle.sliderDir = 1;
  // Make it easy: wide target zone
  sfPuzzle.targetWidth = (sfPhase == 1) ? 0.35 : 0.25;
  sfPuzzle.targetStart =
      0.2 + (rand() % 40) / 100.0; // random zone between 0.2 and 0.6
  sfPuzzle.sliderSpeed = (sfPhase == 1) ? 0.02 : 0.035;
  if (sfPlayer.slowMindTimer > 0)
    sfPuzzle.sliderSpeed *= 0.6;

  sfPuzzle.feedback[0] = '\0';
  sfPuzzle.feedbackTimer = 0;
  sfPuzzle.completed = false;
  sfPuzzle.closeTimer = 0;
  sfPuzzle.timerEnabled = (sfPhase >= 2);
  sfPuzzle.timer =
      (sfPhase >= 2) ? 15.0 + ((sfPlayer.slowMindTimer > 0) ? 5.0 : 0) : 999.0;
}

inline void sfInit() {
  sfPlayer.x = 100;
  sfPlayer.y = 100;
  sfPlayer.health = 100;
  sfPlayer.running = false;
  sfPlayer.crouching = false;
  sfPlayer.dashActive = false;
  sfPlayer.dashTimer = 0;
  sfPlayer.dashCooldown = 0;
  sfPlayer.dashDX = 0;
  sfPlayer.dashDY = 0;
  sfPlayer.atkCooldown = 0;
  sfPlayer.focusSerumTimer = 0;
  sfPlayer.silentBootsTimer = 0;
  sfPlayer.slowMindTimer = 0;
  sfPlayer.empPulseTimer = 0;
  sfPlayer.decoyCount = 2;
  sfPlayer.momentumHits = 0;
  sfPlayer.momentumTimer = 0;
  sfPlayer.momentumActive = false;

  sfBoss.x = 512;
  sfBoss.y = 320;
  sfBoss.hp = 1000;
  sfBoss.shieldLayers = 4;
  sfBoss.state = SF_PATROL;
  sfBoss.visionAngle = 0;
  sfBoss.visionDir = 1;
  sfBoss.patrolX = 300;
  sfBoss.patrolY = 300;
  sfBoss.suspicion = 0;
  sfBoss.huntTimer = 0;
  sfBoss.atkTimer = 3.0;
  sfBoss.stunTimer = 0;
  sfBoss.victoryTimer = 3.0;
  sfBoss.slamActive = false;
  sfBoss.slamHitDealt = false;
  sfBoss.slamRadius = 0;
  sfBoss.chargeActive = false;
  sfBoss.chargeHitDealt = false;
  sfBoss.chargeTimer = 0;
  sfBoss.shockActive = false;
  sfBoss.shockRadius = 0;
  sfBoss.weakSpotHits = 0;
  sfBoss.microPuzzleActive = false;
  sfBoss.microPuzzleTimer = 10.0;
  sfBoss.shieldActive = false;
  sfBoss.shieldTimer = 0;
  sfBoss.adaptiveMode = false;
  sfBoss.pulseActive = false;
  sfBoss.pulseTimer = 25.0;
  sfBoss.pulseHitDealt = false;
  sfBoss.playerAggression = 0;
  sfBoss.playerDodges = 0;
  sfBoss.desperationMode = false;
  sfBoss.slowTimer = 0;

  double tx[] = {120, 880, 120, 880};
  double ty[] = {120, 120, 480, 480};
  for (int i = 0; i < 4; i++) {
    sfTowers[i].x = tx[i];
    sfTowers[i].y = ty[i];
    sfTowers[i].hacked = false;
    sfTowers[i].locked = false;
    sfTowers[i].attemptsLeft = 3;
    sfTowers[i].lockTimer = 0;
  }

  for (int i = 0; i < 3; i++) {
    sfDrones[i].active = true;
    sfDrones[i].speed = 0.015 + i * 0.005;
    sfDrones[i].angle = i * 2.1;
    sfDrones[i].radius = 120 + i * 40;
    sfDrones[i].x = 400;
    sfDrones[i].y = 400;
  }

  sfShadows[0].x = 50;
  sfShadows[0].y = 250;
  sfShadows[0].w = 100;
  sfShadows[0].h = 80;
  sfShadows[1].x = 450;
  sfShadows[1].y = 50;
  sfShadows[1].w = 120;
  sfShadows[1].h = 70;
  sfShadows[2].x = 800;
  sfShadows[2].y = 380;
  sfShadows[2].w = 150;
  sfShadows[2].h = 100;

  // Init new Phase 2/3 elements
  sfPanels[0].x = 200;
  sfPanels[0].y = 520;  // Moved down from 560 so player can reach
  sfPanels[0].type = 0; // Lights
  sfPanels[1].x = 900;
  sfPanels[1].y = 300;
  sfPanels[1].type = 1; // Vents
  sfPanels[2].x = 200;
  sfPanels[2].y = 60;   // Moved up from 40 for accessibility
  sfPanels[2].type = 2; // Slow
  for (int i = 0; i < 3; i++) {
    sfPanels[i].available = true;
    sfPanels[i].cooldown = 0;
  }

  sfSignalFields[0].x = 300;
  sfSignalFields[0].y = 400;
  sfSignalFields[0].w = 150;
  sfSignalFields[0].h = 100;
  sfSignalFields[1].x = 600;
  sfSignalFields[1].y = 150;
  sfSignalFields[1].w = 150;
  sfSignalFields[1].h = 100;

  for (int i = 0; i < 6; i++) {
    sfHazards[i].active = false;
    sfHazards[i].collapsing = false;
  }
  for (int i = 0; i < 2; i++) {
    sfDecoys[i].active = false;
  }
  sfLightsOff = false;
  sfLightsOffTimer = 0;

  sfVents[0].x = 350;
  sfVents[0].y = 200;
  sfVents[0].radius = 40;
  sfVents[0].active = false;
  sfVents[0].timer = 3.0;
  sfVents[0].interval = 4.0;
  sfVents[1].x = 650;
  sfVents[1].y = 400;
  sfVents[1].radius = 35;
  sfVents[1].active = false;
  sfVents[1].timer = 5.0;
  sfVents[1].interval = 3.5;
  sfVents[2].x = 500;
  sfVents[2].y = 100;
  sfVents[2].radius = 45;
  sfVents[2].active = false;
  sfVents[2].timer = 2.0;
  sfVents[2].interval = 5.0;

  for (int i = 0; i < 3; i++) {
    sfPowerups[i].active = false;
    sfPowerups[i].timer = 0;
  }
  sfPuzzle.active = false;
  sfPuzzle.completed = false;
  sfPuzzle.closeTimer = 0;
  sfScreenShake = 0;
  sfDamageFlash = 0;
  sfFlickerTimer = 0;
  sfIntroTimer = 3.0;
  sfExplosionActive = false;
  sfExplosionTimer = 0;
  sfPowerupSpawnTimer = 10.0;
  sfTowersHacked = 0;
  sfPhase = 1;
}

inline void startSilentFoundryLevel() {
  gameState = LEVEL2_BOSS;
  sfInit();
}

inline void drawLevel2Screen() {
  iSetColor(0, 0, 0);
  iFilledRectangle(0, 0, 1024, 640);
  int pulse = (int)(80 + 100 * (0.5 + 0.5 * sin(level2ScreenTimer * 2.0)));
  iSetColor(pulse, 0, 0);
  iRectangle(30, 30, 964, 580);
  iRectangle(32, 32, 960, 576);
  iSetColor(180, 0, 0);
  iLine(50, 590, 90, 590);
  iLine(50, 590, 50, 560);
  iLine(934, 50, 974, 50);
  iLine(974, 50, 974, 80);
  iSetColor(80, 0, 0);
  iText(402, 382, "LEVEL 2", GLUT_BITMAP_TIMES_ROMAN_24);
  iSetColor(255, 50, 50);
  iText(400, 384, "LEVEL 2", GLUT_BITMAP_TIMES_ROMAN_24);
  iSetColor(160, 160, 160);
  iText(390, 340, "THE SILENT FOUNDRY", GLUT_BITMAP_HELVETICA_12);
  iSetColor(100, 0, 0);
  iLine(362, 320, 662, 320);
  int sfAlpha = (int)(150 + 105 * sin(level2ScreenTimer * 3.0));
  iSetColor(sfAlpha, sfAlpha, sfAlpha);
  iText(380, 270, "CLICK ANYWHERE TO CONTINUE", GLUT_BITMAP_HELVETICA_18);
}

inline void handleSilentFoundryKeyPress(unsigned char key) {
  keyStates[key] = true;
  if (key == 27 && sfPuzzle.active) {
    sfPuzzle.active = false;
    sfPuzzle.completed = false;
  }
  if (key == ' ' && !sfPuzzle.active) {
    if (!sfPlayer.dashActive && sfPlayer.dashCooldown <= 0) {
      sfPlayer.dashActive = true;
      sfPlayer.dashTimer = 0.2;
      sfPlayer.dashCooldown = 2.0;
      sfPlayer.dashDX = 0;
      sfPlayer.dashDY = 0;
      if (GetAsyncKeyState('W') & 0x8000)
        sfPlayer.dashDY = 1;
      if (GetAsyncKeyState('S') & 0x8000)
        sfPlayer.dashDY = -1;
      if (GetAsyncKeyState('A') & 0x8000)
        sfPlayer.dashDX = -1;
      if (GetAsyncKeyState('D') & 0x8000)
        sfPlayer.dashDX = 1;
      if (sfPlayer.dashDX == 0 && sfPlayer.dashDY == 0)
        sfPlayer.dashDY = -1;
    }
  }
  if (key == 'e' || key == 'E') {
    if (sfPuzzle.active && !sfPuzzle.completed) {
      checkPuzzleAnswer();
    } else if (sfBoss.microPuzzleActive && sfPhase == 3) {
      double t = sfBoss.microPuzzleTarget;
      if (sfBoss.microPuzzleBar >= t - 0.12 &&
          sfBoss.microPuzzleBar <= t + 0.12) {
        sfBoss.microPuzzleActive = false;
        sfBoss.state = SF_STUNNED;
        sfBoss.stunTimer = 3.0;
        sfBoss.microPuzzleHit = true;
      } else {
        sfBoss.microPuzzleActive = false;
      }
    } else if (!sfPuzzle.active && sfBoss.state != SF_HUNT &&
               sfBoss.state != SF_DEAD && sfPhase < 3) {
      int t = sfPlayerNearTower();
      if (t >= 0) {
        sfGeneratePuzzle(t);
      }
    }
  }
}

inline void handleSilentFoundryMouseClick() {
  if (sfBoss.state == SF_DEAD || sfPlayer.health <= 0) {
    return;
  }
  if (sfPhase == 3 && sfPlayer.atkCooldown <= 0) {
    double sfdx = sfPlayer.x - sfBoss.x, sfdy = sfPlayer.y - sfBoss.y;
    double sfdist = sqrt(sfdx * sfdx + sfdy * sfdy);
    if (sfBoss.state == SF_STUNNED && sfdist < 80) {
      sfBoss.hp -= 60;
      sfPlayer.atkCooldown = 0.5;
      sfScreenShake = 0.3;
      sfBoss.weakSpotHits++;
      sfBoss.playerAggression++;
    } else if (sfBoss.state == SF_COMBAT && sfdist < 60) {
      int sfdmg = (sfBoss.shieldActive) ? 5 : 20;
      sfBoss.hp -= sfdmg;
      sfPlayer.atkCooldown = 0.5;
      sfBoss.playerAggression++;
    }
  }
}

inline void updateLevel2Screen() { level2ScreenTimer += 0.016; }

inline void updateSilentFoundryLevel() {
  static bool sfPrevE = false;
  bool sfCurE = (keyPressed['e'] || keyPressed['E']) ? true : false;
  if (sfCurE && !sfPrevE) {
    bool panelUsed = false;
    if (!sfPuzzle.active && !sfBoss.microPuzzleActive) {
      for (int i = 0; i < 3; i++) {
        double dx = sfPlayer.x - sfPanels[i].x, dy = sfPlayer.y - sfPanels[i].y;
        if (sqrt(dx * dx + dy * dy) < 80 && sfPanels[i].available) {
          activateControlPanel(i);
          panelUsed = true;
          break;
        }
      }
    }
    if (!panelUsed) {
      if (sfPuzzle.active && !sfPuzzle.completed) {
        checkPuzzleAnswer();
      } else if (sfBoss.microPuzzleActive && sfPhase == 3) {
        double t = sfBoss.microPuzzleTarget;
        if (sfBoss.microPuzzleBar >= t - 0.12 &&
            sfBoss.microPuzzleBar <= t + 0.12) {
          sfBoss.microPuzzleActive = false;
          sfBoss.state = SF_STUNNED;
          sfBoss.stunTimer = 3.0;
          sfBoss.microPuzzleHit = true;
        } else {
          sfBoss.microPuzzleActive = false;
        }
      } else if (!sfPuzzle.active && sfBoss.state != SF_HUNT &&
                 sfBoss.state != SF_DEAD && sfPhase < 3) {
        int t = sfPlayerNearTower();
        if (t >= 0) {
          sfGeneratePuzzle(t);
        }
      }
    }
  }
  sfPrevE = sfCurE;

  static bool sfPrevQ = false;
  bool sfCurQ = (keyPressed['q'] || keyPressed['Q']) ? true : false;
  if (sfCurQ && !sfPrevQ) {
    deployDecoy();
  }
  sfPrevQ = sfCurQ;

  updateSFPlayer();
  updateSFBoss();
  updateSuspicion();
  updateSFDrones();
  sfCheckCollisions();
  spawnSFPowerup();
  sfUpdateEffects();
  sfUpdatePanels();
  sfUpdateDecoys();
  sfUpdatePulse();
  sfUpdateHazards();
  sfUpdateMomentum();
  sfUpdateAdaptiveAI();
}

inline bool sfPlayerInShadow() {
  for (int i = 0; i < 3; i++) {
    if (sfPlayer.x >= sfShadows[i].x &&
        sfPlayer.x <= sfShadows[i].x + sfShadows[i].w &&
        sfPlayer.y >= sfShadows[i].y &&
        sfPlayer.y <= sfShadows[i].y + sfShadows[i].h)
      return true;
  }
  return false;
}

inline bool sfPlayerInVisionCone() {
  double dx = sfPlayer.x - sfBoss.x, dy = sfPlayer.y - sfBoss.y;
  double dist = sqrt(dx * dx + dy * dy);
  if (dist > 200.0)
    return false;
  double ang = atan2(dy, dx);
  double diff = ang - sfBoss.visionAngle;
  while (diff > 3.14159)
    diff -= 6.28318;
  while (diff < -3.14159)
    diff += 6.28318;
  return (diff > -0.5 && diff < 0.5);
}

inline int sfPlayerNearTower() {
  for (int i = 0; i < 4; i++) {
    if (sfTowers[i].hacked || sfTowers[i].locked)
      continue;
    double dx = sfPlayer.x - sfTowers[i].x, dy = sfPlayer.y - sfTowers[i].y;
    if (sqrt(dx * dx + dy * dy) < 50)
      return i;
  }
  return -1;
}

inline bool sfPlayerInDroneSpotlight() {
  for (int i = 0; i < 3; i++) {
    if (!sfDrones[i].active)
      continue;
    double sx = sfDrones[i].x + 100 * cos(sfDrones[i].angle);
    double sy = sfDrones[i].y + 100 * sin(sfDrones[i].angle);
    double dx = sfPlayer.x - sx, dy = sfPlayer.y - sy;
    if (sqrt(dx * dx + dy * dy) < sfDrones[i].radius)
      return true;
  }
  return false;
}

inline bool sfPlayerInSignalField() {
  for (int i = 0; i < 2; i++) {
    if (sfPlayer.x > sfSignalFields[i].x &&
        sfPlayer.x < sfSignalFields[i].x + sfSignalFields[i].w &&
        sfPlayer.y > sfSignalFields[i].y &&
        sfPlayer.y < sfSignalFields[i].y + sfSignalFields[i].h)
      return true;
  }
  return false;
}

inline void activateControlPanel(int i) {
  if (!sfPanels[i].available)
    return;
  sfPanels[i].available = false;
  sfPanels[i].cooldown = 20.0;
  if (sfPanels[i].type == 0) { // LGT: Total blackout — boss vision = 0 for 15s
    sfLightsOff = true;
    sfLightsOffTimer = 15.0;
    sfBoss.suspicion = 0; // Instant suspicion reset
    // Force boss out of HUNT/SUSPICIOUS into confused patrol
    if (sfBoss.state == SF_HUNT || sfBoss.state == SF_SUSPICIOUS) {
      sfBoss.state = SF_PATROL;
      sfBoss.patrolX = 100 + rand() % 824;
      sfBoss.patrolY = 80 + rand() % 400;
    }
    sfScreenShake = 0.3;
  } else if (sfPanels[i].type == 1) { // VNT: Stun boss anywhere
    sfBoss.hp -= 50;
    sfBoss.state = SF_STUNNED;
    sfBoss.stunTimer = 3.0;
    sfScreenShake = 0.5;
  } else if (sfPanels[i].type == 2) { // SLW: Heavy slow for 15s
    sfBoss.slowTimer = 15.0;
  }
}

inline void deployDecoy() {
  if (sfPlayer.decoyCount <= 0)
    return;
  for (int i = 0; i < 2; i++) {
    if (!sfDecoys[i].active) {
      sfDecoys[i].active = true;
      sfDecoys[i].x = sfPlayer.x;
      sfDecoys[i].y = sfPlayer.y;
      sfDecoys[i].timer = 4.0;
      sfPlayer.decoyCount--;
      return;
    }
  }
}

inline void sfUpdatePanels() {
  for (int i = 0; i < 3; i++) {
    if (sfPanels[i].cooldown > 0) {
      sfPanels[i].cooldown -= 0.016;
      if (sfPanels[i].cooldown <= 0)
        sfPanels[i].available = true;
    }
  }
  if (sfLightsOffTimer > 0) {
    sfLightsOffTimer -= 0.016;
    if (sfLightsOffTimer <= 0)
      sfLightsOff = false;
  }
}

inline void sfUpdateDecoys() {
  for (int i = 0; i < 2; i++) {
    if (sfDecoys[i].active) {
      sfDecoys[i].timer -= 0.016;
      if (sfDecoys[i].timer <= 0)
        sfDecoys[i].active = false;
    }
  }
}

inline void sfUpdatePulse() {
  if (sfPhase < 2 || sfBoss.state == SF_DEAD)
    return;
  if (!sfBoss.pulseActive) {
    sfBoss.pulseTimer -= 0.016;
    if (sfBoss.pulseTimer <= 0) {
      sfBoss.pulseActive = true;
      sfBoss.pulseRadius = 0;
      sfBoss.pulseTimer = 25.0;
    }
  } else {
    double prevPulse = sfBoss.pulseRadius;
    sfBoss.pulseRadius += 6.0;
    double dx = sfPlayer.x - sfBoss.x, dy = sfPlayer.y - sfBoss.y;
    double dist = sqrt(dx * dx + dy * dy);
    if (dist > prevPulse && dist < sfBoss.pulseRadius) {
      if (fabs(dist - sfBoss.pulseRadius) < 15 && !sfBoss.pulseHitDealt) {
        if (!sfPlayerInShadow() && !sfPlayerInSignalField() &&
            !sfPlayer.crouching) {
          sfBoss.suspicion += 35;
          sfBoss.pulseHitDealt = true;
          sfDamageFlash = 0.2;
          sfScreenShake = 0.5;
        }
      }
    }
  }
  if (sfBoss.pulseRadius > 1000) {
    sfBoss.pulseActive = false;
    sfBoss.pulseHitDealt = false;
  }
}

inline void sfUpdateHazards() {
  if (sfPhase < 3 || sfBoss.state == SF_DEAD)
    return;
  sfHazardSpawnTimer -= 0.032; // speed up spawn check
  if (sfHazardSpawnTimer <= 0) {
    int idx = rand() % 6;
    if (!sfHazards[idx].active && !sfHazards[idx].collapsing) {
      sfHazards[idx].collapsing = true;
      sfHazards[idx].timer = 2.0; // warning
      sfHazards[idx].x = 100 + rand() % 800;
      sfHazards[idx].y = 80 + rand() % 450;
      sfHazards[idx].w = 100;
      sfHazards[idx].h = 100;
    }
    sfHazardSpawnTimer = 5.0 + (rand() % 40) / 10.0;
  }

  for (int i = 0; i < 6; i++) {
    if (sfHazards[i].collapsing) {
      sfHazards[i].timer -= 0.016;
      if (sfHazards[i].timer <= 0) {
        sfHazards[i].collapsing = false;
        sfHazards[i].active = true;
        sfHazards[i].timer = 4.0; // hazard duration
      }
    } else if (sfHazards[i].active) {
      sfHazards[i].timer -= 0.016;
      if (sfHazards[i].timer <= 0)
        sfHazards[i].active = false;
      // Damage check
      if (sfPlayer.x > sfHazards[i].x &&
          sfPlayer.x < sfHazards[i].x + sfHazards[i].w &&
          sfPlayer.y > sfHazards[i].y &&
          sfPlayer.y < sfHazards[i].y + sfHazards[i].h) {
        if (!sfPlayer.dashActive) {
          sfPlayer.health -= 1; // tick damage
          sfDamageFlash = 0.2;
        }
      }
    }
  }
}

inline void sfUpdateMomentum() {
  if (sfPlayer.momentumHits >= 3) {
    sfPlayer.momentumActive = true;
    sfPlayer.momentumTimer = 5.0;
    sfPlayer.momentumHits = 0;
  }
  if (sfPlayer.momentumActive) {
    sfPlayer.momentumTimer -= 0.016;
    if (sfPlayer.momentumTimer <= 0)
      sfPlayer.momentumActive = false;
  }
}

inline void sfUpdateAdaptiveAI() {
  if (sfPhase < 3 || sfBoss.state == SF_DEAD)
    return;
  if (sfBoss.aggressionDecay > 0)
    sfBoss.aggressionDecay -= 0.016;
  else {
    sfBoss.playerAggression = 0;
    sfBoss.playerDodges = 0;
    sfBoss.aggressionDecay = 5.0;
  }

  if (sfBoss.playerAggression > 4 && !sfBoss.shieldActive) {
    sfBoss.shieldActive = true;
    sfBoss.shieldTimer = 3.0;
    sfBoss.playerAggression = 0;
  }
  if (sfBoss.shieldActive) {
    sfBoss.shieldTimer -= 0.016;
    if (sfBoss.shieldTimer <= 0)
      sfBoss.shieldActive = false;
  }

  if (sfBoss.hp < 250)
    sfBoss.desperationMode = true;
}

inline void updateSuspicion() {
  double inc = 0;
  bool inShadow = sfPlayerInShadow();
  bool inSignal = sfPlayerInSignalField();
  bool visible = sfPlayerInVisionCone();
  bool inSpot = sfPlayerInDroneSpotlight();

  if (sfLightsOff) {
    visible = false;
    inSpot = false; // Drones can't see either
    // Boss vision is completely zero — skip all suspicion gains
    if (sfBoss.suspicion > 0)
      sfBoss.suspicion -= 1.5; // Rapidly decay suspicion in darkness
    if (sfBoss.suspicion < 0)
      sfBoss.suspicion = 0;
    return; // No suspicion can build during LGT
  }

  double noiseRadius =
      sfPlayer.running ? 150.0 : (sfPlayer.crouching ? 30.0 : 80.0);
  if (sfPlayer.silentBootsTimer > 0)
    noiseRadius *= 0.3;
  double distToBoss =
      sqrt(pow(sfPlayer.x - sfBoss.x, 2) + pow(sfPlayer.y - sfBoss.y, 2));

  if (visible && !sfPlayer.crouching && !inShadow && !inSignal)
    inc += 1.5;
  else if (visible && sfPlayer.crouching && !inShadow && !inSignal)
    inc += 0.3;
  if (inSpot && !inShadow && !inSignal)
    inc += 1.0;
  if (sfPlayer.running && distToBoss < noiseRadius)
    inc += 0.8;
  if (inShadow || inSignal)
    inc -= 0.6;
  if (!visible && !inSpot && sfPlayer.crouching)
    inc -= 0.4;

  if (inShadow)
    inc -= 0.6;
  if (!visible && !inSpot && sfPlayer.crouching)
    inc -= 0.4;

  sfBoss.suspicion += inc;
  if (sfBoss.suspicion < 0)
    sfBoss.suspicion = 0;
  if (sfBoss.suspicion > 100)
    sfBoss.suspicion = 100;

  if (sfBoss.suspicion >= 100 && sfBoss.state != SF_HUNT &&
      sfBoss.state != SF_COMBAT && sfBoss.state != SF_DEAD) {
    sfBoss.state = SF_HUNT;
    sfBoss.huntTimer = 8.0;
  }
}

inline void updateSFDrones() {
  if (sfPhase < 2)
    return;
  for (int i = 0; i < 3; i++) {
    sfDrones[i].active = true;
    sfDrones[i].angle += sfDrones[i].speed;
    if (sfDrones[i].angle > 6.28318)
      sfDrones[i].angle -= 6.28318;
  }
}

inline void updateSFPlayer() {
  if (sfPlayer.health <= 0 || sfPuzzle.active)
    return;

  sfPlayer.crouching = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
  sfPlayer.running = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;

  double speedMultiplier = 1.0;
  // Floor hazard slowing (collapsed tiles)
  for (int i = 0; i < 6; i++) {
    if (sfHazards[i].active && !sfHazards[i].collapsing) {
      if (sfPlayer.x > sfHazards[i].x &&
          sfPlayer.x < sfHazards[i].x + sfHazards[i].w &&
          sfPlayer.y > sfHazards[i].y &&
          sfPlayer.y < sfHazards[i].y + sfHazards[i].h) {
        speedMultiplier = 0.4; // 60% slow
        break;
      }
    }
  }

  if (sfPlayer.dashActive) {
    sfPlayer.x += sfPlayer.dashDX * 10;
    sfPlayer.y += sfPlayer.dashDY * 10;
    sfPlayer.dashTimer -= 0.016;
    if (sfPlayer.dashTimer <= 0)
      sfPlayer.dashActive = false;
  } else {
    double spd = (sfPlayer.crouching ? 1.5 : (sfPlayer.running ? 5.0 : 3.0)) *
                 speedMultiplier;
    if (GetAsyncKeyState('W') & 0x8000)
      sfPlayer.y += spd;
    if (GetAsyncKeyState('S') & 0x8000)
      sfPlayer.y -= spd;
    if (GetAsyncKeyState('A') & 0x8000)
      sfPlayer.x -= spd;
    if (GetAsyncKeyState('D') & 0x8000)
      sfPlayer.x += spd;
  }

  if (sfPlayer.x < 12)
    sfPlayer.x = 12;
  if (sfPlayer.x > 1012)
    sfPlayer.x = 1012;
  if (sfPlayer.y < 12)
    sfPlayer.y = 12;
  if (sfPlayer.y > 548)
    sfPlayer.y = 548;

  // Steam vent damage
  for (int i = 0; i < 3; i++) {
    if (sfVents[i].active) {
      double dx = sfPlayer.x - sfVents[i].x, dy = sfPlayer.y - sfVents[i].y;
      if (sqrt(dx * dx + dy * dy) < sfVents[i].radius) {
        sfPlayer.health -= 1;
        sfDamageFlash = 0.3;
      }
    }
  }

  // Shadow zone overstay penalty (25 second limit, 10 HP per second after)
  if (sfPlayerInShadow()) {
    sfShadowStayTimer += 0.016;
    if (sfShadowStayTimer > 25.0) {
      // Damage once per second: use modulo-style tick
      double overtime = sfShadowStayTimer - 25.0;
      int prevTick = (int)((overtime - 0.016) * 1.0);
      int curTick = (int)(overtime * 1.0);
      if (curTick > prevTick) {
        sfPlayer.health -= 10;
        sfDamageFlash = 0.3;
        sfScreenShake = 0.2;
      }
    }
  } else {
    sfShadowStayTimer = 0; // Reset when player leaves shadow
  }

  // Floor hazard damage (collapsed tiles) — damage only, slowing moved to top
  for (int i = 0; i < 6; i++) {
    if (sfHazards[i].active && !sfHazards[i].collapsing) {
      if (sfPlayer.x > sfHazards[i].x &&
          sfPlayer.x < sfHazards[i].x + sfHazards[i].w &&
          sfPlayer.y > sfHazards[i].y &&
          sfPlayer.y < sfHazards[i].y + sfHazards[i].h) {
        sfPlayer.health -= 2;
        sfDamageFlash = 0.2;
      }
    }
  }

  // Cooldowns
  if (sfPlayer.dashCooldown > 0)
    sfPlayer.dashCooldown -= 0.016;
  if (sfPlayer.atkCooldown > 0)
    sfPlayer.atkCooldown -= 0.016;
  if (sfPlayer.focusSerumTimer > 0)
    sfPlayer.focusSerumTimer -= 0.016;
  if (sfPlayer.silentBootsTimer > 0)
    sfPlayer.silentBootsTimer -= 0.016;
  if (sfPlayer.slowMindTimer > 0)
    sfPlayer.slowMindTimer -= 0.016;
  if (sfPlayer.empPulseTimer > 0)
    sfPlayer.empPulseTimer -= 0.016;

  if (sfPlayer.health <= 0) {
    sfPlayer.health = 0;
  }
}

inline void updateSFBoss() {
  if (sfBoss.state == SF_DEAD) {
    sfBoss.victoryTimer -= 0.016;
    if (sfExplosionActive) {
      sfExplosionTimer -= 0.016;
      if (sfExplosionTimer <= 0)
        sfExplosionActive = false;
    }
    if (sfBoss.victoryTimer <= 0)
      gameState = MENU;
    return;
  }
  // Player death check
  if (sfPlayer.health <= 0) {
    sfBoss.victoryTimer -= 0.016;
    if (sfBoss.victoryTimer <= 0)
      gameState = MENU;
    return;
  }
  if (sfBoss.hp <= 0 && sfBoss.state != SF_DEAD) {
    sfBoss.hp = 0;
    sfBoss.state = SF_DEAD;
    sfBoss.victoryTimer = 5.0;
    sfExplosionActive = true;
    sfExplosionTimer = 3.0;
    sfScreenShake = 3.0;
    return;
  }
  if (sfIntroTimer > 0) {
    sfIntroTimer -= 0.016;
    return;
  }

  // Phase transitions
  sfTowersHacked = 0;
  for (int i = 0; i < 4; i++)
    if (sfTowers[i].hacked)
      sfTowersHacked++;
  if (sfTowersHacked >= 2 && sfPhase == 1)
    sfPhase = 2;
  if (sfTowersHacked >= 4 && sfPhase == 2) {
    sfPhase = 3;
    sfBoss.shieldLayers = 0;
    sfBoss.state = SF_COMBAT;
    sfBoss.atkTimer = 2.0;
  }

  // Tower lock timers
  for (int i = 0; i < 4; i++) {
    if (sfTowers[i].locked) {
      sfTowers[i].lockTimer -= 0.016;
      if (sfTowers[i].lockTimer <= 0) {
        sfTowers[i].locked = false;
        sfTowers[i].attemptsLeft = 3;
      }
    }
  }

  double baseSpeed = 1.5;
  if (sfBoss.slowTimer > 0) {
    sfBoss.slowTimer -= 0.016;
    baseSpeed *= 0.5;
  }

  if (sfBoss.state == SF_PATROL) {
    // During LGT darkness, boss wanders slowly and randomly
    if (sfLightsOff) {
      double dx = sfBoss.patrolX - sfBoss.x, dy = sfBoss.patrolY - sfBoss.y;
      double dist = sqrt(dx * dx + dy * dy);
      if (dist > 5) {
        sfBoss.x += (dx / dist) * 0.8;
        sfBoss.y += (dy / dist) * 0.8;
      } else {
        sfBoss.patrolX = 100 + rand() % 824;
        sfBoss.patrolY = 80 + rand() % 400;
      }
    } else {
      sfBoss.visionAngle += 0.015 * sfBoss.visionDir;
      if (sfBoss.visionAngle > 1.5)
        sfBoss.visionDir = -1;
      if (sfBoss.visionAngle < -1.5)
        sfBoss.visionDir = 1;

      // Tactical awareness: attract to decoys or last seen
      double targetX = sfBoss.patrolX, targetY = sfBoss.patrolY;
      bool distracted = false;
      for (int i = 0; i < 2; i++) {
        if (sfDecoys[i].active) {
          targetX = sfDecoys[i].x;
          targetY = sfDecoys[i].y;
          distracted = true;
          break;
        }
      }

      double dx = targetX - sfBoss.x, dy = targetY - sfBoss.y;
      double dist = sqrt(dx * dx + dy * dy);
      if (dist > 5) {
        sfBoss.x += (dx / dist) * baseSpeed;
        sfBoss.y += (dy / dist) * baseSpeed;
      } else if (!distracted) {
        sfBoss.patrolX = 100 + rand() % 824;
        sfBoss.patrolY = 80 + rand() % 400;
      }

      if (sfBoss.suspicion >= 50 && sfBoss.suspicion < 100)
        sfBoss.state = SF_SUSPICIOUS;
    } // Close the else block for non-LGT patrol

  } else if (sfBoss.state == SF_SUSPICIOUS) {
    sfBoss.visionAngle = atan2(sfPlayer.y - sfBoss.y, sfPlayer.x - sfBoss.x);
    double dx = sfPlayer.x - sfBoss.x, dy = sfPlayer.y - sfBoss.y;
    double dist = sqrt(dx * dx + dy * dy);
    if (dist > 120) {
      sfBoss.x += (dx / dist) * 1.0;
      sfBoss.y += (dy / dist) * 1.0;
    }
    if (sfBoss.suspicion < 30)
      sfBoss.state = SF_PATROL;

  } else if (sfBoss.state == SF_HUNT) {
    sfBoss.huntTimer -= 0.016;
    double dx = sfPlayer.x - sfBoss.x, dy = sfPlayer.y - sfBoss.y;
    double dist = sqrt(dx * dx + dy * dy);
    sfBoss.visionAngle = atan2(dy, dx);
    if (dist > 40) {
      sfBoss.x += (dx / dist) * 3.5;
      sfBoss.y += (dy / dist) * 3.5;
    }

    // Melee damage
    if (dist < 45) {
      sfBoss.atkTimer -= 0.016;
      if (sfBoss.atkTimer <= 0) {
        sfPlayer.health -= 15;
        sfDamageFlash = 0.5;
        sfScreenShake = 0.3;
        sfBoss.atkTimer = 1.5;
      }
    }
    if (sfBoss.huntTimer <= 0) {
      sfBoss.state = SF_PATROL;
      sfBoss.suspicion = 40;
    }

  } else if (sfBoss.state == SF_STUNNED) {
    sfBoss.stunTimer -= 0.016;
    if (sfBoss.stunTimer <= 0) {
      sfBoss.state = (sfPhase == 3) ? SF_COMBAT : SF_PATROL;
      sfBoss.atkTimer = 2.0;
    }

  } else if (sfBoss.state == SF_COMBAT) {
    // Phase 3 combat
    double dx = sfPlayer.x - sfBoss.x, dy = sfPlayer.y - sfBoss.y;
    double dist = sqrt(dx * dx + dy * dy);
    sfBoss.visionAngle = atan2(dy, dx);
    if (dist > 60) {
      sfBoss.x += (dx / dist) * 2.5;
      sfBoss.y += (dy / dist) * 2.5;
    }

    sfBoss.atkTimer -= 0.016;
    if (sfBoss.atkTimer <= 0) {
      int atk = rand() % 3;
      if (atk == 0) {
        sfBoss.slamActive = true;
        sfBoss.slamRadius = 0;
        sfBoss.slamHitDealt = false;
        sfScreenShake = 1.0;
        sfBoss.atkTimer = 2.5;
      } else if (atk == 1) {
        sfBoss.chargeActive = true;
        sfBoss.chargeHitDealt = false;
        sfBoss.chargeX = sfPlayer.x;
        sfBoss.chargeY = sfPlayer.y;
        sfBoss.chargeTimer = 0.7;
        sfBoss.atkTimer = 2.0;
      } else {
        sfBoss.shockActive = true;
        sfBoss.shockRadius = 0;
        sfBoss.atkTimer = 3.0;
        sfScreenShake = 0.8;
      }
    }

    // Slam
    if (sfBoss.slamActive) {
      sfBoss.slamRadius += 4;
      double pd =
          sqrt(pow(sfPlayer.x - sfBoss.x, 2) + pow(sfPlayer.y - sfBoss.y, 2));
      if (pd < sfBoss.slamRadius + 15 && pd > sfBoss.slamRadius - 15 &&
          !sfPlayer.dashActive && !sfBoss.slamHitDealt) {
        sfPlayer.health -= 12;
        sfDamageFlash = 0.5;
        sfBoss.slamHitDealt = true;
      }
      if (sfBoss.slamRadius > 250)
        sfBoss.slamActive = false;
    }

    // Charge
    if (sfBoss.chargeActive) {
      sfBoss.chargeTimer -= 0.016;
      double cdx = sfBoss.chargeX - sfBoss.x, cdy = sfBoss.chargeY - sfBoss.y;
      double cd = sqrt(cdx * cdx + cdy * cdy);
      if (cd > 5) {
        sfBoss.x += (cdx / cd) * 8;
        sfBoss.y += (cdy / cd) * 8;
      }
      double pd =
          sqrt(pow(sfPlayer.x - sfBoss.x, 2) + pow(sfPlayer.y - sfBoss.y, 2));
      if (pd < 40 && !sfPlayer.dashActive && !sfBoss.chargeHitDealt) {
        sfPlayer.health -= 18;
        sfDamageFlash = 0.5;
        sfScreenShake = 0.5;
        sfBoss.chargeHitDealt = true;
      }
      if (sfBoss.chargeTimer <= 0) {
        sfBoss.chargeActive = false;
        sfBoss.chargeHitDealt = false;
      }
    }

    // Shockwave pulse
    if (sfBoss.shockActive) {
      sfBoss.shockRadius += 3;
      double pd =
          sqrt(pow(sfPlayer.x - sfBoss.x, 2) + pow(sfPlayer.y - sfBoss.y, 2));
      if (pd < sfBoss.shockRadius + 20 && pd > sfBoss.shockRadius - 20 &&
          !sfPlayer.dashActive && !sfBoss.slamHitDealt) {
        sfPlayer.health -= 8;
        sfDamageFlash = 0.3;
        sfBoss.slamHitDealt = true;
      }
      if (sfBoss.shockRadius > 300)
        sfBoss.shockActive = false;
    }

    // Micro-puzzle reaction bar spawn (every ~10 seconds)
    if (!sfBoss.microPuzzleActive) {
      sfBoss.microPuzzleTimer -= 0.016;
      if (sfBoss.microPuzzleTimer <= 0) {
        sfBoss.microPuzzleActive = true;
        sfBoss.microPuzzleBar = 0.0;
        sfBoss.microPuzzleTarget = 0.3 + (rand() % 40) / 100.0;
        sfBoss.microPuzzleHit = false;
        sfBoss.microPuzzleTimer = 4.0; // Time to react
      }
    } else {
      sfBoss.microPuzzleBar += 0.03; // Move the bar
      if (sfBoss.microPuzzleBar > 1.0)
        sfBoss.microPuzzleBar = 0.0;

      sfBoss.microPuzzleTimer -= 0.016;
      if (sfBoss.microPuzzleTimer <= 0) {
        sfBoss.microPuzzleActive = false;
      }
    }
  }

  // Phase advancement
  if (sfTowersHacked >= 2 && sfPhase == 1)
    sfPhase = 2;
  if (sfTowersHacked >= 4 && sfPhase == 2) {
    sfPhase = 3;
    sfBoss.state = SF_COMBAT;
  }

  // Puzzle UI closing delay
  if (sfPuzzle.active && sfPuzzle.completed) {
    sfPuzzle.closeTimer -= 0.016;
    if (sfPuzzle.closeTimer <= 0) {
      sfPuzzle.active = false;
      sfPuzzle.completed = false;
    }
  }

  // Puzzle slider update
  if (sfPuzzle.active && !sfPuzzle.completed) {
    sfPuzzle.sliderPos += sfPuzzle.sliderSpeed * sfPuzzle.sliderDir;
    if (sfPuzzle.sliderPos > 1.0) {
      sfPuzzle.sliderPos = 1.0;
      sfPuzzle.sliderDir = -1;
    }
    if (sfPuzzle.sliderPos < 0.0) {
      sfPuzzle.sliderPos = 0.0;
      sfPuzzle.sliderDir = 1;
    }

    if (sfPuzzle.timerEnabled) {
      sfPuzzle.timer -= 0.016;
      if (sfPuzzle.timer <= 0) {
        sprintf_s(sfPuzzle.feedback, "SYNC LOST!");
        sfPuzzle.feedbackTimer = 1.5;
        sfTowers[sfPuzzle.towerIndex].attemptsLeft--;
        if (sfTowers[sfPuzzle.towerIndex].attemptsLeft <= 0) {
          sfTowers[sfPuzzle.towerIndex].locked = true;
          sfTowers[sfPuzzle.towerIndex].lockTimer = 20.0;
          sfBoss.suspicion += 30;
          sfPuzzle.completed = true;
          sfPuzzle.closeTimer = 1.5;
        } else {
          sfGeneratePuzzle(sfPuzzle.towerIndex); // Reset
        }
      }
    }
  }

  // Boss boundary
  if (sfBoss.x < 35)
    sfBoss.x = 35;
  if (sfBoss.x > 989)
    sfBoss.x = 989;
  if (sfBoss.y < 35)
    sfBoss.y = 35;
  if (sfBoss.y > 525)
    sfBoss.y = 525;

  if (sfBoss.hp <= 0) {
    sfBoss.hp = 0;
    sfBoss.state = SF_DEAD;
  }
}

inline void checkPuzzleAnswer() {
  // Reaction bar success check
  if (sfPuzzle.sliderPos >= sfPuzzle.targetStart &&
      sfPuzzle.sliderPos <= sfPuzzle.targetStart + sfPuzzle.targetWidth) {
    sprintf_s(sfPuzzle.feedback, "SUCCESS! %s Tower Overridden.",
              (sfPuzzle.towerIndex == 0
                   ? "Alpha"
                   : (sfPuzzle.towerIndex == 1
                          ? "Beta"
                          : (sfPuzzle.towerIndex == 2 ? "Gamma" : "Delta"))));
    sfPuzzle.feedbackTimer = 1.5;
    sfTowers[sfPuzzle.towerIndex].hacked = true;
    sfTowersHacked++;
    sfBoss.shieldLayers--;
    if (sfBoss.shieldLayers < 0)
      sfBoss.shieldLayers = 0;

    sfPuzzle.completed = true;
    sfPuzzle.closeTimer = 1.5;
  } else {
    sfTowers[sfPuzzle.towerIndex].attemptsLeft--;
    if (sfPlayer.focusSerumTimer > 0) {
      sfPlayer.focusSerumTimer = 0;
      sprintf_s(sfPuzzle.feedback, "MISSED! Focus Serum stabilized signal.");
      sfPuzzle.feedbackTimer = 1.5;
    } else if (sfTowers[sfPuzzle.towerIndex].attemptsLeft <= 0) {
      sprintf_s(sfPuzzle.feedback, "CIRCUIT OVERLOAD! Tower Locked.");
      sfPuzzle.feedbackTimer = 2.0;
      sfTowers[sfPuzzle.towerIndex].locked = true;
      sfTowers[sfPuzzle.towerIndex].lockTimer = 20.0;
      sfBoss.suspicion += 30;
      sfPuzzle.completed = true;
      sfPuzzle.closeTimer = 2.0;
      if (sfPhase >= 2) {
        sfBoss.slamActive = true;
        sfBoss.slamRadius = 0;
        sfBoss.slamHitDealt = false;
        sfScreenShake = 0.5;
      }
    } else {
      sprintf_s(sfPuzzle.feedback, "MISSED! %d sync attempts left.",
                sfTowers[sfPuzzle.towerIndex].attemptsLeft);
      sfPuzzle.feedbackTimer = 1.0;
      sfBoss.suspicion += 10;
    }
  }
}

inline void sfCheckCollisions() {
  // Powerup pickup
  for (int i = 0; i < 3; i++) {
    if (!sfPowerups[i].active)
      continue;
    double dx = sfPlayer.x - sfPowerups[i].x, dy = sfPlayer.y - sfPowerups[i].y;
    if (sqrt(dx * dx + dy * dy) < 25) {
      sfPowerups[i].active = false;
      if (sfPowerups[i].type == 0)
        sfPlayer.focusSerumTimer = 30.0;
      else if (sfPowerups[i].type == 1)
        sfPlayer.silentBootsTimer = 15.0;
      else if (sfPowerups[i].type == 2)
        sfPlayer.slowMindTimer = 20.0;
      else if (sfPowerups[i].type == 3) {
        sfPlayer.empPulseTimer = 3.0;
        if (sfPuzzle.active) {
          sfBoss.state = SF_STUNNED;
          sfBoss.stunTimer = 3.0;
        }
      } else if (sfPowerups[i].type == 4) {
        sfPlayer.health += 35;
        if (sfPlayer.health > 100)
          sfPlayer.health = 100;
      }
    }
  }
}

inline void spawnSFPowerup() {
  sfPowerupSpawnTimer -= 0.016;
  if (sfPowerupSpawnTimer <= 0) {
    sfPowerupSpawnTimer = 12.0 + rand() % 8;
    for (int i = 0; i < 3; i++) {
      if (!sfPowerups[i].active) {
        sfPowerups[i].active = true;
        sfPowerups[i].x = 80 + rand() % 864;
        sfPowerups[i].y = 80 + rand() % 400;
        sfPowerups[i].type = rand() % 5;
        sfPowerups[i].timer = 15.0;
        sfPowerups[i].floatOffset = 0;
        break;
      }
    }
  }
  // Persistent REPAIR: always keep one health pickup active
  bool hasRepair = false;
  for (int i = 0; i < 3; i++) {
    if (sfPowerups[i].active && sfPowerups[i].type == 4)
      hasRepair = true;
  }
  if (!hasRepair) {
    for (int i = 0; i < 3; i++) {
      if (!sfPowerups[i].active) {
        sfPowerups[i].active = true;
        sfPowerups[i].x = 80 + rand() % 864;
        sfPowerups[i].y = 80 + rand() % 400;
        sfPowerups[i].type = 4;      // REPAIR
        sfPowerups[i].timer = 999.0; // Never expires
        sfPowerups[i].floatOffset = 0;
        break;
      }
    }
  }
  for (int i = 0; i < 3; i++) {
    if (sfPowerups[i].active) {
      sfPowerups[i].timer -= 0.016;
      sfPowerups[i].floatOffset += 0.05;
      if (sfPowerups[i].timer <= 0)
        sfPowerups[i].active = false;
    }
  }
}

inline void drawSFArena() {
  // Dark industrial floor
  iSetColor(18, 18, 24);
  iFilledRectangle(0, 0, 1024, 600);

  // Animated grid — lines pulse from center outward
  for (int gx = 0; gx < 1024; gx += 40) {
    double distC = fabs(gx - 512.0) / 512.0;
    double pulse = 0.5 + 0.5 * sin(sfFlickerTimer * 1.5 - distC * 3);
    int c = (int)(28 + 14 * pulse);
    iSetColor(c, c, c + 5);
    iLine(gx, 0, gx, 600);
  }
  for (int gy = 0; gy < 600; gy += 40) {
    double distC = fabs(gy - 300.0) / 300.0;
    double pulse = 0.5 + 0.5 * sin(sfFlickerTimer * 1.5 - distC * 3);
    int c = (int)(28 + 14 * pulse);
    iSetColor(c, c, c + 5);
    iLine(0, gy, 1024, gy);
  }

  // Phase-colored border
  if (sfPhase == 1)
    iSetColor(40, 60, 80);
  else if (sfPhase == 2)
    iSetColor(80, 60, 30);
  else
    iSetColor(100, 30, 30);
  iRectangle(1, 1, 1022, 598);
  iRectangle(2, 2, 1020, 596);

  // Shadow zones — with animated edge glow
  for (int i = 0; i < 3; i++) {
    int sx = (int)sfShadows[i].x, sy = (int)sfShadows[i].y;
    int sw = (int)sfShadows[i].w, sh = (int)sfShadows[i].h;
    iSetColor(6, 6, 10);
    iFilledRectangle(sx, sy, sw, sh);
    double glow = 0.5 + 0.5 * sin(sfFlickerTimer * 2 + i * 1.2);
    iSetColor((int)(20 + 25 * glow), (int)(20 + 25 * glow),
              (int)(50 + 40 * glow));
    iRectangle(sx, sy, sw, sh);
    iSetColor((int)(40 + 40 * glow), (int)(40 + 40 * glow),
              (int)(80 + 60 * glow));
    char st[10];
    sprintf_s(st, "SAFE");
    iText(sx + sw / 2.0 - 10, sy + sh / 2.0 - 4, st, GLUT_BITMAP_HELVETICA_10);
  }

  // Steam vents — animated particles when active
  for (int i = 0; i < 3; i++) {
    if (sfVents[i].active) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      double p = 0.5 + 0.5 * sin(sfFlickerTimer * 8 + i * 2);
      glColor4f(1.0f, 0.4f, 0.15f, (float)(0.15 + 0.1 * p));
      glBegin(GL_TRIANGLE_FAN);
      glVertex2f((float)sfVents[i].x, (float)sfVents[i].y);
      for (int s = 0; s <= 24; s++) {
        double a = 6.28318 * s / 24.0;
        glVertex2f((float)(sfVents[i].x + (sfVents[i].radius + 8) * cos(a)),
                   (float)(sfVents[i].y + (sfVents[i].radius + 8) * sin(a)));
      }
      glEnd();
      glDisable(GL_BLEND);
      iSetColor((int)(200 + 55 * p), (int)(120 * p), 30);
      iFilledCircle(sfVents[i].x, sfVents[i].y, sfVents[i].radius * 0.7);
      // Spark particles
      for (int sp = 0; sp < 5; sp++) {
        double sa = sfFlickerTimer * 3 + sp * 1.25 + i;
        double sr = sfVents[i].radius * (0.5 + 0.5 * sin(sa * 2));
        iSetColor(255, (int)(180 + 75 * sin(sa)), 50);
        iFilledCircle(sfVents[i].x + sr * cos(sa), sfVents[i].y + sr * sin(sa),
                      2);
      }
      iSetColor(255, 100, 50);
    } else {
      iSetColor(40, 40, 45);
    }
    iCircle(sfVents[i].x, sfVents[i].y, sfVents[i].radius);
  }

  // Signal Towers — with pulsing glow rings
  for (int i = 0; i < 4; i++) {
    int tx = (int)sfTowers[i].x, ty = (int)sfTowers[i].y;
    if (sfTowers[i].hacked) {
      double p = 0.5 + 0.5 * sin(sfFlickerTimer * 2 + i);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4f(0.0f, 1.0f, 0.4f, (float)(0.08 + 0.04 * p));
      glBegin(GL_TRIANGLE_FAN);
      glVertex2f((float)tx, (float)ty);
      for (int s = 0; s <= 24; s++) {
        double a = 6.28318 * s / 24.0;
        glVertex2f((float)(tx + 40 * cos(a)), (float)(ty + 40 * sin(a)));
      }
      glEnd();
      glDisable(GL_BLEND);
      iSetColor(0, (int)(180 + 75 * p), 60);
      iFilledRectangle(tx - 15, ty - 15, 30, 30);
      iSetColor(0, 255, 100);
      iRectangle(tx - 15, ty - 15, 30, 30);
      char ok[5];
      sprintf_s(ok, "OK");
      iText(tx - 6, ty - 5, ok, GLUT_BITMAP_HELVETICA_12);
    } else if (sfTowers[i].locked) {
      double p = 0.5 + 0.5 * sin(sfFlickerTimer * 5);
      iSetColor((int)(160 + 95 * p), 30, 30);
      iFilledRectangle(tx - 15, ty - 15, 30, 30);
      iSetColor(255, (int)(60 + 40 * p), (int)(60 + 40 * p));
      iRectangle(tx - 15, ty - 15, 30, 30);
      char lk[10];
      sprintf_s(lk, "LOCKED");
      iText(tx - 18, ty - 5, lk, GLUT_BITMAP_HELVETICA_10);
      char lt[8];
      sprintf_s(lt, "%.0f", sfTowers[i].lockTimer);
      iSetColor(255, 150, 150);
      iText(tx - 6, ty - 28, lt, GLUT_BITMAP_HELVETICA_10);
    } else {
      double p = 0.5 + 0.5 * sin(sfFlickerTimer * 3 + i * 1.5);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4f(0.0f, 0.7f, 1.0f, (float)(0.06 + 0.06 * p));
      glBegin(GL_TRIANGLE_FAN);
      glVertex2f((float)tx, (float)ty);
      for (int s = 0; s <= 24; s++) {
        double a = 6.28318 * s / 24.0;
        glVertex2f((float)(tx + 45 * cos(a)), (float)(ty + 45 * sin(a)));
      }
      glEnd();
      glDisable(GL_BLEND);
      iSetColor(0, (int)(140 + 80 * p), (int)(200 + 55 * p));
      iFilledRectangle(tx - 15, ty - 15, 30, 30);
      iSetColor(0, 200, 255);
      iRectangle(tx - 16, ty - 16, 32, 32);
      iLine(sfTowers[i].x, sfTowers[i].y + 15, sfTowers[i].x,
            sfTowers[i].y + 32 + 6 * p);
      iSetColor(0, 255, 255);
      iCircle(sfTowers[i].x, sfTowers[i].y + 32 + 6 * p, 2 + p);
      char tn[10];
      sprintf_s(tn, "T%d", i + 1);
      iSetColor(150, 200, 255);
      iText(tx - 6, ty - 28, tn, GLUT_BITMAP_HELVETICA_12);
    }
  }

  // Spotlight drones — with beam line and body glow
  for (int i = 0; i < 3; i++) {
    if (!sfDrones[i].active)
      continue;
    double sx = sfDrones[i].x + 100 * cos(sfDrones[i].angle);
    double sy = sfDrones[i].y + 100 * sin(sfDrones[i].angle);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 0.5f, 0.06f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f((float)sx, (float)sy);
    for (int s = 0; s <= 24; s++) {
      double a = 6.28318 * s / 24.0;
      glVertex2f((float)(sx + sfDrones[i].radius * cos(a)),
                 (float)(sy + sfDrones[i].radius * sin(a)));
    }
    glEnd();
    glDisable(GL_BLEND);
    double dp = 0.5 + 0.5 * sin(sfFlickerTimer * 4 + i * 2);
    iSetColor((int)(200 + 55 * dp), (int)(200 + 55 * dp), (int)(80 + 40 * dp));
    iCircle(sx, sy, sfDrones[i].radius);
    iSetColor(100, 100, 60);
    iLine(sfDrones[i].x, sfDrones[i].y, sx, sy);
    iSetColor((int)(120 + 80 * dp), (int)(120 + 80 * dp), (int)(120 + 80 * dp));
    iFilledCircle(sfDrones[i].x, sfDrones[i].y, 5);
    iSetColor(200, 200, 100);
    iCircle(sfDrones[i].x, sfDrones[i].y, 7);
  }

  // Powerups — floating with glow halo
  for (int i = 0; i < 3; i++) {
    if (!sfPowerups[i].active)
      continue;
    double py = sfPowerups[i].y + 4 * sin(sfPowerups[i].floatOffset);
    double glow = 0.5 + 0.5 * sin(sfPowerups[i].floatOffset * 2);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (sfPowerups[i].type == 0)
      glColor4f(0.0f, 0.8f, 1.0f, (float)(0.08 + 0.05 * glow));
    else if (sfPowerups[i].type == 1)
      glColor4f(0.4f, 0.4f, 1.0f, (float)(0.08 + 0.05 * glow));
    else if (sfPowerups[i].type == 2)
      glColor4f(0.8f, 0.8f, 0.0f, (float)(0.08 + 0.05 * glow));
    else if (sfPowerups[i].type == 3)
      glColor4f(1.0f, 0.3f, 0.2f, (float)(0.08 + 0.05 * glow));
    else
      glColor4f(0.2f, 1.0f, 0.3f,
                (float)(0.08 + 0.05 * glow)); // Green for health
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f((float)sfPowerups[i].x, (float)py);
    for (int s = 0; s <= 16; s++) {
      double a = 6.28318 * s / 16.0;
      glVertex2f((float)(sfPowerups[i].x + 20 * cos(a)),
                 (float)(py + 20 * sin(a)));
    }
    glEnd();
    glDisable(GL_BLEND);
    if (sfPowerups[i].type == 0)
      iSetColor(0, (int)(200 + 55 * glow), 255);
    else if (sfPowerups[i].type == 1)
      iSetColor(100, (int)(100 + 155 * glow), 255);
    else if (sfPowerups[i].type == 2)
      iSetColor(200, (int)(200 + 55 * glow), 0);
    else if (sfPowerups[i].type == 3)
      iSetColor(255, (int)(50 + 205 * glow), 50);
    else
      iSetColor(50, (int)(200 + 55 * glow), 50); // Vibrant Green
    iFilledCircle(sfPowerups[i].x, py, 10);
    iSetColor(255, 255, 255);
    iCircle(sfPowerups[i].x, py, 11);
    char pl[12];
    if (sfPowerups[i].type == 0)
      sprintf_s(pl, "FOCUS");
    else if (sfPowerups[i].type == 1)
      sprintf_s(pl, "SILENT");
    else if (sfPowerups[i].type == 2)
      sprintf_s(pl, "SLOW");
    else if (sfPowerups[i].type == 3)
      sprintf_s(pl, "EMP");
    else
      sprintf_s(pl, "REPAIR");
    iText((int)sfPowerups[i].x - 12, (int)py + 14, pl,
          GLUT_BITMAP_HELVETICA_10);
  }

  // Atmospheric fog vignette
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_QUADS);
  glColor4f(0.05f, 0.05f, 0.1f, 0.3f);
  glVertex2f(0, 560);
  glVertex2f(1024, 560);
  glColor4f(0.05f, 0.05f, 0.1f, 0.0f);
  glVertex2f(1024, 500);
  glVertex2f(0, 500);
  glEnd();
  glBegin(GL_QUADS);
  glColor4f(0.05f, 0.05f, 0.1f, 0.25f);
  glVertex2f(0, 0);
  glVertex2f(1024, 0);
  glColor4f(0.05f, 0.05f, 0.1f, 0.0f);
  glVertex2f(1024, 60);
  glVertex2f(0, 60);
  glEnd();
  glDisable(GL_BLEND);

  // Pulsing E prompt near tower
  if (!sfPuzzle.active && sfBoss.state != SF_HUNT && sfBoss.state != SF_DEAD &&
      sfPhase < 3) {
    int t = sfPlayerNearTower();
    if (t >= 0) {
      double p = 0.5 + 0.5 * sin(sfFlickerTimer * 5);
      iSetColor(0, (int)(200 + 55 * p), (int)(180 + 75 * p));
      char ep[20];
      sprintf_s(ep, "[E] HACK TOWER");
      iText((int)sfPlayer.x - 40, (int)sfPlayer.y + 22, ep,
            GLUT_BITMAP_HELVETICA_12);
    }
  }

  // Phase 2/3 Additions: Signal Fields, Panels, Hazards, Decoys, Pulse
  for (int i = 0; i < 2; i++) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 1.0f, 0.2f, 0.08f);
    iFilledRectangle(sfSignalFields[i].x, sfSignalFields[i].y,
                     sfSignalFields[i].w, sfSignalFields[i].h);
    iSetColor(0, 150, 50);
    iRectangle(sfSignalFields[i].x, sfSignalFields[i].y, sfSignalFields[i].w,
               sfSignalFields[i].h);
    char sft[20];
    sprintf_s(sft, "SIGNAL DAMPENER");
    iSetColor(0, 100, 40);
    iText(sfSignalFields[i].x + 5, sfSignalFields[i].y + 5, sft,
          GLUT_BITMAP_HELVETICA_10);
    glDisable(GL_BLEND);
  }

  for (int i = 0; i < 3; i++) {
    int px = (int)sfPanels[i].x, py = (int)sfPanels[i].y;
    double p = 0.5 + 0.5 * sin(sfFlickerTimer * 4 + i);
    if (sfPanels[i].available)
      iSetColor(0, (int)(150 + 105 * p), 255);
    else
      iSetColor(100, 100, 100);
    iFilledRectangle(px - 15, py - 10, 30, 20);
    iSetColor(255, 255, 255);
    iRectangle(px - 15, py - 10, 30, 20);
    char pl[5];
    if (sfPanels[i].type == 0)
      sprintf_s(pl, "LGT");
    else if (sfPanels[i].type == 1)
      sprintf_s(pl, "VNT");
    else
      sprintf_s(pl, "SLW");
    iText(px - 12, py - 4, pl, GLUT_BITMAP_HELVETICA_10);
    if (!sfPanels[i].available) {
      iSetColor(255, 50, 50);
      iLine(px - 15, py - 12,
            px - 15 + (int)(30 * (sfPanels[i].cooldown / 20.0)), py - 12);
    }
  }

  for (int i = 0; i < 6; i++) {
    if (sfHazards[i].collapsing) {
      double f = 0.5 + 0.5 * sin(sfFlickerTimer * 15);
      iSetColor((int)(255 * f), 0, 0);
      iRectangle(sfHazards[i].x, sfHazards[i].y, sfHazards[i].w,
                 sfHazards[i].h);
      iText(sfHazards[i].x + 5, sfHazards[i].y + 35, "COLLAPSING!",
            GLUT_BITMAP_HELVETICA_10);
    } else if (sfHazards[i].active) {
      iSetColor(10, 5, 5);
      iFilledRectangle(sfHazards[i].x, sfHazards[i].y, sfHazards[i].w,
                       sfHazards[i].h);
      iSetColor(40, 0, 0);
      iRectangle(sfHazards[i].x, sfHazards[i].y, sfHazards[i].w,
                 sfHazards[i].h);
    }
  }

  for (int i = 0; i < 2; i++) {
    if (sfDecoys[i].active) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4f(0.0f, 0.8f, 1.0f, 0.3f);
      iFilledCircle(sfDecoys[i].x, sfDecoys[i].y, 10);
      iSetColor(255, 255, 255);
      iCircle(sfDecoys[i].x, sfDecoys[i].y, 11 + sin(sfFlickerTimer * 5));
      glDisable(GL_BLEND);
    }
  }

  if (sfBoss.pulseActive) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    double r = sfBoss.pulseRadius;
    glColor4f(1.0f, 0.0f, 0.1f, (float)(0.4 * (1.0 - r / 1000.0)));
    iCircle(sfBoss.x, sfBoss.y, (int)r);
    iCircle(sfBoss.x, sfBoss.y, (int)r + 2);
    glDisable(GL_BLEND);
  }
}

inline void drawSFBoss() {
  if (sfBoss.state == SF_DEAD)
    return;

  // Vision cone
  if (sfBoss.state != SF_STUNNED) {
    double range = 200.0;
    double halfA = 0.5;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (sfBoss.state == SF_HUNT)
      glColor4f(1.0f, 0.2f, 0.0f, 0.08f);
    else if (sfBoss.state == SF_SUSPICIOUS)
      glColor4f(1.0f, 1.0f, 0.0f, 0.06f);
    else
      glColor4f(0.3f, 0.3f, 1.0f, 0.05f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f((float)sfBoss.x, (float)sfBoss.y);
    for (int s = 0; s <= 12; s++) {
      double a = sfBoss.visionAngle - halfA + (2.0 * halfA * s / 12.0);
      glVertex2f((float)(sfBoss.x + range * cos(a)),
                 (float)(sfBoss.y + range * sin(a)));
    }
    glEnd();
    glDisable(GL_BLEND);
  }

  // Slam shockwave
  if (sfBoss.slamActive) {
    iSetColor(255, 80, 0);
    iCircle(sfBoss.x, sfBoss.y, sfBoss.slamRadius);
  }
  // Shockwave pulse
  if (sfBoss.shockActive) {
    iSetColor(100, 0, 255);
    iCircle(sfBoss.x, sfBoss.y, sfBoss.shockRadius);
    if (sfBoss.shockRadius > 20)
      iCircle(sfBoss.x, sfBoss.y, sfBoss.shockRadius - 15);
  }

  // Body
  double bs = 28;
  if (sfBoss.state == SF_STUNNED) {
    double p = 0.5 + 0.5 * sin(sfBoss.stunTimer * 10);
    iSetColor(255, (int)(200 * p), 50);
  } else if (sfBoss.state == SF_HUNT)
    iSetColor(220, 30, 30);
  else if (sfBoss.state == SF_COMBAT) {
    double p = 0.5 + 0.5 * sin(sfFlickerTimer * 6);
    iSetColor((int)(180 + 75 * p), 30, (int)(80 * p));
  } else if (sfBoss.state == SF_SUSPICIOUS)
    iSetColor(200, 200, 50);
  else
    iSetColor(100, 100, 140);
  iFilledCircle(sfBoss.x, sfBoss.y, bs);

  // Shield rings
  for (int s = 0; s < sfBoss.shieldLayers; s++) {
    iSetColor(80 + s * 40, 80 + s * 40, 200);
    iCircle(sfBoss.x, sfBoss.y, bs + 4 + s * 5);
  }

  if (sfBoss.shieldActive) {
    double p = 0.5 + 0.5 * sin(sfFlickerTimer * 10);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 1.0f, 1.0f, (float)(0.2 + 0.3 * p));
    iFilledCircle(sfBoss.x, sfBoss.y, bs + 15);
    iSetColor(255, 255, 255);
    iCircle(sfBoss.x, sfBoss.y, bs + 15);
    glDisable(GL_BLEND);
  }

  if (sfBoss.desperationMode) {
    double p = 0.5 + 0.5 * sin(sfFlickerTimer * 12);
    iSetColor((int)(200 + 55 * p), 0, 0);
    iCircle(sfBoss.x, sfBoss.y, bs + 10 + 2 * p);
  }

  // Eyes
  double eyeA = sfBoss.visionAngle;
  iSetColor(255, 255, 0);
  iFilledCircle(sfBoss.x + 12 * cos(eyeA + 0.35),
                sfBoss.y + 12 * sin(eyeA + 0.35), 4);
  iFilledCircle(sfBoss.x + 12 * cos(eyeA - 0.35),
                sfBoss.y + 12 * sin(eyeA - 0.35), 4);

  // Health bar above boss
  iSetColor(50, 50, 50);
  iFilledRectangle(sfBoss.x - 30, sfBoss.y + 45, 60, 6);
  iSetColor(255, 50, 50);
  iFilledRectangle(sfBoss.x - 30, sfBoss.y + 45, (int)(60.0 * sfBoss.hp / 1000),
                   6);

  // Micro-puzzle bar above boss (Phase 3 reaction)
  if (sfBoss.microPuzzleActive && sfPhase == 3) {
    int bw = 80, bh = 10;
    int bx = (int)sfBoss.x - bw / 2, by = (int)sfBoss.y + 60;
    iSetColor(30, 30, 40);
    iFilledRectangle(bx, by, bw, bh);
    // Target zone
    int tx = bx + (int)(sfBoss.microPuzzleTarget * bw) - 8;
    iSetColor(0, 255, 100);
    iFilledRectangle(tx, by, 16, bh);
    // Slider
    int sx = bx + (int)(sfBoss.microPuzzleBar * bw);
    iSetColor(255, 255, 255);
    iLine(sx, by - 2, sx, by + bh + 2);
    iSetColor(255, 255, 255);
    iText(bx, by + 15, "SYNC [E]", GLUT_BITMAP_HELVETICA_10);
  }

  // Boss state label
  iSetColor(200, 200, 200);
  char lb[20];
  if (sfBoss.state == SF_PATROL)
    sprintf_s(lb, "PATROLLING");
  else if (sfBoss.state == SF_SUSPICIOUS)
    sprintf_s(lb, "SEARCHING");
  else if (sfBoss.state == SF_HUNT)
    sprintf_s(lb, "HUNTING!");
  else if (sfBoss.state == SF_STUNNED)
    sprintf_s(lb, "STUNNED");
  else if (sfBoss.state == SF_COMBAT)
    sprintf_s(lb, "READY TO FIGHT");
  else
    sprintf_s(lb, "DEFEATED");
  iText((int)sfBoss.x - 30, (int)sfBoss.y - 45, lb, GLUT_BITMAP_HELVETICA_10);
}

inline void drawSFPlayer() {
  if (sfPlayer.health <= 0)
    return;
  if (sfPlayer.crouching) {
    iSetColor(30, 100, 30);
    iFilledCircle(sfPlayer.x, sfPlayer.y, 8);
    iSetColor(80, 200, 80);
    iCircle(sfPlayer.x, sfPlayer.y, 10);
  } else if (sfPlayer.running) {
    iSetColor(100, 50, 50);
    iFilledCircle(sfPlayer.x, sfPlayer.y, 10);
    iSetColor(200, 100, 100);
    iCircle(sfPlayer.x, sfPlayer.y, 12);
  } else {
    iSetColor(50, 50, 100);
    iFilledCircle(sfPlayer.x, sfPlayer.y, 10);
    iSetColor(100, 100, 255);
    iCircle(sfPlayer.x, sfPlayer.y, 12);
  }

  if (sfPlayer.momentumActive) {
    double p = 0.5 + 0.5 * sin(sfFlickerTimer * 10);
    iSetColor(255, (int)(200 + 55 * p), 0);
    iCircle(sfPlayer.x, sfPlayer.y, 15 + 2 * p);
    iCircle(sfPlayer.x, sfPlayer.y, 17);
  }

  iSetColor(50, 80, 50);
  iFilledCircle(sfPlayer.x, sfPlayer.y, 9);
  iSetColor(100, 180, 100);
  // Direction
  double facing = atan2(sfBoss.y - sfPlayer.y, sfBoss.x - sfPlayer.x);
  iSetColor(200, 200, 200);
  iLine(sfPlayer.x, sfPlayer.y, sfPlayer.x + 12 * cos(facing),
        sfPlayer.y + 12 * sin(facing));

  // Shadow indicator
  if (sfPlayerInShadow()) {
    iSetColor(80, 80, 200);
    char hi[10];
    sprintf_s(hi, "HIDDEN");
    iText((int)sfPlayer.x - 16, (int)sfPlayer.y + 16, hi,
          GLUT_BITMAP_HELVETICA_10);
  }
}

inline void renderPuzzleUI() {
  if (!sfPuzzle.active)
    return;

  // HUD Background
  iSetColor(20, 20, 40);
  iFilledRectangle(300, 200, 400, 250);
  iSetColor(0, 200, 255);
  iRectangle(300, 200, 400, 250);

  iSetColor(255, 255, 255);
  iText(350, 410, "OVERRIDE SEQUENCE: TOWER LINK", GLUT_BITMAP_HELVETICA_12);
  iSetColor(150, 150, 150);
  iText(350, 390, "Press 'E' inside target zone", GLUT_BITMAP_HELVETICA_10);

  // The Bar
  int bx = 350, by = 300, bw = 300, bh = 40;
  iSetColor(40, 40, 60);
  iFilledRectangle(bx, by, bw, bh);

  // Target Zone (Green)
  int tx = bx + (int)(sfPuzzle.targetStart * bw);
  int tw = (int)(sfPuzzle.targetWidth * bw);
  iSetColor(0, 255, 100);
  iFilledRectangle(tx, by, tw, bh);

  // The Slider (Red)
  int sx = bx + (int)(sfPuzzle.sliderPos * bw);
  iSetColor(255, 50, 50);
  iFilledRectangle(sx - 3, by - 5, 6, bh + 10);

  // Attempts & Timer
  iSetColor(200, 200, 200);
  char info[50];
  if (sfPuzzle.timerEnabled)
    sprintf_s(info, "ATTEMPTS: %d/3 | TIME: %.1f",
              sfTowers[sfPuzzle.towerIndex].attemptsLeft, sfPuzzle.timer);
  else
    sprintf_s(info, "ATTEMPTS: %d/3",
              sfTowers[sfPuzzle.towerIndex].attemptsLeft);
  iText(350, 250, info, GLUT_BITMAP_HELVETICA_12);

  if (sfPuzzle.feedbackTimer > 0) {
    iSetColor(255, 255, 0);
    iText(350, 220, sfPuzzle.feedback, GLUT_BITMAP_HELVETICA_12);
    sfPuzzle.feedbackTimer -= 0.016;
  }
}

inline void renderSFHUD() {
  // Top bar
  iSetColor(20, 20, 25);
  iFilledRectangle(0, 600, 1024, 40);

  char buf[60];

  // Boss HP
  iSetColor(50, 50, 50);
  iFilledRectangle(20, 610, 200, 14);
  iSetColor(200, 50, 50);
  int bw = (int)(200.0 * sfBoss.hp / 1000.0);
  if (bw < 0)
    bw = 0;
  iFilledRectangle(20, 610, bw, 14);
  iSetColor(255, 255, 255);
  sprintf_s(buf, "BOSS: %d/1000", sfBoss.hp);
  iText(60, 612, buf, GLUT_BITMAP_HELVETICA_12);

  // Shields
  for (int i = 0; i < sfBoss.shieldLayers; i++) {
    iSetColor(100, 100, 200);
    iFilledRectangle(230 + i * 20, 612, 15, 10);
    iSetColor(180, 180, 255);
    iRectangle(230 + i * 20, 612, 15, 10);
  }

  // Suspicion meter
  iSetColor(50, 50, 50);
  iFilledRectangle(400, 610, 150, 14);
  int sc = (sfBoss.suspicion > 70) ? 255 : (sfBoss.suspicion > 40) ? 200 : 80;
  iSetColor(sc, (int)(200 - sfBoss.suspicion * 2), 50);
  int sw = (int)(150.0 * sfBoss.suspicion / 100);
  iFilledRectangle(400, 610, sw, 14);
  iSetColor(255, 255, 255);
  sprintf_s(buf, "SUSPICION: %.0f%%", sfBoss.suspicion);
  iText(415, 612, buf, GLUT_BITMAP_HELVETICA_12);

  // Towers status
  iSetColor(200, 200, 200);
  sprintf_s(buf, "TOWERS: %d/4", sfTowersHacked);
  iText(600, 612, buf, GLUT_BITMAP_HELVETICA_12);

  // Phase
  iSetColor(180, 180, 180);
  sprintf_s(buf, "PHASE %d", sfPhase);
  iText(720, 612, buf, GLUT_BITMAP_HELVETICA_12);

  // Decoys & Momentum
  iSetColor(0, 200, 255);
  sprintf_s(buf, "DECOYS: %d", sfPlayer.decoyCount);
  iText(800, 612, buf, GLUT_BITMAP_HELVETICA_12);

  if (sfPlayer.momentumActive) {
    iSetColor(255, 200, 0);
    sprintf_s(buf, "MOMENTUM ACTIVE!");
    iText(880, 618, buf, GLUT_BITMAP_HELVETICA_10);
  } else {
    iSetColor(150, 150, 60);
    sprintf_s(buf, "STREAK: %d", sfPlayer.momentumHits);
    iText(880, 618, buf, GLUT_BITMAP_HELVETICA_10);
  }

  // Boss state
  iSetColor(200, 200, 200);
  char stStr[20];
  if (sfBoss.state == SF_PATROL)
    sprintf_s(stStr, "PATROLLING");
  else if (sfBoss.state == SF_SUSPICIOUS)
    sprintf_s(stStr, "SUSPICIOUS");
  else if (sfBoss.state == SF_HUNT)
    sprintf_s(stStr, "HUNTING!");
  else if (sfBoss.state == SF_STUNNED)
    sprintf_s(stStr, "STUNNED");
  else if (sfBoss.state == SF_COMBAT)
    sprintf_s(stStr, "COMBAT!");
  else
    sprintf_s(stStr, "DEFEATED");
  iText(820, 602, stStr, GLUT_BITMAP_HELVETICA_10);

  // Player HP bottom
  iSetColor(50, 50, 50);
  iFilledRectangle(240, 610, 100, 14);
  iSetColor(50, 200, 50);
  int hw = (int)(100.0 * sfPlayer.health / 100.0);
  if (hw < 0)
    hw = 0;
  iFilledRectangle(240, 610, hw, 14);
  iSetColor(255, 255, 255);
  sprintf_s(buf, "HP: %d/100", sfPlayer.health);
  iText(70, 12, buf, GLUT_BITMAP_HELVETICA_12);

  // Player buffs
  int bx = 250;
  if (sfPlayer.focusSerumTimer > 0) {
    iSetColor(0, 200, 255);
    sprintf_s(buf, "FOCUS:%.0f", sfPlayer.focusSerumTimer);
    iText(bx, 12, buf, GLUT_BITMAP_HELVETICA_10);
    bx += 65;
  }
  if (sfPlayer.silentBootsTimer > 0) {
    iSetColor(100, 100, 255);
    sprintf_s(buf, "SILENT:%.0f", sfPlayer.silentBootsTimer);
    iText(bx, 12, buf, GLUT_BITMAP_HELVETICA_10);
    bx += 70;
  }
  if (sfPlayer.slowMindTimer > 0) {
    iSetColor(200, 200, 0);
    sprintf_s(buf, "SLOW:%.0f", sfPlayer.slowMindTimer);
    iText(bx, 12, buf, GLUT_BITMAP_HELVETICA_10);
    bx += 60;
  }

  // Controls
  iSetColor(100, 100, 100);
  char ctrl[80];
  sprintf_s(ctrl,
            "WASD:Move SHIFT:Crouch CTRL:Run SPACE:Dash E:Hack Tower Q:Decoy");
  iText(20, 30, ctrl, GLUT_BITMAP_HELVETICA_10);

  // Visual effects
  if (sfDamageFlash > 0) {
    iSetColor(255, 0, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.0f, 0.0f, (float)(sfDamageFlash * 0.4));
    iFilledRectangle(0, 0, 1024, 640);
    glDisable(GL_BLEND);
  }

  // Intro overlay
  if (sfIntroTimer > 0) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(150, 260);
    glVertex2f(874, 260);
    glVertex2f(874, 380);
    glVertex2f(150, 380);
    glEnd();
    glDisable(GL_BLEND);
    iSetColor(180, 0, 0);
    iRectangle(150, 260, 724, 120);
    iSetColor(0, 200, 255);
    char it1[40];
    sprintf_s(it1, "THE SILENT FOUNDRY");
    iText(330, 330, it1, GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(160, 160, 160);
    char it2[50];
    sprintf_s(it2, "Hack towers. Stay hidden. Survive.");
    iText(330, 290, it2, GLUT_BITMAP_HELVETICA_12);
  }

  // Victory message
  if (sfBoss.state == SF_DEAD) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(200, 250);
    glVertex2f(824, 250);
    glVertex2f(824, 370);
    glVertex2f(200, 370);
    glEnd();
    glDisable(GL_BLEND);
    iSetColor(0, 255, 150);
    char w1[30];
    sprintf_s(w1, "SYSTEM OVERRIDDEN");
    iText(350, 320, w1, GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(200, 200, 200);
    char w2[30];
    sprintf_s(w2, "Foundry neutralized.");
    iText(380, 280, w2, GLUT_BITMAP_HELVETICA_12);
  }

  // Death message
  if (sfPlayer.health <= 0) {
    iSetColor(255, 0, 0);
    char d1[20];
    sprintf_s(d1, "DETECTED");
    iText(420, 320, d1, GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(200, 200, 200);
    char d2[30];
    sprintf_s(d2, "Mission failed.");
    iText(430, 290, d2, GLUT_BITMAP_HELVETICA_12);
  }
}

inline void sfUpdateEffects() {
  sfFlickerTimer += 0.016;
  if (sfScreenShake > 0)
    sfScreenShake -= 0.016;
  if (sfDamageFlash > 0)
    sfDamageFlash -= 0.016;

  // Vent cycling
  for (int i = 0; i < 3; i++) {
    sfVents[i].timer -= 0.016;
    if (sfVents[i].timer <= 0) {
      sfVents[i].active = !sfVents[i].active;
      sfVents[i].timer = sfVents[i].active ? 2.0 : sfVents[i].interval;
    }
  }
}

inline void drawBossDeadImage() {
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

#endif
