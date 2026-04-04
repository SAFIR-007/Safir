#include "GameShared.h"
#include "GameUI.h"
#include "level1ss.h"
#include "BounceShooter.h"
#include "Puzzle.h"
#include "Boss1.h"
#include "Lab.h"
#include "SilentFoundry.h"

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

unsigned int newLevelBG[3];
double scrollX = 0;
double newPlayerX = 200;
double newPlayerY = 50;
double newPlayerVelY = 0;
bool newPlayerOnGround = true;
int newPlayerFrame = 0;
double newPlayerAnimTimer = 0;
int newPlayerDir = 1;
bool newPlayerMoving = false;
bool newPlayerIsShooting = false;
double newPlayerShootTimer = 0;
double newPlayerFireCooldown = 0;
bool arrowKeyLeft = false;
bool arrowKeyRight = false;
bool arrowKeyUp = false;

int newPlayerHealth = 100;
int newPlayerMaxHealth = 100;
NewLevelBullet newBullets[MAX_NEW_BULLETS];
NewLevelEnemy newEnemies[MAX_NEW_ENEMIES];

unsigned int chatgptImage;
unsigned int puzzleTransitionBG;

int puzzleTransitionPhase = 0;
double puzzleTransitionTimer = 0;
double puzzleTransitionAlpha = 0;

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
PuzzleParticle puzzleParticles[40];

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
int labTransitionPhase = 0;
double labDamageFlash = 0;

double labDashTimer = 0;
double labDashCooldown = 0;
double labInstructionTimer = 0;
double labWalkAnimTimer = 0;
int labWalkFrame = 0;
bool labPlayerMoving = false;
const double LAB_MAX_HEALTH = 100;
const double LAB_MAX_HULL = 150;

LabTurret labTurrets[LAB_NUM_TURRETS];
LabPanel labPanels[LAB_NUM_PANELS];
AGZone labAGZones[LAB_NUM_AGZONES];
ElectricFloor labElectricFloors[LAB_NUM_ELECTRIC];
SpeedZone labSpeedZones[LAB_NUM_SPEED];
HealingZone labHealingZones[LAB_NUM_HEALING];

Player player = {100, 32, 100, false, 0, 0, 0};
Bullet bullets[100];
Enemy enemies[3];

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

bool phase3DecoyActive = false;
double phase3DecoyX = 0;
double phase3DecoyY = 0;
double phase3DecoyTimer = 0;

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

P3Shadow phase3Shadows[6];
P3Pillar phase3Pillars[4];
P3Seal phase3Seals[3];
bool keyStates[256] = {false};

double level2ScreenTimer = 0;
SFPlayer sfPlayer;
SFBoss sfBoss;
SFTower sfTowers[4];
SFDrone sfDrones[3];
SFPuzzleSystem sfPuzzle;
SFPowerup sfPowerups[3];
SFShadowZone sfShadows[3];
SFSteamVent sfVents[3];
double sfScreenShake = 0, sfDamageFlash = 0, sfFlickerTimer = 0;
double sfIntroTimer = 0, sfExplosionTimer = 0;
bool sfExplosionActive = false;
double sfPowerupSpawnTimer = 0;
int sfTowersHacked = 0;
int sfPhase = 1;

SFControlPanel sfPanels[3];
SFDecoy sfDecoys[2];
SFArenaHazard sfHazards[6];
SFSignalField sfSignalFields[2];
bool sfLightsOff = false;
double sfLightsOffTimer = 0;
double sfHazardSpawnTimer = 8.0;
double sfShadowStayTimer = 0;

