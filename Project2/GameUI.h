#ifndef GAMEUI_H
#define GAMEUI_H

#include "GameShared.h"

extern unsigned int bgImage;
extern unsigned int level1BG;

void drawMenu();
void drawCredits();
void drawOptions();
void drawWinScreen();
void drawGameOverScreen();

inline void drawMenu() {
  iShowImage(0, 0, 1024, 640, bgImage);

  iSetColor(180, 0, 0);
  iFilledRectangle(100, 360, 4, 100);

  iSetColor(50, 0, 0);
  iText(132, 408, "WRATH OF GOD", GLUT_BITMAP_TIMES_ROMAN_24);
  iSetColor(180, 0, 0);
  iText(130, 410, "WRATH OF GOD", GLUT_BITMAP_TIMES_ROMAN_24);

  iSetColor(160, 160, 160);
  iText(130, 380, "SILENT PULSES, SHIFTING EYES", GLUT_BITMAP_HELVETICA_12);

  if (iMouseX >= 98 && iMouseX <= 248 && iMouseY >= 300 && iMouseY <= 340) {
    iSetColor(220, 0, 0);
    iFilledRectangle(98, 300, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(180, 0, 0);
    iFilledRectangle(98, 300, 150, 40);
    iSetColor(255, 255, 255);
  }
  iText(115, 315, "START CAMPAIGN", GLUT_BITMAP_HELVETICA_12);

  if (iMouseX >= 98 && iMouseX <= 248 && iMouseY >= 250 && iMouseY <= 290) {
    iSetColor(60, 60, 60);
    iFilledRectangle(98, 250, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(30, 30, 30);
    iFilledRectangle(98, 250, 150, 40);
    iSetColor(160, 160, 160);
  }
  iText(135, 265, "OPTIONS", GLUT_BITMAP_HELVETICA_12);

  if (iMouseX >= 98 && iMouseX <= 248 && iMouseY >= 200 && iMouseY <= 240) {
    iSetColor(60, 60, 60);
    iFilledRectangle(98, 200, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(30, 30, 30);
    iFilledRectangle(98, 200, 150, 40);
    iSetColor(160, 160, 160);
  }
  iText(135, 215, "CREDITS", GLUT_BITMAP_HELVETICA_12);

  if (iMouseX >= 98 && iMouseX <= 248 && iMouseY >= 150 && iMouseY <= 190) {
    iSetColor(100, 0, 0);
    iFilledRectangle(98, 150, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(30, 30, 30);
    iFilledRectangle(98, 150, 150, 40);
    iSetColor(160, 160, 160);
  }
  iText(145, 165, "EXIT", GLUT_BITMAP_HELVETICA_12);

  iSetColor(0, 150, 255);
  iText(300, 360, "LEVEL SELECT", GLUT_BITMAP_HELVETICA_18);

  if (iMouseX >= 300 && iMouseX <= 450 && iMouseY >= 300 && iMouseY <= 340) {
    iSetColor(0, 120, 180);
    iFilledRectangle(300, 300, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(0, 60, 100);
    iFilledRectangle(300, 300, 150, 40);
    iSetColor(180, 240, 255);
  }
  iText(325, 315, "SIDE SCROLLER", GLUT_BITMAP_HELVETICA_12);

  if (iMouseX >= 300 && iMouseX <= 450 && iMouseY >= 250 && iMouseY <= 290) {
    iSetColor(0, 120, 180);
    iFilledRectangle(300, 250, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(0, 60, 100);
    iFilledRectangle(300, 250, 150, 40);
    iSetColor(180, 240, 255);
  }
  iText(330, 265, "PUZZLE LEVEL", GLUT_BITMAP_HELVETICA_12);

  if (iMouseX >= 300 && iMouseX <= 450 && iMouseY >= 200 && iMouseY <= 240) {
    iSetColor(0, 120, 180);
    iFilledRectangle(300, 200, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(0, 60, 100);
    iFilledRectangle(300, 200, 150, 40);
    iSetColor(180, 240, 255);
  }
  iText(335, 215, "BOSS LEVEL", GLUT_BITMAP_HELVETICA_12);

  if (iMouseX >= 300 && iMouseX <= 450 && iMouseY >= 150 && iMouseY <= 190) {
    iSetColor(0, 120, 180);
    iFilledRectangle(300, 150, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(0, 60, 100);
    iFilledRectangle(300, 150, 150, 40);
    iSetColor(180, 240, 255);
  }
  iText(340, 165, "LAB LEVEL", GLUT_BITMAP_HELVETICA_12);

  if (iMouseX >= 300 && iMouseX <= 450 && iMouseY >= 100 && iMouseY <= 140) {
    iSetColor(0, 120, 180);
    iFilledRectangle(300, 100, 150, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(0, 60, 100);
    iFilledRectangle(300, 100, 150, 40);
    iSetColor(180, 240, 255);
  }
  iText(315, 115, "SILENT FOUNDRY", GLUT_BITMAP_HELVETICA_12);

  iSetColor(180, 0, 0);
  iLine(30, 610, 50, 610);
  iLine(30, 610, 30, 590);
  iLine(30, 30, 50, 30);
  iLine(30, 30, 30, 50);

  iSetColor(80, 80, 80);
  iText(850, 60, "ALPHA BUILD V0.9.4", GLUT_BITMAP_8_BY_13);
  iText(840, 40, "ONLINE SERVICES: ACTIVE", GLUT_BITMAP_8_BY_13);
  iText(880, 20, "REGION: EU-WEST", GLUT_BITMAP_8_BY_13);
}

inline void drawCredits() {
  iShowImage(0, 0, 1024, 640, bgImage);
  iSetColor(0, 0, 0);
  iFilledRectangle(0, 0, 1024, 640);

  iSetColor(180, 0, 0);
  iText(400, 500, "DEVELOPMENT TEAM", GLUT_BITMAP_TIMES_ROMAN_24);

  iSetColor(255, 255, 255);
  iText(350, 400, "SAFIR RAHMAN", GLUT_BITMAP_HELVETICA_18);
  iSetColor(160, 160, 160);
  iText(350, 375, "00724205101010", GLUT_BITMAP_HELVETICA_12);

  iSetColor(255, 255, 255);
  iText(350, 300, "TASEEN EHFAAZ", GLUT_BITMAP_HELVETICA_18);
  iSetColor(160, 160, 160);
  iText(350, 275, "00724205101016", GLUT_BITMAP_HELVETICA_12);

  iSetColor(255, 255, 255);
  iText(350, 200, "RAISA JAHAN", GLUT_BITMAP_HELVETICA_18);
  iSetColor(160, 160, 160);
  iText(350, 175, "00724205101017", GLUT_BITMAP_HELVETICA_12);

  iSetColor(0, 150, 255);
  iText(350, 180, "LEVEL SELECT", GLUT_BITMAP_HELVETICA_18);
  if (iMouseX >= 350 && iMouseX <= 550 && iMouseY >= 130 && iMouseY <= 170) {
    iSetColor(0, 150, 100);
    iFilledRectangle(350, 130, 200, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(0, 60, 100);
    iFilledRectangle(350, 130, 200, 40);
    iSetColor(180, 240, 255);
  }
  iText(375, 145, "START LABORATORY", GLUT_BITMAP_HELVETICA_10);

  iSetColor(180, 0, 0);
  iText(100, 50, "PRESS [ESC] OR CLICK TO RETURN", GLUT_BITMAP_HELVETICA_12);
}

inline void drawOptions() {
  iShowImage(0, 0, 1024, 640, bgImage);
  iSetColor(0, 0, 0);
  iFilledRectangle(0, 0, 1024, 640);

  iSetColor(180, 0, 0);
  iText(380, 500, "HOW TO PLAY", GLUT_BITMAP_TIMES_ROMAN_24);

  iSetColor(255, 255, 255);
  iText(350, 400, "- AIM: MOVE MOUSE", GLUT_BITMAP_HELVETICA_18);
  iText(350, 350, "- SHOOT: LEFT CLICK", GLUT_BITMAP_HELVETICA_18);
  iText(350, 300, "- ENEMIES: DEFEAT TO CLEAR STAGE", GLUT_BITMAP_HELVETICA_18);
  iText(350, 250, "- WALLS: BULLETS REFLECT OFF STONE",
        GLUT_BITMAP_HELVETICA_18);

  iSetColor(0, 150, 255);
  iText(350, 180, "LEVEL SELECT", GLUT_BITMAP_HELVETICA_18);
  if (iMouseX >= 350 && iMouseX <= 550 && iMouseY >= 130 && iMouseY <= 170) {
    iSetColor(0, 150, 100);
    iFilledRectangle(350, 130, 200, 40);
    iSetColor(255, 255, 255);
  } else {
    iSetColor(0, 60, 100);
    iFilledRectangle(350, 130, 200, 40);
    iSetColor(180, 240, 255);
  }
  iText(375, 145, "START LABORATORY", GLUT_BITMAP_HELVETICA_10);

  iSetColor(180, 0, 0);
  iText(100, 50, "PRESS [ESC] OR CLICK TO RETURN", GLUT_BITMAP_HELVETICA_12);
}

inline void drawWinScreen() {
  iShowImage(0, 0, 1024, 640, level1BG);

  iSetColor(0, 0, 0);
  iFilledRectangle(0, 0, 1024, 640);

  iSetColor(255, 255, 255);
  iText(400, 350, "LEVEL CLEAR!", GLUT_BITMAP_TIMES_ROMAN_24);
  iSetColor(180, 0, 0);
  iText(380, 300, "CLICK ANYWHERE TO CONTINUE", GLUT_BITMAP_HELVETICA_18);
}

inline void drawGameOverScreen() {
  iSetColor(0, 0, 0);
  iFilledRectangle(0, 0, 1024, 640);
  iSetColor(180, 0, 0);
  iText(380, 380, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
  iSetColor(160, 160, 160);
  iText(350, 300, "CLICK ANYWHERE TO RETURN TO MENU",
        GLUT_BITMAP_HELVETICA_18);
}

#endif
