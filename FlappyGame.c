/* FlappyGame.c

    This file written 2020 by Lukas Pöldma
   F Lundevall
   Some parts are original code written by Axel Isaksson / F Lundevall

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "Flappy.h"  /* Declatations for these labs */


#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

#define HOLE_SIZE 7                                                              // storlek på hål i väggen, 7 minimum
#define FLAP_TIME 7                                                              // hur länge ett hopp räcker
#define FLAP_HEIGHT 4                                                            // hur mycket av ett hopp som är rakt upp, FLAP_TIME är maximum
#define START_PAGE 1                                                             // vilken page flappy pixel börjar i
#define START_POS 7                                                              // vilken position i pagen flappy pixel börjar i
#define START_X 16                                                               // vilken x position flappy pixel är på
#define WALL_SPACE 13                                                            // avståndet mellan väggar
#define WALL_RATE 2                                                              // hastigheten på väggar, högre är långsammare

const int const FLAPPY_PAGE[4] = {START_X + 192,                                 // bitmap-index för flappy pixels page
                                  START_X + 128,
                                  START_X + 64,
                                  START_X};

const int const FLAPPY_POS[8] = {128, 64, 32, 16, 8, 4, 2, 1};                   // bitmap-index för flappy pixels page-position

const int const NEW[5] = {360, 624, 552, 696, 360};                              // font-index för "-NEW-"

const int const HIGHSCORE[7] = {360, 576, 584, 664, 536, 656, 360};              // font-index för "-HISCR-"

const int const GAMEOVER[8] = {568, 520, 616, 552, 632, 688, 552, 656};          // font-index för "GAMEOVER"

const int const FLAP[6] = {416, 360, 560, 608, 520, 640};                        // font-index för "4-FLAP"

const int const NEXT[6] = {392, 360, 624, 552, 704, 672};                        // font-index för "1-NEXT"

