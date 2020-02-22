/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
int mytime = 0x5957;

volatile int* trisE = (volatile int*) 0xbf886100; //pekare port E I/O kontroll
volatile int* portE = (volatile int*) 0xbf886110; //pekare port E läge
int timeoutcount = 0; // ska öka när timer 2 når overflow

int prime = 1234567;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
user_isr( void )
{
  if((IFS(0) >> 19) & 0x1){
    *portE += 0x1;  // Ökar minnet som LED läser med 1
    IFSCLR(0) = 0x80000;
  }

  if((IFS(0) >> 8) & 0x1){  // kollar interrupt-flaggan
    timeoutcount++; // ökar timerräknaren
    IFSCLR(0) = 0x100; //återställer flaggan
  }

  if(timeoutcount == 10){ //kollar om räknaren nått 10 (1 sek)
    time2string( textstring, mytime );
    display_string( 3, textstring );
    display_update( );
    tick( &mytime );

    timeoutcount = 0; // återställer räknaren
  }
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  *trisE &= ~0xff;  // Gör LED redo för output
  *portE = 0x0;     // Ser till att alla LED är av
  TRISDSET = 0x800;   // gör switch 4 redo för input
  IFSCLR(0) = 0x80000;
  IECSET(0) = 0x80000;
  IPCSET(4) = 0x1f;

  T2CON = 0x0;      // nollställer kontrollfunktioner för timer 2
  T2CONSET = 0x70;  // väljer skala 1:256
  TMR2 = 0x0;       // nollställer timer
  PR2 = 31250;  // timer 2 når detta på 0,1s | 80 000 000 / 256 / 10 = 31250
  IFSCLR(0) = 0x100; // nollställer interrupt flaggan för timer 2               Interrupt Flag Status
  IECSET(0) = 0x100; // tillåter interrupts för timer 2                         Interrupt Enable Control
  IPCSET(2) = 0x1f; // högsta int prio=7 och subprio=3 för timer 2              Interrupt Priority Control
  enable_interrupt(); // slår på interrupts globalt                             STATUS
  T2CONSET = 0x8000; // slår på timer 2
  return;
}

/* This function is called repetitively from the main program */
void labwork( void )
{
  prime = nextprime( prime );
  display_string( 0, itoaconv( prime ) );
  display_update( );
}
