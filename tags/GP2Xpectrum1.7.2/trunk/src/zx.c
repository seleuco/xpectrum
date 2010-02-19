/* zx.c: 
   Copyright (c) 2005-2008 rlyeh, Hermes/PS2R, Metalbrain, Philip Kendall

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

#include "shared.h"

#include "zxbios.h"

// ok?:
// 48: 224 tstates/line, 312 lines, 69888 tstates/frame
//       0 tstates before int, 14335 tstates before paper
//       3 tstates ula buffer
//       65432100  sequence
//       4 tstates border granularity
//       contended page 5 (0x4000)
//       contended FE port
//       emulate snow
//       emulate in FF
//128: 228 tstates/line, 311 lines, 70908 tstates/frame
//       0 tstates before int, 14361 tstates before paper
//      -2 tstates ula buffer
//       65432100  sequence
//       4 tstates border granularity
//       contended page 1,3,5,7
//       contended FE port
//       emulate snow
//       emulate in FF
//+2:  228 tstates/line, 311 lines, 70908 tstates/frame
//       0 tstates before int, 14361 tstates before paper
//      -2 tstates ula buffer
//       65432100  sequence
//       4 tstates border granularity
//       contended page 4,5,6,7 (no!: cssfaq)
//       contended FE port
//       emulate snow
//       emulate in FF
//+2A: 228 tstates/line, 311 lines, 70908 tstates/frame
// +3:   0 tstates before int, 14364 tstates before paper
//      -2 tstates ula buffer
//       10765432  sequence
//       4 tstates border granularity
//       contended page 4,5,6,7
//       contended FE port (no!: cssfaq)
//       emulate snow (no!: cssfaq)
//       emulate in FF (no!: cssfaq)
//pent:224 tstates/line, 320 lines, 71680 tstates/frame
//       0 tstates before int, 17982 tstates before paper
//       0 tstates ula buffer
//       000000000  sequence
//       1 tstates border granularity
//       no contended pages 
//       no contended FE port
//       no emulate snow
//       no emulate in FF

// old faq (right?) :
// Bit 6 of port #FE of the +2A/+3 does not show the same dependence on what was written to port #FE as it does on the other machines, and always returns 0 if there is no signal. Finally, reading from a non-existing port (eg #FF) will always return 255, and not give any screen/attribute bytes as it does on the 48K/128K/+2.

extern MCONFIG mconfig;

byte inline Z80ReadMem(register word where);
byte inline Z80ReadMem_notiming(register word where);
extern void msg(char *s);
extern scan_convert[];

#define debugmsg(a,b) { char text[255]; sprintf(text,"%s = %04x",a,b); msg(text); }


          byte cycles_delay[76000];
          byte cycles_delay2[76000];
unsigned short floating_bus[76000];

// *** NEW VIDEO VARIABLES ***
int memwrites=0;
int memwriteaddr[5500];
int memwritetime[5500];
int memwritevalue[5500];

int outwrites=0;
int outwritetime[5500];
int outwritevalue[5500];

int pagewrites=0;
int pagewritetime[5500];
int pagewritevalue[5500];

unsigned int Significance[65536];

byte tape_format;

/*
extern int TSTATES_PER_LINE, TOP_BORDER_LINES, BOTTOM_BORDER_LINES, SCANLINES;
*/


struct tipo_emuopt emuopt =
  { "\0", "\0", "\0", GS_INACTIVE, 'n', {'o', 'p', 'q', 'a', ' '} };


// switch between display by scanlines or display at end of frame
//int displayByScanlines=1;

/* Some global variables used in the emulator */
Z80Regs *spectrumZ80;



#ifdef SOUND_X128
//extern AY8910_CORE AY;
#endif

void io_routine_out(void) {}
  u8 io_routine_in(void)  { return 0; }




//128 y plus2 estan usando timing de +2a y +3
//pte buscar timing 128 y plus2 exacto

extern tipo_hwopt hwopt;

/*
  48:
  interrupt, then first pixel of screen is displayed after 64 lines
              16 lines post interrupt
  then after  48 lines top border
  then after 192 lines main lines (24T lb + 128T scr + 24T rb + 48 hsync = 224T/line)
  then after  48 lines top bottom
  then after   8 lines vsync
                       +
  ----------------------
             312 lines / (64+192+56)*224T=69888T

  128:
  interrupt, then first pixel of screen is displayed after 64 lines
              15 lines post interrupt
  then after  48 lines top border
  then after 192 lines main lines (26T lb + 128T scr + 26T rb + 48 hsync = 228T/line)
  then after  48 lines top bottom
  then after   8 lines vsync
                       +
  ----------------------
             311 lines / (63+192+56)*228T=70908T

  - o es 15/48 o es 16/47 para que sumen 63
  - 52 de hsync? o 26+26?, para que sumen 228
*/

tipo_hwopt hwopt_48 = {
  0xFF,                               //emulate the port ff
  24, 128, 24, 48,     224,           //tstates: left,graphic,right,hsync, tstates/scaline (sum)
  16, 48, 192, 48, 8,                 //lines: post interrupt, lines upper, graphic, bottom, retrace
                       224,           //tstates per line (again)
      64, 192, 56,                    //top scan,main scanlines,bottom scanlines
  24, 128,     72                     //border left,graphic,border right (including hsync)
};


//128: o es 15/48 o es 16/47, pero la suma tiene que dar 63 top border lines
//128: = q antes, los 228 no se como se reparten, los he puesto en 48+4

//128, +2
tipo_hwopt hwopt_128 = {
  0xFF,                               //emulate the port ff
  24, 128, 24, 52,     228,           //tstates: left,graphic,right,hsync, tstates/scaline (sum)
  15, 48, 192, 48, 8,                 //lines: post interrupt, lines upper, graphic, bottom, retrace
                       228,           //tstates per line (again)
      63, 192, 56,                    //top scan,main scanlines,bottom scanlines
  24, 128,     76                     //border left,graphic,border right (including hsync)
};


//+2A,+3
tipo_hwopt hwopt_plus2a = {
  0xFF,                               //emulate the port ff
  24, 128, 24, 52,     228,           //tstates: left,graphic,right,hsync, tstates/scaline (sum)
  15, 48, 192, 48, 8,                 //lines: post interrupt, lines upper, graphic, bottom, retrace
                       228,           //tstates per line (again)
      63, 192, 56,                    //top scan,main scanlines,bottom scanlines
  24, 128,     76                     //border left,graphic,border right (including hsync)
};