const uint8_t const font[] = {                                                   // innehåller bitmap-värden för tecken som kan skrivas på skärmen
	0, 0, 0, 0, 0, 0, 0, 0,                                                        //0-7
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,                                                        //64-71
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,                                                        //128-135
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,                                                        //192-199
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0,                                                        //256-263
	0, 0, 0, 94, 0, 0, 0, 0,
	0, 0, 4, 3, 4, 3, 0, 0,
	0, 36, 126, 36, 36, 126, 36, 0,
	0, 36, 74, 255, 82, 36, 0, 0,
	0, 70, 38, 16, 8, 100, 98, 0,
	0, 52, 74, 74, 52, 32, 80, 0,
	0, 0, 0, 4, 3, 0, 0, 0,

	0, 0, 0, 126, 129, 0, 0, 0,                                                    //320-327
	0, 0, 0, 129, 126, 0, 0, 0,
	0, 42, 28, 62, 28, 42, 0, 0,
	0, 8, 8, 62, 8, 8, 0, 0,
	0, 0, 0, 128, 96, 0, 0, 0,
	0, 8, 8, 8, 8, 8, 0, 0,
	0, 0, 0, 0, 96, 0, 0, 0,
	0, 64, 32, 16, 8, 4, 2, 0,

	0, 62, 65, 73, 65, 62, 0, 0,                                                   //384-391
	0, 0, 66, 127, 64, 0, 0, 0,
	0, 0, 98, 81, 73, 70, 0, 0,
	0, 0, 34, 73, 73, 54, 0, 0,
	0, 0, 14, 8, 127, 8, 0, 0,
	0, 0, 35, 69, 69, 57, 0, 0,
	0, 0, 62, 73, 73, 50, 0, 0,
	0, 0, 1, 97, 25, 7, 0, 0,

	0, 0, 54, 73, 73, 54, 0, 0,                                                    //448-455
	0, 0, 6, 9, 9, 126, 0, 0,
	0, 0, 0, 102, 0, 0, 0, 0,
	0, 0, 128, 102, 0, 0, 0, 0,
	0, 0, 8, 20, 34, 65, 0, 0,
	0, 0, 20, 20, 20, 20, 0, 0,
	0, 0, 65, 34, 20, 8, 0, 0,
	0, 2, 1, 81, 9, 6, 0, 0,

	0, 28, 34, 89, 89, 82, 12, 0,                                                  //512-519
	0, 0, 126, 9, 9, 126, 0, 0,
	0, 0, 127, 73, 73, 54, 0, 0,
	0, 0, 62, 65, 65, 34, 0, 0,
	0, 0, 127, 65, 65, 62, 0, 0,
	0, 0, 127, 73, 73, 65, 0, 0,
	0, 0, 127, 9, 9, 1, 0, 0,
	0, 0, 62, 65, 81, 50, 0, 0,

	0, 0, 127, 8, 8, 127, 0, 0,                                                    //576-583
	0, 0, 65, 127, 65, 0, 0, 0,
	0, 0, 32, 64, 64, 63, 0, 0,
	0, 0, 127, 8, 20, 99, 0, 0,
	0, 0, 127, 64, 64, 64, 0, 0,
	0, 127, 2, 4, 2, 127, 0, 0,
	0, 127, 6, 8, 48, 127, 0, 0,
	0, 0, 62, 65, 65, 62, 0, 0,

	0, 0, 127, 9, 9, 6, 0, 0,                                                      //640-647
	0, 0, 62, 65, 97, 126, 64, 0,
	0, 0, 127, 9, 9, 118, 0, 0,
	0, 0, 38, 73, 73, 50, 0, 0,
	0, 1, 1, 127, 1, 1, 0, 0,
	0, 0, 63, 64, 64, 63, 0, 0,
	0, 31, 32, 64, 32, 31, 0, 0,
	0, 63, 64, 48, 64, 63, 0, 0,

	0, 0, 119, 8, 8, 119, 0, 0,                                                    //704-711
	0, 3, 4, 120, 4, 3, 0, 0,
	0, 0, 113, 73, 73, 71, 0, 0,
	0, 0, 127, 65, 65, 0, 0, 0,
	0, 2, 4, 8, 16, 32, 64, 0,
	0, 0, 0, 65, 65, 127, 0, 0,
	0, 4, 2, 1, 2, 4, 0, 0,
	0, 64, 64, 64, 64, 64, 64, 0,

	0, 0, 1, 2, 4, 0, 0, 0,                                                        //768-775
	0, 0, 48, 72, 40, 120, 0, 0,
	0, 0, 127, 72, 72, 48, 0, 0,
	0, 0, 48, 72, 72, 0, 0, 0,
	0, 0, 48, 72, 72, 127, 0, 0,
	0, 0, 48, 88, 88, 16, 0, 0,
	0, 0, 126, 9, 1, 2, 0, 0,
	0, 0, 80, 152, 152, 112, 0, 0,

	0, 0, 127, 8, 8, 112, 0, 0,                                                    //832-839
	0, 0, 0, 122, 0, 0, 0, 0,
	0, 0, 64, 128, 128, 122, 0, 0,
	0, 0, 127, 16, 40, 72, 0, 0,
	0, 0, 0, 127, 0, 0, 0, 0,
	0, 120, 8, 16, 8, 112, 0, 0,
	0, 0, 120, 8, 8, 112, 0, 0,
	0, 0, 48, 72, 72, 48, 0, 0,

	0, 0, 248, 40, 40, 16, 0, 0,                                                   //896-903
	0, 0, 16, 40, 40, 248, 0, 0,
	0, 0, 112, 8, 8, 16, 0, 0,
	0, 0, 72, 84, 84, 36, 0, 0,
	0, 0, 8, 60, 72, 32, 0, 0,
	0, 0, 56, 64, 32, 120, 0, 0,
	0, 0, 56, 64, 56, 0, 0, 0,
	0, 56, 64, 32, 64, 56, 0, 0,

	0, 0, 72, 48, 48, 72, 0, 0,                                                    //960-967
	0, 0, 24, 160, 160, 120, 0, 0,
	0, 0, 100, 84, 84, 76, 0, 0,
	0, 0, 8, 28, 34, 65, 0, 0,
	0, 0, 0, 126, 0, 0, 0, 0,
	0, 0, 65, 34, 28, 8, 0, 0,
	0, 0, 4, 2, 4, 2, 0, 0,
	0, 120, 68, 66, 68, 120, 0, 0,
};