void iDraw() {
  iClear();

  if (gameState == MENU) {
    drawMenu();
  } else if (gameState == NEW_LEVEL) {
    drawNewLevel();
  } else if (gameState == GAME_OVER) {
    drawGameOverScreen();
  } else if (gameState == CREDITS_STATE) {
    drawCredits();
  } else if (gameState == OPTIONS_STATE) {
    drawOptions();
  } else if (gameState == LEVEL1) {
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
  } else if (gameState == BOSS_DEAD_IMAGE) {
    drawBossDeadImage();
  } else if (gameState == LAB_TRANSITION) {
    drawLabTransition();
  } else if (gameState == LAB_LEVEL) {
    drawLabLevel();
  } else if (gameState == PUZZLE_TRANSITION) {
    drawPuzzleTransition();
  } else if (gameState == LEVEL2_SCREEN) {
    drawLevel2Screen();
  } else if (gameState == LEVEL2_BOSS) {
    drawSFArena();
    drawSFPlayer();
    drawSFBoss();
    renderPuzzleUI();
    if (sfLightsOff) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4f(0.0f, 0.0f, 0.02f, 0.85f);
      glBegin(GL_QUADS);
      glVertex2f(0, 0);
      glVertex2f(1024, 0);
      glVertex2f(1024, 640);
      glVertex2f(0, 640);
      glEnd();
      float sfpx = (float)sfPlayer.x, sfpy = (float)sfPlayer.y;
      for (int ring = 0; ring < 3; ring++) {
        float rad = 50.0f + ring * 20.0f;
        float sfra = 0.12f - ring * 0.04f;
        glColor4f(0.15f, 0.15f, 0.25f, sfra);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(sfpx, sfpy);
        for (int s = 0; s <= 24; s++) {
          float a = 6.28318f * s / 24.0f;
          glVertex2f(sfpx + rad * cos(a), sfpy + rad * sin(a));
        }
        glEnd();
      }
      glDisable(GL_BLEND);
    }
    renderSFHUD();
  }
}

void iMouseMove(int mx, int my) {}

void iPassiveMouseMove(int mx, int my) {}

void iKeyboard(unsigned char key) {
  if (key == 'b' || key == 'B' || key == '\b' || key == 27) {
    if (gameState == CREDITS_STATE || gameState == OPTIONS_STATE) {
      gameState = MENU;
    }
  }

  if ((key == 'l' || key == 'L') && gameState == MENU) {
    startLabLevel();
  }

  if (gameState == LAB_LEVEL) {
    handleLabKeyboard(key);
  }

  if (key == 27 && gameState == NEW_LEVEL) {
    gameState = MENU;
  }

  if (key == 'm' || key == 'M') {
    if (gameState != LEVEL1 && gameState != BOSS_LEVEL &&
        gameState != NEW_LEVEL && gameState != LEVEL2_BOSS) {
      gameState = MENU;
    }
  }

  if (gameState == NEW_LEVEL) {
    keyboardNewLevel(key);
  }

  if ((key == 't' || key == 'T') && gameState == MENU) {
    startBossLevel();
  }

  if (gameState == BOSS_LEVEL) {
    handlePhase3KeyPress(key);
  }

  if ((key == 'y' || key == 'Y') && gameState == MENU) {
    startSilentFoundryLevel();
  }

  if (gameState == LEVEL2_BOSS) {
    handleSilentFoundryKeyPress(key);
  }
}

void iKeyUp(unsigned char key) {
  keyStates[key] = false;

  if (gameState == NEW_LEVEL) {
    keyUpNewLevel(key);
  }
}

void iSpecialKeyboard(unsigned char key) {
  if (gameState == BOSS_LEVEL) {
    keyStates[key] = true;
  }
  if (key == 27) {
    if (gameState == CREDITS_STATE || gameState == OPTIONS_STATE) {
      gameState = MENU;
    }
  }
  if (gameState == NEW_LEVEL) {
    specialKeyboardNewLevel(key);
  }
}

void iSpecialKeyUp(unsigned char key) {
  if (gameState == BOSS_LEVEL) {
    keyStates[key] = false;
  }
  if (gameState == NEW_LEVEL) {
    specialKeyUpNewLevel(key);
  }
}