byte *MEMr[4]; //solid block of 16*4 = 64kb for reading
byte *MEMw[4]; //solid block of 16*4 = 64kb for writing
byte  MEMc[4]; //contended 16k block? 1/0
byte  MEMs[4]; //screen 16k block? 1/0
byte  GAME[1*1024*1024]; 
byte  DSK[1*1024*1024]; 
long GAME_size;   //game(s) workspace
byte  RAM_dummy[16384*1];  //to emulate spectrum 16
byte  ROM_dummy[16384*1];
byte  RAM_pages[16384*16]; //up 16 pages (ZS Scorpion)
byte  ROM_pages[16384*4];  //up  4 pages (ZX +2A/+3)
byte  model, pagination_128, pagination_plus2a, contended_mask;
byte  BorderColor;
//byte  trap_rom_loading;
byte  kempston, fuller;



int
initialize_16 (void)
{
  model=ZX_16;

  memcpy(&ROM_pages[0x4000*0],&spectrum_rom_48[0x4000*0],0x4000);

  memcpy(&hwopt, &hwopt_48, sizeof(tipo_hwopt));

  //new layout to support flat 48k at UncompressZ80() (0567 instead of 0520)

  MEMr[3]=&RAM_dummy[0x4000*0-3*0x4000]; MEMc[3]=0; MEMs[3]=0;
  MEMr[2]=&RAM_dummy[0x4000*0-2*0x4000]; MEMc[2]=0; MEMs[2]=0;
  MEMr[1]=&RAM_pages[0x4000*5-1*0x4000]; MEMc[1]=1; MEMs[1]=1; //contended, screen 1
  MEMr[0]=&ROM_pages[0x4000*0]; MEMc[0]=0; MEMs[0]=0;

  MEMw[3]=&RAM_pages[0x4000*7-3*0x4000];
  MEMw[2]=&RAM_pages[0x4000*6-2*0x4000];
  MEMw[1]=&RAM_pages[0x4000*5-1*0x4000];
  MEMw[0]=&ROM_dummy[0x4000*0];

  pagination_128=32;
  pagination_plus2a=128;
  contended_mask=1;

  return 1;
}

int
initialize_48 (void)
{
  model=ZX_48;

  memcpy(&ROM_pages[0x4000*0],&spectrum_rom_48[0x4000*0],0x4000);

  memcpy(&hwopt, &hwopt_48, sizeof(tipo_hwopt));

  //new layout to support flat 48k at UncompressZ80() (0567 instead of 0520)

  MEMw[3]=MEMr[3]=&RAM_pages[0x4000*7-3*0x4000]; MEMc[3]=0; MEMs[3]=0;
  MEMw[2]=MEMr[2]=&RAM_pages[0x4000*6-2*0x4000]; MEMc[2]=0; MEMs[2]=0;
  MEMw[1]=MEMr[1]=&RAM_pages[0x4000*5-1*0x4000]; MEMc[1]=1; MEMs[1]=1; //contended, screen 1
  
  MEMr[0]=&ROM_pages[0x4000*0]; MEMc[0]=0; MEMs[0]=0;
  MEMw[0]=&ROM_dummy[0x4000*0];

  pagination_128=32;
  pagination_plus2a=128;
  contended_mask=1;

  return 1;
}


int
initialize_128 (void)
{
  model=ZX_128;

  memcpy(&ROM_pages[0x4000*0],&spectrum_rom_128[0x4000*0],0x4000);
  memcpy(&ROM_pages[0x4000*1],&spectrum_rom_128[0x4000*1],0x4000);
  
  memcpy(&hwopt, &hwopt_128, sizeof(tipo_hwopt));

  MEMw[3]=MEMr[3]=&RAM_pages[0x4000*0-3*0x4000]; MEMc[3]=0; MEMs[3]=0;
  MEMw[2]=MEMr[2]=&RAM_pages[0x4000*2-2*0x4000]; MEMc[2]=0; MEMs[2]=0;
  MEMw[1]=MEMr[1]=&RAM_pages[0x4000*5-1*0x4000]; MEMc[1]=1; MEMs[1]=1; //contended,screen 1
          MEMr[0]=&ROM_pages[0x4000*0]; MEMc[0]=0; MEMs[0]=0;

  MEMw[0]=&ROM_dummy[0x4000*0];

  pagination_128=0;
  pagination_plus2a=128;
  contended_mask=1;

  return 1;
}


int
initialize_plus2 (void)
{
  initialize_128();

  model=ZX_PLUS2;

  memcpy(&ROM_pages[0x4000*0],&spectrum_rom_plus2[0x4000*0],0x4000);
  memcpy(&ROM_pages[0x4000*1],&spectrum_rom_plus2[0x4000*1],0x4000);

  return 1;
}

int
initialize_plus2a (void)
{
  initialize_128();

  model=ZX_PLUS2A;

  memcpy(&ROM_pages[0x4000*0],&spectrum_rom_plus3[0x4000*0],0x4000*4);

  pagination_plus2a=0;
  contended_mask=4;

  return 1;
}

int
initialize_plus3 (void)
{
  initialize_plus2a();

  model=ZX_PLUS3;

  return 1;
}


// control of port 0x7ffd
void
port_0x7ffd (byte value)
{
  if (pagination_128 & 32) return; //if previously locked (bit 5), return

  // check bit 2-0: RAM0/7 -> 0xc000-0xffff
  MEMw[3]=MEMr[3]=&RAM_pages[0x4000*(value&7)-3*0x4000];

  //16/48/128/+2: pages 1,3,5,7 are contended (1), 0,2,4,6 not contended (0) -> so mask is 0001 (1)
  //+2A/+3:       pages 4,5,6,7 are contended (1), 0,1,2,3 not contended (0) -> so mask is 0100 (4)
  MEMc[3]=value & contended_mask;
  MEMs[3]=2*(value==7)+(value==5);

  //locked regions
  //
  //MEM[2]=RAM_page[0x4000*2] locked
  //MEM[1]=RAM_page[0x4000*5] locked

  //128:    check bit 4 : rom selection (ROM0/1 -> 0x0000-0x3FFF)
  //+2A/+3: check high bit of rom selection too (same as offset ROM selection to 0x8000) 1x0 101
  MEMr[0]=&ROM_pages[(value & 16 ? 0x4000 : 0 ) | ((pagination_plus2a & 5) == 4 ? 0x8000 : 0)];
//MEMw[0]=MEMw[0];      

      
  pagewritetime[pagewrites]=spectrumZ80->ICount;
  pagewritevalue[pagewrites++]=value&8;

  pagination_128=value;
}


