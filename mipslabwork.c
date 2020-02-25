/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall
   This file modified 2020 by Lukas Pöldma

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

#define HOLE_SIZE 7
#define FLAP_TIME 6
#define FLAP_HEIGHT 4
#define START_PAGE 1
#define START_POS 7
#define START_X 16
#define WALL_SPACE 13
#define WALL_RATE 2

#define DATA_ADDRESS 0x9D007000
int *ptr = (int *) DATA_ADDRESS + 4;


const int FLAPPY_PAGE[4] = {START_X + 192,
                            START_X + 128,
                            START_X + 64,
                            START_X};

const int FLAPPY_POS[8] = {128, 64, 32, 16, 8, 4, 2, 1};

//const int HIGHSCORE[7] = {576, 584, 664, 536, 632, 656, 552};
const int NEW[5] = {360, 624, 552, 696, 360};
const int HIGHSCORE[7] = {360, 576, 584, 664, 536, 656, 360};
const int GAMEOVER[8] = {568, 520, 616, 552, 632, 688, 552, 656};
const int FLAP[6] = {392, 360, 560, 608, 520, 640};
const int NEXT[6] = {416, 360, 624, 552, 704, 672};



int iPage;
int iPos;

int wall[10][5];
int wallFrames;
int wallWait;
int wallsActive;
int wallRisk;
int wallRiskIndex;

int flapFrames;
int flapBtnCtrl;

int dead;
int scores[4][6] = {{0, 0, 0, 520, 520, 520},
                    {0, 0, 0, 520, 520, 520},
                    {0, 0, 0, 520, 520, 520},
                    {0, 0, 0, 520, 520, 520}};

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

uint8_t scorefield[] = {
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,

  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,

  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,

  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
};

void resetGamefield( void ){
  int i;
  for (i = 0; i < 256; i++){
    gamefield[i] = 0;
  }
}

void resetScorefield( void ){
  int i;
  for (i = 0; i < 256; i++){
    scorefield[i] = 255;
  }
}

void drawName(int rank, int pos){
  int i, j;
  for(i=0; i < 3; i++){
    for(j = 0; j < 8; j++){
      scorefield[j + (i * 8) + pos] = ~font[j + scores[rank][i + 3]];
    }
  }
}

void drawScore(int rank, int pos){
  int i, j;
  for (i = 0; i < 3; i++)
    for(j = 0; j < 8; j++){
      scorefield[j + (i * 8) + pos] = ~font[j + 384 + (scores[rank][i] * 8)];
  }
}

void newHighscore(int rank){
  resetScorefield();
  int i, j;
  for(i = 0; i < 5; i++){
    for(j = 0; j < 8; j++){
      scorefield[j + (i * 8) + 12] = ~font[j + NEW[i]];
    }
  }
  for(i = 0; i < 7; i++){
    for(j = 0; j < 8; j++){
      scorefield[j + (i * 8) + 68] = ~font[j + HIGHSCORE[i]];
    }
  }
  for(i = 0; i < 6; i++){
    for(j = 0; j < 8; j++){
      scorefield[j + (i * 8) + 200] = ~font[j + NEXT[i]];
    }
  }
  drawName(0, 148);
  display_image(64, scorefield);
  int menuBtnCtrl = 1;
  int loop = 1;
  while(loop){
    for (i = 1; i < 4; i++){                     // Körs tre gånger
      //int btn = ((getbtns() >> i) & 0x1);       // Hämtar läget för en knapp
      if (((getbtns() >> i) & 0x1) && (menuBtnCtrl == 0)){  // Kollar om knappen är intryckt
        scores[0][6 - i] += 8;
        if(scores[0][6 - i] > 720){
          scores[0][6 - i] = 520;
        }
        menuBtnCtrl = 1;
      }
      else if((getbtns() & 0x1) && (menuBtnCtrl == 0)){
        loop = 0;
      }
      else if ((getbtns() < 1)  && (menuBtnCtrl == 1)){
        menuBtnCtrl = 0;
      }
    }
    drawName(0, 148);
    display_image(64, scorefield);
  }
  for (i = 3; i > rank; i--){
    for(j = 0; j < 6; j++){
      scores[i][j] = scores[i - 1][j];
    }
  }
  for(i = 0; i < 6; i++){
    scores[rank][i] = scores[0][i];
  }
}

void checkHighscore( void ){
  if((scores[0][0] * 100 + scores[0][1] * 10 + scores[0][2]) >
     (scores[1][0] * 100 + scores[1][1] * 10 + scores[1][2])){
    newHighscore(1);
  }
  else if((scores[0][0] * 100 + scores[0][1] * 10 + scores[0][2]) >
          (scores[2][0] * 100 + scores[2][1] * 10 + scores[2][2])){
    newHighscore(2);
  }
  else if((scores[0][0] * 100 + scores[0][1] * 10 + scores[0][2]) >
          (scores[3][0] * 100 + scores[3][1] * 10 + scores[3][2])){
    newHighscore(3);
  }
}

