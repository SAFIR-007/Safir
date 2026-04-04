#ifndef GAMESHARED_H
#define GAMESHARED_H

#include "iGraphics.h"

enum GameStateId {
  MENU = 0,
  LEVEL1 = 1,
  OPTIONS_STATE = 2,
  CREDITS_STATE = 3,
  WIN_SCREEN = 4,
  FINAL_BG = 5,
  SURPRISE_SCREEN = 6,
  PUZZLE_SCREEN = 7,
  INTEL_SCREEN = 8,
  WARNING_SCREEN = 9,
  SECOND_BG = 10,
  BOSS_LEVEL = 11,
  NEW_LEVEL = 12,
  GAME_OVER = 13,
  BOSS_DEAD_IMAGE = 14,
  LAB_TRANSITION = 15,
  LAB_LEVEL = 16,
  PUZZLE_TRANSITION = 17,
  LEVEL2_SCREEN = 18,
  LEVEL2_BOSS = 19
};

extern int gameState;
extern bool keyStates[256];

inline void iShowSubImage(int x, int y, int width, int height,
                          unsigned int texture, int sx, int sy, int sw, int sh,
                          int tw, int th) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

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

#endif
