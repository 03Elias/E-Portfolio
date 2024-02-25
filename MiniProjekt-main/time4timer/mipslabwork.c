/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include "/Applications/mcb32tools.app/Contents/Resources/Toolchain/include/pic32mx.h" /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int mytime = 0x5957;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  volatile int* trise = (int) 0xbf886100;
  *trise &= 0xFFFFFF00;
  volatile int* porte = (int) 0xbf886110;
  *porte = 0xFFFFFF00;
  TRISDSET = 0x00000FE0;

T2CON = 0x070; // prescaler är 256:1. Allt annat cleared.
TMR2 = 0x0; // Töm timer register 
PR2 = 0x7A12; // period register  = 31 250
T2CONSET = 0x8000; // starta timer 2 
  return;
}

int timeoutcount = 0;
/* This function is called repetitively from the main program */
void labwork( void )
{
  int temp;
  volatile int* porte = (int) 0xbf886110;
  (*porte)++;

  if (getbtns() & 0x4){
  mytime = (mytime &= 0x0FFF) | (getsw() << 12); 
  }

  if (getbtns() & 0x2){
  mytime = (mytime &= 0xF0FF) | (getsw() << 8); 
  }
  if (getbtns() & 0x1){
  mytime = (mytime &= 0xFF0F) | (getsw() << 4); 
  }

  if( IFS(0) & 0x0100){ //bit 8
    IFS(0) &= 0x11111E11; // flag = 0
    timeoutcount++; 
  }
  if (timeoutcount == 10){
    display_update();
    tick( &mytime );
    timeoutcount = 0;
  }
    display_image(96, icon);
}
