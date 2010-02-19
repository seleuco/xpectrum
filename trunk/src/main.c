/*  gp2xpectrum  for GP2X

    Copyright (C) 2006-2008 Metalbrain, Seleuco
    Copyright (C) 2006  Hermes/PS2Reality  * Made in Vallecas - Madrid (Spain)

    Copyright (C) 2004  rlyeh (fZX32 for GP32)

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
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

#define DEBUG_MSG 
#define USE_ZIP
//#define CAPTURE

#ifdef USE_ZIP

#include "zip.h"


#endif

#include "bzip/bzlib.h"
#include "microlib.h"
#include "cpuctrl.h"
#include "flush_uppermem_cache.h"

//#include "zx.h"
//#include "z80.h"
//#include "fdc.h"
#include "empty_dsk.h"
#include "shared.h"
#include "usbkeymap.h"

long cur_frame;

extern Z80Regs * spectrumZ80;
extern void Sound_Loop();
extern int full_screen;

char * get_name(char *name);

extern byte  DSK[1*1024*1024]; 
extern t_drive driveA;
extern t_FDC FDC;
extern t_track *active_track;

extern int keyboardmap[128];

unsigned gp2x_nKeys=0;

void write_keyfile(char *name);
void read_keyfile(char *name);

char * MY_filename="default_name.fk";
char menustring[32];
char globalpath[247]="/mnt/sd/roms/spectrum";
char pathstring[247];
char pathstring2[247];
char pathstring3[247];
int volume=50;
int skip=0;
int delayvalue=0;
int f200=0;

int usbkeyboard_found=0;

int init_speed_loading=0;
MCONFIG mconfig;


//int delay_volume=0;

//int auto_fire=0;
int dsk_flipped=0;
int load_state(int st);
int save_state(int st);
int dsk_save (char *pchFileName/*, t_drive *drive, char chID*/);
int dsk_format ();

#define DEFAULT_SPEED_NOFS 185
#define DEFAULT_SPEED_FS1 165
#define DEFAULT_SPEED_NOFSNC 155
#define DEFAULT_SPEED_FS1NC 140
#define MIN_SPEED 100
#define MAX_SPEED 220

/*********************************************************************/
/* GP2X System                                                       */
/*********************************************************************/

int gp2x_speed_nofs=DEFAULT_SPEED_NOFS;
int gp2x_speed_fs1=DEFAULT_SPEED_FS1;
int gp2x_speed_nofsnc=DEFAULT_SPEED_NOFSNC;
int gp2x_speed_fs1nc=DEFAULT_SPEED_FS1NC;

int gp2x_speed=0;

void set_speed_clock(int speed)
{        
   if(speed==gp2x_speed)
      return;
   
//   if(mconfig.speed_loading && tape_playing)
//   {
//      if(speed!=200)return;
//   }
//   else
//   prev_speed = speed;

   gp2x_speed=speed;
   set_display_clock_div(64 | (16+8*(speed>=220)));
   set_FCLK(speed); // set_DCLK_Div(0);set_920_Div(0);
}

void SetVideoScaling(int pixels,int width,int height)
{
static int pixels2=0,width2=0,height2=0;
if(width2!=width || height2!=height)
	{
    gp2x_set_scaling(width,height);
	width2=width;height2!=height;
	}
if(pixels2!=pixels)
    {
	pixels2=pixels;
	gp2x_width_screen(pixels);
	}

}

void sleep_mode()
{
	int n;
	unsigned a,b;
	a=get_status_UCLK();
	b=get_status_ACLK();

set_display(0);
set_battery_led(0);
n=gp2x_speed;

gp2x_volume(0,0);

set_status_UCLK(0);// apaga los relojes
set_status_ACLK(0);

 // velocidad minima que admite la funcion
set_speed_clock(20);set_DCLK_Div(3);set_920_Div(3); // 5MHZ
while(1)
	{
		if((gp2x_joystick_poll() & (GP2X_SELECT | GP2X_L))==(GP2X_SELECT | GP2X_L)) break;
	}
set_DCLK_Div(0);set_920_Div(0);

set_speed_clock(n);

set_status_UCLK(a); // recupera los relojes
set_status_ACLK(b);

set_display(1);gp2x_volume(volume,volume);

}

int battery_icon=1;
int volt=27; // initialize
static int devbatt=-1;

void battery_status_end()
{
if(devbatt>=0) close (devbatt);devbatt=-1;
}

int battery_status() // Thanks Squidge by the trick ;)
{

static int battval;
int i;
 //HACK return 26;
static int step=0;
static int substep=0;

 unsigned short cbv;
int v;

substep++;
if (substep!=5)		// Don't poll in all calls
	return -1;

substep=0;

if(step==0)
{

if(devbatt<0)  devbatt = open ("/dev/batt", O_RDONLY);
if (devbatt<0) return -1;

battval = 0;  
step++;
} 
if(step<1001)
{
if (read (devbatt, &cbv, 2) == 2)
	{battval += cbv;step++;}

return -1;
}
else
{
 battval /= 1000;

 // do a rough translation
 if (battval > 1016) v = 37;
 else if (battval > 974) v = 33;
 else if (battval > 943) v = 32;
 else if (battval > 915) v = 31;
 else if (battval > 896) v = 30;
 else if (battval > 837) v = 29;
 else if (battval > 815) v = 28;
 else if (battval > 788) v = 27;
 else if (battval > 745) v = 26;
 else if (battval > 708) v = 25;
 else if (battval > 678) v = 24;
 else if (battval > 649) v = 23;
 else if (battval > 605) v = 22;
 else if (battval > 573) v = 21;
 else if (battval > 534) v = 20;
 else if (battval > 496) v = 19;
 else if (battval > 448) v = 18;
 else v = 17;

 
step=0;
}

//v>26 Full
//v>24 Medium
//v<=24 Empty

return v;
} 

void battery_box(int x,int y,unsigned char col,int volt) // display a box of  15x8 with the battery status
{
int n,m;
static int flip=0;

flip++;
if((flip & 2)!=0  && volt<=24) {set_battery_led(0);return;} 
if(volt<=24) set_battery_led(1);
// if(!battery_icon) return;

if(x<0) x=0;
if((x+14)>319) x=320-15;
if(y<0) y=0;
if((y+7)>319) y=240-8;

volt-=22;
if(volt<0) volt=0;
if(volt>4) volt=5;

volt<<=1;

m=y*320;
for(n=0;n<14;n++) {gp2x_screen[x+n+m]=0;gp2x_screen[x+n+m+7*320]=0;} // mask rectangle
for(n=0;n<3;n++) {gp2x_screen[x+m]=0;gp2x_screen[x+m+13]=0;m+=320;}
for(n=0;n<2;n++) {gp2x_screen[x+m-1]=0;gp2x_screen[x+m+13]=0;m+=320;}
for(n=0;n<3;n++) {gp2x_screen[x+m]=0;gp2x_screen[x+m+13]=0;m+=320;}
m=(y+1)*320;
x++;
for(n=0;n<12;n++) {gp2x_screen[x+n+m]=col;gp2x_screen[x+n+m+5*320]=col;} // rectangle

for(n=0;n<2;n++) {gp2x_screen[x+m+11]=col;m+=320;}
for(n=0;n<2;n++) {gp2x_screen[x+m-1]=col;gp2x_screen[x+m+11]=col;m+=320;}
for(n=0;n<2;n++) {gp2x_screen[x+m+11]=col;m+=320;}
m=(y+2)*320;
for(n=1;n<volt;n++) {gp2x_screen[x+10-n+m]=col;
                 gp2x_screen[x+10-n+m+320]=col;
		 gp2x_screen[x+10-n+m+320*2]=col;
		 gp2x_screen[x+10-n+m+320*3]=col;
		 gp2x_screen[x+10-n+m+320*4]=col;
		 }
}

/*********************************************************************/
/* Save/Load configuration of gp2xpectrum                             */
/*********************************************************************/

void load_mconfig()
{FILE *fp;
sprintf(pathstring,"%s/saves/gp2xpectrum.cfg",globalpath);
fp=fopen(pathstring,"rb");
int read=0;
if(fp!=NULL)
	{ 	   
	fread(&mconfig, 1, sizeof(mconfig), fp);
	fclose(fp);
	read=1;
	}
if(mconfig.id!=0xABCD0011 || !read)
	{
	mconfig.id=0xABCD0011;
    	mconfig.zx_screen_mode=0;//lo pongo en no full yo por defecto. OK MetalBrain
    	mconfig.battery_icon=0	;//le quito yo por defecto el icono de la bateria
    	mconfig.frameskip=0;//le pongo por defecto el frameskip a cero
    	mconfig.contention=1;
    	mconfig.sound_volume=50;
    	if(!f200)
    	  mconfig.sound_gain=2;//high
    	else
    	  mconfig.sound_gain=1;//Medium    	
    	mconfig.sound_mode=1;//Mono
    	mconfig.sound_freq=44100;//44100
    	mconfig.speed_mode=100;//100% emulation
    	mconfig.wait_vsync=0;//no vsync
    	mconfig.show_fps=0;
    	mconfig.speed_loading=1;
    	mconfig.flash_loading=1;
    	mconfig.edge_loading=1;
    	mconfig.auto_loading=1;
	}
volume = mconfig.sound_volume;
battery_icon=mconfig.battery_icon;

int factor = (20 * 100 ) / mconfig.speed_mode;
delayvalue= factor +(mconfig.frameskip * factor);

int speed;
if (mconfig.contention==1)
	{
	if (mconfig.frameskip==0)
		{
		speed=gp2x_speed_nofs;
		skip=0;
		}
	else	{
		speed=gp2x_speed_fs1;
		}
	}
else	{
	if (mconfig.frameskip==0)
		{
		speed=gp2x_speed_nofsnc;
		skip=0;
		}
	else	{
		speed=gp2x_speed_fs1nc;
		}
	}
set_speed_clock(speed);
}

void save_mconfig()
{
FILE *fp;

mconfig.battery_icon=battery_icon;
mconfig.sound_volume=volume;
sprintf(pathstring,"%s/saves/gp2xpectrum.cfg",globalpath);
fp=fopen(pathstring,"wb");
if(fp==NULL) return;
fwrite(&mconfig, 1, sizeof(mconfig), fp);
fclose(fp);
sync();
}




/****************************************************************************************************************************************/
// PANTALLA
/****************************************************************************************************************************************/


void speccy_corner()
{
int x1=320-1,y1=240-64,x;
int n,m;
unsigned char *p;
for(n=0;n<32;n++)
	{
	if(y1>=240) break;
	p=&gp2x_screen[x1+y1*320];
	x=x1;
	for (m=0;m<6;m++,x++) {if(x<320) {p[320]=3;*p++=3;}} // rojo
	for (m=0;m<6;m++,x++) {if(x<320) {p[320]=4;*p++=4;}} // amarillo
	for (m=0;m<6;m++,x++) {if(x<320) {p[320]=1;*p++=1;}} // verde
	for (m=0;m<6;m++,x++) {if(x<320) {p[320]=5;*p++=5;}} // cyan
	y1+=2;
	x1--;
	}
}
extern unsigned char msx[];  // define la fuente externa usada para dibujar letras y numeros 

unsigned char COLORFONDO=0; // color de fondo (vaya noticia :P)

unsigned char mypalette_rgb[7][3]={
{0x00,0x40,0x00},
{0x00,0xff,0x00},
{0xff,0xff,0xff},
{0xff,0x00,0x00},
{0xff,0xff,0x00},
{0x00,0xff,0xff},
{0x00,0x00,0x00},
};
int is_my_palette=1;

void set_mypalette()
{
int n;
for(n=0;n<7;n++)
gp2x_set_palette_color(mypalette_rgb[n][0],mypalette_rgb[n][1],mypalette_rgb[n][2],n);

gp2x_set_palette_color(255,255,255,255);
gp2x_set_palette();
is_my_palette=1;

}

void set_emupalette()
{
int i;
unsigned char r,g,b;

//unsigned  ga=(unsigned) (255.0*((float) (1024+170*(mconfig.bright & 3)))/1024.0);



  for(i=0;i<17;i++)
   {
   
    
     r=zx_colours[i][0];
     g=zx_colours[i][1];
     b=zx_colours[i][2];

     
    gp2x_set_palette_color(r,g,b,i);
	gp2x_set_palette_color(r>>1,(g>>2) | 64,b>>1,i+18);
   }
gp2x_set_palette_color(0,255,0,252);
gp2x_set_palette_color(255,0,0,253);
gp2x_set_palette_color(255,255,0,254);
gp2x_set_palette_color(255,255,255,255);
gp2x_set_palette();

is_my_palette=0;

}


void v_putchar( unsigned x, unsigned y, unsigned char color, unsigned char ch) // rutina usada para dibujar caracteres (coordenadas de 8x8)
{
   volatile int 	i,j;
   volatile unsigned char	*font;
  volatile unsigned char col,col2;
  volatile unsigned char *p;
   if(x>=40 || y>=30) return;

  

col=color;
col2=COLORFONDO;

  
   font = &msx[ (int)ch * 8];
   p=&gp2x_screen[(y*320*8)+(x<<3)];
   if(col!=col2)
   {
   for (i=0; i < 8; i++, font++)
		{  
		 for (j=0; j < 8; j++)
          {
          if ((*font & (128 >> j))) p[j]=col;
			  
			else  p[j]=col2;
          }
	  p+=320;
	 }
   }
   else
   {
   for (i=0; i < 8; i++, font++)
		{  
		 for (j=0; j < 8; j++)
          {
          if ((*font & (128 >> j))) p[j]=col;
			  
          }
	  p+=320;
	 }
   }
}

// display array of chars

int v_breakcad=40;
int v_forcebreakcad=0;
int lastx=0;
void v_putcad(int x,int y,unsigned color,char *cad)  // dibuja una cadena de texto
{
int n=0;	
while(cad[0]!=0) {if(n==v_breakcad) break; v_putchar(x,y,color,cad[0]);cad++;x++;n++;}
if(v_forcebreakcad)
	{
	while(1) {if(x>=40 || n==v_breakcad) break; v_putchar(x,y,color,' ');cad++;x++;n++;}
	}
lastx=x;
}


void ClearScreen(unsigned char c) // se usa para 'borrar' la pantalla virtual con un color
{
int n;

for(n=0;n<320*240;n++)
	{
	gp2x_screen[n]=c;
	}	
}

unsigned char mask_keyb[32*8*11*8];

void mask_putchar( unsigned x, unsigned y, unsigned char color, unsigned char ch) // rutina usada para dibujar caracteres (coordenadas de 8x8)
{
  volatile  int 	i,j,v;
   volatile unsigned char	*font;
  unsigned char col,col2;
    volatile unsigned char *p;
   if(x>=36 || y>=20) return;
x-=4;
y-=9;

  

col=color;
col2=COLORFONDO;


   font = &msx[ (int)ch * 8];
   p=&mask_keyb[(y*32*8*8)+(x<<3)];
   if(col!=col2) 
   {
   for (i=0; i < 8; i++, font++)
		{  
		 for (j=0; j < 8; j++)
          {
          if ((*font & (128 >> j)))  p[j]=col;
			else p[j]=col2;
          }
	  p+=32*8;
	 }
   }
   else
   {
   for (i=0; i < 8; i++, font++)
		{  
		 for (j=0; j < 8; j++)
          {
          if ((*font & (128 >> j)))  p[j]=col;

          }
	  p+=32*8;
	 }
   }
 
}


void mask_out(int  x,int y)
{
int   n,m;
volatile unsigned char *p,*p2,v;
for(n=0;n<11*8;n++)
	{
	p=&gp2x_screen[(y+n)*320+x];
	p2=&mask_keyb[n*32*8];
	for(m=0;m<32*8;m++)
		{
		if(*p2==0)
		{
		v=*p;if(v<17) v+=18;*p++=v;p2++;
		}
		else *p++=*p2++;
		
		
		}
	}
}


int mask_chars=0;

