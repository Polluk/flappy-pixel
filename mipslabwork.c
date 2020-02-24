/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall
   This file modified 2020 by Lukas Pöldma

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
//#include <stdlib.h>
#include "mipslab.h"  /* Declatations for these labs */

#define START_PAGE 1
#define START_POS 7
#define START_X 16
#define WALL_SPACE 13
#define WALL_RATE 2

const int FLAPPY_PAGE[4] = {START_X + 192,
                            START_X + 128,
                            START_X + 64,
                            START_X};

const int FLAPPY_POS[8] = {128, 64, 32, 16, 8, 4, 2, 1};

int iPage;
int iPos;

int wall[10][5];

int wallFrames;
int wallWait;
int wallsActive;
int wallRisk;
int wallRiskIndex;

int jumpFrames;
int jumpBtnCtrl;

int dead;
int points;
char pointsStr[3];

uint8_t gamefield[] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
};


void flappyDraw(void){
  gamefield[FLAPPY_PAGE[iPage]] |= FLAPPY_POS[iPos];
}

void flappyUndraw(void){
  gamefield[FLAPPY_PAGE[iPage]] &= ~FLAPPY_POS[iPos];
}

void flappyDown(void){
  flappyUndraw();
  if(iPos > 0){
    iPos--;
  }
  else if(iPage > 0){
    iPos = 7;
    iPage--;
  }
  else{
    dead = 1;
  }
  flappyDraw();
}

void flappyUp(void){
  flappyUndraw();
  if(iPos < 7){
    iPos++;
  }
  else if(iPage < 3){
    iPos = 0;
    iPage++;
  }
  else{
    dead = 1;
  }
  flappyDraw();
}

void crashCheck(int wallNumber){
  if ((wall[wallNumber][iPage] | FLAPPY_POS[iPos]) == wall[wallNumber][iPage]){
    dead = 1;
  }
}

void wallMove(int wallNumber){
  int i, j;
  for(i = 192, j = 0; i >= 0; i -= 64, j++){
    gamefield[wall[wallNumber][4] + i] &= ~wall[wallNumber][j];
  }
  if(wall[wallNumber][4] > 1)
  {
    wall[wallNumber][4]--;
  }
  else{
    int i;
    for(i = wallNumber; i < wallsActive; i++){
      int j;
      for (j = 0; j < 5; j++){
        wall[i][j] = wall[i + 1][j];
      }
    }
    wallsActive--;
  }
  for(i = 192, j = 0; i >= 0; i -= 64, j++){
    gamefield[wall[wallNumber][4] + i] |= wall[wallNumber][j];
  }
  if(wall[wallNumber][4] == START_X){
    wallRisk = 1;
    wallRiskIndex = wallNumber;
  }
  else if(wall[wallNumber][4] == START_X - 1){
    points++;
    sprintf(pointsStr, "%d", points);
    display_string(0, pointsStr);
    display_update();
    wallRisk = 0;
  }
}

void randomWall(int wallNumber){
  int hole = rand() % 20 + 2;
  int holePage = hole / 8;
  int holePos = hole % 8;

  wall[wallNumber][0]= 255;
  wall[wallNumber][1]= 255;
  wall[wallNumber][2]= 255;
  wall[wallNumber][3]= 255;
  wall[wallNumber][4]= 64;

  wall[wallNumber][holePage] = wall[wallNumber][holePage] << (8 - holePos);
  wall[wallNumber][holePage + 1] = wall[wallNumber][holePage + 1] >> holePos;
}

/* Interrupt Service Routine */
user_isr( void )
{
  if ((((getbtns() >> 3) & 0x1) == 0x1) && (jumpBtnCtrl == 0)){
      jumpFrames = 7;
      jumpBtnCtrl = 1;
  }
  else if ((((getbtns() >> 3) & 0x1) == 0x0) && (jumpBtnCtrl == 1)){
    jumpBtnCtrl = 0;
  }

  if (jumpFrames > 3){
    flappyUp();
    jumpFrames--;
  }
  else if(jumpFrames > 0){
    jumpFrames--;
  }
  else{
    flappyDown();
  }

  if (wallRisk == 1){
    crashCheck(wallRiskIndex);
  }

  wallFrames++;
  if (wallFrames == WALL_RATE){
    wallWait++;
    if (wallWait == WALL_SPACE){
      randomWall(wallsActive);
      wallsActive++;
      wallWait = 0;
    }

    int i;
    for (i = 0; i < wallsActive; i++){
      wallMove(i);

    }
    wallFrames = 0;
  }

  if (wallRisk == 1){
    crashCheck(wallRiskIndex);
  }


  if (dead == 1){
    disable_interrupt();
  }
  else{
    display_image(0, gamefield);
  }

  IFSCLR(0) = 0x100; //återställer flaggan
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  TRISDSET = 0xe0;    // gör knappar 4-2 redor för input
  TRISFSET = 0x2;     // gör knapp 1 redo för input

  iPage = START_PAGE;
  iPos = START_POS;

  wallFrames = 0;
  wallWait = 0;
  wallsActive = 1;
  wallRisk = 0;
  wallRiskIndex = 0;

  jumpFrames = 0;
  jumpBtnCtrl = 0;

  dead = 0;
  points = 0;
  sprintf(pointsStr, "%d", points);

  int i, j;

  for (i = 0; i < 10; i++){
    for(j = 0; j < 5; j++){
      wall[i][j] = 0;
    }
  }

  for (i = 0; i < 256; i++){
    gamefield[i] = 0;
  }

  flappyDraw();
  display_string(0, pointsStr);
  display_update();
  display_image(0, gamefield);

  T2CONSET = 0x8000;

  while(1){
    if ((((getbtns() >> 3) & 0x1) == 0x1)){
      break;
    }
  }

  srand(TMR2);
  randomWall(0);

  T2CON = 0x0;        // nollställer kontrollfunktioner för timer 2
  T2CONSET = 0x70;    // väljer skala 1:256
  TMR2 = 0x0;         // nollställer timer
  PR2 = 15625;  // timer 2 når detta på 0,1s | 80 000 000 / 256 / 10 = 31250
  IFSCLR(0) = 0x100;  // nollställer interrupt flaggan för timer 2              Interrupt Flag Status
  IECSET(0) = 0x100;  // tillåter interrupts för timer 2                        Interrupt Enable Control
  IPCSET(2) = 0x1f;   // högsta int prio=7 och subprio=3 för timer 2            Interrupt Priority Control
  enable_interrupt(); // slår på interrupts globalt                             STATUS
  T2CONSET = 0x8000;  // slår på timer 2
  return;
}

/* This function is called repetitively from the main program */
void labwork( void )
{
  if ((dead == 1) && (((getbtns() >> 2) & 0x1) == 0x1)){
    labinit();
  }
}