// control of port 0x1ffd (+2a/+3 -> mem/fdc/ptr)
void
port_0x1ffd (byte value)
{
  if (pagination_plus2a & 128) return; //if not in +2a/+3 mode, return

  pagination_plus2a = value & 0x1f;    //save bits 0-4

  // check bit 1: special RAM banking or not

  if(value & 1)
  {
    switch(value & 7)
    {
     case 1 : //001=ram:0123
              MEMw[3]=MEMr[3]=&RAM_pages[0x4000*3-3*0x4000]; MEMc[3]=0; MEMs[3]=0;
              MEMw[2]=MEMr[2]=&RAM_pages[0x4000*2-2*0x4000]; MEMc[2]=0; MEMs[2]=0;
              MEMw[1]=MEMr[1]=&RAM_pages[0x4000*1-1*0x4000]; MEMc[1]=0; MEMs[1]=0;
              MEMw[0]=MEMr[0]=&RAM_pages[0x4000*0]; MEMc[0]=0; MEMs[0]=0;
              break;

     case 3 : //011=ram:4567
              MEMw[3]=MEMr[3]=&RAM_pages[0x4000*7-3*0x4000]; MEMc[3]=1; MEMs[3]=2;
              MEMw[2]=MEMr[2]=&RAM_pages[0x4000*6-2*0x4000]; MEMc[2]=1; MEMs[2]=0;
              MEMw[1]=MEMr[1]=&RAM_pages[0x4000*5-1*0x4000]; MEMc[1]=1; MEMs[1]=1;
              MEMw[0]=MEMr[0]=&RAM_pages[0x4000*4]; MEMc[0]=1; MEMs[0]=0;
              break;

     case 5 : //101=ram:4563
              MEMw[3]=MEMr[3]=&RAM_pages[0x4000*3-3*0x4000]; MEMc[3]=0; MEMs[3]=0;
              MEMw[2]=MEMr[2]=&RAM_pages[0x4000*6-2*0x4000]; MEMc[2]=1; MEMs[2]=0;
              MEMw[1]=MEMr[1]=&RAM_pages[0x4000*5-1*0x4000]; MEMc[1]=1; MEMs[1]=1;
              MEMw[0]=MEMr[0]=&RAM_pages[0x4000*4]; MEMc[0]=1; MEMs[0]=0;
              break;

     case 7 : //111=ram:4763
              MEMw[3]=MEMr[3]=&RAM_pages[0x4000*3-3*0x4000]; MEMc[3]=0; MEMs[3]=0;
              MEMw[2]=MEMr[2]=&RAM_pages[0x4000*6-2*0x4000]; MEMc[2]=1; MEMs[2]=0;
              MEMw[1]=MEMr[1]=&RAM_pages[0x4000*7-1*0x4000]; MEMc[1]=1; MEMs[1]=2;
              MEMw[0]=MEMr[0]=&RAM_pages[0x4000*4]; MEMc[0]=1; MEMs[0]=0;
              break;
    }
  }
  else
  {
   port_0x7ffd(pagination_128);

   //restart locked_in_128_model values if touched by +2A or +3 emulation previously (see value & 1)
   MEMw[2]=MEMr[2]=&RAM_pages[0x4000*2-2*0x4000]; MEMc[2]=0; MEMs[2]=0;
   MEMw[1]=MEMr[1]=&RAM_pages[0x4000*5-1*0x4000]; MEMc[1]=1; MEMs[1]=1;
   MEMw[0]        =&ROM_dummy[0x4000*0]; MEMc[0]=0; MEMs[0]=0;
  }

  //bit 3: motor on/off
  fdc_motor(value & 8);

  //bit 4: printer strobe
}

// control of port 0x2ffd (fdc in) 
byte port_0x2ffd_in (void)
{
return (model==ZX_PLUS3 ? fdc_read_status() : 0xFF);
}

// control of port 0x3ffd (fdc out & in)
void port_0x3ffd (byte value)
{
if(model==ZX_PLUS3) fdc_write_data(value);
}

byte port_0x3ffd_in (void)
{
return (model==ZX_PLUS3 ? fdc_read_data() : 0xFF);
}



// control of AY-3-8912 ports

unsigned char ay_current_reg;
unsigned char ay_registers[ 16 ]; //local copy, for port_0xfffd_in()

void
port_0xfffd (byte value)
{
ay_current_reg=(value&15);
}

void
port_0xbffd (byte value)
{
ay_registers[ay_current_reg]=value;

sound_ay_write(ay_current_reg,value,spectrumZ80->IPeriod - spectrumZ80->ICount);
}

u8
port_0xfffd_in (void)
{
  //taken from fuse

  unsigned char ay_registers_mask[ 16 ] = {

  0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0x1f, 0xff,
  0x1f, 0x1f, 0x1f, 0xff, 0xff, 0x0f, 0xff, 0xff,

  };

  /* The AY I/O ports return input directly from the port when in
     input mode; but in output mode, they return an AND between the
     register value and the port input. So, allow for this when
     reading R14... */

  if( ay_current_reg == 14 ) 
      return (ay_registers[7] & 0x40 ? 0xbf & ay_registers[14] : 0xbf);
   
  /* R15 is simpler to do, as the 8912 lacks the second I/O port, and
     the input-mode input is always 0xff */

  if( ay_current_reg == 15 && !( ay_registers[7] & 0x80 ) ) return 0xff;

  /* Otherwise return register value, appropriately masked */
  return ay_registers[ay_current_reg] & ay_registers_mask[ay_current_reg];
}


























/*----------------------------------------------------------------*/
extern void CreateVideoTables (void);

void CreateScreenTable (int);

// byte *zx_bordercolour, zx_bordercolours[240]; //one per scanline

//byte *zx_tapfile,*zx_tapfile_,*zx_tapfile_eof;  
//int   zx_pressed_play=0;
int   vram_touched=0;
byte  mic_on,mic;

