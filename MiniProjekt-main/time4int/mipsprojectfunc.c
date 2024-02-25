/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include "/Applications/mcb32tools.app/Contents/Resources/Toolchain/include/pic32mx.h" /* Declarations of system-specific addresses etc */
  /* Declarations of system-specific addresses etc */
#include <stdlib.h>
#include "mipsproject.h"  /* Declatations for these labs */
#include "pipes.h"

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

/*Inte ändrad från labb 3. 
  Skickar och tar emot data från bufferts*/
uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

/*Inte ändrad från labb 3.*/
void display_init(void) {
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
  /*8Dh ; Charge Pump Setting
    14h ; Enable Charge Pump */
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
  /*A[3:0] : Phase 1 period of up to 15 DCLK
    A[7:4] : Phase 2 period of up to 15 DCLK
    (A[3:0] -> 0001)
    (A[4:7] -> 1111)
*/
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);	/*A1h, X[0]=1b: column address 127 is mapped to
    SEG0 */
	spi_send_recv(0xC8);
/*C8h, X[3]=1b: remapped mode. Scan from
COM[N-1] to COM0
Where N is the Multiplex ratio. */

	spi_send_recv(0xDA);
	spi_send_recv(0x20);
  /*A[4]=0b, Sequential COM pin configuration 
    A[5]=1b, Enable COM Left/Right remap 
    */
	
	spi_send_recv(0xAF);
  /*AFh X[0]=1b:Display ON in normal mode  */
}

/*Inte ändrad från labb 3. */
void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}
/*display_icon med några skillnader.
parameter är nu bitmap, här kommer vi (nog alltid) ha
whole[] (hela skärmen). 
eftersom vi nu täcker hela skärmen så är nu 
for-loop j<128 (tidigare j<32)
bitmap[i*128 + j] (tidigare array[i*32 + j])*/
void display_screen(uint8_t bitmap[]) {
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);
    /*Setup page start and end address
    A[2:0] : Page start Address, range : 0-7d,
    (RESET = 0d) */
		
		spi_send_recv(0x10);
    /*Set the higher nibble of the column start address
    register for Page Addressing Mode using X[3:0]
    as data bits. The initial display line register is
    reset to 0000b after RESET. */
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
    /*i representerar varje page på skärmen*/
		for(j = 0; j < 128; j++)
			spi_send_recv(bitmap[i*128 + j]); // So it is responsible to send if pixels should be on or not.
	}
}
/*Inte ändrad från labb 3. */
void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/*Ger oss ett pseudo-slumpmässigt nummer mellan 0-15
Aron 25/02 */
int random_number(){ 
   int number = (TMR4 % 15);
   return number;
}

/*Stänger av alla lampor.
Aron 23/02 */
void clear_screen(){
  int i;
  for (i = 0; i<512; i++)
    whole[i] = 0;
}


/*
pow() i math.h funkade inte så denna fungerar som substitut. 
Borde vara lite snabbare också
*/
int pow[8] = {1,2,4,8,16,32,64,128}; 
/*Tar ett input (x,y) och tänder den pixeln på skärmen. 
(0,0) är pixeln i övre vänstra hörnet. 
(127,31) är pixeln i nedre högra hörnet.
Aron 24/02*/
light_up(int x, int y){
  if (x > 127) {x = 127;} 
  //innan: Om x > 127, utanför skärmen och hoppar till nästa rad
  int page = y / 8 ;
  whole[(128 * page) + x] |= pow[y - (page*8)]; // caculates the x and y cordinate 
	                                       //of which pixel on which page should be activated.
}

/* Aron 25/02 */
/*första parametern är rörens x-pos.
gap_position är vart hålet börjar i y-led  
- eller likvärdigt där det första röret slutar i y-led. */
draw_pipe(pipe_pos_x, gap_position){
int i;
int j;

/* Övre röret*/
  for(j = (pipe_width+pipe_pos_x); j>pipe_pos_x;j--){
  for(i = gap_position; i>0;i--)
      light_up(j, i);
  }

/*Hålet*/

/*Nedre röret*/
  for(j =  (pipe_width+pipe_pos_x); j>pipe_pos_x;j--){
  for(i = 31; i>(gap_position + pipe_gap);i--)
      light_up(j, i);
  }
}
/*Lyser upp pixlarna som ritar fågeln som finns i bird.h
Alla x-värden är adderade med 10 (bird_x_pos)*/
draw_bird(int y){
y--;
light_up(15,0+y);

light_up(13,1+y);
light_up(14,1+y);
light_up(15,1+y);
light_up(16,1+y);

light_up(11,2+y);
light_up(12,2+y);
light_up(17,2+y);

light_up(10,3+y);
light_up(15,3+y);
light_up(17,3+y);

light_up(10,4+y);
light_up(17,4+y);

light_up(11,5+y);
light_up(16,5+y);

light_up(10,6+y);
light_up(12,6+y);
light_up(13,6+y);
light_up(14,6+y);
light_up(15,6+y);

light_up(11,7+y);
}

/*Denna uppdaterar all information.*/
void next_frame(bird_pos,pipe_pos_x,gap_position){
  clear_screen();
  draw_pipe(pipe_pos_x, gap_position);
  draw_bird(bird_pos);
  display_screen(whole);
}

/*Intro-animation*/
void intro_animation(){
  display_string(0,"");
  display_string(1,"");
int i;
int j;
clear_screen();

for(j = 0; j<5; j++){
for(i = 0; i<512; i++)
  whole[i] = intro2[i];
    display_screen(whole);
quicksleep(100000);
}

for(j = 0; j<5; j++){
for(i = 0; i<512; i++)
  whole[i] = intro1[i];
    display_screen(whole);
quicksleep(100000);
}
for(j = 0; j<5; j++){
for(i = 0; i<512; i++)
  whole[i] = intro2[i];
    display_screen(whole);
quicksleep(100000);
}
for(j = 0; j<5; j++){
for(i = 0; i<512; i++)
  whole[i] = intro3[i];
    display_screen(whole);
quicksleep(100000);
}
for(j = 0; j<5; j++){
for(i = 0; i<512; i++)
  whole[i] = intro4[i];
    display_screen(whole);
quicksleep(100000);
}
for(j = 0; j<5; j++){
for(i = 0; i<512; i++)
  whole[i] = intro5[i];
    display_screen(whole);
quicksleep(100000);
}
for(j = 0; j<5; j++){
for(i = 0; i<512; i++)
  whole[i] = intro6[i];
    display_screen(whole);
quicksleep(100000);
}
for(j = 0; j<5; j++){
for(i = 0; i<512; i++)
  whole[i] = intro6[i];
    display_screen(whole);
quicksleep(100000);
}
for(j = 0; j<5; j++){
for(i = 0; i<512; i++)
  whole[i] = intro2[i];
    display_screen(whole);
quicksleep(100000);
}
for(j = 0; j<5; j++){
for(i = 0; i<512; i++)
  whole[i] = intro6[i];
    display_screen(whole);
quicksleep(100000);
}}

/*
 * itoa
 * 
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 * 
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 * 
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 * 
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 * 
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 * 
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 * 
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 * 
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";
  
  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );



}