void mask_putcad(int x,int y,unsigned color,char *cad)  // dibuja una cadena de texto
{
int n=0;	
while(cad[0]!=0) {if(n==v_breakcad) break; if(mask_chars) mask_putchar(x,y,color,' '); else mask_putchar(x,y,color,cad[0]);cad++;x++;n++;}
if(v_forcebreakcad)
	{
	while(1) {if(x>=40 || n==v_breakcad) break; mask_putchar(x,y,color,' ');cad++;x++;n++;}
	}
lastx=x;
}

/*********************************************************************/
//  saving bitmap (photo mode)
/*********************************************************************/

int make_photo=0; // variable asignada cuando queremos hacer una foto

int photo_number=0;  // contador de numero de foto
char photo_name[256]; // nombre del fichero

typedef struct{
	
	 
	unsigned size;
	unsigned reserved; 
	unsigned offset; 

	unsigned header_size; 
	unsigned width ; 
	unsigned height; 
	unsigned short planes;  
	unsigned short bpp; 
	unsigned compression; 
	unsigned image_size; 
	unsigned ppm[2]; 
	unsigned color_info[2]; 

} bmp_header;
bmp_header bmph;

struct 
{
unsigned char b,g,r,a;
} prgba;

void save_bmp(char *name,unsigned char *punt, int ancho, int alto, int align)
{
FILE *fp;
int i,n;
fp=fopen(name,"wb");
if(fp==NULL) return;
fputc('B',fp);
fputc('M',fp);
bmph.reserved=0;
bmph.size=54+ancho*alto+256*4;
bmph.offset=256*4+54;
bmph.header_size=40;
bmph.width=ancho;
bmph.height=alto;
bmph.planes=1;
bmph.bpp=8;
bmph.compression=0;
bmph.image_size=ancho*alto;
bmph.ppm[0]=bmph.ppm[1]=0;
bmph.color_info[0]=0; // 0 es 256
bmph.color_info[1]=0;
fwrite(&bmph, 1, 52, fp);
prgba.a=0;
for(i=0;i<256;i++)
  {
  if(is_my_palette && i<7)
	  {
	  prgba.r=mypalette_rgb[i][0];
	  prgba.g=mypalette_rgb[i][1];
	  prgba.b=mypalette_rgb[i][2];
      }
 else
  if(i<17)
	  {
      prgba.r=zx_colours[i][0];
      prgba.g=zx_colours[i][1];
      prgba.b=zx_colours[i][2];
	  }
  else 
	 if(i<34)
	  {
	  n=i-18;
      prgba.r=zx_colours[n][0]>>1;
      prgba.g=(zx_colours[n][1]>>2) | 64;
      prgba.b=zx_colours[n][2]>>1;
	  }
  else
  if(i==252)
	  {
	  prgba.r=0;
      prgba.g=255;
      prgba.b=0;
	  }
  else
  if(i==253)
	  {
	  prgba.r=255;
      prgba.g=0;
      prgba.b=0;
	  }
  else
  if(i==254)
	  {
	  prgba.r=255;
      prgba.g=255;
      prgba.b=0;
	  }
  else
  if(i==255)
	  {
	  prgba.r=255;
      prgba.g=255;
      prgba.b=255;
	  }
  else
	  {
	  prgba.r=((i&0x1c)<<3);
      prgba.g=(((i&0xe0)>>2)<<2);
      prgba.b=(((i&3)<<6));
	  }
  fwrite(&prgba, 1, 4, fp);
  }

for(i=1;i<=alto;i++)
	{
	fwrite(punt+(alto-i)*align, 1, ancho, fp);
	}
fclose(fp);
sync();
}



/***************************************************************************/
// seleccion de roms y otras pantallas del emu 
/***************************************************************************/


#define MAX_ENTRY 10240

char files[MAX_ENTRY][257];
char is_directory[MAX_ENTRY];
int nfiles=0;
int ndirs=0;

int is_ext(const char *a,char *b) // compara extensiones
{
int n,m;
m=0;n=0;


while(a[n]!=0) {if(a[n]=='.') m=n; n++;}
n=0;
while(b[n]!=0) {if(a[m]!=b[n]) return 0; m++; n++;}
if(a[m]==0) return 1;
return 0;
}

void read_list_rom(unsigned char *dirname)
{DIR *fd;
static struct dirent *direntp;
static unsigned char *olddirname=NULL;
static char swap[257],sw;
char path_rom[257];
int aa;
sprintf(pathstring,"%s/",globalpath);
for(aa=0;aa<strlen(pathstring);aa++)
	{
	path_rom[aa]=pathstring[aa];
	}
path_rom[aa]=0;

int n,m,f;
if(((int) dirname)==-1) dirname=olddirname;
else olddirname=dirname;
ndirs=0;
nfiles = 0;
if(dirname)
	{if(dirname[0] == '.') {sprintf(swap, "%s", path_rom);dirname=0;} // raiz
     else sprintf(swap, "%s%s/", path_rom, dirname);
	
	}
else sprintf(swap, "%s", path_rom);


fd = opendir(swap);

if(fd != NULL) 
	{
	
	while(nfiles < MAX_ENTRY)
		{is_directory[nfiles]=0;
		direntp=readdir(fd);
	    if(direntp==NULL) break;
		if(direntp->d_name[0] == '.' && direntp->d_name[1]==0) continue;
        
		sprintf(&files[nfiles][0], "%s%s", swap, direntp->d_name);
	   
		if( direntp->d_type & DT_DIR) 
			{if((dirname==0 && direntp->d_name[0] != '.') || (direntp->d_name[0] == '.' && dirname!=0)) {ndirs++;is_directory[nfiles]=1; }
		      else continue; 
		    }
		else 
		
		if(!(//is_ext(&files[nfiles][0],".trd") ||is_ext(&files[nfiles][0],".TRD") ||
			is_ext(&files[nfiles][0],".z80") ||is_ext(&files[nfiles][0],".Z80") ||
			is_ext(&files[nfiles][0],".sna") || is_ext(&files[nfiles][0],".SNA") ||
			is_ext(&files[nfiles][0],".tzx") || is_ext(&files[nfiles][0],".TZX") 
			|| is_ext(&files[nfiles][0],".tap") || is_ext(&files[nfiles][0],".TAP")
			|| is_ext(&files[nfiles][0],".sp") || is_ext(&files[nfiles][0],".SP")
			|| is_ext(&files[nfiles][0],".dsk") || is_ext(&files[nfiles][0],".DSK")
			|| is_ext(&files[nfiles][0],".sav")
			|| is_ext (&files[nfiles][0], ".bz2") || is_ext (&files[nfiles][0], ".BZ2")
			#ifdef USE_ZIP
			|| is_ext (&files[nfiles][0], ".zip") || is_ext (&files[nfiles][0], ".ZIP")
			#endif
		   ) ) continue;
		
		nfiles++;
		}
	
	closedir(fd);	
	}
	

for(n=0;n<(nfiles);n++) //ordena
for(m=n+1;m<nfiles;m++)
	{f=0;
 
	
		if(is_directory[m]==1 && is_directory[n]==0) f=1;
	    if(f==0) if(is_directory[m]==is_directory[n]) if(strcmp(&files[m][0],&files[n][0])<0) f=1;
		if(f)
		{f=0;
		sw=is_directory[m];
		is_directory[m]=is_directory[n];
		is_directory[n]=sw;
		memcpy(swap,&files[m][0],256);
		memcpy(&files[m][0],&files[n][0],256);
		memcpy(&files[n][0],swap,256);
		}
	}

} 

char * get_name_short(char *name) // devuelve el nombre del fichero recortado a 38 caracteres
{
static  char name2[39];
int n,m;
m=0;n=-1;

while(name[m]!=0) {if(name[m]=='/') n=m;m++;}
n++;
char *s = name+n;
int len = strlen(s);

if(len>=38)
{
   memcpy(name2,s,27);
   name2[27]='|';
   memcpy(name2+28,s+len-10,10);
   name2[39]=0;
}
else
  strcpy(name2,s);


/*
for(m=0;m<38;m++)
	{
	name2[m]=name[n];
	if(name[n]==0) break;
	n++;
	}
name2[m]=0;
*/
return (char*)name2;
}

char * get_name(char *name) // devuelve el nombre del fichero completo
{
static unsigned char name2[256];
int n,m;
m=0;n=-1;

while(name[m]!=0) {if(name[m]=='/') n=m;m++;}
n++;
for(m=0;m<255;m++)
	{
	name2[m]=name[n];
	if(name[n]==0) break;
	n++;
	}
name2[m]=0;
return (char*)name2;
}




int get_rom(int tape)
{
static int init=1;
static  int posfile=0;
/*unsigned*/ char cad[256];
int n,m,y,f;
static int bright=1;
int old_speed=gp2x_speed;
unsigned new_key=0,old_key=0;
set_mypalette();
if(init)
	{
   read_list_rom(0);if(nfiles)  init=0; else init=1;
   posfile=0;
	}

while(gp2x_nKeys & (GP2X_START | GP2X_A | GP2X_X | GP2X_PUSH)) gp2x_nKeys=gp2x_joystick_poll(); // para quieto!!

if(posfile>=nfiles) posfile=0;
COLORFONDO=0;  gp2x_flip();
f=0;
while(1)
	{
	if ((f200==0)&&(battery_icon==1))
		{
		n=battery_status();
		if(n>=0 && n<volt) volt=n;
		battery_box(300,4,255,volt);
		}
	gp2x_waitvsync();gp2x_noflip_buffering();ClearScreen(COLORFONDO);
		
	/*speccy_corner();*/
    sprintf(cad,"PROGRAM LIST (%u)",nfiles-ndirs);
    v_putcad(0,1,4,cad);
    m=0;
	for(n=posfile-10;n<posfile+24;n++)
		{if(n<0) continue;
	     if(n>=nfiles) break;
		 if(m>23) break;
		 if(n==posfile) COLORFONDO=1; else COLORFONDO=6;
		v_breakcad=38;v_forcebreakcad=1;
		if(is_directory[n])
			{
			sprintf(cad,"<%s>",get_name_short(&files[n][0]));
			v_putcad(1,m+3,5,cad);
			}
		else v_putcad(1,m+3,2,get_name_short(&files[n][0]));
		v_breakcad=40;v_forcebreakcad=0;
		COLORFONDO=0;
		 m++;
		}
	
	//////////////
	for(;m<24;n++)
		{
		COLORFONDO=6;
		v_breakcad=38;v_forcebreakcad=1;
		v_putcad(1,m+3,2," ");
		v_breakcad=40;v_forcebreakcad=0;
		COLORFONDO=0;
		 m++;
		}
	//////////////
/*
    y=6;
	v_putcad(25,y,4,"SELECT to Exit");y+=2;
	v_putcad(25,y,4,"A/X to Play");y+=2;
	v_putcad(25,y,4,"START to create");y+=1;
	v_putcad(25,y,4,"compressed files");y+=2;
*/	
    v_putcad(1,28,4,"Press Select to Exit, A/X to Play");
    //v_putcad(1,29,4,"Start to create compressed files");

    gp2x_nKeys=gp2x_joystick_poll();
	
	new_key=gp2x_nKeys & (~old_key); 
	old_key=gp2x_nKeys;
    if(!(old_key  & (GP2X_UP | GP2X_DOWN))) f=0;
	if(old_key  & GP2X_UP)
		{
		if(f==0) f=2;
		else if(f & 1) f=2;
		else {f+=2;if(f>40) {f=34;new_key|=GP2X_UP;}}
		}
	if(old_key  & GP2X_DOWN)
		{
		if(f==0) f=1;
		else if(!(f & 1)) f=1;
		else {f+=2;if(f>41) {f=35;new_key|=GP2X_DOWN;}}
		}
		
    if(nfiles>0) if(new_key & (GP2X_A | GP2X_PUSH | GP2X_X)) 
		{
		
		if(!is_directory[posfile]) 
		    {
		      return posfile;
		    }
          else
			{
			 read_list_rom((void *)get_name(&files[posfile][0]));if(nfiles)  init=0; else init=1;posfile=0;
			}
		}

     if(new_key & GP2X_START && !tape)
		{
	    m=gp2x_speed;
        set_speed_clock(200);
	    for(n=0;n<nfiles;n++)
			{
			ClearScreen(0);
    
			v_putcad((40-17)>>1,10,4,"COMPRESSING FILES");
			sprintf(cad,"%% %u",(n*100)/nfiles);
			v_putcad(18,12,5,cad);
			v_putcad((40-17)>>1,15,2,"Press Y to Cancel");
			gp2x_waitvsync();gp2x_noflip_buffering();

			if(!is_directory[n]) 
				
				if(is_ext(&files[n][0],".z80") ||is_ext(&files[n][0],".Z80") ||
			    is_ext(&files[n][0],".sna") || is_ext(&files[n][0],".SNA") ||
			    is_ext(&files[n][0],".tzx") || is_ext(&files[n][0],".TZX") 
			    || is_ext(&files[n][0],".tap") || is_ext(&files[n][0],".TAP")
				|| is_ext(&files[n][0],".sp") || is_ext(&files[n][0],".SP")
				|| is_ext(&files[n][0],".dsk") || is_ext(&files[n][0],".DSK")
				)
			  if(compress_rom(&files[n][0])) break;
			gp2x_nKeys=gp2x_joystick_poll();
			if(gp2x_nKeys  & GP2X_Y) break;
			
			}
        set_speed_clock(m);
		read_list_rom((void *)-1);if(nfiles)  init=0; else init=1;posfile=0;
		
		}

    if((new_key & GP2X_L)) {posfile-=24;if(posfile<0) posfile=0;}
    if((new_key & GP2X_R)) {posfile+=24;if(posfile>=nfiles) posfile=nfiles-1;}

    if((new_key & GP2X_UP)) {posfile--;if(posfile<0) posfile=nfiles-1;}
    if((new_key & GP2X_DOWN)){posfile++;if(posfile>=nfiles) posfile=0;} 
    if(new_key & GP2X_SELECT) {return -1;}

	}

return -1;
}

/****************************************************************************************************************/
// credits
/****************************************************************************************************************/

void credits()
{

int y;
int count=30*6* 10000000;

set_mypalette();
COLORFONDO=0;  gp2x_flip();
while(count>0)
	{
	count--;
	gp2x_waitvsync();gp2x_noflip_buffering();ClearScreen(COLORFONDO);
	speccy_corner();

	y=1;
    v_putcad((40-21)/2,y,2,"GP2Xpectrum v1.7.2WIP");y+=2;
    v_putcad((40-27)/2,y,2,"ported by Hermes/PS2Reality");y+=2;
    v_putcad((40-33)/2,y,2,"continued by Metalbrain & Seleuco");y+=2;
    v_putcad((40-30)/2,y,2,"with patches by kounch,GnoStiC");y+=3;
    v_putcad((40-21)/2,y,4,"Based on the work of:");y+=2;
    v_putcad((40-13)/2,y,4,"rlyeh / fZX32");y+=2;
    v_putcad((40-27)/2,y,4,"Santiago Romero / ASpectrum");y+=2;
    v_putcad((40-33)/2,y,4,"Philip Kendall / FUSE+libspectrum");y+=2;
    v_putcad((40-18)/2,y,4,"James McKay / X128");y+=2;
    v_putcad((40-24)/2,y,4,"Ulrich Doewich / Caprice");y+=2;
    v_putcad((40-22)/2,y,4,"Sergey Bulba /  AY2SNA");y+=2;
    v_putcad((40-27)/2,y,4,"and others (thanks for all)");//y+=2;
   
    gp2x_nKeys=gp2x_joystick_poll();

	if((gp2x_nKeys & (GP2X_A | GP2X_B | GP2X_X | GP2X_Y | GP2X_START))) 
		{while(gp2x_nKeys & (GP2X_A | GP2X_B | GP2X_X | GP2X_Y | GP2X_START)) gp2x_nKeys=gp2x_joystick_poll();return;}
	}
	
	//10 seg
	
	//
	
while(gp2x_nKeys & (GP2X_A | GP2X_B | GP2X_X | GP2X_Y | GP2X_START)) gp2x_nKeys=gp2x_joystick_poll();

}

