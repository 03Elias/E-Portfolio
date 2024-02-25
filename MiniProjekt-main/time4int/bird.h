#include <stdint.h>   /* Declarations of uint_32 and the like */
#include "mipsproject.h"  /* Declatations for these labs */

#define bird_x_pos 10
#define bird_height 6
#define bird_width 8
          
//Agerar som score.
int pipes_cleared;
//Fågelns y-position, alltså den enda som kan ändras av spelaren.
float bird_pos;
float bird_speed;
float gravity;

//bitmap av fågeln :)
uint8_t const bird[] = { 
0x58, 0xa4, 0x44, 0x42, 0x42, 0x4b, 0x22, 0x1c 
};