// original:
int zx_colours[17][3] = { 
  {  0,   0,   0 }, 
  {  0,   0, 192 }, 
  {192,   0,   0 }, 
  {192,   0, 192 }, 
  {  0, 192,   0 }, 
  {  0, 192, 192 }, 
  {192, 192,   0 }, 
  {192, 192, 192 }, 
  {  0,   0,   0 }, 
  {  0,   0, 255 }, 
  {255,   0,   0 }, 
  {255,   0, 255 }, 
  {  0, 255,   0 }, 
  {  0, 255, 255 }, 
  {255, 255,   0 }, 
  {255, 255, 255 }, 
  {255,   0,   0 } 
};


#if 0

// old:
int zx_colours[17][3] = {
  {   0,   0,   0},
  {   0,   0, 205},
  { 205,   0,   0},
  { 205,   0, 205},
  {   0, 205,   0},
  {   0, 205, 205},
  { 205, 205,   0},
  { 212, 212, 212},
  {   0,   0,   0},
  {   0,   0, 255},
  { 255,   0,   0},
  { 255,   0, 255},
  {   0, 255,   0},
  {   0, 255, 255},
  { 255, 255,   0},
  { 255, 255, 255},
  { 255,   0,   0}
};


#endif




  int fila[5][5];

  const unsigned char teclas_fila[NUM_KEYB_KEYS][3] = {
    {1, 2, 0xFE}, /* 0 */ {1, 1, 0xFE}, /* 1 */ {1, 1, 0xFD},	/* 2 */
    {1, 1, 0xFB}, /* 3 */ {1, 1, 0xF7}, /* 4 */ {1, 1, 0xEF},	/* 5 */
    {1, 2, 0xEF}, /* 6 */ {1, 2, 0xF7}, /* 7 */ {1, 2, 0xFB},	/* 8 */
    {1, 2, 0xFD},		/* 9 */
    {3, 1, 0xFE}, /* a */ {4, 2, 0xEF}, /* b */ {4, 1, 0xF7},	/* c */
    {3, 1, 0xFB}, /* d */ {2, 1, 0xFB}, /* e */ {3, 1, 0xF7},	/* f */
    {3, 1, 0xEF}, /* g */ {3, 2, 0xEF}, /* h */ {2, 2, 0xFB},	/* i */
    {3, 2, 0xF7}, /* j */ {3, 2, 0xFB}, /* k */ {3, 2, 0xFD},	/* l */
    {4, 2, 0xFB}, /* m */ {4, 2, 0xF7}, /* n */ {2, 2, 0xFD},	/* o */
    {2, 2, 0xFE}, /* p */ {2, 1, 0xFE}, /* q */ {2, 1, 0xF7},	/* r */
    {3, 1, 0xFD}, /* s */ {2, 1, 0xEF}, /* t */ {2, 2, 0xF7},	/* u */
    {4, 1, 0xEF}, /* v */ {2, 1, 0xFD}, /* w */ {4, 1, 0xFB},	/* x */
    {2, 2, 0xEF}, /* y */ {4, 1, 0xFD},	/* z */
    {4, 2, 0xFE}, /*SPACE*/
    {3, 2, 0xFE}, /*ENTER*/
    {4, 1, 0xFE}, /*RSHIFT*/ {4, 2, 0xFD}, /*ALT*/ {1, 2, 0xEF}, /*CTRL*/
  };




int mouse_x,mouse_y,mouse_b;







byte Z80InPort (register word port)
{
  int code=255;
  byte valor;
  int x, y;
  extern struct tipo_emuopt emuopt;
  extern tipo_hwopt hwopt;
  extern int v_border;
  
  //Para que funcione bien tiene que procesar todos los OUT
  loader_hook (spectrumZ80);  
 
  
  /* kempston joystick */
  
  if(!(port & (0xFF^0xDF))) //bit 5 low = reading kempston (as told pera putnik)
  {
   return kempston;
  }
  
  //if(contended_mask & 4) //if +2a or +3 then apply (fixes fairlight games)
  {
   if (!(port & (0xFFFF^0x2FFD))) return port_0x2ffd_in();
   if (!(port & (0xFFFF^0x3FFD))) return port_0x3ffd_in(); 
  }

  if (!(port & (0xFFFF^0xFFFD))) return port_0xfffd_in();

  //falta fffd_in para fuller
 
  // fuller joystick 
  if (!(port & (0xFF^0x7F)))
  {
   return fuller;
  }
  
  /* keyboard */
  if (!(port & (0xFF^0xFE)))  
    {
      code = 0xFF;

      if (!(port & 0x0100))
	code &= fila[4][1];
      if (!(port & 0x0200))
	code &= fila[3][1];
      if (!(port & 0x0400))
	code &= fila[2][1];
      if (!(port & 0x0800))
	code &= fila[1][1];
      if (!(port & 0x1000))
	code &= fila[1][2];
      if (!(port & 0x2000))
	code &= fila[2][2];
      if (!(port & 0x4000))
	code &= fila[3][2];
      if (!(port & 0x8000))
	code &= fila[4][2];

      /* la gunstick */
      #if 0
      if (!(port & 0x1000) && (emuopt.gunstick & GS_GUNSTICK) &&
	  (emuopt.gunstick & GS_HAYMOUSE))
	{
	  /* disparo de la gunstick con el raton. */
	  if (mouse_b & 1)
	    code &= (0xFE);
	  /* Miramos a ver si los atributos son blanco  */
	  if ((mouse_x > 31) && (mouse_x < 287) &&
	      (mouse_y > v_border) && (mouse_y < (192 + v_border)))
	    {
	      x = (mouse_x - 32) / 8;
	      y = (mouse_y - 1 - v_border) / 8;
	      valor = Z80ReadMem_notiming(0x5800 + 32 * y + x);
	      if (((valor & 0x07) == 0x07) || ((valor & 0x38) == 0x38))
		code &= 0xFB;
	    }			// mouse x>....
	}			// port & 0x1000
      #endif

      /*
         issue 2 emulation, thx to Raul Gomez!!!!!
         I should implement this also:
         if( !ear_on && mic_on )
         code &= 0xbf;
         where earon = bit 4 of the last OUT to the 0xFE port
         and   micon = bit 3 of the last OUT to the 0xFE port
       */

      code = code & 0xbf;
      
      code = code | (tape_microphone ? 0x40 : 0); 
           
      /*
      if(mic_on)
      {
       //byte x = LoadMIC(spectrumZ80->IPeriod-spectrumZ80->ICount);
        x=mic;
        code = code | x;
        sound_beeper_1(x, spectrumZ80->IPeriod - spectrumZ80->ICount);
      }
      */

      return code;
    }



  //unattached port, read from floating bus

  if ((port & 0xFF) == 0xFF)

    {
       return (floating_bus[spectrumZ80->ICount] ?
               Z80ReadMem_notiming(floating_bus[spectrumZ80->ICount]) : 0xFF );
    }

  /*
  switch(port&0xff)
  {
   case 0x1f: //jack2 11f, kempston
   case 0xff:
   case 0xfe:
   case 0x7f: break;

   default: debugmsg("port",port);
  }
  */

  return (code);
}