int iPage;                                                                       // kontrollerar vilken page flappy pixel är på
int iPos;                                                                        // Kontrollerar var i pagen flappy pixel är

int wall[10][5];                                                                 // håller väggar som visas i spelet
int wallFrames;                                                                  // räknar upp tills väggar ska flytta sig
int wallWait;                                                                    // räknar upp tills en ny vägg ska skapas
int wallsActive;                                                                 // antalet väggar på skärmen
int wallRisk;                                                                    // sätts till 1 när kollisionskontroll behövs
int wallRiskIndex;                                                               // vilken vägg som behöver kollisionskontroll

int flapFrames;                                                                  // antalet skärmuppdateringar som ett hopp räcker
int flapBtnCtrl;                                                                 // kollar så att knapp släpps innan den ger input igen

int dead;                                                                        // sätt till 1 när spelaren förlorar

int scores[4][6] = {                                                             // håller och namn för spelare
                    {0, 0, 0, 520, 520, 520},                                    // nuvarande poäng
                    {0, 0, 0, 520, 520, 520},                                    // highscore 1
                    {0, 0, 0, 520, 520, 520},                                    // highscore 2
                    {0, 0, 0, 520, 520, 520}};                                   // highscore 3

uint8_t gamefield[] = {                                                          // bitmap för spelet
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

uint8_t scorefield[] = {                                                         //bitmap för poäng och highscore
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


/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init( void ) {
  DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);

	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);

	spi_send_recv(0x8D);
	spi_send_recv(0x14);

	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);

	spi_send_recv(0xA1);
	spi_send_recv(0xC8);

	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	spi_send_recv(0xAF);
}


void display_image(int x, const uint8_t *data) {
	int i, j;

	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 64; j++)
			spi_send_recv(~data[i*64 + j]);
	}
}

/* returnerar läget på knappar */
int getBtns( void ){
  return (((PORTD & 0xe0) >> 4) | ((PORTF & 0x2) >> 1));
}

/* initiera / återställ poäng */
void resetScore( void ){
  int i, j;
  for (i = 0, j = 3; i < 3; i++, j++){
    scores[0][i] = 0;
    scores[0][j] = 520;
  }
}

/* initiera / återställ väggar */
void resetWalls( void ){
  int i, j;
  for (i = 0; i < 10; i++){
    for(j = 0; j < 5; j++){
      wall[i][j] = 0;
    }
  }
}

/* återställ spelfält */
void resetGamefield( void ){
  int i;
  for (i = 0; i < 256; i++){
    gamefield[i] = 0;
  }
}

/* återställ spelfält */
void resetScorefield( void ){
  int i;
  for (i = 0; i < 256; i++){
    scorefield[i] = 255;
  }
}

/* ritar spelares namn i poängfält */
void drawName(int rank, int pos){
  int i, j;
  for(i=0; i < 3; i++){
    for(j = 0; j < 8; j++){
      scorefield[j + (i * 8) + pos] = ~font[j + scores[rank][i + 3]];
    }
  }
}

/* ritar spelares poäng i poängfält */
void drawScore(int rank, int pos){
  int i, j;
  for (i = 0; i < 3; i++)
    for(j = 0; j < 8; j++){
      scorefield[j + (i * 8) + pos] = ~font[j + 384 + (scores[rank][i] * 8)];
  }
}