void showHighscore( void ){
  int i, j;
  for(i = 0; i < 7; i++){
    for(j = 0; j < 8; j++){
      scorefield[j + (i * 8) + 4] = ~font[j + HIGHSCORE[i]];
    }
  }

  for (i = 0; i < 3; i++){
    drawName((i + 1), (68 + (i * 64)));
    drawScore((i + 1), (100 + (i * 64)));
  }
  display_image(64, scorefield);
}

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

void wallPassed( void ){
  scores[0][2]++;
  if (scores[0][2] > 9){
    scores[0][2] = 0;
    scores[0][1]++;
    if (scores[0][1] > 9){
      scores[0][1] = 0;
      scores[0][0]++;
    }
  }
  drawScore(0, 84);
  display_image(64, scorefield);
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
    wallPassed();
    wallRisk = 0;
  }
}

void randomWall(int wallNumber){
  int hole = rand() % (28 - HOLE_SIZE) + 2;
  int holePage = hole / 8;
  int holePos = hole % 8;

  wall[wallNumber][0]= 255;
  wall[wallNumber][1]= 255;
  wall[wallNumber][2]= 255;
  wall[wallNumber][3]= 255;
  wall[wallNumber][4]= 64;

  wall[wallNumber][holePage] = wall[wallNumber][holePage] << (HOLE_SIZE - holePos);
  wall[wallNumber][holePage + 1] = wall[wallNumber][holePage + 1] >> holePos;
}

/* Interrupt Service Routine */
user_isr( void )
{
  if ((((getbtns() >> 3) & 0x1) == 0x1) && (flapBtnCtrl == 0)){
      flapFrames = FLAP_TIME;
      flapBtnCtrl = 1;
  }
  else if ((((getbtns() >> 3) & 0x1) == 0x0) && (flapBtnCtrl == 1)){
    flapBtnCtrl = 0;
  }

  if (flapFrames > (FLAP_TIME - FLAP_HEIGHT)){
    flappyUp();
    flapFrames--;
  }
  else if(flapFrames > 0){
    flapFrames--;
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
/*
  int data[4] = {1,2,3,4};
  NVMADDR = DATA_ADDRESS;
  NVMDATA = data[0];
  NVMCON = 4001;
  NVMKEY = 0xAA996655;
  NVMKEY = 0x556699AA;
  NVMCONSET = 0x8000;
  while (NVMCON & 0x8000);
*/
  TRISDSET = 0xe0;    // gör knappar 4-2 redor för input
  TRISFSET = 0x2;     // gör knapp 1 redo för input

  iPage = START_PAGE;
  iPos = START_POS;

  wallFrames = 0;
  wallWait = 0;
  wallsActive = 1;
  wallRisk = 0;
  wallRiskIndex = 0;

  flapFrames = 0;
  flapBtnCtrl = 0;

  dead = 0;

  int i, j;

  // initiate / reset current score
  for (i = 0, j = 3; i < 3; i++, j++){
    scores[0][i] = 0;
    scores[0][j] = 520;
  }

  // initiate / reset walls
  for (i = 0; i < 10; i++){
    for(j = 0; j < 5; j++){
      wall[i][j] = 0;
    }
  }

  // initiate / reset game area
  resetGamefield();

  // initiate / reset score area
  resetScorefield();

  flappyDraw();
  for(i = 0; i < 6; i++){
    for(j = 0; j < 8; j++){
      gamefield[j + (i * 8) + 200] = font[j + FLAP[i]];
    }
  }
  display_image(0, gamefield);
  showHighscore();

  T2CONSET = 0x8000;

  // paused until jump button pressed
  while(1){
    if ((((getbtns() >> 3) & 0x1) == 0x1)){
      break;
    }
  }

  srand(TMR2);
  randomWall(0);
  resetGamefield();
  resetScorefield();
  drawScore(0, 84);
  for(i = 72; i <= 112; i+=40){
    for(j = 0; j < 8; j++){
      scorefield[i + j] = ~font[336 + j];
    }
  }
  display_image(64, scorefield);

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
  if (dead == 1){
    int i, j;
    for(i = 0; i < 8; i++){
      for(j = 0; j < 8; j++){
        scorefield[j + (i * 8)] = ~font[j + GAMEOVER[i]];
      }
    }
    for(i = 0; i < 6; i++){
      for(j = 0; j < 8; j++){
        scorefield[j + (i * 8) + 200] = ~font[j + NEXT[i]];
      }
    }
    display_image(64, scorefield);

    while(1){
      if (((getbtns() & 0x1) == 0x1)){
        break;
      }
    }

    checkHighscore();
    labinit();
  }
}