void Z80OutPort (register word port, register byte value)
{
/*
  if (!(port & (0xFFFF^0xFFFD)))
    {
    if(contended_mask & 4) //if +2a or +3 then apply (fixes fairlight games)
      {
      if (!(port & (0xFFFF^0x1FFD)))   
        { port_0x1ffd(value); return; }
      if (!(port & (0xFFFF^0x3FFD)))   
        { port_0x3ffd(value); return; }
      }
    if (!(port & (0xFFFF^0x7FFD)))   //ordenar de mayor a menor los puertos
      { port_0x7ffd(value); return; }
    if (!(port & (0xFFFF^0xBFFD)))
      { port_0xbffd(value); return; }
//  if (!(port & (0xFFFF^0xFFFD))) {
      port_0xfffd(value); return; 
//  }
    }
*/

  if(contended_mask & 4) //if +2a or +3 then apply (fixes fairlight games)
  {
  if (!(port & (0xFFFF^0x1FFD)))   
   { port_0x1ffd(value); return; }

  if (!(port & (0xFFFF^0x3FFD)))   
   { port_0x3ffd(value); return; }
  }

  if (!(port & (0xFFFF^0x7FFD)))   //ordenar de mayor a menor los puertos
   { port_0x7ffd(value); return; }

  if (!(port & (0xFFFF^0xBFFD)))
   { port_0xbffd(value); return; }

  if (!(port & (0xFFFF^0xFFFD)))
   { port_0xfffd(value); return; }


  //fuller audio box emulation
  if (!(port & (0xFF^0x3F)))
   { port_0xfffd(value); return; }

  if (!(port & (0xFF^0x5F)))
   { port_0xbffd(value); return; }


  /* change border colour */
  if (!(port & (0xFF^0xFE)))
    {
      BorderColor = (value & 0x07);
      
      outwritetime[outwrites]=spectrumZ80->ICount;
      outwritevalue[outwrites++]=value & 0x07;

      #ifdef SOUND_X128
      //logSound (value & 0x10);
      //logSound2 (0, value & 0x10);
      #endif

      #ifdef SOUND_BULBA
      //0x10 : speaker
      //0x08 : tape when saving
      //0x18 : both
      //mic  : tape when loading

      //asi no suenan primeras notas de manic miner
      //sound_beeper(0, (value & 0x18), spectrumZ80->IPeriod - spectrumZ80->ICount);

        sound_beeper_0((value & 0x10), spectrumZ80->IPeriod - spectrumZ80->ICount);
//      sound_beeper(1, (value & 0x08), spectrumZ80->IPeriod - spectrumZ80->ICount);

      #endif

      return;
    }

                                    

/*
  switch(port&0xff)
  {
   case 0x3f:
   case 0x5f:
   case 0xfe:
              break;

   default: debugmsg("port",port);
  }
*/
}

//#undef PC
void
Z80Patch (register Z80Regs * regs)
{
  // address contributed by Ignacio Burgueño :)
#undef POP
#define POP(rreg)\
  regs->rreg.B.l = Z80ReadMem(regs->SP.W); regs->SP.W++;\
  regs->rreg.B.h = Z80ReadMem(regs->SP.W); regs->SP.W++
#if 0

  /* OLD

  if (spectrumZ80->PC.W >= 0x0556 && spectrumZ80->PC.W <= 0x056c)
  {
     //if (zx_pressed_play)
     {
       LoadTAP (regs, zx_tapfile);
       POP (PC);
     }
  }
  */

   if (spectrumZ80->PC.W == 0x56c)
  {
     if(trap_rom_loading)
     //only in 48 basic, or 48 basic under 128k
     //if(model>=ZX_128) if(!(pagination_128&16)) return;
      Load_Tape (regs, zx_tapfile);
  }
#endif


   // fuse: if( PC==0x056c || PC == 0x0112 )
   // x128: 1386 ||  1387
   // mia : 56c
   if(/*trap_rom_loading*/ tape_is_tape() && mconfig.flash_loading)
   {          	
      // if (spectrumZ80->PC.W != 0x05f1)              return; //wrong trap addr
      // if(!(pagination_128 & 16)) if(model>=ZX_128)  return; //wrong rom bank       

      //if (spectrumZ80->PC.W == 0x056c || spectrumZ80->PC.W == 0x0112)
      // LoadTZX(regs);
      {                               
          if(Tape_load(regs));
		     POP(PC);//??
		  // byte Z80InPort (register word port)
      }

      /*
         if (spectrumZ80->PC.W == 0x056c)
         Load_Tape(regs);
      */
   }
   /* 
   else//TAPE PLAY! 
   {
       //CUIDADO: 
       tape_do_play(1);
   }
   */
   
}

void inline
Z80WriteMem (register word where, register byte A)
{
volatile word whereA;
whereA=where>>14;
if(MEMc[whereA])
	{
	spectrumZ80->ICount-=(cycles_delay[spectrumZ80->ICount]);
	if(MEMs[whereA] && (Significance[where] < spectrumZ80->ICount))
		{
		memwriteaddr[memwrites]=(where%16384)+6912*(MEMs[whereA]-1);
		memwritetime[memwrites]=spectrumZ80->ICount;
		memwritevalue[memwrites++]=A;
		}
	}
*((byte *)(MEMw[whereA]+(where)))=A;
spectrumZ80->ICount-=3;
}

void inline
Z80WriteMem_notiming (register word where, register byte A)
{
volatile word whereA;
whereA=where>>14;
*((byte *)(MEMw[whereA]+(where)))=A;
}

void POKE(unsigned dir,unsigned char dat)
{
Z80WriteMem_notiming(dir,dat);
}

byte inline
Z80ReadMem(register word where)
{
volatile word whereA;
whereA=where>>14;
if(MEMc[whereA]) spectrumZ80->ICount-=(cycles_delay[spectrumZ80->ICount]);
spectrumZ80->ICount-=3;
return *((byte *)(MEMr[whereA]+(where)));
}

