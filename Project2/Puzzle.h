#ifndef PUZZLE_H
#define PUZZLE_H

struct GravityTile {
  int type;     // 0: Straight, 1: L-shape, 2: Cross
  int rotation; // 0: 0, 1: 90, 2: 180, 3: 270 degrees
  bool isActive;
};

extern GravityTile grid[4][4];
extern double surpriseTimer;
extern double puzzleTimer;
extern bool puzzleCleared;
extern double screenShake;
extern unsigned int finalBG;

bool isTilePortOpen(int row, int col, int port);
bool isPuzzleAlreadySolved();
void initPuzzle();
void drawSurpriseScreen();
void drawGravityNode(int x, int y, int size, GravityTile t);
void drawPuzzle();

#endif