/* låter spelaren skriva in nytt namn till highscore */
void newHighscore(int rank){
  resetScorefield();
  int i, j;
  for(i = 0; i < 5; i++){                                                        // "-NEW-"
    for(j = 0; j < 8; j++){
      scorefield[j + (i * 8) + 12] = ~font[j + NEW[i]];
    }
  }
  for(i = 0; i < 7; i++){                                                        // "-HISCR-"
    for(j = 0; j < 8; j++){
      scorefield[j + (i * 8) + 68] = ~font[j + HIGHSCORE[i]];
    }
  }
  for(i = 0; i < 6; i++){                                                        // "1-NEXT"
    for(j = 0; j < 8; j++){
      scorefield[j + (i * 8) + 200] = ~font[j + NEXT[i]];
    }
  }
  drawName(0, 148);                                                              // ritar namnet som ska ändras
  display_image(64, scorefield);

  int menuBtnCtrl = 1;                                                           // sätts så att knapp släpps innan input ges igen
  int loop = 1;                                                                  // kontrollerar loop där namn ändras
  while(loop){
    for (i = 1; i <= 3; i++){
      if (((getBtns() >> i) & 0x1) && (menuBtnCtrl == 0)){                       // Kollar om knappen är intryckt
        scores[0][6 - i] += 8;                                                   // ökar font-index till nästa tecekn
        if(scores[0][6 - i] > 720){                                              // om förra tecknet var 'Z'
          scores[0][6 - i] = 520;                                                // sätt tecken till 'A'
        }
        menuBtnCtrl = 1;
      }
      else if((getBtns() & 0x1) && (menuBtnCtrl == 0)){                          // avslutar när knapp 1 trycks
        loop = 0;
      }
      else if ((getBtns() < 1)  && (menuBtnCtrl == 1)){                          // kontrollerar så att knappar släpps innan nytt input
        menuBtnCtrl = 0;
      }
    }
    drawName(0, 148);                                                            // ritar om namnet som ändras
    display_image(64, scorefield);
  }
  for (i = 3; i > rank; i--){                                                    // flyttar in det ny highscoret
    for(j = 0; j < 6; j++){
      scores[i][j] = scores[i - 1][j];
    }
  }
  for(i = 0; i < 6; i++){                                                        // lägger till det nya namnet
    scores[rank][i] = scores[0][i];
  }
}

/* kontrollerar om spelaren fått nytt highscore */
void checkHighscore( void ){
  if((scores[0][0] * 100 + scores[0][1] * 10 + scores[0][2]) >                  // om spelaren slagit högsta highscore
     (scores[1][0] * 100 + scores[1][1] * 10 + scores[1][2])){
    newHighscore(1);
  }
  else if((scores[0][0] * 100 + scores[0][1] * 10 + scores[0][2]) >             // om spelaren slagit mellersta highscore
          (scores[2][0] * 100 + scores[2][1] * 10 + scores[2][2])){
    newHighscore(2);
  }
  else if((scores[0][0] * 100 + scores[0][1] * 10 + scores[0][2]) >             //// om spelaren slagit lägsta highscore
          (scores[3][0] * 100 + scores[3][1] * 10 + scores[3][2])){
    newHighscore(3);
  }
}

/* ritar upp highscores på poängfält */
void drawHighscore( void ){
  int i, j;
  for(i = 0; i < 7; i++){                                                        // "-HISCR-"
    for(j = 0; j < 8; j++){
      scorefield[j + (i * 8) + 4] = ~font[j + HIGHSCORE[i]];
    }
  }
  for (i = 0; i < 3; i++){                                                       // riatar namn och poäng i hiscore-listan
    drawName((i + 1), (68 + (i * 64)));
    drawScore((i + 1), (100 + (i * 64)));
  }
  display_image(64, scorefield);
}

/* ritar flappy pixel på spelfält */
void drawFlappy( void ){
  gamefield[FLAPPY_PAGE[iPage]] |= FLAPPY_POS[iPos];
}

/* suddar flappy pixel på spelfält */
void undrawFlappy( void ){
  gamefield[FLAPPY_PAGE[iPage]] &= ~FLAPPY_POS[iPos];
}