byte inline
Z80ReadMem_notiming(register word where)
{
volatile word whereA;
whereA=where>>14;
return *((byte *)(MEMr[whereA]+(where)));
}

void inline contend_read(word where)
{
volatile word whereA;
whereA=where>>14;
if(MEMc[whereA]) spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);
spectrumZ80->ICount--;
}

void inline contend_read_jr(void)
{
volatile word whereA;
whereA=spectrumZ80->PC.W>>14;
if(MEMc[whereA]) spectrumZ80->ICount-=(cycles_delay[spectrumZ80->ICount]);
spectrumZ80->PC.W++;
spectrumZ80->ICount-=3;
}

void inline contend_read_byte(void)
{
volatile byte whereA;
whereA=spectrumZ80->I>>6;
if(MEMc[whereA]) spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);
spectrumZ80->ICount--;
}

void inline contend_read_byte_x2(void)
{
volatile byte whereA;
whereA=spectrumZ80->I>>6;
if(MEMc[whereA])
	{
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	}
else	{
	spectrumZ80->ICount-=2;
	}
}

void inline contend_read_byte_x7(void)
{
volatile byte whereA;
whereA=spectrumZ80->I>>6;
if(MEMc[whereA])
	{
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	}
else	{
	spectrumZ80->ICount-=7;
	}
}

void inline contend_read_x5(word where)
{
volatile word whereA;
whereA=where>>14;
if(MEMc[whereA])
	{
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	}
else	{
	spectrumZ80->ICount-=5;
	}
}

void inline contend_read_x4(word where)
{
volatile word whereA;
whereA=where>>14;
if(MEMc[whereA])
	{
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	}
else	{
	spectrumZ80->ICount-=4;
	}
}

void inline contend_read_x2(word where)
{
volatile word whereA;
whereA=where>>14;
if(MEMc[whereA])
	{
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);spectrumZ80->ICount--;
	}
else	{
	spectrumZ80->ICount-=2;
	}
}

void inline ula_contend_port_early(word port)
{
if((port & 0xc000)==0x4000)
  	spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]);
spectrumZ80->ICount-=1;
}

void inline ula_contend_port_late(word port)
{
if((contended_mask!=4)&&((port & 0x0001) == 0))
	{
    	spectrumZ80->ICount-=(cycles_delay[spectrumZ80->ICount]); spectrumZ80->ICount-=3;
  	}
else 	{
    	if( ( port & 0xc000 ) == 0x4000 ) 
    		{
      		spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]); spectrumZ80->ICount-=1;
      		spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]); spectrumZ80->ICount-=1;
      		spectrumZ80->ICount-=(cycles_delay2[spectrumZ80->ICount]); spectrumZ80->ICount-=1;
    		}
	else 	{
		spectrumZ80->ICount-=3;
    		}
  	}
}

/*----------------------------------------------------------------
 Main function. It inits all the emulator stuff and executes it.
----------------------------------------------------------------*/

void ZX_Init(void)
{
 CreateVideoTables();

 model=ZX_128;
}


void ZX_SetModel(void)
{

 memset(RAM_dummy,0xFF,16384*1); 
 memset(ROM_dummy,0x00,16384*1);

 memset(RAM_pages,0x00,16384*16);
 memset(ROM_pages,0x00,16384*4);

 switch(model)
 {
  case ZX_16:       initialize_16();
                    break;

  case ZX_48:       initialize_48();
                    break;

  case ZX_128_USR0: initialize_128();
                    model=ZX_128_USR0;
                    break;

  case ZX_128:      initialize_128();
                    break;

  case ZX_PLUS2:    initialize_plus2();
                    break;

  case ZX_PLUS2A:   initialize_plus2a();
                    break;

  case ZX_PLUS3:    initialize_plus3();
                    break;

  default:          initialize_128();
                    break;
 }
}




void ZX_Reset(int preffered_model)
{
 int i,j,k,k2,totalcycles,irqtime;

 if(preffered_model!=-1) model=preffered_model;

 ZX_SetModel();

 totalcycles=(model<ZX_128?69888:70908);
 switch(model)
 {
  case ZX_128_USR0: 
  case ZX_128:      
  case ZX_PLUS2:    irqtime=36;
                    break;
  default:          irqtime=32;
                    break;
 }
 
  for(i=0;i<76000;i++) cycles_delay[i]=cycles_delay2[i]=floating_bus[i]=0;

 switch(model)
 {
 case ZX_16:
 case ZX_48:
 case ZX_128:
 case ZX_128_USR0:
 case ZX_PLUS2:

  for(k=(model<ZX_128?TIMING_48:TIMING_128),j=0;j<192;j++,k+=(model<ZX_128?96:100))
  {
   extern int scan_convert[192];
          int paper=0x4000+scan_convert[j], attr=0x5800+32*(j/8);

   for(i=0;i<16;i++) //16*8=128 T states per line (main screen)
   {
    //k2 = 10 & 13 get FLOATSPY.TAP to work
    // int k2=model<ZX_128? 0 : 10;
    int k2=-4;


/*

#define TIMING_48   14335       , entre ellos dos 6
#define TIMING_128  14361

*/

    floating_bus[totalcycles-k+k2]=paper;  cycles_delay2[totalcycles-k]=cycles_delay[totalcycles-k]=6; k++; paper++;
    floating_bus[totalcycles-k+k2]=attr;   cycles_delay2[totalcycles-k]=cycles_delay[totalcycles-k]=5; k++; attr++;
    floating_bus[totalcycles-k+k2]=paper;  cycles_delay2[totalcycles-k]=cycles_delay[totalcycles-k]=4; k++; paper++;
    floating_bus[totalcycles-k+k2]=attr;   cycles_delay2[totalcycles-k]=cycles_delay[totalcycles-k]=3; k++; attr++;
    floating_bus[totalcycles-k+k2]=0x0000; cycles_delay2[totalcycles-k]=cycles_delay[totalcycles-k]=2; k++;
    floating_bus[totalcycles-k+k2]=0x0000; cycles_delay2[totalcycles-k]=cycles_delay[totalcycles-k]=1; k++;
    floating_bus[totalcycles-k+k2]=0x0000; cycles_delay2[totalcycles-k]=cycles_delay[totalcycles-k]=0; k++;
    floating_bus[totalcycles-k+k2]=0x0000; cycles_delay2[totalcycles-k]=cycles_delay[totalcycles-k]=0; k++; 

   }
  }
 break;

 case ZX_PLUS2A:
 case ZX_PLUS3:
              //no floating bus emulation

              for(k=14365,j=0;j<192;j++,k+=100) //100 T=skip border (24+ 24+52)
               for(i=0;i<16;i++) //16*8=128 T states per line (main screen)
                  {
                   cycles_delay[totalcycles-k]=1; k++;
                   cycles_delay[totalcycles-k]=0; k++;
                   cycles_delay[totalcycles-k]=7; k++;
                   cycles_delay[totalcycles-k]=6; k++;
                   cycles_delay[totalcycles-k]=5; k++;
                   cycles_delay[totalcycles-k]=4; k++;
                   cycles_delay[totalcycles-k]=3; k++;
                   cycles_delay[totalcycles-k]=2; k++;
                  }

              break;
 }

 CreateScreenTable(model);
 Z80Reset (spectrumZ80, totalcycles, irqtime ); //69888 for 48k, 70908 for 128k
 Z80FlagTables ();

// memset(zx_bordercolours,0,240);
// zx_bordercolour=(byte *)&zx_bordercolours[0];

 fila[1][1] = fila[1][2] = fila[2][2] = fila[3][2] = fila[4][2] =
    fila[4][1] = fila[3][1] = fila[2][1] = 0xFF;

// zx_pressed_play=0;

 kempston=0;
 fuller=0xff;

 vram_touched=1;

 memset(ay_registers,0,16); 
 ay_current_reg=0;

 //mic_on=0;
 //mic=0;

 sound_init(0, model<ZX_128?69888:70908);
 sound_ay_reset();
 

 fdc_init(0,0);

 if(mconfig.flash_loading)
    ZX_Patch_ROM();
    
 Tape_rewind();
}

