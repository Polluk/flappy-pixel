/* Flappy.h

   This file written 2020 by Lukas Pöldma

*/

/* funktioner definierade i FlappyGame */
void quicksleep(int cyc);
uint8_t spi_send_recv(uint8_t data);
void display_init( void );
void display_image(int x, const uint8_t *data);
int getBtns( void );
void resetScore( void );
void resetWalls( void );
void resetGamefield( void );
void resetScorefield( void );
void drawName(int rank, int pos);
void drawScore(int rank, int pos);
void newHighscore(int rank);
void checkHighscore( void );
void drawHighscore( void );
void drawFlappy( void );
void undrawFlappy( void );
void flappyDown( void );
void flappyUp( void );
void checkCrash(int wallNumber);
void wallPassed( void );
void wallMove(int wallNumber);
void randomWall(int wallNumber);
void flappyInit( void );
void flappyLoop( void );

/* funktioner definierade i Interrupt.s */
void enable_interrupt(void);
void disable_interrupt(void);