/* flyttar flappy pixel en pixel nedåt */
void flappyDown( void ){
  undrawFlappy();                                                                // tar bort flappy pixel från gammal position
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
  drawFlappy();                                                                  // ritar flappy pixel på ny position
}

/* flyttar flappy pixel en pixel uppåt */
void flappyUp( void ){
  undrawFlappy();                                                                // tar bort flappy pixel från gammal position
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
  drawFlappy();                                                                  // ritar flappy pixel på ny position
}

/* kontrollerar om flappy flygit in i en given vägg */
void checkCrash(int wallNumber){
  if ((wall[wallNumber][iPage] | FLAPPY_POS[iPos]) == wall[wallNumber][iPage]){
    dead = 1;
  }
}

/* ökar spelaren poäng */
void wallPassed( void ){
  scores[0][2]++;                                                                // ökar 00x
  if (scores[0][2] > 9){
    scores[0][2] = 0;
    scores[0][1]++;                                                              // ökar 0x0
    if (scores[0][1] > 9){
      scores[0][1] = 0;
      scores[0][0]++;                                                            // ökar x00
    }
  }
  drawScore(0, 84);                                                              // ritar ny poäng
  display_image(64, scorefield);
}

/* flyttar vägg ett steg åt vänster */
void wallMove(int wallNumber){
  int i, j;
  for(i = 192, j = 0; i >= 0; i -= 64, j++){                                     //tar bort vägg från gamma position
    gamefield[wall[wallNumber][4] + i] &= ~wall[wallNumber][j];
  }
  if(wall[wallNumber][4] > 1){                                                   // om väggen inte nått slutet på skärmen
    wall[wallNumber][4]--;                                                       // flytta väggen åt vänster
  }
  else{
    for(i = wallNumber; i < wallsActive; i++){
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

/* skapar en vägg med slumpat hål */
void randomWall(int wallNumber){
  int hole = rand() % (28 - HOLE_SIZE) + 2;                                      // slumpad position för hålet
  int holePage = hole / 8;                                                       // page för hålet
  int holePos = hole % 8;                                                        // page-position för hålet

  wall[wallNumber][0]= 255;                                                      // skapar heltäckande vägg
  wall[wallNumber][1]= 255;
  wall[wallNumber][2]= 255;
  wall[wallNumber][3]= 255;
  wall[wallNumber][4]= 64;

  wall[wallNumber][holePage] = wall[wallNumber][holePage] <<                     // skapar nedre del av hålet i väggen
                              (HOLE_SIZE - holePos);
  wall[wallNumber][holePage + 1] = wall[wallNumber][holePage + 1] >> holePos;    // skapar övre del av hålet i väggen
}

/* Interrupt Service Routine

  Körs 20 ggr per sekund, anropar spelets logik och uppdaterar  display*/
user_isr( void )
{
  if ((((getBtns() >> 3) & 0x1) == 0x1) && (flapBtnCtrl == 0)){                  // om knapp fyra trycks
      flapFrames = FLAP_TIME;                                                    // starta hopp
      flapBtnCtrl = 1;
  }
  else if ((((getBtns() >> 3) & 0x1) == 0x0) && (flapBtnCtrl == 1)){             // om knapp släppts
    flapBtnCtrl = 0;                                                             // tillåt ny input
  }

  if (flapFrames > (FLAP_TIME - FLAP_HEIGHT)){                                   // om flapframes inte nått FLAP_HEIGHT
    flappyUp();                                                                  // flyttar upp flappy pixel
    flapFrames--;                                                                // sänk flapframes
  }
  else if(flapFrames > 0){                                                       // om det finns flapframes kvar
    flapFrames--;                                                                // sänk flapframes
  }
  else{                                                                          // om hopp inte pågår
    flappyDown();                                                                // fall
  }

  if (wallRisk == 1){                                                            // om flappy pixel är på samma x position som vägg
    checkCrash(wallRiskIndex);                                                   // kontrollera kollision
  }

  wallFrames++;
  if (wallFrames == WALL_RATE){                                                  // om WALL_RATE nåtts
    wallWait++;
    if (wallWait == WALL_SPACE){                                                 // om WALL_SPACE nåtts
      randomWall(wallsActive);                                                   // skapar ny vägg
      wallsActive++;
      wallWait = 0;
    }

    int i;
    for (i = 0; i < wallsActive; i++){                                           // flyttar väggar
      wallMove(i);
    }
    wallFrames = 0;
  }

  if (wallRisk == 1){                                                            // om flappy pixel är på samma x position som vägg
    checkCrash(wallRiskIndex);                                                   // kontrollera kollision
  }

  if (dead == 1){                                                                // om spelaren förlorat
    disable_interrupt();                                                         // stoppar spelets logik
  }
  else{                                                                          // om spelaren inte förlorat
    display_image(0, gamefield);                                                 // uppdatera spelfält
  }

  IFSCLR(0) = 0x100;                                                             // återställer interrupt-flaggan
}

/* initierar spelet */
void flappyInit( void )
{
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

  resetScore();
  resetWalls();
  resetGamefield();
  resetScorefield();

  drawFlappy();
  int i, j;
  for(i = 0; i < 6; i++){                                                        // "4-FLAP"
    for(j = 0; j < 8; j++){
      gamefield[j + (i * 8) + 200] = font[j + FLAP[i]];
    }
  }
  display_image(0, gamefield);
  drawHighscore();

  T2CONSET = 0x8000;                                                             //startar timer för rand seed


  while(1){                                                                      // pausad till knapp 4 trycks
    if ((((getBtns() >> 3) & 0x1) == 0x1)){
      break;
    }
  }

  srand(TMR2);                                                                   // seeda rand
  randomWall(0);                                                                 // skapar första väggen

  resetGamefield();
  resetScorefield();
  drawScore(0, 84);
  for(i = 72; i <= 112; i+=40){                                                  // ritar '*' bredvid poäng
    for(j = 0; j < 8; j++){
      scorefield[i + j] = ~font[336 + j];
    }
  }
  display_image(64, scorefield);

  T2CON = 0x0;                                                                   // nollställer kontrollfunktioner för timer 2
  T2CONSET = 0x70;                                                               // väljer skala 1:256
  TMR2 = 0x0;                                                                    // nollställer timer
  PR2 = 15625;                                                                   // timer 2 når detta på 0,05s | 80 000 000 / 256 / 20 = 15625
  IFSCLR(0) = 0x100;                                                             // nollställer interrupt flaggan för timer 2
  IECSET(0) = 0x100;                                                             // tillåter interrupts för timer 2
  IPCSET(2) = 0x1f;                                                              // högsta int prio=7 och subprio=3 för timer 2
  enable_interrupt();                                                            // slår på interrupts globalt
  T2CONSET = 0x8000;                                                             // slår på timer 2
  return;
}

/* Körs konstant, skapar nyspelomgång när spelaren förlorat */
void flappyLoop( void )
{
  if (dead == 1){                                                                // om spelaren förlorat
    int i, j;
    for(i = 0; i < 8; i++){                                                      // "GAMEOVER"
      for(j = 0; j < 8; j++){
        scorefield[j + (i * 8)] = ~font[j + GAMEOVER[i]];
      }
    }
    for(i = 0; i < 6; i++){                                                      // "1-NEXT"
      for(j = 0; j < 8; j++){
        scorefield[j + (i * 8) + 200] = ~font[j + NEXT[i]];
      }
    }
    display_image(64, scorefield);

    while(1){                                                                    // väntar tills knapp 1 tryckts
      if (((getBtns() & 0x1) == 0x1)){
        break;
      }
    }
    checkHighscore();                                                            // kollar om spelaren fått highscore
    flappyInit();                                                                // startar om spel
  }
}