void ZX_Patch_ROM(){
// patch rom

{
	int i;
	for(i=0;i<4;i++)
	{
	/*if(ROM_pages[0x4000*i+0x05f1]==219 && ROM_pages[0x4000*i+0x05f2]==254)
		{
	    ROM_pages[0x4000*i+0x05f1]=0xed;
	    ROM_pages[0x4000*i+0x05f2]=63;
		}
		*/
	if(ROM_pages[0x4000*i+0x562]==0xdb && ROM_pages[0x4000*i+0x563]==0xfe)
		{
	    ROM_pages[0x4000*i+0x562]=0xed;
	    ROM_pages[0x4000*i+0x563]=0x3f;
		}
	}
}

/*i=1;
ROM_pages[0x4000*i+1378]=0xed;
ROM_pages[0x4000*i+1379]=63;
*/
}

void ZX_Unpatch_ROM(){
	
  int i;
  for(i=0;i<4;i++)//unpatch rom so protecction loader schemes work
  {
	 if(ROM_pages[0x4000*i+0x562]==0xed && ROM_pages[0x4000*i+0x563]==0x3f)
	 {
		 ROM_pages[0x4000*i+0x562]=0xdb;
		 ROM_pages[0x4000*i+0x563]=0xfe;
     }
  }	
}

void ZX_Frame(int do_skip)
{
 static int f_flash2 = 0;

 f_flash2++;
 f_flash2 %= 32;

 if (f_flash2 < 16)
   SpectrumFlashFlag = 1;
 else
   SpectrumFlashFlag = 0;


 JustRun(spectrumZ80, do_skip);

 // zx_bordercolour=(byte *)&zx_bordercolours[0];

 fila[1][1] = fila[1][2] = fila[2][2] = fila[3][2] = fila[4][2] =
 fila[4][1] = fila[3][1] = fila[2][1] = 0xFF;

 kempston=0;
 fuller=0xff;
}


void ZX_LoadGame(int preferred_model, unsigned long crc, int quick)
{
 if(preferred_model!=-1)
   ZX_Reset(preferred_model); 
 Tape_close();

 tape_format=0;

 if(GAME[0]=='Z'&&GAME[1]=='X'&&GAME[2]=='A'&&GAME[3]=='Y'&&
    GAME[4]=='E'&&GAME[5]=='M'&&GAME[6]=='U'&&GAME[7]=='L')
 {
  extern void play_track(Z80Regs *regs, int track);
  extern int read_ay_file(Z80Regs *regs, void *fp, int ay_size);

  if(read_ay_file(spectrumZ80,GAME,GAME_size))
   play_track(spectrumZ80, 0);
  else msg("ay read error!");

  //trap_rom_loading=0;
 }
 else
 if(GAME[0]=='Z'&&GAME[1]=='X'&&GAME[2]=='T'&&GAME[3]=='a'&&GAME[4]=='p'&&
 GAME[5]=='e'&&GAME[6]=='!'&&GAME[7]==26)
 {
    Tape_init(GAME,GAME_size);
    tape_format=1; 
 }
 else
 if(GAME[0]==0x13&&GAME[1]==0&&GAME[2]==0)
  {
    Tape_init(GAME,GAME_size);
    tape_format=2;
  }
 else if(GAME_size==131103||GAME_size==147487)
  {
  LoadSNA(spectrumZ80,GAME,ZX_128);
  }
 else if(GAME_size==49179)
  {
  LoadSNA(spectrumZ80,GAME,ZX_48);
  }
 else if(GAME_size==49190)
  {
  LoadSP(spectrumZ80,GAME);
  }
 else if(GAME_size==16384)
 {
  LoadCart(spectrumZ80,GAME);
  //trap_rom_loading=0;
 }
 else if(GAME_size==6912)
 {
  LoadSCR(spectrumZ80,GAME);
 }
 else if(GAME[0]=='M'&&GAME[1]=='V'&&GAME[2]==' '&&GAME[3]=='-'&&GAME[4]==' '&&
 GAME[5]=='C'&&GAME[6]=='P'&&GAME[7]=='C')
 {
  if(model!=ZX_PLUS3)
  	ZX_Reset(ZX_PLUS3);
  //LoadZ80(spectrumZ80, load_plus3_disk, load_128_usr0); 
  memcpy(DSK,GAME,GAME_size);
  dsk_load((void *)DSK);
 }
 else if(GAME[0]=='E'&&GAME[1]=='X'&&GAME[2]=='T'&&GAME[3]=='E'&&GAME[4]=='N'&&
 GAME[5]=='D'&&GAME[6]=='E'&&GAME[7]=='D')
 {
  if(model!=ZX_PLUS3)
  	ZX_Reset(ZX_PLUS3);
   //LoadZ80(spectrumZ80, load_plus3_disk, load_128_usr0); 
   memcpy(DSK,GAME,GAME_size);
   dsk_load((void *) DSK);
 }
 else
 {
   LoadZ80(spectrumZ80,GAME,&GAME[GAME_size]); 
 }

 //exceptions

 switch(crc)
 {
  case 0xED86CEBE: //Shadow Of The Unicorn - Side A.tzx
  case 0xD26CEC63: //Shadow Of The Unicorn - Side B.tzx
  case 0x0990611C: //Shadow Of The Unicorn.z80
                   memcpy(&ROM_pages[0],shadow_of_the_unicorn_rom,16384);
                   //trap_rom_loading=0;
                   break;
 }

 if(tape_format) //if any tape inserted...
 {
    //ZX_Reset(model);  
 }
 else //putting a dummy tape fixes my donkey kong snap
 {
    tape_format=2;
    Tape_init(GAME,GAME_size); 
 }
 
}