void iMouse(int button, int state, int mx, int my) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    if (gameState == MENU) {
      if (mx >= 98 && mx <= 248 && my >= 300 && my <= 340) {
        startLevel1();
      } else if (mx >= 98 && mx <= 248 && my >= 250 && my <= 290) {
        gameState = OPTIONS_STATE;
      } else if (mx >= 98 && mx <= 248 && my >= 200 && my <= 240) {
        gameState = CREDITS_STATE;
      } else if (mx >= 98 && mx <= 248 && my >= 150 && my <= 190) {
        exit(0);
      } else if (mx >= 300 && mx <= 450 && my >= 300 && my <= 340) {
        initNewLevel();
      } else if (mx >= 300 && mx <= 450 && my >= 250 && my <= 290) {
        startPuzzleSequence();
      } else if (mx >= 300 && mx <= 450 && my >= 200 && my <= 240) {
        startBossLevel();
      } else if (mx >= 300 && mx <= 450 && my >= 150 && my <= 190) {
        startLabLevel();
      } else if (mx >= 300 && mx <= 450 && my >= 100 && my <= 140) {
        startSilentFoundryLevel();
      }
    } else if (gameState == GAME_OVER) {
      gameState = MENU;
    } else if (gameState == BOSS_DEAD_IMAGE) {
      gameState = LEVEL2_SCREEN;
      level2ScreenTimer = 0;
    } else if (gameState == LEVEL2_SCREEN) {
      startSilentFoundryLevel();
    } else if (gameState == NEW_LEVEL) {
      mouseNewLevel(button, state, mx, my);
    } else if (gameState == CREDITS_STATE || gameState == OPTIONS_STATE) {
      gameState = MENU;
    } else if (gameState == LEVEL1) {
      handleLevel1MouseClick(mx, my);
    } else if (gameState == FINAL_BG) {
      gameState = SURPRISE_SCREEN;
      surpriseTimer = 5.0;
    } else if (gameState == PUZZLE_SCREEN) {
      handlePuzzleMouseClick(mx, my);
    } else if (gameState == WIN_SCREEN) {
      gameState = FINAL_BG;
    } else if (gameState == BOSS_LEVEL) {
      handlePhase3MouseClick();
    } else if (gameState == LEVEL2_BOSS) {
      handleSilentFoundryMouseClick();
    }
  }
}

void fixedUpdate() {
  if (gameState == CREDITS_STATE || gameState == OPTIONS_STATE) {
    if (p3IsKeyPressed(27) || p3IsKeyPressed('b') || p3IsKeyPressed('B')) {
      gameState = MENU;
    }
  }

  if (gameState == NEW_LEVEL) {
    updateNewLevel();
  }

  if (gameState == PUZZLE_TRANSITION) {
    updatePuzzleTransition();
  }

  if (gameState == LEVEL1) {
    updateLevel1();
  }

  if (gameState == LAB_TRANSITION) {
    updateLabTransition();
  } else if (gameState == LAB_LEVEL) {
    updateLabLevel();
  }

  if (gameState == SURPRISE_SCREEN) {
    updateSurpriseScreen();
  }

  if (gameState == PUZZLE_SCREEN) {
    updatePuzzleScreen();
  }

  if (gameState == INTEL_SCREEN) {
    intelTimer -= 0.016;
    if (intelTimer <= 0) {
      startLabTransition();
    }
  }

  if (gameState == WARNING_SCREEN) {
    updateWarningScreen();
  }

  if (gameState == SECOND_BG) {
    updateSecondBGScreen();
  }

  if (gameState == BOSS_LEVEL) {
    updatePhase3Level();
  }

  if (gameState == LEVEL2_SCREEN) {
    updateLevel2Screen();
  }

  if (gameState == LEVEL2_BOSS) {
    updateSilentFoundryLevel();
  }
}

int main() {
  iInitialize(1024, 640, "Wrath of God");

  bgImage = iLoadImage("Assets/menu_bg.png");
  level1BG = iLoadImage("Assets/Stage.png");
  playerSpritesheet = iLoadImage("Assets/PC.png");
  enemySpritesheet = iLoadImage("Assets/NPC.png");
  finalBG = iLoadImage("Assets/1st bg.png");
  digitalRainBG = iLoadImage("Assets/Digital Rain.png");
  secondBG = iLoadImage("Assets/vault room.png");

  newLevelBG[0] = iLoadImage("Assets/BG_1.png");
  newLevelBG[1] = iLoadImage("Assets/BG_2.png");
  newLevelBG[2] = iLoadImage("Assets/BG_3.png");

  newPlayerSpritesheet = iLoadImage("Assets/PC2.png");
  npc1Spritesheet = iLoadImage("Assets/NPC.png");
  npc2Spritesheet = iLoadImage("Assets/NPC2.png");
  chatgptImage = iLoadImage("Assets/ChatGPT Image Feb 28, 2026, 01_02_51 AM.png");
  puzzleTransitionBG = iLoadImage("Assets/2nd bg.png");

  iStart();
  return 0;
}
