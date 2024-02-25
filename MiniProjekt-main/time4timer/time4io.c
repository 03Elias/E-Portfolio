#include <stdint.h>   
#include "/Applications/mcb32tools.app/Contents/Resources/Toolchain/include/pic32mx.h"
#include "mipslab.h" 

int getsw( void ){

volatile int switches = (PORTD >> 8);
switches &= 0x0000000F;
return switches;
}

int getbtns(void){

volatile int buttons42 = (PORTD >> 5);
buttons42 &= 0x00000007;
return buttons42;

}