/****************************************************************************************************************/
// POKE MANAGER
/****************************************************************************************************************/

// funciones para manejar los ficheros .pok
/*
formato
[TITLE1]
POKE add,data
*/

unsigned poke_table[5][10][2];
char poke_title[5][31];

void default_pokes()
{
int n,m;
for(n=0;n<5;n++)
	{
	sprintf(&poke_title[n][0],"POKE #%u",n+1);
    for(m=0;m<10;m++) {poke_table[n][m][0]=0;poke_table[n][m][1]=0;}
	}
}

void write_pokefile(char *name)
{
FILE *fp;
int n,m;

fp=fopen(name,"w");
if(fp==0) return;
for(n=0;n<5;n++)
	{
	fprintf(fp,"[%s]\n",&poke_title[n][0]);
	for(m=0;m<10;m++)
		{
		if(poke_table[n][m][0]>=16384) fprintf(fp,"POKE %u,%u\n",poke_table[n][m][0],poke_table[n][m][1]);
		
		}
	}
fclose(fp);
sync();
}
void read_pokefile(char *name)
{
FILE *fp;
int mode=0;
int n=0,m=0,i;
/*unsigned*/ char str_buf[256];


default_pokes();

fp=fopen(name,"r");
if(fp==0) return;

while(1)
	{
	if(fgets(str_buf,255,fp)==0) break;
	if(str_buf[0]=='[')
		{
		if(mode==1)  n++;
		if(n>4) break; // demasiadas entradas
		i=0;
		while(i<30 && str_buf[1+i]>=32 && str_buf[1+i]!=']')
			{poke_title[n][i]=str_buf[1+i];i++;}
		poke_title[n][i]=0; 
		mode=1;m=0;
		}
	else
	   {
	if(mode==0) continue; //ignora lineas hasta el proximo titulo
		if(sscanf(str_buf,"POKE %u,%u",&poke_table[n][m][0],&poke_table[n][m][1])==2) 
		   {
		   m++;if(m>=10) {mode=0;n++;}
		   }
		// else mode=0; //ignora lineas hasta el proximo titulo
	   }

	}
fclose(fp);
}


static char get_char_digit(unsigned num,int dig)
{
int n,m;
static char cad[5];
m=10000;
for(n=0;n<5;n++)
	{
	cad[n]=48+(num/m);
	num%=m;
	m/=10;
	}


return cad[4-dig];
}

static void change_digit1(unsigned *v,unsigned char ch,int pos)
{
static char cad[5];
unsigned m,n,num;
m=10000;
num=*v;
for(n=0;n<5;n++)
	{
	cad[n]=48+(num/m);
	num%=m;
	m/=10;
	}
cad[pos]=ch;
m=0;
for(n=0;n<5;n++)
	{
	m*=10;
	m+=cad[n]-48;
	}
if(m<65536) *v=m;
}
static void change_digit2(unsigned *v,unsigned char ch,int pos)
{
static char cad[5];
unsigned m,n,num;
m=100;
num=*v;
for(n=0;n<3;n++)
	{
	cad[n]=48+(num/m);
	num%=m;
	m/=10;
	}
cad[pos]=ch;
m=0;
for(n=0;n<3;n++)
	{
	m*=10;
	m+=cad[n]-48;
	}
if(m<256) *v=m;
}

extern void POKE(unsigned dir,unsigned char dat); // funcion de escritura en memoria en zx.c



int poke_manager()
{

int old_speed;
int n,m,y,f,g;
int op=0,op2=0;
unsigned new_key=0,old_key=0;
///unsigned char cad[256];
int ret=0;
int file_modified=0;
int editor_mode=0;
int poke_select=0;
 char *mname;

int pos=0,keypos=0;
char str_key[30][4]={
	 "A","B","C","D","E","F","G","H","I","J","K","L","M","N","Ent",
	 "O","P","Q","R","S","T","U","V","W","X","Y","Z"," ","/","Del"};
char num_key[10][4]={
	 "1","2","3","4","5","6","7","8","9","0"};
	 mname=get_name(MY_filename);
	 // obten nombre sin extension
	 n=0;while(mname[n]!=0) n++;
	 while(n>0) {if(mname[n]=='.') {mname[n]=0;break;} n--;}
	 sprintf(photo_name,"%s/saves/%s.pok",globalpath,mname);


read_pokefile(photo_name);


COLORFONDO=0;
while(gp2x_nKeys & (GP2X_PUSH | GP2X_A | GP2X_B | GP2X_X | GP2X_Y | GP2X_START)) gp2x_nKeys=gp2x_joystick_poll();

ClearScreen(COLORFONDO);
old_speed=gp2x_speed;

g=0;
while(1)
{COLORFONDO=0;
	if ((f200==0)&&(battery_icon==1))
		{
		n=battery_status();
	 	if(n>=0 && n<volt) volt=n;
	 	battery_box(300,4,255,volt);
		}
#ifdef  CAPTURE
if((old_key & GP2X_VOL_UP) && (old_key & GP2X_SELECT))
	{
	sprintf(pathstring,"%s/img/poke_manager.bmp",globalpath);
	sprintf(pathstring2,"%s/img/poke_edit_name.bmp",globalpath);
	sprintf(pathstring3,"%s/img/poke_edit_values.bmp",globalpath);
    if(editor_mode==0) save_bmp(pathstring, gp2x_screen, 320, 240, 320);
	if(editor_mode==1) save_bmp(pathstring2, gp2x_screen, 320, 240, 320);
	if(editor_mode==2) save_bmp(pathstring3, gp2x_screen, 320, 240, 320);
	}
 #endif
 gp2x_waitvsync();gp2x_noflip_buffering();ClearScreen(COLORFONDO);speccy_corner();
    
	if(editor_mode==0)
	{
	COLORFONDO=0;
    v_putcad(14,0,4,"POKE MANAGER");
	y=4;
	if(op==0) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(5,y,2,&poke_title[0][0]);
	y+=2;
	if(op==1) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(5,y,2,&poke_title[1][0]);
	y+=2;
	if(op==2) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(5,y,2,&poke_title[2][0]);
	y+=2;
	if(op==3) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(5,y,2,&poke_title[3][0]);
	y+=2;
	if(op==4) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(5,y,2,&poke_title[4][0]);
	y+=2;
	
	
	if(op==5) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(5,y,2,"Return");y+=2;
    COLORFONDO=0;
	y+=4;
	v_putcad(2,y,4,"Use X to SET, A to Edit and Y to Exit");
	}

    if(editor_mode==1)
	{
	COLORFONDO=0;
	v_putcad(11,0,4,"POKE TITLE EDITOR");
	y=4;
	COLORFONDO=0;
    v_putcad(5,y,2,&poke_title[poke_select][0]);
	COLORFONDO=1; 
	if(poke_title[poke_select][pos]<32) v_putchar(5+pos,y,2,' ');
    else v_putchar(5+pos,y,2,poke_title[poke_select][pos]);
	
	y=15; //teclado virtual
	n=0;
	for(n=0;n<30;n++)
	{
	if(keypos==n) COLORFONDO=1; else COLORFONDO=0;
	if(n==0 || n==15)  v_putcad(5,y+2*(n>=15),2,&str_key[n][0]);
	else v_putcad(lastx,y+2*(n>=15),2,&str_key[n][0]);
	lastx+=1;
	}
     
	 y=20;

	COLORFONDO=0;
	v_putcad((40-34)>>1,y,4,"Use L/R to move the string cursor.");y+=2;
	v_putcad((40-34)>>1,y,4,"Use LEFT/RIGHT in Virtual Keyboard");y+=2;
    v_putcad((40-34)>>1,y,4,"Use X to select and Y to abort");y+=2;
	
	}

	if(editor_mode==2)
	{
	COLORFONDO=0;
	v_putcad(11,0,4,"POKE VALUE EDITOR");
	y=4;
	
	for(n=0;n<10;n++)
		{
		COLORFONDO=0;
        v_putcad(5+16*(n & 1),y,2,"POKE ");
		for(m=0;m<5;m++) {if(pos==m && op2==n) COLORFONDO=1; else COLORFONDO=0;
			              v_putchar(lastx,y,2,get_char_digit(poke_table[poke_select][n][0],4-m));lastx++;
						  }
		COLORFONDO=0;
		v_putcad(lastx,y,2,",");lastx++;
		for(m=0;m<3;m++) {if(pos==m+5 && op2==n) COLORFONDO=1; else COLORFONDO=0;
		                  v_putchar(lastx,y,2,get_char_digit(poke_table[poke_select][n][1],2-m));lastx++;
					  }
					  	
		if(n & 1) y+=2;
		}
   

    y+=2;
   	n=0;
	for(n=0;n<10;n++) // teclado virtual
	{
	if(keypos==n) COLORFONDO=3; else COLORFONDO=1;
	if(n==0)  v_putcad(10,y,2,&num_key[n][0]);
	else v_putcad(lastx,y,2,&num_key[n][0]);
	v_putcad(lastx,y,2," ");
	}
     
	 y+=3;

	COLORFONDO=0;
	v_putcad((40-32)>>1,y,4,"Use UP/DOWN to select the POKE");y+=2;
	v_putcad((40-34)>>1,y,4,"Use L/R to move the numeric cursor.");y+=2;
	v_putcad((40-34)>>1,y,4,"Use LEFT/RIGHT in Virtual Keyboard");y+=2;
    v_putcad((40-34)>>1,y,4,"Use X to select and Y to finish");y+=2;
	
	}

    gp2x_nKeys=gp2x_joystick_poll();
	
	new_key=gp2x_nKeys & (~old_key);
	old_key=gp2x_nKeys;  
   
    
	if(editor_mode==0)
	{
	if(!(old_key  & (GP2X_UP | GP2X_DOWN))) g=0;
	if(old_key  & GP2X_UP)
		{
		if(g==0) g=2;
		else if(g & 1) g=2;
		else {g+=2;if(g>80) {g=68;new_key|=GP2X_UP;}}
		}
	if(old_key  & GP2X_DOWN)
		{
		if(g==0) g=1;
		else if(!(g & 1)) g=1;
		else {g+=2;if(g>81) {g=69;new_key|=GP2X_DOWN;}}
		}
    if(new_key & GP2X_UP) {op--;if(op<0) op=5;}
	if(new_key & GP2X_DOWN) {op++;if(op>5) op=0;}
	

	if(new_key & GP2X_Y) {if(editor_mode) editor_mode=0; else break;}
    
	if((new_key & GP2X_A) && op<5) {editor_mode=1;poke_select=op;pos=0;keypos=0;}
	if(new_key & (GP2X_PUSH | GP2X_X))
	{
	if(op>=0 && op<=4)
		{m=0;
		for(n=0;n<10;n++)
			{
			if(poke_table[op][n][0]!=0) {m=1;POKE(poke_table[op][n][0],poke_table[op][n][1]);}
			
			}
		if(m)
			{
			 for(n=0;n<60;n++)
			 {
			 gp2x_waitvsync();gp2x_noflip_buffering();ClearScreen(COLORFONDO);speccy_corner();COLORFONDO=0;
			 v_putcad(17,14,4,"Done!");
			 }
			}
		}

	if(op==5) break;
    }
	}
	else
	if(editor_mode==1)
	{
	if(new_key & GP2X_Y) {file_modified=0;if(editor_mode) editor_mode=0;}
	
	if(!(old_key  & (GP2X_LEFT | GP2X_RIGHT))) g=0;
    if(old_key  & GP2X_LEFT)
		{
		if(g==0) g=2;
		else if(g & 1) g=2;
		else {g+=2;if(g>80) {g=68;new_key|=GP2X_LEFT;}}
		}
	if(old_key  & GP2X_RIGHT)
		{
		if(g==0) g=1;
		else if(!(g & 1)) g=1;
		else {g+=2;if(g>81) {g=69;new_key|=GP2X_RIGHT;}}
		}
	if(new_key & GP2X_LEFT) {keypos--;if(keypos<0) keypos=29;}
    if(new_key & GP2X_RIGHT) {keypos++;if(keypos>29) keypos=0;}
	if(new_key & GP2X_UP) {keypos-=15;if(keypos<0) keypos=29;}
    if(new_key & GP2X_DOWN) {keypos+=15;if(keypos>29) keypos=0;}

	if(new_key & GP2X_L) {pos--;if(pos<0) pos=0;}
    if(new_key & GP2X_R) {if(poke_title[poke_select][pos]!=0) pos++;}
    if(new_key & (GP2X_PUSH | GP2X_X))
		{
		if(keypos==14) // enter
			{
			editor_mode=2;
			op2=0;keypos=0;pos=0;

			}
		else
		if(keypos==29) // del
			{
			if(pos!=0)
				{
				m=strlen(&poke_title[poke_select][0])+1;
				for(n=pos-1;n<m;n++) poke_title[poke_select][n]=poke_title[poke_select][n+1];
				pos--;if(pos<0)  pos=0;file_modified=1;
				}
			else
				{
				m=strlen(&poke_title[poke_select][0])+1;
				for(n=pos;n<m;n++) poke_title[poke_select][n]=poke_title[poke_select][n+1];
				file_modified=1;
				}
			}
		else 
			{
			if((m=strlen(&poke_title[poke_select][0]))<29)
				{
				m++;
				for(n=m;n>=pos;n--) poke_title[poke_select][n+1]=poke_title[poke_select][n];
				poke_title[poke_select][pos]=str_key[keypos][0];
				/*if(poke_title[poke_select][pos+1]!=0) */pos++;file_modified=1;
				}
			}
		}
	}
else
	if(editor_mode==2)
	{
	if(new_key & GP2X_Y) {file_modified=1;if(editor_mode) editor_mode=0;}
	if(!(old_key  & (GP2X_LEFT | GP2X_RIGHT))) g=0;
	if(old_key  & GP2X_LEFT)
		{
		if(g==0) g=2;
		else if(g & 1) g=2;
		else {g+=2;if(g>80) {g=68;new_key|=GP2X_LEFT;}}
		}
	if(old_key  & GP2X_RIGHT)
		{
		if(g==0) g=1;
		else if(!(g & 1)) g=1;
		else {g+=2;if(g>81) {g=69;new_key|=GP2X_RIGHT;}}
		}
	if(new_key & GP2X_LEFT) {keypos--;if(keypos<0) keypos=9;}
    if(new_key & GP2X_RIGHT) {keypos++;if(keypos>9) keypos=0;}
	if(new_key & GP2X_UP) {pos=0;op2--;if(op2<0) op2=9;}
    if(new_key & GP2X_DOWN) {pos=0;op2++;if(op2>9) op2=0;}

	if(new_key & GP2X_L) {pos--;if(pos<0) {pos=7;op2--;} if (op2<0) op2=0;}
    if(new_key & GP2X_R) {pos++;if(pos>7) {pos=0;op2++;} if (op2>9) op2=0;}
    if(new_key & (GP2X_PUSH | GP2X_X))
		{
		if(pos<5)
			{
			file_modified=1;
			change_digit1(&poke_table[poke_select][op2][0],num_key[keypos][0],pos);

			}
			else
			{
			file_modified=1;
			change_digit2(&poke_table[poke_select][op2][1],num_key[keypos][0],pos-5);

			}
			pos++;if(pos>7) {pos=0;op2++;} if (op2>9) op2=0;

		}
	}
}

if(file_modified) 
	write_pokefile(photo_name);

while(gp2x_nKeys & (GP2X_PUSH | GP2X_A | GP2X_B | GP2X_X | GP2X_Y | GP2X_START)) gp2x_nKeys=gp2x_joystick_poll();


COLORFONDO=0;
ClearScreen(COLORFONDO);gp2x_waitvsync();


return ret;
}

/****************************************************************************************************************/
// disk manager
/****************************************************************************************************************/

