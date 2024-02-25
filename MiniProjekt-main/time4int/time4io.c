#include <stdint.h>   
#include "/Applications/mcb32tools.app/Contents/Resources/Toolchain/include/pic32mx.h"
#include "mipsproject.h" 

int getsw( void ){ //den här kan vi slänga (tror jag)

volatile int switches = (PORTD >> 8);
switches &= 0x0000000F;
return switches;
}

int getbtns(void){ //Nu tar vi bara info från knappar 4 och 3 (tror jag)

volatile int buttons42 = (PORTD >> 6);
buttons42 &= 0x00000003;
return buttons42;

}