#include <stdint.h>   /* Declarations of uint_32 and the like */
#include "mipsproject.h"  /* Declatations for these labs */

#define pipe_width 2
#define pipe_height 10
#define pipe_gap 15

float pipe_pos_x;
float pipe_speed;
float pipe_speed_speedup;
float temp_pipeSpeed; 

int gap_position;