int disk_manager()
{

int old_speed;
int n,m,y,f,g;
int op=0,op2=0;
unsigned new_key=0,old_key=0;
/*unsigned*/ char cad[256]="";
int ret=0;



 char *mname;

int pos=0,keypos=0;
char str_key[30][4]={
	 "A","B","C","D","E","F","G","H","I","J","K","L","M","N","Ent",
	 "O","P","Q","R","S","T","U","V","W","X","Y","Z"," ","_","Del"};



read_pokefile(photo_name);


COLORFONDO=0;
while(gp2x_nKeys & (GP2X_PUSH | GP2X_A | GP2X_B | GP2X_X | GP2X_Y | GP2X_START)) gp2x_nKeys=gp2x_joystick_poll();

ClearScreen(COLORFONDO);
old_speed=gp2x_speed;
pos=0;keypos=0;
g=0;
while(1)
{COLORFONDO=0;
if ((f200==0)&&(battery_icon==1))
	{
	n=battery_status();
	if(n>=0 && n<volt) volt=n;
	battery_box(300,4,255,volt);
	}
#ifdef  CAPTURE
if((old_key & GP2X_VOL_UP) && (old_key & GP2X_SELECT))
	{
	sprintf(pathstring,"%s/img/disk_manager.bmp",globalpath);
    save_bmp(pathstring, gp2x_screen, 320, 240, 320);
	
	}
 #endif
 gp2x_waitvsync();gp2x_noflip_buffering();ClearScreen(COLORFONDO);speccy_corner();
    
	
	COLORFONDO=0;
	v_putcad(11,0,4,"DISK TITLE EDITOR");
	y=4;
	COLORFONDO=0;
    v_putcad(12,y,2,&cad[0]);
	COLORFONDO=1; 
	if(cad[pos]<32) v_putchar(12+pos,y,2,' ');
    else v_putchar(12+pos,y,2,cad[pos]);
	
	y=15; //teclado virtual
	n=0;
	for(n=0;n<30;n++)
	{
	if(keypos==n) COLORFONDO=1; else COLORFONDO=0;
	if(n==0 || n==15)  v_putcad(5,y+2*(n>=15),2,&str_key[n][0]);
	else v_putcad(lastx,y+2*(n>=15),2,&str_key[n][0]);
	lastx+=1;
	}
     
	 y=20;

	COLORFONDO=0;
	v_putcad((40-34)>>1,y,4,"Use L/R to move the string cursor.");y+=2;
	v_putcad((40-34)>>1,y,4,"Use LEFT/RIGHT in Virtual Keyboard");y+=2;
    v_putcad((40-34)>>1,y,4,"Use X to select and Y to abort");y+=2;
	
	
    gp2x_nKeys=gp2x_joystick_poll();
	
	new_key=gp2x_nKeys & (~old_key);
	old_key=gp2x_nKeys;  
   
    
	
	if(new_key & GP2X_Y) {break;}
	
	if(!(old_key  & (GP2X_LEFT | GP2X_RIGHT))) g=0;
    if(old_key  & GP2X_LEFT)
		{
		if(g==0) g=2;
		else if(g & 1) g=2;
		else {g+=2;if(g>80) {g=68;new_key|=GP2X_LEFT;}}
		}
	if(old_key  & GP2X_RIGHT)
		{
		if(g==0) g=1;
		else if(!(g & 1)) g=1;
		else {g+=2;if(g>81) {g=69;new_key|=GP2X_RIGHT;}}
		}
	if(new_key & GP2X_LEFT) {keypos--;if(keypos<0) keypos=29;}
    if(new_key & GP2X_RIGHT) {keypos++;if(keypos>29) keypos=0;}
	if(new_key & GP2X_UP) {keypos-=15;if(keypos<0) keypos=29;}
    if(new_key & GP2X_DOWN) {keypos+=15;if(keypos>29) keypos=0;}

	if(new_key & GP2X_L) {pos--;if(pos<0) pos=0;}
    if(new_key & GP2X_R) {if(cad[pos]!=0) pos++;}
    if(new_key & (GP2X_PUSH | GP2X_X))
		{
		if(keypos==14) // enter
			{
			
			op2=0;keypos=0;pos=0;
			m=-1;
			if(cad[0]!=0)
			{
			
			 sprintf(photo_name,"%s/saves/%s.dsk",globalpath,cad);

			m=dsk_save(photo_name);sync();
			}

             for(n=0;n<60;n++)
			 {
			 gp2x_waitvsync();gp2x_noflip_buffering();ClearScreen(COLORFONDO);speccy_corner();COLORFONDO=0;
			 if(m==0) v_putcad(17,14,4,"Done!"); else v_putcad(17,14,4,"Fail!");
			 }
			break;

			}
		else
		if(keypos==29) // del
			{
			if(pos!=0)
				{
				m=strlen(&cad[0])+1;
				for(n=pos-1;n<m;n++) cad[n]=cad[n+1];
				pos--;if(pos<0)  pos=0;
				}
			else
				{
				m=strlen(&cad[0])+1;
				for(n=pos;n<m;n++) cad[n]=cad[n+1];
				
				}
			}
		else 
			{
			if((m=strlen(&cad[0]))<16)
				{
				m++;
				for(n=m;n>=pos;n--) cad[n+1]=cad[n];
				cad[pos]=str_key[keypos][0];
				/*if(cad[pos+1]!=0) */pos++;

				}
			}
		}	
}



while(gp2x_nKeys & (GP2X_PUSH | GP2X_A | GP2X_B | GP2X_X | GP2X_Y | GP2X_START)) gp2x_nKeys=gp2x_joystick_poll();


COLORFONDO=0;
ClearScreen(COLORFONDO);gp2x_waitvsync();


return ret;
}

/****************************************************************************************************************/
// config screen
/****************************************************************************************************************/

void load_empty_dsk();
int Config_SCR()
{

int old_speed;
int n,m,y,f,g;
int op=24;
unsigned new_key=0,old_key=0;
unsigned char cad[256];
int ret=0;
char *mname;
COLORFONDO=0;
while(gp2x_nKeys & (GP2X_PUSH | GP2X_A | GP2X_B | GP2X_X | GP2X_Y | GP2X_START)) gp2x_nKeys=gp2x_joystick_poll();


set_mypalette();
SetVideoScaling(320,320,240);
ClearScreen(COLORFONDO);
old_speed=gp2x_speed;

g=0;
while(1)
{COLORFONDO=0;
if ((f200==0)&&(battery_icon==1))
	{
 	n=battery_status();
	if(n>=0 && n<volt) volt=n;
	battery_box(300,4,255,volt);
	}
#ifdef  CAPTURE
if((old_key & GP2X_VOL_UP) && (old_key & GP2X_SELECT))
  {
  sprintf(pathstring,"%s/img/config.bmp",globalpath);
  save_bmp(pathstring, gp2x_screen, 320, 240, 320);
  }
 #endif


 
gp2x_waitvsync();gp2x_noflip_buffering();ClearScreen(COLORFONDO);speccy_corner();

        v_putcad(14,0,4,"CONFIGURATION");
	if(model==ZX_PLUS3) y= f200 ? 2 : 1; else y=2;

	//opcion 0
	if(op==0) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(10,y,2,"Load Game State");y+=1;
	
	//opcion 1
	if(op==1) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(10,y,2,"Save Keyboard & Speed Profile");y+=1;
	
    //opcion 2
	if(op==2) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(10,y,2,"Save Game State");y+=1;	
	
	y+=1;///SALTO
	
	//opcion 3
	if(op==3) COLORFONDO=1; else COLORFONDO=0;
	if(mconfig.zx_screen_mode) v_putcad(10,y,2,"Full Screen ON");
	else v_putcad(10,y,2,"Full Screen OFF");y+=1;
	
    //opcion 4
    if(op==4) COLORFONDO=1; else COLORFONDO=0;
	if(mconfig.wait_vsync)
	   {v_putcad(10,y,2,"Wait VSYNC ON");y+=1;}
	else
	   {v_putcad(10,y,2,"Wait VSYNC OFF");y+=1;}

    //opcion 5
    if(op==5) COLORFONDO=1; else COLORFONDO=0;
    if(mconfig.show_fps) v_putcad(10,y,2,"Show FPS ON");
    else v_putcad(10,y,2,"Show FPS OFF");    
	y+=1;
	
	//opcion 6 Saltarsela sino F200
	if(op==6) COLORFONDO=1; else COLORFONDO=0;
	if(!f200)
	{
    	if(battery_icon) v_putcad(10,y,2,"Battery Icon ON");
    	else v_putcad(10,y,2,"Battery Icon OFF");
    	y+=1; 
    } 
        
	//y+=1;///SALTO
	
	//opcion 7
    if(op==7) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(10,y,2,"POKE Manager");y+=1;
    //opcion 8
	if(op==8) COLORFONDO=1; else COLORFONDO=0;
	if(make_photo)
	v_putcad(10,y,2,"Photo Mode Enabled");
	else v_putcad(10,y,2,"Photo Mode Disabled");
	y+=1;
    //opcion 9
    if(op==9) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(10,y,2,"Sleep Mode ");y+=1;


	//opciones 10,11,12 saltarse si no plus3
	if(model==ZX_PLUS3)
	{
	//y+=1;///SALTO	
	if(op==10) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(10,y,2,"New +3 Disk");y+=1;
	if(op==11) COLORFONDO=1; else COLORFONDO=0;
	if(dsk_flipped) v_putcad(10,y,2,"Use SIDE B for +3 Disk");
	else v_putcad(10,y,2,"Use SIDE A for +3 Disk");
	y+=1;	
	if(op==12) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(10,y,2,"Save +3 Disk");y+=1;	
	} 
	
    //y+=1;///SALTO
    //opcion 13
	if(op==13) COLORFONDO=1; else COLORFONDO=0;
	
	if (mconfig.contention)
		{
		if(mconfig.frameskip)
			sprintf(menustring,"FS 1, Contention on (%i MHz)",gp2x_speed_fs1);
		else 	sprintf(menustring,"FS 0, Contention on (%i MHz)",gp2x_speed_nofs);
		}
	else	{
		if(mconfig.frameskip)
			sprintf(menustring,"FS 1, Contention off (%i MHz)",gp2x_speed_fs1nc);
		else 	sprintf(menustring,"FS 0, Contention off (%i MHz)",gp2x_speed_nofsnc);
		}
	v_putcad(10,y,2,menustring);
	y+=1;
	//opcion 14
    if(op==14) COLORFONDO=1; else COLORFONDO=0;
	sprintf(menustring,"Emulation Speed %i\%%",mconfig.speed_mode);
	v_putcad(10,y,2,menustring);y+=1;

    y+=1;///SALTO
    //opcion 15
	if(op==15) COLORFONDO=1; else COLORFONDO=0;
	if(mconfig.sound_gain==0)	
	  {v_putcad(10,y,2,"Sound Mute High");y+=1;}
	else if(mconfig.sound_gain==1)
	  {v_putcad(10,y,2,"Sound Mute Low");y+=1;}
	else if(mconfig.sound_gain==2)
	  {v_putcad(10,y,2,"Sound Mute OFF");y+=1;}
	//opcion 16  
	if(op==16) COLORFONDO=1; else COLORFONDO=0;
	if(mconfig.sound_mode==0)	
	  {v_putcad(10,y,2,"Sound OFF");y+=1;}
	else if(mconfig.sound_mode==1)
	  {v_putcad(10,y,2,"Sound Mono");y+=1;}
	else if(mconfig.sound_mode==2)
	  {v_putcad(10,y,2,"Sound Stereo Beeper");y+=1;}
	else if(mconfig.sound_mode==3)
	  {v_putcad(10,y,2,"Sound Stereo ABC AY");y+=1;}
	else if(mconfig.sound_mode==4)
	  {v_putcad(10,y,2,"Sound Stereo ALL");y+=1;}
	//opcion 17  
	if(op==17) COLORFONDO=1; else COLORFONDO=0;
	sprintf(menustring,"Sound Rate %i KHz",mconfig.sound_freq);
	v_putcad(10,y,2,menustring);y+=1;
	
	y+=1;///SALTO
    //opcion 18
    if(op==18) COLORFONDO=1; else COLORFONDO=0;
	if(mconfig.speed_loading)
	   {v_putcad(10,y,2,"Fast Loading ON");y+=1;}
	else
	   {v_putcad(10,y,2,"Fast Loading OFF");y+=1;}
	
    if(op==19) COLORFONDO=1; else COLORFONDO=0;
	if(mconfig.flash_loading)
	   {v_putcad(10,y,2,"Flash Loading ON");y+=1;}
	else
	   {v_putcad(10,y,2,"Flash Loading OFF");y+=1;}
	   
    if(op==20) COLORFONDO=1; else COLORFONDO=0;
	if(mconfig.edge_loading)
	   {v_putcad(10,y,2,"Edge Loading ON");y+=1;}
	else
	   {v_putcad(10,y,2,"Edge Loading OFF");y+=1;}	   
	
    if(op==21) COLORFONDO=1; else COLORFONDO=0;
	if(mconfig.auto_loading)
	   {v_putcad(10,y,2,"Auto Tape Play/Stop ON");y+=1;}
	else
	   {v_putcad(10,y,2,"Auto Tape Play/Stop OFF");y+=1;}
	   
    if(op==22) COLORFONDO=/*3*/1; else COLORFONDO=0;
	{v_putcad(10,y,2,"Tape Browser");y+=1;}
	       
    
    y+=1;///SALTO
    //opcion 23
	if(op==23) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(10,y,2,"Exit from Game");y+=1;
	  
	//opcion 24  
	if(op==24) COLORFONDO=1; else COLORFONDO=0;
	v_putcad(10,y,2,"Return to Game");y+=2;
	
    gp2x_nKeys=gp2x_joystick_poll();
	
	new_key=gp2x_nKeys & (~old_key);
	old_key=gp2x_nKeys;  

    if(!(old_key  & (GP2X_UP | GP2X_DOWN))) g=0;
	if(old_key  & GP2X_UP)
		{
		if(g==0) g=2;
		else if(g & 1) g=2;
		else {g+=2;if(g>80) {g=68;new_key|=GP2X_UP;}}
		}
	if(old_key  & GP2X_DOWN)
		{
		if(g==0) g=1;
		else if(!(g & 1)) g=1;
		else {g+=2;if(g>81) {g=69;new_key|=GP2X_DOWN;}}
		}
    if(new_key & GP2X_UP) {op--;if(f200 && op==6)op=5;if(model!=ZX_PLUS3 && op==12) op=9; if(op<0) op=24;}
	if(new_key & GP2X_DOWN) {op++;if(f200 && op==6)op=7;if(model!=ZX_PLUS3 && op==10) op=13; if(op>24) op=0;}
	if(new_key & GP2X_VOL_UP)
		{
		if(op!=14)
			{	
			int speed;
			if (mconfig.contention==1)
				{
				if (mconfig.frameskip==0)
					{
					if (gp2x_speed_nofs<MAX_SPEED)
						gp2x_speed_nofs++;
					    speed=gp2x_speed_nofs;
					}
				else	{
					if (gp2x_speed_fs1<MAX_SPEED)
						gp2x_speed_fs1++;
					    speed=gp2x_speed_fs1;
					}
				}
			else	{
				if (mconfig.frameskip==0)
					{
					if (gp2x_speed_nofsnc<MAX_SPEED)
						gp2x_speed_nofsnc++;
					    speed=gp2x_speed_nofsnc;
					}
				else	{
					if (gp2x_speed_fs1nc<MAX_SPEED)
						gp2x_speed_fs1nc++;
					    speed=gp2x_speed_fs1nc;
					}
				}
			set_speed_clock(speed);
			}
		else	{
			if(mconfig.speed_mode<175)
				{
			   	mconfig.speed_mode+=5;
			   	int factor = (20 * 100 ) / mconfig.speed_mode;
               			delayvalue= factor +(mconfig.frameskip * factor);
               			sound_end();	    	    
	        		sound_init(-1,-1);
				}
			}
		}
	if(new_key & GP2X_VOL_DOWN)
		{
		if(op!=14)
			{
			int speed;
			if (mconfig.contention==1)
				{
				if (mconfig.frameskip==0)
					{
					if (gp2x_speed_nofs>MIN_SPEED)
						gp2x_speed_nofs--;
					speed=gp2x_speed_nofs;
					}
				else	{
					if (gp2x_speed_fs1>MIN_SPEED)
						gp2x_speed_fs1--;
					speed=gp2x_speed_fs1;
					}
				}
			else	{
				if (mconfig.frameskip==0)
					{
					if (gp2x_speed_nofsnc>MIN_SPEED)
						gp2x_speed_nofsnc--;
					speed=gp2x_speed_nofsnc;
					}
				else	{
					if (gp2x_speed_fs1nc>MIN_SPEED)
						gp2x_speed_fs1nc--;
					speed=gp2x_speed_fs1nc;
					}
				}
			set_speed_clock(speed);
			}
		else 	{
			if(mconfig.speed_mode>25)
				{
			   	mconfig.speed_mode-=5;
			   	int factor = (20 * 100 ) / mconfig.speed_mode;
               			delayvalue= factor +(mconfig.frameskip * factor);
               			sound_end();
	           		sound_init(-1,-1);
				}
			}
		}
	if(new_key & GP2X_Y) {break;}

	if(new_key & (GP2X_A | GP2X_PUSH | GP2X_X))
	{
	if(op==0) {load_state(0);break;}
	if(op==1)
		{
		mname=get_name(MY_filename);
        // obten nombre sin extension
	    n=0;while(mname[n]!=0) n++;
	    while(n>0) {if(mname[n]=='.') {mname[n]=0;break;} n--;}
	    sprintf(photo_name,"%s/saves/%s.key",globalpath,mname);
	    write_keyfile(photo_name);break;
	}
	if(op==2)  {save_state(0);break;}
	
	if(op==3) mconfig.zx_screen_mode^=1;
	if(op==4){mconfig.wait_vsync^=1;}	
	if(op==5){mconfig.show_fps^=1;}	
	if(op==6) battery_icon^=1;
	
	if(op==7) {poke_manager();}
	if(op==8) make_photo^=1;
	if(op==9) sleep_mode();
		
	if(op==10) {load_empty_dsk();dsk_load((void *) DSK);break;}
	if(op==11) {if(driveA.sides) {dsk_flipped^=1;driveA.flipped=dsk_flipped;} else driveA.flipped=0;}
	if(op==12) {disk_manager();}
	
	if(op==13)
		{
		mconfig.frameskip^=1;
		if (mconfig.frameskip==0)
			mconfig.contention^=1;
		
		int factor = (20 * 100 ) / mconfig.speed_mode;
        delayvalue= factor +(mconfig.frameskip * factor);
        
        	int speed;
        
		if (mconfig.contention==1)
			{
			if (mconfig.frameskip==0)
				{
				speed=gp2x_speed_nofs;
				skip=0;
				}
			else	{
				speed=gp2x_speed_fs1;
				}
			}
		else	{
			if (mconfig.frameskip==0)
				{
				speed=gp2x_speed_nofsnc;
				skip=0;
				}
			else	{
				speed=gp2x_speed_fs1nc;
				}
			}
		set_speed_clock(speed);
		}
		
	if(op==14)
	{
	     mconfig.speed_mode+=5;
	     if(mconfig.speed_mode>175)
	        mconfig.speed_mode=25;
	     int factor = (20 * 100 ) / mconfig.speed_mode;
         delayvalue= factor +(mconfig.frameskip * factor);
         sound_end();	    	    
	     sound_init(-1,-1);
	     if(tape_playing && mconfig.speed_loading)
	        sound_pause();
	}
			
	if(op==15)
	{
	    sound_end();	    
	    mconfig.sound_gain = (mconfig.sound_gain+1) % 3;
	    sound_init(-1,-1);
	    if(tape_playing && mconfig.speed_loading)
	        sound_pause();	    
	} 
	
	if(op==16)
	{
	  sound_end();
	  gp2x_close_sound();		  
	  mconfig.sound_mode = (mconfig.sound_mode+1) % 5;
	  if(mconfig.sound_mode!=0)
	  {
	      gp2x_open_sound(mconfig.sound_freq,16,mconfig.sound_mode >= 2);
	      sound_init(-1,-1);
	      if(tape_playing && mconfig.speed_loading)
	        sound_pause();	
	  }	  
	}
	
	if(op==17)
	{
         if(mconfig.sound_freq==11025)
         //   mconfig.sound_freq=16000;
         //if(mconfig.sound_freq==16000)
            mconfig.sound_freq=22050;            
         else if(mconfig.sound_freq==22050)
            mconfig.sound_freq=32000;
         else if(mconfig.sound_freq==32000)
            mconfig.sound_freq=44100;            
         else if(mconfig.sound_freq==44100)
            mconfig.sound_freq=11025;
         
         sound_end();
	     gp2x_close_sound();
         gp2x_open_sound(mconfig.sound_freq,16,mconfig.sound_mode >= 2); 
         sound_init(-1,-1);
         if(tape_playing && mconfig.speed_loading)
	        sound_pause();
	}
	
	if(op==18){
		 mconfig.speed_loading^=1;
		 if(mconfig.speed_loading && tape_playing)
		   sound_pause();
		 else
		   sound_unpause();	
	}
	if(op==19){
		mconfig.flash_loading^=1;
		if(mconfig.flash_loading)
		  ZX_Patch_ROM();
	    else
	      ZX_Unpatch_ROM();
	}
	
	if(op==20){mconfig.edge_loading^=1;}
	
	if(op==21){mconfig.auto_loading^=1;}
	
	if(op==22){tape_browser();}
	
	if(op==23){ret=1;break;}
			   	
	if(op==24) break;}
}
while(gp2x_nKeys & (GP2X_PUSH | GP2X_A | GP2X_B | GP2X_X | GP2X_Y | GP2X_START)) gp2x_nKeys=gp2x_joystick_poll();

set_emupalette();
gp2x_flip();
COLORFONDO=0;
ClearScreen(COLORFONDO);gp2x_waitvsync();

return ret;
}



 
/*************************************************************************************************************************/
//
// SPECTRUM interface
//
//
/**************************************************************************************************************************/

