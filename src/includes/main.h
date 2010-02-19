#ifndef _MAIN_H
#define _MAIN_H 1



#define GS_HAYMOUSE 0x01
#define GS_GUNSTICK 0x02
#define GS_INACTIVE 0x04

struct tipo_emuopt
{
	char romfile[256];
	char snapfile[256];
	char tapefile[256];
	unsigned int gunstick;
//        BITMAP * raton_bmp;
	char joytype;
	char joy1key[5];
};


typedef struct 
{
  int port_ff;                  // 0xff = emulate the port,  0x00 always 0xFF
  int ts_lebo;			// left border t states
  int ts_grap;			// graphic zone t states
  int ts_ribo;			// right border t states
  int ts_hore;			// horizontal retrace t states
  int ts_line;			// to speed the calc, the sum of 4 abobe
  int line_poin;		// lines in retraze post interrup
  int line_upbo;		// lines of upper border
  int line_grap;		// lines of graphic zone = 192
  int line_bobo;		// lines of bottom border
  int line_retr;		// lines of the retrace
   
  int TSTATES_PER_LINE;
   
  int TOP_BORDER_LINES;
   
  int SCANLINES;
   
  int BOTTOM_BORDER_LINES;
   
  int tstate_border_left;
   
  int tstate_graphic_zone;
   
  int tstate_border_right;
} tipo_hwopt;




extern tipo_hwopt hwopt;

int emuMain (int argc, char *argv[]);

int Z80Initialization (void);

/* Esto permite suprimir CASI TODOS los mensajes en aquellos sistemas que
no dispongan de STDOUT o esta corrompa la pantalla como en MacOS o MS-Dos

simplemente utilizar ASprintf en lugar de printf
*/

#ifdef ENABLE_LOGS
#define ASprintf printf
#else
void ASprintf (char *, ...);
#endif

#endif	/* main.h */  
