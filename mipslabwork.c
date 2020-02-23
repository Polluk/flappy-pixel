/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall
   This file modified 2020 by Lukas Pöldma

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

//int flappyPosI = 3;
//int flappyPosJ = 0;
int flappyPosI[5] = {4, 200, 136, 72, 8};
int flappyPosJ[9] = {1, 128, 64, 32, 16, 8, 4, 2, 1};

int wall[5] = {63, 255, 0, 255, 255};
int wallCtr = 0;

int jumpFrames = 0;
int jumpBtnCtrl = 0;

int dead = 0;

//int wallPos

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

char textstring[] = "text, more text, and even more text!";

void flappyDraw(){
  gamefield[flappyPosI[flappyPosI[0]]] = flappyPosJ[flappyPosJ[0]];
}

void flappyDown(){
  gamefield[flappyPosI[flappyPosI[0]]] = 0;
  if(flappyPosJ[0] > 1){
    flappyPosJ[0]--;
  }
  else if(flappyPosI[0] > 1){
    flappyPosJ[0] = 8;
    flappyPosI[0]--;
  }
  else{
    dead = 1;
  }
}

void flappyUp(){
  gamefield[flappyPosI[flappyPosI[0]]] = 0;
  if(flappyPosJ[0] < 8){
    flappyPosJ[0]++;
  }
  else if(flappyPosI[0] < 4){
    flappyPosJ[0] = 1;
    flappyPosI[0]++;
  }
  else{
    dead = 1;
  }
}

void crashCheck(){
  if (wall[flappyPosI[0]] == gamefield[flappyPosI[flappyPosI[0]]]){
    dead = 1;
  }
}

wallDraw(){
  int i, j;
  for(i = 192, j = 1; i >= 0; i -= 64, j++){
    gamefield[wall[0] + i] = wall[j];
  }
  if((wall[0] == 8) || (wall[0] == 7)){
    gamefield[flappyPosI[flappyPosI[0]]] = flappyPosJ[flappyPosJ[0]] | wall[flappyPosI[0]];
  }
}

void wallMove(){
  int i;
  for(i = 192; i >= 0; i -= 64){
    gamefield[wall[0] + i] = 0;
  }
  if(wall[0] > 0)
  {
    wall[0]--;
  }
  else{
    wall[0] = 63;
  }
}

/* Interrupt Service Routine */
user_isr( void )
{
  if (((getbtns() >> 3) & 0x1) == 0x1){
    if (jumpBtnCtrl == 0){
      jumpFrames = 7;
      jumpBtnCtrl = 1;
    }
  }
  else{
    jumpBtnCtrl = 0;
  }

  if (jumpFrames > 2){
    flappyUp();
    jumpFrames--;
  }
  else if(jumpFrames > 0){
    jumpFrames--;
  }
  else{
    flappyDown();
  }
  flappyDraw();

  wallCtr++;
  if(wallCtr == 2){
    wallMove();
    wallCtr = 0;
  }
  wallDraw();

  if (dead == 1){
    while(dead > 0){}
  }

  if((IFS(0) >> 8) & 0x1){  // kollar interrupt-flaggan

    display_image(0, gamefield);
    IFSCLR(0) = 0x100; //återställer flaggan
  }
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  flappyDraw();
  TRISDSET = 0xe0;    // gör knappar 4-2 redor för input
  TRISFSET = 0x2;     // gör knapp 1 redo för input

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

}