// spectrum
unsigned new_key=0,old_key=0;

unsigned char * Picture;
unsigned char *LCDs8;
void msg(char *s)
{
 /*DrawMessageC(s);
 FlipScreen(1,0);
 DrawMessageC(s);
 FlipScreen(1,0);
 DrawMessageC(s);
 FlipScreen(1,0);
 while(!GpKeyGet()); while(GpKeyGet());*/

}

int keyb_num[4][10]={

	{SPECKEY_1, SPECKEY_2, SPECKEY_3, SPECKEY_4, SPECKEY_5,SPECKEY_6, SPECKEY_7, SPECKEY_8, SPECKEY_9, SPECKEY_0},
	{SPECKEY_Q,SPECKEY_W,SPECKEY_E,SPECKEY_R, SPECKEY_T,SPECKEY_Y,SPECKEY_U,SPECKEY_I,SPECKEY_O, SPECKEY_P},
	{SPECKEY_A,SPECKEY_S,SPECKEY_D, SPECKEY_F, SPECKEY_G, SPECKEY_H,SPECKEY_J, SPECKEY_K, SPECKEY_L,SPECKEY_ENTER},
	{SPECKEY_SHIFT,SPECKEY_Z,SPECKEY_X, SPECKEY_C, SPECKEY_V,SPECKEY_B, SPECKEY_N, SPECKEY_M, SPECKEY_SYMB,SPECKEY_SPACE}
};    

char keyb_str[8][10][4]={
	{" 1 "," 2 "," 3 "," 4 "," 5 "," 6 "," 7 "," 8 "," 9 "," 0 "},
	{" Q "," W "," E "," R "," T "," Y "," U "," I "," O "," P "},
	{" A "," S "," D "," F "," G "," H "," J "," K "," L ","ENT"},
	{"CAP"," Z "," X "," C "," V "," B "," N "," M ","SYM","SPC"},
	{" ! "," @ "," # "," $ "," % "," & "," ' "," ( "," ) "," _ "},
	{" <="," <>"," >="," < "," > "," &&"," ||"," at"," ; "," \42 "},
	{" sp"," no"," st"," to"," th"," ^ "," - "," + "," = ","ENT"},
	{"CAP","  :","  l"," ? "," / "," * "," , "," . ","SYM","SPC"}
};


/*
fill_mask(int  x,int  y,int anc,int  alt)
{
int   n,m;
volatile unsigned char *p,v;
for(n=0;n<alt;n++)
	{
	p=&gp2x_screen8[(y+n)*320+x];
	for(m=0;m<anc;m++)
		{
		v=*p;if(v<17) v+=18;*p++=v;
		
		}
	}
}
*/
int keyboard_on=0;
int keyboard_press=0;
int autoload=0;
int mJoystick =1;//DAV: cambio defecto a kemston
				//left/right/up/down/a,b,x,y/L/R
int map_keys[14]={SPECKEY_O,SPECKEY_P,SPECKEY_Q,SPECKEY_A,SPECKEY_SPACE,SPECKEY_M,SPECKEY_ENTER,SPECKEY_0,SPECKEY_SHIFT,SPECKEY_SYMB,DEFAULT_SPEED_NOFS,DEFAULT_SPEED_FS1,DEFAULT_SPEED_NOFSNC,DEFAULT_SPEED_FS1NC};

int multiple_key[10]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

/****************************************************************************************************************/
// POKE MANAGER
/****************************************************************************************************************/

void write_keyfile(char *name)
{
FILE *fp;
int n;
map_keys[10]=gp2x_speed_nofs;
map_keys[11]=gp2x_speed_fs1;
map_keys[12]=gp2x_speed_nofsnc;
map_keys[13]=gp2x_speed_fs1nc;
fp=fopen(name,"w");
if(fp==0) return;
fprintf(fp,"mJoystick: %i\n",mJoystick);
fprintf(fp,"LEFT: %i\n",map_keys[0]);
fprintf(fp,"RIGHT: %i\n",map_keys[1]);
fprintf(fp,"UP: %i\n",map_keys[2]);
fprintf(fp,"DOWN: %i\n",map_keys[3]);
fprintf(fp,"A: %i\n",map_keys[4]);
fprintf(fp,"B: %i\n",map_keys[5]);
fprintf(fp,"X: %i\n",map_keys[6]);
fprintf(fp,"Y: %i\n",map_keys[7]);
fprintf(fp,"L: %i\n",map_keys[8]);
fprintf(fp,"R: %i\n",map_keys[9]);
fprintf(fp,"SpeedNoFs: %i\n",map_keys[10]);
fprintf(fp,"SpeedFs1: %i\n",map_keys[11]);
fprintf(fp,"SpeedNoFsNc: %i\n",map_keys[12]);
fprintf(fp,"SpeedFs1Nc: %i\n",map_keys[13]);
fclose(fp);
sync();
}

void read_keyfile(char *name)
{
FILE *fp;
int mode=0;
int n=0,m=0,i;
unsigned char str_buf[512];


default_pokes();

fp=fopen(name,"r");
if(fp==0) return;

if(fscanf(fp,"mJoystick: %i\n",&m)==1) mJoystick=m;
if(fscanf(fp,"LEFT: %i\n",&m)==1) map_keys[0]=m;
if(fscanf(fp,"RIGHT: %i\n",&m)==1) map_keys[1]=m;
if(fscanf(fp,"UP: %i\n",&m)==1) map_keys[2]=m;
if(fscanf(fp,"DOWN: %i\n",&m)==1) map_keys[3]=m;
if(fscanf(fp,"A: %i\n",&m)==1) map_keys[4]=m;
if(fscanf(fp,"B: %i\n",&m)==1) map_keys[5]=m;
if(fscanf(fp,"X: %i\n",&m)==1) map_keys[6]=m;
if(fscanf(fp,"Y: %i\n",&m)==1) map_keys[7]=m;
if(fscanf(fp,"L: %i\n",&m)==1) map_keys[8]=m;
  else map_keys[8]=SPECKEY_SHIFT;
if(fscanf(fp,"R: %i\n",&m)==1) map_keys[9]=m;
  else map_keys[9]=SPECKEY_SYMB;
if(fscanf(fp,"SpeedNoFs: %i\n",&m)==1) map_keys[10]=m;
  else map_keys[10]=DEFAULT_SPEED_NOFS;
if(fscanf(fp,"SpeedFs1: %i\n",&m)==1) map_keys[11]=m;
  else map_keys[11]=DEFAULT_SPEED_FS1;
if(fscanf(fp,"SpeedNoFsNc: %i\n",&m)==1) map_keys[12]=m;
  else map_keys[12]=DEFAULT_SPEED_NOFSNC;
if(fscanf(fp,"SpeedFs1Nc: %i\n",&m)==1) map_keys[13]=m;
  else map_keys[13]=DEFAULT_SPEED_FS1NC;
gp2x_speed_nofs=map_keys[10];
if ((gp2x_speed_nofs<MIN_SPEED)||(gp2x_speed_nofs>MAX_SPEED))
	{
	gp2x_speed_nofs=DEFAULT_SPEED_NOFS;
	}
gp2x_speed_fs1=map_keys[11];
if ((gp2x_speed_fs1<MIN_SPEED)||(gp2x_speed_fs1>MAX_SPEED))
	{
	gp2x_speed_fs1=DEFAULT_SPEED_FS1;
	}
gp2x_speed_nofsnc=map_keys[12];
if ((gp2x_speed_nofsnc<MIN_SPEED)||(gp2x_speed_nofsnc>MAX_SPEED))
	{
	gp2x_speed_nofsnc=DEFAULT_SPEED_NOFSNC;
	}
gp2x_speed_fs1nc=map_keys[13];
if ((gp2x_speed_fs1nc<MIN_SPEED)||(gp2x_speed_fs1nc>MAX_SPEED))
	{
	gp2x_speed_fs1nc=DEFAULT_SPEED_FS1NC;
	}
int speed;
if (mconfig.contention==1)
	{
	if (mconfig.frameskip==0)
		{
		speed=gp2x_speed_nofs;
		}
	else	{
		speed=gp2x_speed_fs1;
		}
	}
else	{
	if (mconfig.frameskip==0)
		{
		speed=gp2x_speed_nofsnc;
		}
	else	{
		speed=gp2x_speed_fs1nc;
		}
	}
set_speed_clock(speed);
fclose(fp);
}


static int program_mode=0;
static int unprogram=0;
static int menu_mode=0;