#define EXPORT_VER 0
#define EXPORT_MARK (('f'<<24)|('Z'<<16)|('X'<<8)|('!'<<0))
    int EXPORT, EXPORT_SIGN;
#define TRANSIT(val, size)  {int i;unsigned char *p=(unsigned char *)val; count+=size; if(!EXPORT) for(i=0;i<size;i++) *p++=*mem++; else for(i=0;i<size;i++) *mem++=*p++;  }


#define BLOCK(mode)   EXPORT=mode;                         \
                                                           \
  if(EXPORT) EXPORT_SIGN=EXPORT_MARK;                      \
                                                           \
  TRANSIT(&EXPORT_SIGN, 4);                                \
                                                           \
  if(EXPORT==0) if(EXPORT_MARK!=EXPORT_SIGN) return 0;     \
                                                           \
  TRANSIT(&version, 1);                                    \
                                                           \
  if(EXPORT==0) if(version!=EXPORT_VER) return 0;          \
                                                           \
  TRANSIT(&model, 1);                                      \
                                                           \
  if(EXPORT==0) ZX_Reset(model);                           \
                                                           \
  TRANSIT(spectrumZ80, sizeof(Z80Regs));                  \
                                                           \
  TRANSIT(&nRAM_pages, 1);                                 \
                                                           \
  switch(nRAM_pages)                                       \
  {                                                        \
   case 1: TRANSIT(&RAM_pages[0x4000*5], 0x4000);          \
           break;                                          \
                                                           \
   case 3: TRANSIT(&RAM_pages[0x4000*5], 0x4000);          \
           TRANSIT(&RAM_pages[0x4000*6], 0x4000);          \
           TRANSIT(&RAM_pages[0x4000*7], 0x4000);          \
           break;                                          \
                                                           \
   case 8: TRANSIT(&RAM_pages[0x4000*0], 0x4000);          \
           TRANSIT(&RAM_pages[0x4000*1], 0x4000);          \
           TRANSIT(&RAM_pages[0x4000*2], 0x4000);          \
           TRANSIT(&RAM_pages[0x4000*3], 0x4000);          \
           TRANSIT(&RAM_pages[0x4000*4], 0x4000);          \
           TRANSIT(&RAM_pages[0x4000*5], 0x4000);          \
           TRANSIT(&RAM_pages[0x4000*6], 0x4000);          \
           TRANSIT(&RAM_pages[0x4000*7], 0x4000);          \
           break;                                          \
  }                                                        \
                                                           \
  { unsigned char nROM_pages=0; TRANSIT(&nROM_pages, 1); } \
                                                           \
  TRANSIT(&ay_registers[0], 16);                           \
                                                           \
  TRANSIT(&ay_current_reg,1);                              \
                                                           \
  TRANSIT(&pagination_plus2a,1);                           \
                                                           \
  TRANSIT(&pagination_128,1);                              \
                                                           \
  TRANSIT(&BorderColor,1);                                 



int ZX_SaveState(void *memx)
{
  int count=0;
  unsigned char version=EXPORT_VER, nRAM_pages;
  unsigned char *mem=(unsigned char *)memx;

  switch(model)
  {
   case ZX_16       : nRAM_pages=1; break;
   case ZX_48       : nRAM_pages=3; break;
   case ZX_128      : 
   case ZX_128_USR0 :
   case ZX_PLUS2    :
   case ZX_PLUS2A   :
   case ZX_PLUS3    : nRAM_pages=8; break;
  }                       

  BLOCK(1); //saving

  return count;
}

int ZX_LoadState(void *memx)
{
  int count=0;
  unsigned char version, nRAM_pages, n;
  unsigned char *mem=(unsigned char *)memx;

  BLOCK(0); //loading

  for(n=0;n<16;n++) { port_0xfffd (n); port_0xbffd(ay_registers[n]); }

  port_0xfffd(ay_current_reg);

  if (model == ZX_PLUS2A || model == ZX_PLUS3) port_0x1ffd(pagination_plus2a);

  port_0x7ffd(pagination_128);

  return count;
}


void CreateScreenTable(int model)
{
int x,y,start,pause,memoryadd,timing,repeat;
memoryadd=0;
for( y=0; y<65536; y++)
	{
    	Significance[y] = 72000;
   	}
switch(model)
 	{
 	case ZX_16:
 	case ZX_48:
 		start=69888-TIMING_48;
		pause=96;
		repeat=0;
		break;
 	case ZX_128:
	case ZX_128_USR0:
 	case ZX_PLUS2:
	case ZX_PLUS2A:
	case ZX_PLUS3:
 		start=70908-TIMING_128;
		pause=100;
		repeat=32768;
		break;
	}
timing=start;
for(y=0;y<192;y++)
	{
	memoryadd=16384+scan_convert[y];
	for(x=0;x<32;x++)
		{
		Significance[memoryadd+repeat]=timing;
		Significance[memoryadd++]=timing;
		timing-=4;
		}
	timing-=pause;
	}
timing=start-7*(pause+128);
for(y=0;y<24;y++)
	{
	for(x=0;x<32;x++)
		{
		Significance[memoryadd+repeat]=timing;
		Significance[memoryadd++]=timing;
		timing-=4;
		}
	timing-=(pause+128)*7+pause;
	}
}
