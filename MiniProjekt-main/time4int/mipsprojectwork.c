/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include "/Applications/mcb32tools.app/Contents/Resources/Toolchain/include/pic32mx.h" /* Declarations of system-specific addresses etc */
#include "mipsproject.h"  /* Declatations for these labs */
#include "pipes.h"
#include "bird.h"

/*counter som plussas på i varje timer-interrupt, labb 3*/
int timeoutcount = 0;

/*Bitmap som är hela skärmen,
det är här allt händer! initieras till 0.
(Varje element är 1 byte och är 8 pixlar. Alltså 128 * 8 * 4 = 128*32 pixlar)
Aron, 24/02*/
uint8_t whole[128 *4] = {0};

/* Interrupt Service Routine */
void user_isr( void ) {

  if(IFS(0) & 0x0000100){
    IFS(0) &= 0xFFFEFF;// Acknowledge interrupt from Timer 2
    timeoutcount++;
}
if(IFS(0) & 0x0008000){
IFS(0) &= 0xF7FFF; // Acknowledge interrupt from INT3

if(pipe_speed > 0){
int i;
int count;
temp_pipeSpeed = pipe_speed; 
pipe_speed = 0;
pipe_speed_speedup = 0;
bird_speed = 0;
gravity = 0;

}
else {
pipe_speed = temp_pipeSpeed;
pipe_speed_speedup = 0.000000002;
bird_speed = 0.003;
gravity =  0.0004;

}
}
}

/* Lab-specific initialization goes here */
void projectinit( void )
{
TRISDSET = 0x00000FE0; //PortD innehåller knappar och switches.

/*Här sätter vi upp timer 2 som styr fps*/
/*Detta är taget från vår labb3, dock har period register ändrats.*/
/*80 000 000 / 256 /521 ≈ 600 */
/*Klockan flaggas alltså ca 600/s 
  vilket ger oss ~60 fps då vår info uppdateras var 10 gång. */
T2CON = 0x070; // prescaler är 256:1. Allt annat cleared.
TMR2 = 0x0; // Töm timer register 
PR2 = 521; // period register  = 521
T2CONSET = 0x8000; // starta timer 2 

IEC(0) |= 0x00000100; // Timer2 interrupt enable (bit 8)
IPC(2) |= 0x0000000F; // Priority 3 (bits 0-1 är sub, 2-4 är vanlig)
asm volatile("ei");  // enable interrupts on the micro-controller

/*Fixar interrupt som styrs av switch 2, 
denna används till pausning.
Denna gjorde vi på labb3 (surprise assignment). */
IEC(0) |= 0x00008000; // INT3 interrupt enable (bit 15)
IPC(3) |= 0x0000001F; // Högsta priority och sub-priority. (bits 0-1 är sub, 2-4 är vanlig)
INTCONSET = 0x08;     // Upwards polarity

/*Startar timer 4 enbart för random_number-funktionen. */
T4CON = 0x060; // prescaler är 64:1. Allt annat cleared.
TMR4 = 0x0; // Töm timer register 
PR4 = 25333; // period register - tar annan siffra så inte delbar med vår timer
T4CONSET = 0x8000; // starta timer 4

/*Initerar spelvariabler*/
pipes_cleared= -1; //Första pipen spawnar vänster om fågeln. 
pipe_pos_x = -5;
pipe_speed = 0.0006; 
pipe_speed_speedup = 0.000000002;
bird_speed = 0.003;
gravity =  0.0004;
return;
}

/* This function is called repetitively from the main program */
void projectwork( void ) {

  if(getbtns() & 0b010){
    bird_pos-= bird_speed;
    if(bird_pos < 0){ bird_pos = 0;} //fågel får inte gå ovanför skärmen
  }
  if(getbtns() & 0b001){
    bird_pos+= bird_speed;
  }
        bird_pos += gravity;
        if(bird_pos> 31 - bird_height ) { bird_pos = 31-bird_height;} //får inte gå under marken.
        
        /*Respawn pipes*/
        if (pipe_pos_x < -5){
          pipe_pos_x = 128;
          gap_position = random_number()+1;
          pipes_cleared++;
        }
        /*Rörets hastighetsfunktion*/
        pipe_pos_x -= pipe_speed;
        /*Acceleration*/
        pipe_speed += pipe_speed_speedup;
        
        /*Funktionerna kallas*/
        if(timeoutcount >= 10){
        next_frame((int)bird_pos, 
        (int)pipe_pos_x,(int)gap_position);
        timeoutcount = 0;
        }

/*Kollision*/

if (((int)bird_x_pos) + bird_width == (int) pipe_pos_x)
    if( (bird_pos < gap_position) ||
     (bird_pos + bird_height > (gap_position + pipe_gap))){
  int i;
   clear_screen();
   /*Visa score*/
   char* score = itoaconv(pipes_cleared);
    
    display_string(0,"Pipes cleared: ");
    display_string(1,score);

  for(i = 0; i<2000; i++){
    display_update();
  }

main();

}
  }