static int redrawmask=1;
int display_keyboard()
{
int n,m,ret,x;
static int curx=0;
static int cury=0;
static int f1=0,f2=0,f3=0,f4=0;
static int sub_men=0;
static int flip=0;

if(menu_mode==0)
{
if(program_mode && cury<4)
	{
	 if((new_key & GP2X_LEFT))  {map_keys[0]=keyb_num[cury][curx];program_mode=0;redrawmask=1;return -1;}
	 if((new_key & GP2X_RIGHT)) {map_keys[1]=keyb_num[cury][curx];program_mode=0;redrawmask=1;return -1;}
	 if((new_key & GP2X_UP))    {map_keys[2]=keyb_num[cury][curx];program_mode=0;redrawmask=1;return -1;}
	 if((new_key & GP2X_DOWN))  {map_keys[3]=keyb_num[cury][curx];program_mode=0;redrawmask=1;return -1;}
	 if((new_key & GP2X_A))     {map_keys[4]=keyb_num[cury][curx];program_mode=0;redrawmask=1;return -1;}
	 if((new_key & GP2X_B))     {map_keys[5]=keyb_num[cury][curx];program_mode=0;redrawmask=1;return -1;}
	 if((new_key & GP2X_X))     {map_keys[6]=keyb_num[cury][curx];program_mode=0;redrawmask=1;return -1;}
	 if((new_key & GP2X_Y))     {map_keys[7]=keyb_num[cury][curx];program_mode=0;unprogram=40;redrawmask=1;return -1;}
	 if((new_key & GP2X_L))     {map_keys[8]=keyb_num[cury][curx];program_mode=0;redrawmask=1;return -1;}
	 if((new_key & GP2X_R))     {map_keys[9]=keyb_num[cury][curx];program_mode=0;redrawmask=1;return -1;}
	}
if(!program_mode)
{
if(old_key  & GP2X_UP)
		{
		if(f1==0) f1=1;
		else {f1++;if(f1>20) {f1=17;new_key|=GP2X_UP;}}
		} else f1=0;
if(old_key  & GP2X_DOWN)
		{
		if(f2==0) f2=1;
		else {f2++;if(f2>20) {f2=17;new_key|=GP2X_DOWN;}}
		} else f2=0;
if(old_key  & GP2X_LEFT)
		{
		if(f3==0) f3=1;
		else {f3++;if(f3>20) {f3=17;new_key|=GP2X_LEFT;}}
		} else f3=0;
if(old_key  & GP2X_RIGHT)
		{
		if(f4==0) f4=1;
		else {f4++;if(f4>20) {f4=17;new_key|=GP2X_RIGHT;}}
		} else f4=0;		
	
if(new_key & GP2X_UP) {redrawmask=1;cury--;if(cury<0) cury=4;if(cury==4) if(curx>4)  curx=4;}
if(new_key & GP2X_DOWN) {redrawmask=1;cury++;if(cury>4) cury=0;if(cury==4) if(curx>4)  curx=4;}
if(new_key & GP2X_LEFT) {redrawmask=1;curx--;if(curx<0) curx=9;if(cury==4) if(curx>4)  curx=4;}
if(new_key & GP2X_RIGHT) {redrawmask=1;curx++;if(curx>9) curx=0;if(cury==4) if(curx>4)  curx=0;}

if((new_key & GP2X_A) && cury<4)
	{
	for(n=0;n<8;n++)
		 if(multiple_key[n]==-1) {multiple_key[n]=keyb_num[cury][curx];break;}
	}
if(old_key & GP2X_B)
    {for(n=0;n<8;n++)
		             if(multiple_key[n]!=-1) ZXKey(multiple_key[n]);
	}
   
}

if((new_key & GP2X_SELECT) && cury!=4) {redrawmask=1;program_mode^=1;}

if(program_mode) redrawmask=1;
else
	{
    if(old_key & GP2X_L) {redrawmask=1;}
	if(old_key & GP2X_R) {redrawmask=1;}
	}
if(redrawmask)
	{
	memset(&mask_keyb[0],0,32*8*11*8);
	for(n=0;n<4;n++)
	for(m=0;m<10;m++)
	{
	if(program_mode) 
		{
		if(curx==m && cury==n) {if(flip & 2) COLORFONDO=252; else COLORFONDO=255;} else COLORFONDO=255;
		}
	else 
		{if(curx==m && cury==n) COLORFONDO=252; else COLORFONDO=255;
		if(n==3 && m==0 && (old_key & GP2X_L)) {redrawmask=1;COLORFONDO=254;}
		if(n==3 && m==8 && (old_key & GP2X_R)) {redrawmask=1;COLORFONDO=254;}
		}
	if(n==3 && m==9) {COLORFONDO=252+(COLORFONDO==255);mask_putcad(5+m*3,n*2+10,253,&keyb_str[n][m][0]);}
	else mask_putcad(5+m*3,n*2+10,255,&keyb_str[n+4*((old_key & GP2X_R)!=0)][m][0]);
	}
	if(curx==0 && cury==n) COLORFONDO=252; else COLORFONDO=255;
	mask_putcad(5,n*2+10,255,"RESET ");
	
	
	if(curx==1 && cury==n) COLORFONDO=252; else COLORFONDO=255;
	mask_putcad(lastx,n*2+10,255,"LOAD ");
	
	
	if(curx==2 && cury==n) COLORFONDO=252; else COLORFONDO=255;
	
	if(mJoystick==1) mask_putcad(lastx,n*2+10,255,"KEMPSTON ");
	if(mJoystick==2) mask_putcad(lastx,n*2+10,255,"INTERFII ");
	if(mJoystick==3) mask_putcad(lastx,n*2+10,255,"INTERF I ");
	if(mJoystick==4) mask_putcad(lastx,n*2+10,255,"CURSOR ");
	if(mJoystick==5) mask_putcad(lastx,n*2+10,255,"FULLER ");
	if(mJoystick==0) mask_putcad(lastx,n*2+10,255,"KEYBOARD ");

	if(curx==3 && cury==n) COLORFONDO=252; else COLORFONDO=255;
	mask_putcad(lastx,n*2+10,255,"MKEY ");

	if(curx==4 && cury==n) COLORFONDO=252; else COLORFONDO=255;
	mask_putcad(lastx,n*2+10,255,"TAPE ");	
	
	redrawmask=0;
	}// redraw

if(!program_mode) 
	{
    if(old_key & GP2X_L) {redrawmask=1;}
	if(old_key & GP2X_R) {redrawmask=1;}
	}

mask_out(4*8,9*8);
//fill_mask(4*8,9*8,32*8,11*8);

flip++;	
COLORFONDO=0;
if(cury==4 && !program_mode && !menu_mode) 
	{
	if(new_key & (GP2X_X | GP2X_PUSH))
		{program_mode=0;redrawmask=1;
		if(curx==0) {sub_men=0;menu_mode=1;} //ZX_Reset(ZX_48);
		if(curx==1) autoload=1;
		if(curx==2) {mJoystick++;if(mJoystick>5) mJoystick=0;}
		if(curx==3) {for(n=0;n<8;n++)
		             if(multiple_key[n]!=-1) ZXKey(multiple_key[n]);
			        multiple_key[0]=multiple_key[1]=multiple_key[2]=multiple_key[3]=
					multiple_key[4]=multiple_key[5]=multiple_key[6]=multiple_key[7]=-1;
                    return -1;
					}
		
		if(curx==4) {
            flush_uppermem_cache(gp2x_screen, (char *)gp2x_screen + 320*240, 0);
            set_mypalette();

			tape_browser();while(gp2x_nKeys & (GP2X_SELECT | GP2X_PUSH)) gp2x_nKeys=gp2x_joystick_poll();keyboard_on=0;
			
			set_emupalette();
            gp2x_flip(); 
            COLORFONDO=0;
            ClearScreen(COLORFONDO);gp2x_waitvsync();			
			
			}
		}
	return -1;
	}
}
if(menu_mode==1)
	{

	//fill_mask(4*8,9*8,32*8,11*8);
	if(redrawmask)
	{
	memset(&mask_keyb[0],0,32*8*11*8);
	COLORFONDO=254;
     	mask_putcad(7,12,254,"SELECT SPECTRUM EMULATION");
	if(sub_men==0) COLORFONDO=252; else COLORFONDO=255;
	mask_putcad(6,14,255,"48K ");
	if(sub_men==1) COLORFONDO=252; else COLORFONDO=255;
	mask_putcad(lastx,14,255,"128K ");
	if(sub_men==2) COLORFONDO=252; else COLORFONDO=255;
	mask_putcad(lastx,14,255,"PLUS2 ");
	if(sub_men==3) COLORFONDO=252; else COLORFONDO=255;
	mask_putcad(lastx,14,255,"PLUS2A ");
	if(sub_men==4) COLORFONDO=252; else COLORFONDO=255;
	mask_putcad(lastx,14,255,"PLUS3");
	redrawmask=0;
	}// redraw

mask_out(4*8,9*8);

	if(new_key & GP2X_LEFT) {redrawmask=1;sub_men--;if(sub_men<0) sub_men=4;}
	if(new_key & GP2X_RIGHT) {redrawmask=1;sub_men++;if(sub_men>4) sub_men=0;}
	if(new_key & GP2X_Y) {redrawmask=1;menu_mode=0;}
	if(new_key & (GP2X_X | GP2X_PUSH))
	{redrawmask=1;
	switch(sub_men)
		{
		case 1:ZX_Reset(ZX_128);break;
		case 2:ZX_Reset(ZX_PLUS2);break;
		case 3:ZX_Reset(ZX_PLUS2A);break;
		case 4:ZX_Reset(ZX_PLUS3);dsk_load((void *) DSK);break;
		default:ZX_Reset(ZX_48);break;
		}
	menu_mode=0;
	}
	return -1;
	}

ret=keyb_num[cury][curx];
return ret;
}


#ifdef USE_ZIP


int zip_load(char *name)
{
 struct zip *za;
 struct zip_file * fp;
 struct zip_stat st;

 int n,m,idx;
 int err;
 
 if ((za=zip_open(name, 0, &err)) == NULL)     return -1;
 m= zip_get_num_files(za);
 if(m<=0) {zip_close(za);return -2;}
 
 
////seleccionar

 char *zip_files[MAX_ENTRY];
 int  zip_pos_file[MAX_ENTRY];
 int  zip_num_files = 0;
 int y = 0;

 for(idx=0;idx<m/*,zip_num_files<MAX_ENTRY*/;idx++)
 {
	zip_stat_index(za, idx , 0, &st);
	if(( 
	is_ext(st.name,".z80") ||is_ext(st.name,".Z80") ||
			is_ext(st.name,".sna") || is_ext(st.name,".SNA") ||
			is_ext(st.name,".tzx") || is_ext(st.name,".TZX") 
			|| is_ext(st.name,".tap") || is_ext(st.name,".TAP")
			|| is_ext(st.name,".sp") || is_ext(st.name,".SP")
			|| is_ext(st.name,".dsk") || is_ext(st.name,".DSK")
			
	 )) 
	 {
	    zip_files[zip_num_files] = (char *)st.name;
	    //zip_files[zip_num_files]="kk";
	    zip_pos_file[zip_num_files] = idx;
	    zip_num_files++;
	   
	    //strcpy(&zip_files[zip_num_files++][0],st.name);	    	    	    	    
	    //strcpy(kk,st.name);
	 }	  	
  }



char cad[256];
int posfile=0;
unsigned new_key=0,old_key=0;
int f=0;

if(zip_num_files!=1)
	{
	COLORFONDO=0;  gp2x_flip();
	while(gp2x_nKeys & (GP2X_X | GP2X_PUSH)) gp2x_nKeys=gp2x_joystick_poll(); // para quieto!!   
	while(1)
		{
		if ((f200==0)&&(battery_icon==1))
    			{
	    		n=battery_status();
			if(n>=0 && n<volt) volt=n;
			battery_box(300,4,255,volt);
			}
		gp2x_waitvsync();gp2x_noflip_buffering();ClearScreen(COLORFONDO);
    		sprintf(cad,"ZIP LIST (%u)",zip_num_files);  
    		v_putcad(0,1,4,cad);
    		m=0;
   
		for(n=posfile-10;n<posfile+24;n++)
	    		{
       			if(n<0) continue;
	   		if(n>=zip_num_files) break;
	   		if(m>23) break;
	   		if(n==posfile) COLORFONDO=1; else COLORFONDO=6;
	   		v_breakcad=38;v_forcebreakcad=1;
       			v_putcad(1,m+3,2,zip_files[n]);
	   		v_breakcad=40;v_forcebreakcad=0;
	   		COLORFONDO=0;
	   		m++;
			}
	
		for(;m<24;n++)//lo que falta
			{
			COLORFONDO=6;
			v_breakcad=38;v_forcebreakcad=1;
			v_putcad(1,m+3,2," ");
			v_breakcad=40;v_forcebreakcad=0;
			COLORFONDO=0;
			m++;
			}
		v_putcad(1,28,4,"Press A/X to Select ZIP Entry");
	
		/*	
    		int y=6;
		v_putcad(30,y,4,"Press A/X");y+=2;
		v_putcad(30,y,4,"to Play");y+=2;
    		*/
	    	gp2x_nKeys=gp2x_joystick_poll();
		new_key=gp2x_nKeys & (~old_key);
		old_key=gp2x_nKeys;
		
		if(!(old_key & (GP2X_UP | GP2X_DOWN))) f=0;
		if(old_key & GP2X_UP)
			{
			if(f==0) f=2;
			else if(f & 1) f=2;
			else {f+=2;if(f>40) {f=34;new_key|=GP2X_UP;}}
			}
		if(old_key & GP2X_DOWN)
			{
			if(f==0) f=1;
			else if(!(f & 1)) f=1;
			else {f+=2;if(f>41) {f=35;new_key|=GP2X_DOWN;}}
			}
	
		if(new_key & (GP2X_A | GP2X_PUSH | GP2X_X)) 
			{
			break;
			}
		if((new_key & GP2X_L)) {posfile-=25;if(posfile<0) posfile=0;}
	    	if((new_key & GP2X_R)) {posfile+=25;if(posfile>=zip_num_files) posfile=zip_num_files-1;}

		if((new_key & GP2X_UP)) {posfile--;if(posfile<0) posfile=zip_num_files-1;}
    		if((new_key & GP2X_DOWN)){posfile++;if(posfile>=zip_num_files) posfile=0;} 	              
		}
	}
else	{
   	posfile=0;
	}

////seleccionar
idx=zip_pos_file[posfile];

//if(idx==m) {zip_close(za);return -2;}

zip_stat_index(za, idx , 0, &st);
 
GAME_size=st.size;
 //st.crc;
		
fp=zip_fopen_index(za, idx,0);

m=0;
while(1)
	{
     n=zip_fread(fp,GAME+m,GAME_size-m);
	if(n>0) m+=n; else break;
	}
if(n<0 || m!=GAME_size) GAME_size=0;
zip_fclose(fp);
zip_close(za);
}

#endif


void load_empty_dsk()
{
int len_d=1*1024*1024;

	BZ_API(BZ2_bzBuffToBuffDecompress) ((void *)DSK, (void *)&len_d,(void *)empty_dsk, size_empty_dsk,0,0);
}

int load_game(char *name)
{
FILE *fp;

GAME_size=0;

MY_filename=name;
fp=fopen(name,"rb");
if(fp==NULL) return;

#ifdef USE_ZIP
if(is_ext (name, ".zip") || is_ext (name, ".ZIP"))
	{
	zip_load(name); return 0;
	}
#endif
if(is_ext (name, ".bz2") || is_ext (name, ".BZ2"))
	{
	BZFILE * my_bzip;
	int bzip_err=0;
	int n,m;
	
	my_bzip=BZ2_bzReadOpen( &bzip_err, fp, 0, 1, NULL, 0 );
	
	GAME_size=BZ2_bzRead (&bzip_err, my_bzip, GAME, 1*1024*1024);
	
	BZ2_bzReadClose (&bzip_err, my_bzip);
    fclose(fp);
	return 0;
	}
fseek(fp,0,SEEK_END);
GAME_size=ftell(fp);
fseek(fp,0,SEEK_SET);
fread(GAME, 1, GAME_size, fp);
fclose(fp);
return 0;
}

int compress_rom(char *name)
{

int i,n,m;
char filename[256];
BZFILE * my_bzip;
int bzip_err=0;
char *temp;
FILE *fp, *fp_bzip;
strcpy(filename,name);

temp=malloc(1024*2048);
if(!temp) return -3;

strcat(filename,".bz2");

fp = fopen(name, "rb");
if (fp == NULL) {free(temp);return -1;}

fp_bzip = fopen(filename, "wb");
if (fp_bzip == NULL) {free(temp);fclose(fp);return -2;}
	
fseek(fp,0,SEEK_END);
m = ftell(fp);
fseek(fp,0,SEEK_SET);
if(m>(1024*2048)) {free(temp);fclose(fp);fclose(fp_bzip);return -3;}
n=fread(temp,1,m,fp);
fclose(fp); 
if(n<m) {free(temp);fclose(fp_bzip);return -4;}



my_bzip=BZ2_bzWriteOpen( &bzip_err, fp_bzip, 4, 0,30);
if(bzip_err!=BZ_OK)  {free(temp);fclose(fp_bzip);return -5;}

BZ2_bzWrite (&bzip_err, my_bzip, temp,  m);
i=bzip_err;	
BZ2_bzWriteClose (&bzip_err, my_bzip,0,(void*)&n,(void*)&m);
fclose(fp_bzip);
free(temp);
sync();
if(i!=BZ_OK || bzip_err!=BZ_OK)
	{
	remove(filename);
	 return -5;
	}
else remove(name);


return 0;
}
struct 
{
unsigned header;
int mJoystick;
int map_keys[16];
unsigned char filename[256];
int have_fd_info;
unsigned reserved[63];
} state_header;


