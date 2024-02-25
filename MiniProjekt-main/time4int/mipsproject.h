/* mipsproject.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from projectfunc.c */
void display_screen(uint8_t bitmap[]);
void display_init(void);
void display_string(int line, char *s);

void display_update(void);
void clear_screen();
uint8_t spi_send_recv(uint8_t data);

/* Declare project-related functions from projectfunc.c */
char * itoaconv( int num );
void projectwork(void);
void quicksleep(int cyc);

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];
//HÄR KOMMER FÅGELN
extern const uint8_t const bird[8];

/*När spelet är pausat: */
extern const uint8_t const pause[128 * 4];
//Det här är hela skärmen
extern uint8_t whole[128 * 4];



/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */

//void delay(int);
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);


/*Bitmaps för intro*/
extern const uint8_t const intro1[512];
extern const uint8_t const intro2[512];
extern const uint8_t const intro3[512];
extern const uint8_t const intro4[512];
extern const uint8_t const intro5[512];
extern const uint8_t const intro6[512];