#define SIZEOFZ80REGS sizeof(Z80Regs)




t_track  track_temp;
#define RESTA_PUNT(a) if((int)a!=0)  a-=(unsigned) DSK; else a= (void*)/*(unsigned)*/ -1
#define SUMA_PUNT(a) if((int) a!=-1)  a+=(unsigned) DSK; else a= (void*)/*(unsigned)*/ 0
int save_state(int st)
{


    FILE * fp; unsigned char *mem;
    int count,n,m;
	char *mname;
    char savefile[256];
    BZFILE * my_bzip;
	int bzip_err=0;
	int file_error=0;
	
	

	mname=get_name(MY_filename);
	// obten nombre sin extension
	n=0;while(mname[n]!=0) n++;
	while(n>0) {if(mname[n]=='.') {mname[n]=0;break;} n--;}

	sprintf(photo_name,"%s/saves/%s.tmp",globalpath,mname);
    sprintf(savefile,"%s/saves/%s.sav",globalpath,mname);
	fp=fopen(photo_name,"wb");
	if(fp==NULL) return 0;

	my_bzip=BZ2_bzWriteOpen( &bzip_err, fp, 4, 0,30);
   if(bzip_err!=BZ_OK)  {fclose(fp);return 0;}

				

    mem=(unsigned char*) malloc(16384*16+SIZEOFZ80REGS+100);
    if(mem==NULL) {BZ2_bzWriteClose (&bzip_err, my_bzip,0,(void*)&n,(void*)&m);fclose(fp);return 0;}

    state_header.header=0x12345678;

    state_header.mJoystick=mJoystick;
    map_keys[10]=gp2x_speed_nofs;
    map_keys[11]=gp2x_speed_fs1;
    map_keys[12]=gp2x_speed_nofsnc;
    map_keys[13]=gp2x_speed_fs1nc;
	for(n=0;n<14;n++) state_header.map_keys[n]=map_keys[n];
	if(!is_ext(MY_filename,".sav")) strcpy((void*)state_header.filename,MY_filename);
    
	if(model==ZX_PLUS3) state_header.have_fd_info=1; else state_header.have_fd_info=0;

	BZ2_bzWrite (&bzip_err, my_bzip, (void *)&state_header,  sizeof(state_header));
    if(bzip_err!=BZ_OK) {file_error=1;}

	else
	{

	if(state_header.have_fd_info==1)
		{
		
		BZ2_bzWrite (&bzip_err, my_bzip, (void *) &driveA.current_track,  4);
		BZ2_bzWrite (&bzip_err, my_bzip, (void *) &driveA.current_side,  4);
		BZ2_bzWrite (&bzip_err, my_bzip, (void *) &driveA.current_sector,  4);
		BZ2_bzWrite (&bzip_err, my_bzip, (void *) &driveA.flipped,  4);
		RESTA_PUNT(FDC.buffer_ptr);
		RESTA_PUNT(FDC.buffer_endptr);
		BZ2_bzWrite (&bzip_err, my_bzip, (void *) &FDC,  sizeof (t_FDC));
		SUMA_PUNT(FDC.buffer_ptr);
		SUMA_PUNT(FDC.buffer_endptr);
		if(active_track) memcpy(&track_temp,active_track,sizeof (t_track)); else memset(&track_temp,0,sizeof (t_track));
		RESTA_PUNT(track_temp.data);
		for(n=0;n<track_temp.sectors;n++)
        {
        RESTA_PUNT(track_temp.sector[n].data);
        }
		BZ2_bzWrite (&bzip_err, my_bzip, (void *) &track_temp,  sizeof (t_track));
        //if(bzip_err!=BZ_OK) {file_error=1;}
		}
	if(!file_error)
	{
    count=ZX_SaveState(&mem[0]);

    BZ2_bzWrite (&bzip_err, my_bzip, (void *)&mem[0],  count);
    if(bzip_err!=BZ_OK) {file_error=1;}
	}
	}

	BZ2_bzWriteClose (&bzip_err, my_bzip,0,(void*)&n,(void*)&m);
	if(bzip_err!=BZ_OK) {file_error=1;}
	fclose(fp);
	if(!file_error)
    {
    remove(savefile);
    rename(photo_name,savefile);
    }
    else remove(photo_name);
	sync();

    free(mem);

    return 1;
}

t_FDC FDC_temp;
t_drive disk_temp;

void fdc_readtrk2(void);

int load_state(int st)
{
    FILE * fp; unsigned char *mem;
    unsigned temp, size;

	char *mname;

	BZFILE * my_bzip;
	int bzip_err=0;
	int n;
	mname=get_name(MY_filename);
	// obten nombre sin extension
	n=0;while(mname[n]!=0) n++;
	while(n>0) {if(mname[n]=='.') {mname[n]=0;break;} n--;}

	sprintf(photo_name,"%s/saves/%s.sav",globalpath,mname);

	fp=fopen(photo_name,"rb");
	if(fp==NULL) return 0;

	my_bzip=BZ2_bzReadOpen( &bzip_err, fp, 0, 1, NULL, 0 );

    	size=BZ2_bzRead (&bzip_err, my_bzip, (void *)&state_header, sizeof(state_header));
	
	if(state_header.header!=0x12345678) {BZ2_bzReadClose (&bzip_err, my_bzip);fclose(fp);return 0;}
	
	if(state_header.have_fd_info==1)
	{
		BZ2_bzRead (&bzip_err, my_bzip, (void *) &disk_temp.current_track,  4);
		BZ2_bzRead (&bzip_err, my_bzip, (void *) &disk_temp.current_side,  4);
		BZ2_bzRead (&bzip_err, my_bzip, (void *) &disk_temp.current_sector,  4);
		BZ2_bzRead (&bzip_err, my_bzip, (void *) &disk_temp.flipped,  4);
		BZ2_bzRead (&bzip_err, my_bzip, (void *) &FDC_temp,  sizeof (t_FDC));
		BZ2_bzRead (&bzip_err, my_bzip, (void *) &track_temp,  sizeof (t_track));
		SUMA_PUNT(track_temp.data);
        for(n=0;n<track_temp.sectors;n++)
        {
        	SUMA_PUNT(track_temp.sector[n].data);
        }		
	}
   
    mem=(unsigned char*) malloc(16384*16+SIZEOFZ80REGS+100);
    if(mem==NULL) {BZ2_bzReadClose (&bzip_err, my_bzip);fclose(fp);return 0;}

    size=BZ2_bzRead (&bzip_err, my_bzip, (void *)&mem[0], 16384*16+SIZEOFZ80REGS+100);
	
	BZ2_bzReadClose (&bzip_err, my_bzip);fclose(fp);
	mJoystick=state_header.mJoystick;
	for(n=0;n<12;n++) map_keys[n]=state_header.map_keys[n];
  	if(map_keys[8]==0) map_keys[8]=SPECKEY_SHIFT;
	if(map_keys[9]==0) map_keys[9]=SPECKEY_SYMB;
  	if(map_keys[10]==0) map_keys[10]=DEFAULT_SPEED_NOFS;
	if(map_keys[11]==0) map_keys[11]=DEFAULT_SPEED_FS1;
	gp2x_speed_nofs=map_keys[10];
	gp2x_speed_fs1=map_keys[11];
	gp2x_speed_nofsnc=map_keys[12];
	gp2x_speed_fs1nc=map_keys[13];	

	int speed;
	if (mconfig.contention==1)
		{
		if (mconfig.frameskip==0)
			{
			speed=gp2x_speed_nofs;
			}
		else	{
			speed=gp2x_speed_fs1;
			}
		}
	else	{
		if (mconfig.frameskip==0)
			{
			speed=gp2x_speed_nofsnc;
			}
		else	{
			speed=gp2x_speed_fs1nc;
			}
		}
	set_speed_clock(speed);
	MY_filename=(char*)state_header.filename ;

	#ifdef DEBUG_MSG
	printf("Load State:\n%s\n",state_header.filename);
    #endif
	
    memset(DSK,0,1*1024*1024);
    load_empty_dsk(); // disco comprimido formateado por defecto

	if(!is_ext((char*)state_header.filename,".sav"))
	load_game((char*)state_header.filename );
	else {load_game("fake_name.fak");}
    ZX_LoadGame(ZX_128, /*MyGameCRC*/0, 0/*mQuick*/);

	

    {
    int code = 0;
    code=ZX_LoadState(&mem[0]);
	
    
    free(mem);
	
	
   if(state_header.have_fd_info==1)
	{dsk_load(DSK);
    SUMA_PUNT(FDC_temp.buffer_ptr);
    SUMA_PUNT(FDC_temp.buffer_endptr);
    FDC_temp.cmd_handler=FDC.cmd_handler;
    FDC=FDC_temp;
     driveA.current_track=disk_temp.current_track;
     driveA.current_side=disk_temp.current_side; 
	 driveA.current_sector=disk_temp.current_sector;
	 driveA.flipped=disk_temp.flipped;
	
	
	active_track=&track_temp;
	}
    return (code?1:0);
    }
	
}


int
tape_browser()
{
	
	char cad[256];
	int posfile=0;
	unsigned new_key=0,old_key=0;
	int n=0;
	int m=0;
	int f=0;
	
	int max_blocks = 1024;
	char block_entries[max_blocks][256];	
	int num_entries = tape_blocks_entries(block_entries,38);
		
	posfile=tape_get_current_block();
	
	COLORFONDO=0;  gp2x_flip();
	while(gp2x_nKeys & (GP2X_A | GP2X_X | GP2X_PUSH)) gp2x_nKeys=gp2x_joystick_poll(); // para quieto!!   
	while(1)
		{			
		if ((f200==0)&&(battery_icon==1))
	    		{
		    	n=battery_status();
			if(n>=0 && n<volt) volt=n;
			battery_box(300,4,255,volt);
			}
		gp2x_waitvsync();gp2x_noflip_buffering();ClearScreen(COLORFONDO);//speccy_corner();
		sprintf(cad,"TAPE BLOCK LIST (%u) TAPE:%s",num_entries,tape_playing?"ON":"OFF");  
	    	v_putcad(0,1,4,cad);
	    	m=0;
	   
		for(n=posfile-10;n<posfile+22;n++)
	    		{
	       		if(n<0) continue;
		   	if(n>=num_entries) break;
		   	if(m>21) break;
		   	int col = 2;
		   	if (n==tape_get_current_block()) 
			col = tape_playing ? 4 : 3 ; 
		   		   
		   	if(n==posfile) COLORFONDO=1;
		   	else COLORFONDO=6;
		   	v_breakcad=38;v_forcebreakcad=1;
	       		v_putcad(1,m+3,col,block_entries[n]);     	       
		   	v_breakcad=40;v_forcebreakcad=0;
		   	if (n==tape_get_current_block()) 
			v_putcad(6,m+3,col,"*");
		   	COLORFONDO=0;
		   	m++;
			}
		
		for(;m<22;n++)//lo que falta
		{
			COLORFONDO=6;
			v_breakcad=38;v_forcebreakcad=1;
			v_putcad(1,m+3,2," ");
			v_breakcad=40;v_forcebreakcad=0;
			COLORFONDO=0;
			m++;
		}
		
	    
	    v_putcad(1,26,4,"Press Select to Exit,Start to New Tape"); 
	    v_putcad(1,28,4,"A/X to Toggle Tape Play or Change Block");
	    
	    gp2x_nKeys=gp2x_joystick_poll();	
		new_key=gp2x_nKeys & (~old_key); 
		old_key=gp2x_nKeys;	
		
		if(new_key & (GP2X_A | GP2X_PUSH | GP2X_X)) 
		{
			if(posfile==tape_get_current_block())
			{
			   tape_toggle_play(1);//cuidado que avanza
			   if(posfile==tape_get_current_block())
			      tape_select_block(posfile);
			}   
			else
			  tape_select_block(posfile);
		}
	    
		if(new_key & (GP2X_SELECT | GP2X_PUSH )) 
		{
		    break;
		}
		
		if(new_key & (GP2X_START | GP2X_PUSH )) 
		{
            int r=get_rom(1);
            while(gp2x_nKeys & (GP2X_SELECT  | GP2X_A | GP2X_X | GP2X_PUSH)) gp2x_nKeys=gp2x_joystick_poll(); // para quieto!!   
            if(r==-1) continue;            
            int ret=0;
		    ret=load_game(&files[r][0]);
		    char *mname=get_name(MY_filename);
		    // obten nombre sin extension
		    n=0;while(mname[n]!=0) n++;
		    while(n>0) {if(mname[n]=='.') {mname[n]=0;break;} n--;}
		       sprintf(photo_name,"%s/saves/%s.key",globalpath,mname);
		    read_keyfile(photo_name);
		    ZX_LoadGame(-1,0,0);	
		    while(gp2x_nKeys & (GP2X_A | GP2X_X | GP2X_PUSH)) gp2x_nKeys=gp2x_joystick_poll(); // para quieto!!   	    
		    num_entries = tape_blocks_entries(block_entries,38);
		    posfile=tape_get_current_block();
		    //TODO chequear esto	     
		}		    
	    
	    if((new_key & GP2X_L)) {posfile-=24;if(posfile<0) posfile=0;}
	    if((new_key & GP2X_R)) {posfile+=24;if(posfile>=num_entries) posfile=num_entries-1;}
	    
        if(!(old_key  & (GP2X_UP | GP2X_DOWN))) f=0;
	    if(old_key  & GP2X_UP)
		{
		   if(f==0) f=2;
		   else if(f & 1) f=2;
		   else {f+=2;if(f>40) {f=34;new_key|=GP2X_UP;}}
		}
	    if(old_key  & GP2X_DOWN)
		{
		   if(f==0) f=1;
		   else if(!(f & 1)) f=1;
		   else {f+=2;if(f>41) {f=35;new_key|=GP2X_DOWN;}}
		}
	
	    if((new_key & GP2X_UP)) {posfile--;if(posfile<0) posfile=num_entries-1;}
	    if((new_key & GP2X_DOWN)){posfile++;if(posfile>=num_entries) posfile=0;} 	              
	}
	
}


unsigned oldtime=0;
unsigned fpstime=0;

int main(int argc, char *argv[])
{
int i,r,n,ret;
int dclock;
int sel_key=0;
int fd;

unsigned time;
int nvol=0;
Z80Regs regs_z80;
char *mname;
char *currentdir;
FILE *fp;

int count_fps=0;
int fpsseg=0;
long tape_stop_delay=0;

#ifdef DEBUG_MSG
printf("Iniciando gp2xpectrum\n");
#endif
gp2x_init();

cur_frame=0;


fd=open("/dev/touchscreen/wm97xx", O_RDONLY | O_NOCTTY);
if (fd!=-1)
	{
	f200=1;
	///HACK f200=0;
	close(fd); // Just close it for now...
	}

#ifdef DEBUG_MSG
	printf("Init USB Joys\n");
#endif
	gp2x_usbjoy_init();

usbkeyboard_found=findUSBkeyboard();

MY_filename="default_name.fak";
spectrumZ80=&regs_z80;

cpuctrl_init();  // ATENCION: si no se hace esto, cuelgue seguro (aprovecho la definicion de rlyeh y debe ir despues de gp2x_init)
Disable_940(); // desconecta el 940t
save_system_regs();
dclock=get_display_clock_div();
//load_mconfig();//error no esta puesto el global path

mount("-t vfat -o remount,sync,iocharset=utf8 /dev/mmcsd/disc0/part1 /mnt/sd");

int aa,bb;
int cutpath=0;

currentdir=(char*)get_current_dir_name();

if ((currentdir[0]=='/')&&(currentdir[1]=='t')&&(currentdir[2]=='m')&&(currentdir[3]=='p'))
	{
	cutpath=4;
	}
for(aa=cutpath;aa<strlen(currentdir);aa++)
	{
	pathstring[aa-cutpath]=currentdir[aa];
	}
pathstring[aa-cutpath]=0;
int l=strlen(pathstring);
int largv0=strlen(argv[0]);
int startname=l;
int lextension=3;

if(argv[0][largv0-1]=='p')
	{
	// Ejecutable comprimido
	startname=8;
	lextension=7;
	}
else if(argv[0][0]=='.')
	{
	startname=1;
	}
for(aa=startname;aa<(largv0-lextension);aa++)
	{
	pathstring[aa+l-startname]=argv[0][aa];
	}
	
pathstring[aa+l-startname]='i';
pathstring[aa+l-startname+1]='n';
pathstring[aa+l-startname+2]='i';
pathstring[aa+l-startname+3]=0;

fp=fopen(pathstring,"rb");
if(fp!=NULL)
	{
	aa=fread(pathstring3, 1, 300, fp);
	pathstring3[aa]=0;
	fclose(fp);
	if (pathstring3[0]=='/')
		{
		if ((pathstring3[1]=='m')&&(pathstring3[2]=='n')&&(pathstring3[3]=='t'))
			{
			// Si comienza con /mnt -> se copia tal cual
			for(bb=0;(bb<=aa)&&(pathstring3[bb]>13);bb++)
				{
				globalpath[bb]=pathstring3[bb];
				}
			globalpath[bb]=0;
			}
		else	{
			// Si comienza con /(otra cosa) -> se copia tal cual a partir del /mnt/sd
			for(bb=0;(bb<=aa)&&(pathstring3[bb]>13);bb++)
				{
				globalpath[bb+7]=pathstring3[bb];
				}
			globalpath[bb+7]=0;
			}
		}
	else	{
		if ((pathstring3[0]=='.')&&(pathstring3[1]=='/'))
			{
			// Si comienza con ./ -> se copia del pathstring a partir de l
			for(bb=0;(bb<l)&&(pathstring[bb]>13);bb++)
				{
				globalpath[bb]=pathstring[bb];
				}
			for(bb=0;(bb<=aa)&&(pathstring3[bb]>13);bb++)
				{
				globalpath[bb+l]=pathstring3[bb];
				}
			globalpath[bb+l]=0;
			}
		else	{
			// Si comienza con (otra cosa) -> se copia tal cual a partir del /mnt/sd/
			for(bb=0;(bb<=aa)&&(pathstring3[bb]>13);bb++)
				{
				globalpath[bb+8]=pathstring3[bb];
				}
			globalpath[bb+8]=0;
			}
		}
	}

// CREAR RUTA...
aa=0;
for(i=1;i<strlen(globalpath);i++)
	{
	if (globalpath[i]=='/')
		{
		aa++;
		if (aa>2)
			{
			strncpy(pathstring2,globalpath,i);
			sprintf(pathstring,"%s",pathstring2);
			mkdir(pathstring,S_IREAD | S_IWRITE);
			}
		}
	}
	
sprintf(pathstring,"%s",globalpath);
sprintf(pathstring2,"%s/saves",globalpath);
sprintf(pathstring3,"%s/img",globalpath);	
mkdir(pathstring,S_IREAD | S_IWRITE);
mkdir(pathstring2,S_IREAD | S_IWRITE);
mkdir(pathstring3,S_IREAD | S_IWRITE);

credits();
//volume=50;
 
load_mconfig(); 
gp2x_open_sound(mconfig.sound_freq,16,mconfig.sound_mode >= 2);
gp2x_volume(volume,volume);

tape_init();

ZX_Init();

while(1)
{
do{gp2x_nKeys=gp2x_joystick_poll();}while(gp2x_nKeys  & (GP2X_SELECT | GP2X_L | GP2X_R | GP2X_X | GP2X_PUSH));

SetVideoScaling(320,320,240);

memset(DSK,0,1*1024*1024);
load_empty_dsk(); // disco comprimido formateado por defecto
nvol=0;
set_battery_led(0);
dsk_flipped=0;
r=get_rom(0);
if(r==-1) break;
ret=0;
if(r>=0)
	{if(is_ext (&files[r][0], ".sav"))	{MY_filename=&files[r][0];load_state(0);ret=666;}
     else
		{ret=load_game(&files[r][0]);
	    mname=get_name(MY_filename);
		// obten nombre sin extension
		n=0;while(mname[n]!=0) n++;
		while(n>0) {if(mname[n]=='.') {mname[n]=0;break;} n--;}
		sprintf(photo_name,"%s/saves/%s.key",globalpath,mname);
		read_keyfile(photo_name);
		}
	}


// inicializa variables de teclado virtual
keyboard_on=0;
keyboard_press=0;
autoload=0;
program_mode=0;
menu_mode=0;
redrawmask=1;

ClearScreen(COLORFONDO);gp2x_waitvsync();
gp2x_flip();
ClearScreen(COLORFONDO);gp2x_waitvsync();
gp2x_flip();

set_emupalette();
//GAME_size=MyGameSize;
if(ret!=666) ZX_LoadGame(ZX_128, /*MyGameCRC*/0, 0/*mQuick*/);

skip=0;
count_fps=0;
 while(1)
 {
 #ifdef  CAPTURE
if((old_key & GP2X_VOL_UP) && (old_key & GP2X_SELECT))
 {
 sprintf(pathstring,"%s/img/ingame.bmp",globalpath);
 save_bmp(pathstring, gp2x_screen, 320-64*full_screen, 240-48*full_screen, 320);
 }
 #endif
if ((f200==0)&&(battery_icon==1))
	{
	n=battery_status();
	if(n>=0 && n<volt) volt=n;
	}

Picture=gp2x_screen;
if (keyboard_on==0 && !tape_playing)
{
full_screen=mconfig.zx_screen_mode;
if(full_screen) SetVideoScaling(320,256,192); else SetVideoScaling(320,320,240);
}
else
	 {
	 full_screen=0;
     SetVideoScaling(320,320,240);
	 }
	 
  ZX_Frame(skip);
 
  Sound_Loop();
 
  if (keyboard_on==0)
	 {
	 if ((gp2x_nKeys & GP2X_L) && (gp2x_nKeys & GP2X_SELECT) && make_photo!=0)
	 {
	 char *mname;
	 mname=get_name(MY_filename);
	 // obten nombre sin extension
	 n=0;while(mname[n]!=0) n++;
	 while(n>0) {if(mname[n]=='.') {mname[n]=0;break;} n--;}

//	 make_photo=2;
	 sprintf(photo_name,"%s/img/%s-%3.3u.bmp",globalpath,mname,photo_number);
	 save_bmp(photo_name, gp2x_screen, 320-64*full_screen, 240-48*full_screen, 320);
	 photo_number++;memset(gp2x_screen,0,320*240);
	 }
	 else
	 if (gp2x_nKeys & GP2X_SELECT)
		 {set_battery_led(0);
		 if(Config_SCR()==1) break;
		 }
	 }
  
  gp2x_nKeys=gp2x_joystick_poll();
	
	new_key=gp2x_nKeys & (~old_key);
	old_key=gp2x_nKeys;
//    if((old_key & GP2X_L) && (gp2x_nKeys  & GP2X_SELECT)) break;
//SONIDO!

	    if((old_key & GP2X_VOL_UP) && (nvol % 3 == 0))
	       {volume+=1;if(volume>100) volume=100;gp2x_volume(volume,volume);nvol=50*5;}
	    if((old_key & GP2X_VOL_DOWN) && (nvol % 3 == 0))
			{volume-=1;if(volume<0) volume=0;gp2x_volume(volume,volume);nvol=50*5;}
        

	 if(nvol!=0)
     {
        char result[3]; 
        sprintf( result, "%d", volume ); 
        v_putcad(0,1,4,result);
        nvol--;
     }
     
    if(mconfig.show_fps)
    {        
        if(gp2x_timer_poll() - fpstime > 1000)
        {           
            fpsseg = count_fps;
            count_fps = 0;   
            fpstime = gp2x_timer_poll();
        }
        count_fps++;
        char result[4]; 
        sprintf( result, "%d", fpsseg );   
        int tmp = COLORFONDO;
        if(!full_screen)
          COLORFONDO=4;    
        v_putcad(0,0,4,result); 
        COLORFONDO=tmp;
        cur_frame++;
    }
        
    if(tape_playing && mconfig.speed_loading)
    {
         if(!init_speed_loading)
         {
             init_speed_loading=1;
             //set_speed_clock(200); estar cambiando el clock hace que se vuelva inestable... mejor lo dejamos a 200 por defecto
             sound_pause();
         }
         skip = !(cur_frame % 4 == 0);//1 de 3 frames    
    }    
    else
    {
        if(init_speed_loading)
        {
        	init_speed_loading=0;
        	//set_speed_clock(gp2x_speed);
        	skip = 0;
        	sound_unpause(); 
        }
        skip^=mconfig.frameskip;
    }
    
    if(tape_playing)
    {                                 	     	
        int tmp = COLORFONDO;
        
        if(fast_edge_loading!=2)
        {
           COLORFONDO=/*6*/0;	
           v_putcad(320/8-5,240/8-2,/*0*/6,"PLAY");
        }
        else
        {
           COLORFONDO=/*4*/0;
           v_putcad(320/8-5,240/8-2,/*0*/4,"PLAY");
        } 
        /*TEST*/
//        i=0;
//        for(i=0;i<10;i++)
//        {
//           COLORFONDO=i;
//           char result[3]; 
//           sprintf( result, "%d", i);  
//           v_putcad(i,240/8-1,i,result);
//        }
        /* TEST */
        COLORFONDO=tmp;
        tape_stop_delay = 3000;                        
    }
    else if(tape_stop_delay >=0)
    {
        tape_stop_delay -= 20;
        int tmp = COLORFONDO;
        
        int x = full_screen ? 256 : 320;
        int y = full_screen ? 192 : 240;
        COLORFONDO=/*2*/0;
        v_putcad(x/8-5,y/8-2,2,"STOP");
        COLORFONDO=tmp;
    }

if(new_key & GP2X_START) {keyboard_on^=1;program_mode=0;menu_mode=0;}
if(keyboard_on) 
	{
	if(old_key & (GP2X_X | GP2X_PUSH))
		{
		ZXKey(sel_key);
		}
     	if(autoload)
	 	{
		if(autoload==1 || autoload==2)
			{
			ZXKey(SPECKEY_J);
			}
		if(autoload==11 || autoload==12 || autoload==21|| autoload==22)
			{
			ZXKey(SPECKEY_SYMB);
			ZXKey(SPECKEY_P);
			}
		if(autoload==30)
			{
			ZXKey(SPECKEY_ENTER);
			}
		if(autoload==31)
			{
			ZXKey(SPECKEY_ENTER);
			autoload=-1;
			}
	 	autoload++;
	 	}
	}
else
	 {//left/right/up/down/a,b,x,y/L,R
	
      switch(mJoystick)
		{
		 case 1://kempston
         
  
		  if(old_key & GP2X_LEFT)  kempston|=2;
		  if(old_key & GP2X_RIGHT) kempston|=1; 
	      if(old_key & GP2X_UP)    kempston|=8;
	      if(old_key & GP2X_DOWN)  kempston|=4;
	      if(old_key & GP2X_A)     kempston|=16;
          break;

		 case 2:
          //Sinclair Joystick 1,Interface II (port 1)

		  if(old_key & GP2X_LEFT)  ZXKey(SPECKEY_6);
		  if(old_key & GP2X_RIGHT) ZXKey(SPECKEY_7);
	      if(old_key & GP2X_UP)    ZXKey(SPECKEY_9);
	      if(old_key & GP2X_DOWN)  ZXKey(SPECKEY_8);
	      if(old_key & GP2X_A)     ZXKey(SPECKEY_0);
          break;

	     case 3:
          //Sinclair Joystick 2,Interface II (port 2)
		  if(old_key & GP2X_LEFT)  ZXKey(SPECKEY_1);
		  if(old_key & GP2X_RIGHT) ZXKey(SPECKEY_2);
	      if(old_key & GP2X_UP)    ZXKey(SPECKEY_4);
	      if(old_key & GP2X_DOWN)  ZXKey(SPECKEY_3);
	      if(old_key & GP2X_A)     ZXKey(SPECKEY_5);
		
          break;
         
		 case 4:
          // Cursor
		  if(old_key & GP2X_LEFT)  ZXKey(SPECKEY_5);
		  if(old_key & GP2X_RIGHT) ZXKey(SPECKEY_8);
	      if(old_key & GP2X_UP)    ZXKey(SPECKEY_7);
	      if(old_key & GP2X_DOWN)  ZXKey(SPECKEY_6);
	      if(old_key & GP2X_A)     ZXKey(SPECKEY_0);
		
          break;
   
         case 5:
		  //fuller
		  
		  if(old_key & GP2X_LEFT)  fuller&=0xFF-4; 
		  if(old_key & GP2X_RIGHT) fuller&=0xFF-8; 
	      if(old_key & GP2X_UP)    fuller&=0xFF-1; 
	      if(old_key & GP2X_DOWN)  fuller&=0xFF-2; 
	      if(old_key & GP2X_A)     fuller&=0xFF-128;
          break;

	     default: // keyboard
		 if(old_key & GP2X_LEFT) ZXKey(map_keys[0]);
		 if(old_key & GP2X_RIGHT) ZXKey(map_keys[1]);
	     if(old_key & GP2X_UP) ZXKey(map_keys[2]);
	     if(old_key & GP2X_DOWN) ZXKey(map_keys[3]);
	     if(old_key & GP2X_A) ZXKey(map_keys[4]);
		 break;
	 
	 }
	 if(old_key & GP2X_B) ZXKey(map_keys[5]);
	 if(old_key & GP2X_X) ZXKey(map_keys[6]);
	 if(old_key & GP2X_Y) ZXKey(map_keys[7]);
	 }
if(old_key & GP2X_L) ZXKey(map_keys[8]);
if(old_key & GP2X_R) ZXKey(map_keys[9]);

if(keyboard_on)
	{
	sel_key=display_keyboard();
	flush_uppermem_cache(gp2x_screen, (char *)gp2x_screen + 320*240, 0);
	if((old_key & (GP2X_Y))&&(unprogram==0))
		{
		ZXKey(sel_key);keyboard_on=0;keyboard_press=4;
		}
	}
else	{
	if (keyboard_press)
		{
		ZXKey(sel_key);
		keyboard_press--;
		}
	}
if(unprogram)
	{
	unprogram--;
	}

if (usbkeyboard_found==1)
        {
        usbkeyboard_found=readUSBkeyboard();

        for(i=0;i<58;i++)
                {
                if (keyboardmap[usbkey_keymap[(i*2)+1]]==1)
                        {
                        ZXKey(usbkey_keymap[i*2]);
                        }
                }
        }
	
if(skip==0)
	{
	if ((f200==0)&&(battery_icon==1))
		{
		if(full_screen)
			{
			if (volt<=24)
			battery_box(236,4,255,volt);
			}
		else 	battery_box(300,4,255,volt);
		}
	  
    	if(mconfig.wait_vsync)
           gp2x_waitvsync();
    	gp2x_flip();
        
      //Seleuco: Esta temporización solamente se tiene que hacer si no se reproduce Audio. Para evitar underuns dejar que temporice el audio si existe sonido.   
      if(mconfig.sound_mode==0 && !(mconfig.speed_loading && tape_playing))
      	{
     	do	{
    		time=gp2x_timer_poll();
		}
	while((time-oldtime)<delayvalue); // asegura la temporizacion
    		oldtime=time;    	
      	}
	}
//if(!keyboard_on)
	 //skip^=mconfig.frameskip;
//else skip=0;
    
 }
 
sound_end();
}
SetVideoScaling(320,320,240);

if (f200==0)
	{
	battery_status_end();
	}
save_mconfig();


set_speed_clock(200);
set_display_clock_div(dclock);
ClearScreen(0);
gp2x_flip();
ClearScreen(0);
gp2x_flip();

tape_finish();

gp2x_volume(70,70);
gp2x_close_sound();

load_system_regs();

gp2x_end();
return(0);
}
