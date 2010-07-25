/*  gp2xpectrum  for GP2X/Wiz/Linux/Iphone

    Copyright (C) 2010 Seleuco
    Copyright (C) 2006-2009 Metalbrain, Seleuco, SplinterGU
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
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

//#define DEBUG_MSG
//#define USE_ZIP
//#define CAPTURE

#ifdef USE_ZIP
#include "zip.h"
#endif

#ifdef USE_ZLIB
#include "minizip/unzip.h"
#endif

#include "bzip/bzlib.h"
#include "microlib.h"

//#include "zx.h"
//#include "z80.h"
//#include "fdc.h"
#include "empty_dsk.h"
#include "shared.h"

#if defined(IPHONE)
char globalpath[247]="/var/mobile/Media/ROMs/iXpectrum/";
#elif defined(ANDROID)
char globalpath[247]="/sdcard/ROMs/xpectroid/";
#else
char globalpath[247]="roms/";
#endif
char actual_roms_dir[512] = "";

long cur_frame;
int keyboardKeys[MAX_KEYS];
int numKeys = 0;
int isShiftKey;
int isSymbolKey;
int emulating = 0;
int ext_keyboard = 0;

extern Z80Regs * spectrumZ80;
extern void Sound_Loop();
extern int full_screen;

char * get_name(char *name);

extern byte  DSK[1*1024*1024];
extern t_drive driveA;
extern t_FDC FDC;
extern t_track *active_track;

extern int keyboardmap[128];

unsigned nKeys = 0;

void write_keyfile(char *name);
void read_keyfile(char *name);

char * MY_filename = "default_name.fk";
char menustring[32];
int volume = 70;
int skip = 0;
int delayvalue = 0;
int f200 = 0;

int init_speed_loading = 0;
MCONFIG mconfig;

//int delay_volume = 0;

//int auto_fire = 0;
int dsk_flipped = 0;
int load_state(int st);
int save_state(int st);
int dsk_save (char *pchFileName/*, t_drive *drive, char chID*/);
int dsk_format ();


#define DEFAULT_SPEED_NOFS 250
#define DEFAULT_SPEED_FS1 250
#define DEFAULT_SPEED_NOFSNC 250
#define DEFAULT_SPEED_FS1NC 250

struct
{
    unsigned header;
    int mJoystick;
    int map_keys[16];
    unsigned char filename[256];
    int have_fd_info;
    char ulaplus[8];
    unsigned reserved[61];
    //unsigned reserved[63];
} state_header;


extern unsigned char msx[];  // define la fuente externa usada para dibujar letras y numeros

int COLORFONDO = 128; // color de fondo (vaya noticia :P)

unsigned char mypalette_rgb[7][3] = {
#if defined(IPHONE) || defined(ANDROID)
    {0x02,0x02,0x02}, // 128 = 0 = black
    {0x00,0xb0,0x00}, // 129 = 1 = green
#else
    {0x00,0x40,0x00}, // 128 = 0 = dark green
    {0x00,0xff,0x00}, // 129 = 1 = green
#endif

    {0xff,0xff,0xff}, // 130 = 2 = white
    {0xff,0x00,0x00}, // 131 = 3 = red
    {0xff,0xff,0x00}, // 132 = 4 = yellow
    {0x00,0xff,0xff}, // 133 = 5 = cyan
    {0x00,0x00,0x00}, // 134 = 6 = black
};

void set_emupalette()
{
    int i;
    unsigned char r, g, b;

    palette_t palette;

    memset(palette,0,sizeof(palette_t));

    if(!zx_ula64_enabled)
    {
		for (i = 0; i < 17; i++)
		{
			palette[i].r = r = zx_colours[i][0];
			palette[i].g = g = zx_colours[i][1];
			palette[i].b = b = zx_colours[i][2];

			palette[i+18].r = r >> 1;
			palette[i+18].g = (g >> 2) | 64;
			palette[i+18].b = b >> 1;

		}
    }
    else
    {
		for (i = 0; i < 64; i++)
		{

	    	byte r = (zx_ula64_palette[i] & 0x1C ) >> 2;
	    	byte g = (zx_ula64_palette[i] & 0xE0 ) >> 5;
	    	byte b = ((zx_ula64_palette[i] & 0x03 ) << 1 ) | (zx_ula64_palette[i] & 0x01 );

	    	r = ( r << 5 )+( r <<2 )+( r & 0x03 );
	    	g = ( g << 5 )+( g <<2 )+( g & 0x03 );
	    	b = ( b << 5 )+( b <<2 )+( b & 0x03 );

	        palette[i].r =  r;
	        palette[i].g =  g;
	        palette[i].b =  b;

			palette[i+64].r = r >> 1;
			palette[i+64].g = (g >> 2) | 64;
			palette[i+64].b = b >> 1;
		}
    }


    for (i = 0; i < 7; i++)
    {
        palette[128+i].r = mypalette_rgb[i][0];
        palette[128+i].g = mypalette_rgb[i][1];
        palette[128+i].b = mypalette_rgb[i][2];
    }

    palette[252].r = 0;
    palette[252].g = 255;
    palette[252].b = 0;

    palette[253].r = 255;
    palette[253].g = 0;
    palette[253].b = 0;

    palette[254].r = 255;
    palette[254].g = 255;
    palette[254].b = 0;

    palette[255].r = 255;
    palette[255].g = 255;
    palette[255].b = 255;


    set_palette(palette);
}

void ClearScreen(unsigned char c) // se usa para 'borrar' la pantalla virtual con un color
{
    memset(video_screen8, c, 320*240);
}

/*********************************************************************/
/* GP2X System                                                       */
/*********************************************************************/
inline int min(int a, int b) {
  return a < b ? a : b;
}

//int k =0;
void SyncFreq()
{
    unsigned time;
    static int oldtime = 0;

/*
    time = getTicks();
    unsigned cur = time -oldtime;
    if(cur < delayvalue)
    {
    	int toWait = delayvalue - (time - oldtime);
    	if(toWait > 0 && toWait < 500)
             usleep(toWait *1000);//10-6 //menos energia, bueno para el thread

    }
*/

    do
    {
    	time = getTicks();
    	usleep(100);
    }
    while ((time - oldtime) < delayvalue); // asegura la temporizacion

    oldtime = time;

//    oldtime = getTicks();
}

void SyncFreq2()
{
    unsigned timet;
    static int oldtimet = 0;

    do
    {
    	timet = getTicks();
    	usleep(100);
    }
    while ((timet - oldtimet) < /*5*/10); // asegura la temporizacion

    oldtimet = timet;
}

/*********************************************************************/

int volt = 27; // initialize

/*********************************************************************/
/* Save/Load configuration of gp2xpectrum                             */
/*********************************************************************/

void load_mconfig()
{
    FILE *fp;

    char pathstring[512];
    sprintf(pathstring,"%s/saves/_xpectrum.cfg",globalpath);

    fp = fopen(pathstring,"rb");

    int read = 0;
    if (fp != NULL)
    {
        fread(&mconfig, 1, sizeof(mconfig), fp);
        fclose(fp);
        read = 1;
    }
    if (mconfig.id != 0xABCD0019 || !read)
    {
        mconfig.id = 0xABCD0019;
#if defined(IPHONE)
        mconfig.zx_screen_mode = 0;
        mconfig.frameskip = 1;
        //mconfig.frameskip = 2;
#elif defined(ANDROID)
        mconfig.zx_screen_mode = 0;
        mconfig.frameskip = 2;
#else
        mconfig.zx_screen_mode = 0;
        mconfig.frameskip = 0;
#endif
        mconfig.contention = 1;
        mconfig.sound_volume = 70;
        mconfig.sound_gain = 2;//high
        mconfig.sound_mode = 4;//Mono
        mconfig.sound_freq = 44100;//44100
        mconfig.speed_mode = 100;//100% emulation
        mconfig.wait_vsync = 0;//no vsync
        mconfig.show_fps = 0;
        mconfig.speed_loading = 1;
        mconfig.flash_loading = 1;
        mconfig.edge_loading = 1;
        mconfig.auto_loading = 1;
        mconfig.cpu_freq = 250;
        mconfig.ula64 = 1;
    }
    volume = mconfig.sound_volume;

    int factor = (20 * 100 ) / mconfig.speed_mode;//(20 * 100 ) / mconfig.speed_mode;
    delayvalue =  factor +(mconfig.frameskip * factor);
}

void save_mconfig()
{
    FILE *fp;
	char pathstring[512];

    sprintf(pathstring,"%s/saves/_xpectrum.cfg",globalpath);

    mconfig.sound_volume = volume;
    fp = fopen(pathstring,"wb");
    if (fp == NULL) return;
    fwrite(&mconfig, 1, sizeof(mconfig), fp);
    fclose(fp);
    sync();
}


/****************************************************************************************************************************************/
// PANTALLA
/****************************************************************************************************************************************/

void speccy_corner()
{
#if !defined(IPHONE) && !defined(ANDROID)
	int x1 = 320-1,y1 = 240-64,x;
    int n,m;
    unsigned char *p;
    for(n = 0;n<32;n++)
    {
        if (y1>= 240) break;
        p = &video_screen8[x1+y1*320];
        x = x1;
        for (m = 0;m<6;m++,x++) {if (x<320) {p[320] = 131;*p++ = 131;}} // rojo
        for (m = 0;m<6;m++,x++) {if (x<320) {p[320] = 132;*p++ = 132;}} // amarillo
        for (m = 0;m<6;m++,x++) {if (x<320) {p[320] = 129;*p++ = 129;}} // verde
        for (m = 0;m<6;m++,x++) {if (x<320) {p[320] = 133;*p++ = 133;}} // cyan
        y1 += 2;
        x1--;
    }
#endif
}

void __v_putchar( unsigned x, unsigned y, int color, unsigned char ch) // rutina usada para dibujar caracteres (coordenadas de 8x8)
{
    volatile int     i,j;
    volatile unsigned char   *font;
    volatile unsigned char col;
    volatile int col2;
    volatile unsigned char *p;
//    if (x>= 40 || y>= 30) return;

    col = color;
    col2 = COLORFONDO;

    font = &msx[ (int)ch * 8];
    p = &video_screen8[y*320+x];
    if (col != col2 && col2 != -1 )
    {
        for (i = 0; i < 8; i++, font++)
        {
            for (j = 0; j < 8; j++)
            {
                if ((*font & (128 >> j))) p[j] = col;
                else p[j] = col2;
            }
            p += 320;
        }
    }
    else
    {
        for (i = 0; i < 8; i++, font++)
        {
            for (j = 0; j < 8; j++)
            {
                if ((*font & (128 >> j))) p[j] = col;
            }
            p += 320;
        }
    }
}

void v_putchar( unsigned x, unsigned y, int color, unsigned char ch) // rutina usada para dibujar caracteres (coordenadas de 8x8)
{
//    int bg = COLORFONDO;
    int rx, ry;
    if (x>= 40 || y>= 30) return;

    rx = x<<3;
    ry = y*8;
/*
    __v_putchar(rx-1,ry-1,0,ch);
    COLORFONDO = -1;
    __v_putchar(rx  ,ry-1,0,ch);
    __v_putchar(rx+1,ry-1,0,ch);

    __v_putchar(rx+1,ry  ,0,ch);
    __v_putchar(rx-1,ry  ,0,ch);

    __v_putchar(rx-1,ry+1,0,ch);
    __v_putchar(rx  ,ry+1,0,ch);
    __v_putchar(rx+1,ry+1,0,ch);
*/
    __v_putchar( rx, ry, color, ch); // rutina usada para dibujar caracteres (coordenadas de 8x8)
//    COLORFONDO = bg;
}

// display array of chars

int v_breakcad = 40;
int v_forcebreakcad = 0;
int lastx = 0;
void v_putcad(int x,int y,int color,char *cad)  // dibuja una cadena de texto
{
    int n = 0;
    while (cad[0] != 0)
    {
        if (n == v_breakcad) break;
        v_putchar(x,y,color,cad[0]);
        cad++;
        x++;
        n++;
    }
    if (v_forcebreakcad)
    {
        while(1)
        {
            if (x >= 40 || n == v_breakcad) break;
            v_putchar(x,y,color,' ');
            cad++;
            x++;
            n++;
        }
    }
    lastx = x;
}

unsigned char mask_keyb[32*8*11*8];

void mask_putchar( unsigned x, unsigned y, unsigned char color, unsigned char ch) // rutina usada para dibujar caracteres (coordenadas de 8x8)
{
    volatile  int     i,j,v;
    volatile unsigned char   *font;
    unsigned char col,col2;
    volatile unsigned char *p;
    if (x >= 36 || y >= 20) return;
    x -= 4;
    y -= 9;

    col = color;
    col2 = COLORFONDO;

    font = &msx[ (int)ch * 8];
    p = &mask_keyb[(y*32*8*8)+(x<<3)];
    if (col != col2)
    {
        for (i = 0; i < 8; i++, font++)
        {
            for (j = 0; j < 8; j++)
            {
                if ((*font & (128 >> j)))  p[j] = col;
                else p[j] = col2;
            }
            p += 32*8;
        }
    }
    else
    {
        for (i = 0; i < 8; i++, font++)
        {
            for (j = 0; j < 8; j++)
            {
                if ((*font & (128 >> j)))  p[j] = col;
            }
            p += 32*8;
        }
    }
}


void mask_out(int  x,int y)
{
    int n, m;
    volatile unsigned char *p,*p2,v;
    for(n = 0;n<11*8;n++)
    {
        p = &video_screen8[(y+n)*320+x];
        p2 = &mask_keyb[n*32*8];
        for(m = 0;m<32*8;m++)
        {
            if (*p2 == 0)
            {
                v = *p;

                if (zx_ula64_enabled)
                {
                	if(v<63)v += 64;
                }
                else
                {
                    if(v<17)v += 18;
                }
                *p++ = v;
                p2++;
            }
            else *p++ = *p2++;
        }
    }
}


int mask_chars = 0;

void mask_putcad(int x,int y,unsigned color,char *cad)  // dibuja una cadena de texto
{
    int n = 0;
    while (cad[0] != 0)
    {
        if (n == v_breakcad) break;
        if (mask_chars) mask_putchar(x,y,color,' ');
        else
            mask_putchar(x,y,color,cad[0]);
        cad++;
        x++;
        n++;
    }
    if (v_forcebreakcad)
    {
        while(1)
        {
            if (x>= 40 || n == v_breakcad) break;
            mask_putchar(x,y,color,' ');
            cad++;
            x++;
            n++;
        }
    }
    lastx = x;
}

/*********************************************************************/
//  saving bitmap (photo mode)
/*********************************************************************/

int make_photo = 0; // variable asignada cuando queremos hacer una foto

int photo_number = 0;  // contador de numero de foto
char photo_name[512]; // nombre del fichero

typedef struct
{
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

#ifdef  CAPTURE
void save_bmp(char *name,unsigned char *punt, int ancho, int alto, int align)
{
    FILE *fp;
    int i,n;

    printf("grabando imagen\n");
    fp = fopen(name,"wb");
    if (fp == NULL) return;
    fputc('B',fp);
    fputc('M',fp);
    bmph.reserved = 0;
    bmph.size = 54+ancho*alto+256*4;
    bmph.offset = 256*4+54;
    bmph.header_size = 40;
    bmph.width = ancho;
    bmph.height = alto;
    bmph.planes = 1;
    bmph.bpp = 8;
    bmph.compression = 0;
    bmph.image_size = ancho*alto;
    bmph.ppm[0] = bmph.ppm[1] = 0;
    bmph.color_info[0] = 0; // 0 es 256
    bmph.color_info[1] = 0;
    fwrite(&bmph, 1, 52, fp);
    prgba.a = 0;
    for(i = 0;i<256;i++)
    {
        if (i>=128 && i<=128+7)
        {
            prgba.r = mypalette_rgb[i+128][0];
            prgba.g = mypalette_rgb[i+128][1];
            prgba.b = mypalette_rgb[i+128][2];
        }
        else
            if (i<17)
            {
                prgba.r = zx_colours[i][0];
                prgba.g = zx_colours[i][1];
                prgba.b = zx_colours[i][2];
            }
            else
                if (i<34)
                {
                    n = i-18;
                    prgba.r = zx_colours[n][0]>>1;
                    prgba.g = (zx_colours[n][1]>>2) | 64;
                    prgba.b = zx_colours[n][2]>>1;
                }
                else
                    if (i == 252)
                    {
                        prgba.r = 0;
                        prgba.g = 255;
                        prgba.b = 0;
                    }
                    else
                        if (i == 253)
                        {
                            prgba.r = 255;
                            prgba.g = 0;
                            prgba.b = 0;
                        }
                        else
                            if (i == 254)
                            {
                                prgba.r = 255;
                                prgba.g = 255;
                                prgba.b = 0;
                            }
                            else
                                if (i == 255)
                                {
                                    prgba.r = 255;
                                    prgba.g = 255;
                                    prgba.b = 255;
                                }
                                else
                                {
                                    prgba.r = ((i&0x1c)<<3);
                                    prgba.g = (((i&0xe0)>>2)<<2);
                                    prgba.b = (((i&3)<<6));
                                }
        fwrite(&prgba, 1, 4, fp);
    }

    for(i = 1;i<= alto;i++)
    {
        fwrite(punt+(alto-i)*align, 1, ancho, fp);
    }
    fclose(fp);
    sync();
}
#endif

/***************************************************************************/
// seleccion de roms y otras pantallas del emu
/***************************************************************************/


#define MAX_ENTRY 16384

struct files
{
    int is_directory;
    char file[512];
} files[MAX_ENTRY];

//#ifdef IPHONE
//char actual_roms_dir[512] = "/var/mobile/Media/ROMs/ISpectrum/";
//#else
//char actual_roms_dir[257] = "roms/";
//#endif

int nfiles = 0;
int ndirs = 0;

void add_dir(char * dir)
{
    strcat(actual_roms_dir, dir);
    strcat(actual_roms_dir, "/");
}

void goto_parent_dir()
{
    char * p, *p1;
    if ( ( p1 = strrchr(actual_roms_dir, '/') ) )
    {
        *p1 = '\0';
        if ( ( p = strrchr(actual_roms_dir, '/') ) )
            *(++p) = '\0';
        else
            *p1 = '/';
    }
}

int is_ext(const char *a,char *b) // compara extensiones
{
    int n,m;
    m = 0;n = 0;

    while(a[n] != 0)
    {
        if (a[n] == '.') m = n;
        n++;
    }
    n = 0;
    while(b[n] != 0)
    {
        if (toupper(a[m]) != toupper(b[n])) return 0;
        m++; n++;
    }
    if (a[m] == 0) return 1;
    return 0;
}


int compare(const struct files *a, const struct files *b)
{
    int r;
    if ( !(r = ( b->is_directory - a->is_directory )))
        return (strcasecmp(a->file,b->file));
    return (r);
}

void read_list_rom()
{
    DIR *fd;
    static struct dirent *direntp;
    static char swap[257], sw;
    int aa;

    int n,m,f;
    ndirs = 0;
    nfiles = 0;

    fd = opendir(actual_roms_dir);
    if (fd  !=  NULL)
    {
        while(nfiles < MAX_ENTRY)
        {
            files[nfiles].is_directory = 0;
            direntp = readdir(fd);
            if (direntp == NULL) break;
            if (direntp->d_name[0] ==  '.' && direntp->d_name[1] == 0) continue;

            sprintf(files[nfiles].file, "%s%s", actual_roms_dir, direntp->d_name);

            if ( direntp->d_type & DT_DIR)
            {
                if (!strcmp(direntp->d_name,"..") && !strcmp(actual_roms_dir,/*"roms/*/globalpath))//TODO FIX!
                    continue;
                else
                {
                    ndirs++;
                    files[nfiles].is_directory = 1;
                }
            }
            else
                if (!(is_ext(files[nfiles].file,".trd") ||
                      is_ext(files[nfiles].file,".z80") ||
                      is_ext(files[nfiles].file,".sna") ||
                      is_ext(files[nfiles].file,".tzx") ||
                      is_ext(files[nfiles].file,".tap") ||
                      is_ext(files[nfiles].file,".sp")  ||
                      is_ext(files[nfiles].file,".dsk") ||
                      is_ext(files[nfiles].file,".sav") ||
                      is_ext(files[nfiles].file,".bz2")
#if defined( USE_ZIP) || defined( USE_ZLIB)
                     ||
                     is_ext(files[nfiles].file,".zip")
#endif
                    ) ) continue;
                    nfiles++;
        }

        closedir(fd);
    }

    qsort(files, nfiles, sizeof(files[0]), &compare);

/*
    for(n = 0;n<(nfiles);n++) //ordena
        for(m = n+1;m<nfiles;m++)
        {
            f = 0;
            if (is_directory[m] == 1 && is_directory[n] == 0) f = 1;
            if (f == 0) if (is_directory[m] == is_directory[n]) if (strcasecmp(files[m],files[n])<0) f = 1;
            if (f)
            {
                f = 0;
                sw = is_directory[m];
                is_directory[m] = is_directory[n];
                is_directory[n] = sw;
                memcpy(swap,files[m],256);
                memcpy(files[m],files[n],256);
                memcpy(files[n],swap,256);
            }
        }
*/
}

char * get_name_short(char *name, int sz) // devuelve el nombre del fichero recortado a 38 caracteres
{
    static  char name2[39];
    int n, m;

    m = 0;
    n = -1;

    while(name[m] != 0) { if (name[m] == '/') n = m; m++; }
    n++;
    char *s = name+n;
    int len = strlen(s);

    if (len>= /*38*/sz)
    {
        memcpy(name2,s,/*27*/sz-11);
        name2[/*27*/sz-11] = '|';
        memcpy(name2+sz-10,s+len-10,10);
        name2[sz+1] = 0;
    }
    else
        strcpy(name2,s);

    return (char*)name2;
}

byte  scrbuff[2*16*16384 * 2]; //TODO He aumentado el buffer * 2 ver porque petaba antes los z80 al cargarlos
char last_rom_name[512];

int load_scr(char *name, char * buffer, int size)
{
    FILE *fp;

    if(strcmp(name,last_rom_name)==0)return 0;
    strcpy(last_rom_name,name);

	//if(zx_ula64_enabled !=0 && (mconfig.ula64 == 1 || mconfig.ula64 == 0))
    if(zx_ula64_enabled !=0 && (mconfig.ula64 != 2))
	{
	    zx_ula64_enabled = 0;
	    set_emupalette();
	}

    fp = fopen(name,"rb");
    if (fp == NULL) return 1;

#if defined( USE_ZIP) || defined( USE_ZLIB)
    if (is_ext (name, ".zip"))
    {
        zip_load(name); return 0;
    }
#endif
    if (is_ext (name, ".bz2"))
    {
        BZFILE * my_bzip;
        int bzip_err = 0;

        my_bzip = BZ2_bzReadOpen( &bzip_err, fp, 0, 1, NULL, 0 );
        BZ2_bzRead (&bzip_err, my_bzip, buffer, size);
        BZ2_bzReadClose (&bzip_err, my_bzip);
        fclose(fp);
        return 0;
    }
    fread(buffer, 1, size, fp);
    fclose(fp);
    return 0;
}



int load_savsrc(char *name, char * buffer, int size)
{
    FILE *fp;
    byte ula64colors[64];

    if(strcmp(name,last_rom_name)==0)return 0;
    strcpy(last_rom_name,name);

    fp = fopen(name,"rb");
    if (fp == NULL) return 1;

    BZFILE * my_bzip;
    int bzip_err = 0;


    my_bzip = BZ2_bzReadOpen( &bzip_err, fp, 0, 1, NULL, 0 );

    BZ2_bzRead (&bzip_err, my_bzip, (void *)&state_header, sizeof(state_header));

    if (state_header.header != 0x12345678) {BZ2_bzReadClose (&bzip_err, my_bzip);fclose(fp);return 0;}

    if(state_header.ulaplus[0]=='u' && state_header.ulaplus[1]=='l' && state_header.ulaplus[2]=='a'
       && state_header.ulaplus[3]=='6' && state_header.ulaplus[4]=='4' )

    {
    	BZ2_bzRead (&bzip_err, my_bzip, (void *)&ula64colors, 64);
    	if(mconfig.ula64 != 2)
    	{
   	      zx_ula64_enabled = 1;
   	      memcpy(zx_ula64_palette,ula64colors,64);
   	      set_emupalette();
    	}
    	/*
    	if(mconfig.ula64 == 1)
    	{
   	      zx_ula64_enabled = 1;
   	      memcpy(zx_ula64_palette,ula64colors,64);
   	      set_emupalette();
    	}
    	else if(mconfig.ula64 == 0)
    	{
     	   zx_ula64_enabled = 0;
     	   set_emupalette();
    	}
    	*/
    }
    else
    {
    	//if(zx_ula64_enabled !=0 && (mconfig.ula64 == 1 || mconfig.ula64 == 0))
    	if(zx_ula64_enabled !=0 && (mconfig.ula64 !=2))
    	{
   	      zx_ula64_enabled = 0;
   	      set_emupalette();
    	}
    }

    if (state_header.have_fd_info == 1)
    {
    	byte tmp[4+4+4+4+ sizeof (t_FDC)+sizeof (t_track)];
    	BZ2_bzRead (&bzip_err, my_bzip, (void *) &tmp,sizeof(tmp));
    }

    BZ2_bzRead (&bzip_err, my_bzip, buffer, size);
    BZ2_bzReadClose (&bzip_err, my_bzip);
    fclose(fp);
    return 0;

}


char * get_name(char *name) // devuelve el nombre del fichero completo
{
    static unsigned char name2[256];
    int n,m;

    m = 0;
    n = -1;

    while(name[m] != 0) { if (name[m] == '/') n = m; m++; }
    n++;
    for(m = 0;m<255;m++)
    {
        name2[m] = name[n];
        if (name[n] == 0) break;
        n++;
    }
    name2[m] = 0;
    return (char*)name2;
}

int scale = 1;

int get_rom(int tape)
{
    static int init = 1;
    static int posfile = 0;
    int downloads_dir = 1;
    /*unsigned*/ char cad[256];
    int n, m, y, f;
    static int bright = 1;
    unsigned new_key = 0,old_key = 0;

    read_list_rom();
    if (nfiles) init = 0;
    else init = 1;

    while(nKeys & (JOY_BUTTON_MENU | JOY_BUTTON_X |  JOY_BUTTON_A |  JOY_BUTTON_B)) nKeys = joystick_read(); // para quieto!!

    if (posfile>= nfiles) posfile = 0;
    COLORFONDO = 128;

    f = 0;
    while(1)
    {
        ClearScreen(COLORFONDO); //speccy_corner();

        //////////////
        for(m=0;m<24;n++)
        {
            COLORFONDO = 134;
            v_breakcad = 38;
            v_forcebreakcad = 1;
            v_putcad(1,m+3,130," ");
            v_breakcad = 40;v_forcebreakcad = 0;
            COLORFONDO = 128;
            m++;
        }

        /*speccy_corner();*/
        downloads_dir = strcasecmp(actual_roms_dir+strlen(actual_roms_dir)-10,"downloads/") == 0 ? 1 : 0;

        if(downloads_dir)
          sprintf(cad,"DOWNLOADS (%u) (Use A to refresh)",nfiles-ndirs);
        else
          sprintf(cad,"PROGRAM LIST (%u)",nfiles-ndirs);


        v_putcad(1,1,132,cad);

        /* Show file screen */

        if (!files[posfile].is_directory)
        {
            int l  = strlen(files[posfile].file);


            if ( !strcasecmp(files[posfile].file+l-4,".sna") || !strcasecmp(files[posfile].file+l-8,".sna.bz2") )
            {
                if (!load_scr(files[posfile].file, scrbuff, sizeof(scrbuff)))
                {
                    DrawZXtoScreen(video_screen8, &scrbuff[27], scale, 2);
                }
            }
            else if ( !strcasecmp(files[posfile].file+l-4,".sav") )
            {

            	if (!load_savsrc(files[posfile].file, scrbuff, sizeof(scrbuff)))
                {

             		byte nPages = scrbuff[6+sizeof(Z80Regs)];

             		if(nPages==3 || nPages==1)
            		   DrawZXtoScreen(video_screen8, &scrbuff[7+sizeof(Z80Regs)], scale, 2);
             		else if(nPages==8)
             		   DrawZXtoScreen(video_screen8, &scrbuff[7+sizeof(Z80Regs)+  (0x4000 * 5)], scale, 2);

                }
             }
            if ( !strcasecmp(files[posfile].file+l-3,".sp") || !strcasecmp(files[posfile].file+l-7,".sp.bz2") )
            {
                if (!load_scr(files[posfile].file, scrbuff, sizeof(scrbuff)))
                {
                   DrawZXtoScreen(video_screen8, &scrbuff[38], scale, 2);
                }
            }
            else
            if ( !strcasecmp(files[posfile].file+l-4,".z80") || !strcasecmp(files[posfile].file+l-8,".z80.bz2") )
            {
                if (!load_scr(files[posfile].file, scrbuff, sizeof(scrbuff)))
                {
                    if ((scrbuff[6] != 0)||(scrbuff[7] != 0))
                    {
                        UncompressZ80 (&scrbuff[512*1024], (scrbuff[12] & 0x20) ? 1 /*Z80BL_V1COMPRE*/ : 0/*Z80BL_V1UNCOMP*/, 0, NULL, &scrbuff[30]);
                        DrawZXtoScreen(video_screen8, &scrbuff[512*1024], scale, 2);
                    }
                    else
                    {
                        int f, tam, sig, ver = 0, scr_page = 0, hdr_sz;
                        byte pag;
                        byte *source=(byte *)scrbuff;

                        hdr_sz = source[30] + source[31] * 256 ;
                        switch(hdr_sz)
                        {
                            case    23:  /* Version 2 */
                                    switch(source[34])
                                    {
                                        case    0:
                                        case    1:
                                        case    2:
                                                scr_page = 8;
                                                break;

                                        case    3:
                                        case    4:
                                                scr_page = 8;
                                                break;
                                    }
                                    ver = 2;
                                    break;

                            case    54: /* Version 3 */
                            case    55:
                                    switch(source[34])
                                    {
                                        case    0:
                                        case    1:
                                        case    2:
                                                scr_page = 8;
                                                break;

                                        case    3:
                                                if ( source[37] & 0x80 )
                                                    scr_page = 8;
                                                else
                                                    scr_page = 5;
                                                break;

                                        case    4:
                                        case    5:
                                        case    6:
                                                scr_page = 8;
                                                break;
                                    }
                                    ver = 3;
                                    break;
                        }

                        sig = 30 + 2 + (hdr_sz);

                        for (f = 0; f < 16 ; f++) //up 16 pages (ZS Scorpion)
                        {
                            source=&scrbuff[sig];
                            tam = *(source++) ;
                            tam += (*(source++)) * 256 ;
                            pag = *(source++);
                            if (tam==65535)
                                sig += ( 3 + 16384 );
                            else
                                sig += ( 3 + tam );

                            if (pag == scr_page)
                            {
                                UncompressZ80(&scrbuff[512*1024], (tam == 0xffff ? 3/*Z80BL_V2UNCOMP*/ : 4/*Z80BL_V2COMPRE*/), tam, NULL, source);
                                break;
                            }
                        }
                        DrawZXtoScreen(video_screen8, &scrbuff[512*1024], scale, 2);
                    }
                }
            }
        }

        m = 0;
        for(n = posfile-10; n<posfile+24; n++)
        {
            if (n<0) continue;
            if (n>= nfiles) break;
            if (m>23) break;
            if (n == posfile) COLORFONDO = 129; else COLORFONDO = -1 /*6*/;

            v_breakcad = 38;
            v_forcebreakcad = 1;

            if (files[n].is_directory)
            {
                sprintf(cad,"<%s>",get_name_short(files[n].file,v_breakcad));
                v_putcad(1,m+3,133,cad);
            }
            else
            {
                v_putcad(1,m+3,130,get_name_short(files[n].file,v_breakcad));
            }

            v_breakcad = 40;
            v_forcebreakcad = 0;
            COLORFONDO = 128;
            m++;
        }
/*
        //////////////
        for(;m<24;n++)
        {
            COLORFONDO = -1;
            v_breakcad = 38;
            v_forcebreakcad = 1;
            v_putcad(1,m+3,130," ");
            v_breakcad = 40;v_forcebreakcad = 0;
            COLORFONDO = 128;
            m++;
        }
*/
#if  defined(IPHONE) || defined(ANDROID)
        if(downloads_dir)
        	v_putcad(1,28,132,"Use X to Exit, B to Play, Y to delete");
        else
            v_putcad(1,28,132,"Use X to Exit, Use B to Play");
#else
        v_putcad(1,28,132,"Use X to Exit, B to Play, Y to delete");
#endif

        SyncFreq2();
        dump_video();
       // usleep(10000);

        nKeys = joystick_read();

        new_key = nKeys & (~old_key);
        old_key = nKeys;
        if (!(old_key  & ( JOY_BUTTON_UP | JOY_BUTTON_DOWN ))) f = 0;
        if (old_key  & JOY_BUTTON_UP)
        {
            if (f == 0) f = 2;
            else if (f & 1) f = 2;
            else {f += 2;if (f>40) {f = 34; new_key |= JOY_BUTTON_UP;}}
        }
        if (old_key & JOY_BUTTON_DOWN)
        {
            if (f == 0) f = 1;
            else if (!(f & 1)) f = 1;
            else {f += 2;if (f>41) {f = 35; new_key |= JOY_BUTTON_DOWN;}}
        }

        if (nfiles>0)
        {
            if (new_key & JOY_BUTTON_VOLDOWN)
            {
                if ( scale < 3 ) scale++;
            }
            if (new_key & JOY_BUTTON_VOLUP)
            {
                if ( scale > 0 ) scale--;
            }
            if (new_key & JOY_BUTTON_B)
            {
                if (!files[posfile].is_directory)
                {
                	last_rom_name[0]=0;
                	return posfile;
                }
                else
                {
                    char * dir = get_name(files[posfile].file);
                    if ( strcmp( dir, ".." ) )
                        add_dir(dir);
                    else
                        goto_parent_dir();

                    read_list_rom();
                    if (nfiles) init = 0;
                    else init = 1;
                    posfile = 0;
                }
            }
            if ( new_key & JOY_BUTTON_Y )
            {
#if  defined(IPHONE) || defined(ANDROID)
            	if(!downloads_dir)
            	   continue;
#endif
            	char fname[257];
                ClearScreen(COLORFONDO);

                COLORFONDO = 128+3;
                v_putcad((40-34)>>1,10,132,"You really want delete this file ?");
                COLORFONDO = 128;
                sprintf( fname, "%s%s", actual_roms_dir, get_name(files[posfile].file));
                v_putcad((40-strlen(fname))>>1,12,133,fname);
                v_putcad((40-19)>>1,14,130,"Press B Yes or X No");

                dump_video();

                while(!(nKeys & (JOY_BUTTON_X|JOY_BUTTON_B))) nKeys = joystick_read(); // para quieto!!
                if ( nKeys & JOY_BUTTON_B ) remove(fname);
                while(nKeys & (JOY_BUTTON_X|JOY_BUTTON_B)) nKeys = joystick_read(); // para quieto!!

                read_list_rom();
                if (nfiles) init = 0;
                else init = 1;
                if ( posfile >= nfiles ) posfile = nfiles - 1;
            }
        }


        if ( downloads_dir && new_key & JOY_BUTTON_A )
        {
        	char fname[257];
            ClearScreen(COLORFONDO);

            COLORFONDO = 128+3;
            v_putcad((40-18)>>1,10,132,"Refreshing files");
            COLORFONDO = 128;
            v_putcad((40-19)>>1,14,130,"Press B to continue");

            dump_video();

            while(!(nKeys & (JOY_BUTTON_B))) nKeys = joystick_read();
            while(nKeys & (JOY_BUTTON_B)) nKeys = joystick_read(); // para quieto!!

            read_list_rom();
            if (nfiles) init = 0;
            else init = 1;
            if ( posfile >= nfiles ) posfile = nfiles - 1;
        }

        if (new_key & JOY_BUTTON_MENU && !tape)
        {
#if defined(IPHONE) || defined(ANDROID)
            	continue;
#endif
			ClearScreen(COLORFONDO);

            COLORFONDO = 128+3;
            v_putcad((40-35)>>1,10,132,"You really want compress all files?");
            COLORFONDO = 128;
            v_putcad((40-19)>>1,14,130,"Press B Yes or X No");

            dump_video();

            while(!(nKeys & (JOY_BUTTON_X|JOY_BUTTON_B))) nKeys = joystick_read(); // para quieto!!


            if ( nKeys & JOY_BUTTON_B )
            {
                for(n = 0; n<nfiles; n++)
                {
                    ClearScreen(COLORFONDO);

                    v_putcad((40-17)>>1,10,132,"COMPRESSING FILES");
                    sprintf(cad,"%% %u",(n*100)/nfiles);
                    v_putcad(18,12,133,cad);
                    v_putcad((40-17)>>1,14,130,"Press X to Cancel");

                    dump_video();

                    if (!files[n].is_directory)
                        if (is_ext(files[n].file,".z80") ||
                            is_ext(files[n].file,".sna") ||
                            is_ext(files[n].file,".tzx") ||
                            is_ext(files[n].file,".tap") ||
                            is_ext(files[n].file,".sp")  ||
                            is_ext(files[n].file,".dsk")
                           )
                            if (compress_rom(files[n].file)) break;
                    nKeys = joystick_read();

                    if (nKeys  & JOY_BUTTON_X) break;
                }
            }
            while(nKeys & (JOY_BUTTON_X|JOY_BUTTON_B)) nKeys = joystick_read(); // para quieto!!
            read_list_rom();
            if (nfiles) init = 0;
            else init = 1;
/*            posfile = 0; */
        }

        if ((new_key & JOY_BUTTON_L) ) { posfile = 0;}
        if ((new_key & JOY_BUTTON_R) ) { posfile = nfiles - 1; }
#if !defined(IPHONE) && !defined(ANDROID)

        if ((new_key & JOY_BUTTON_LEFT )) { posfile -= 24; if ( posfile < 0       ) posfile = 0;          }
        if ((new_key & JOY_BUTTON_RIGHT)) { posfile += 24; if ( posfile >= nfiles ) posfile = nfiles - 1; }
#endif

        //if ((new_key & JOY_BUTTON_L) || (new_key & JOY_BUTTON_LEFT )) { posfile -= 24; if ( posfile < 0       ) posfile = 0;          }
        //if ((new_key & JOY_BUTTON_R) || (new_key & JOY_BUTTON_RIGHT)) { posfile += 24; if ( posfile >= nfiles ) posfile = nfiles - 1; }

        if ((new_key & JOY_BUTTON_UP   )) { posfile--; if ( posfile < 0       ) posfile = nfiles - 1; }
        if ((new_key & JOY_BUTTON_DOWN )) { posfile++; if ( posfile >= nfiles ) posfile = 0;          }

        if (new_key & JOY_BUTTON_X)
        {
        	last_rom_name[0]=0;
        	return -1;
        }

    }

    return -1;
}

/****************************************************************************************************************/
// credits
/****************************************************************************************************************/

void credits()
{
    int y;
    unsigned  old  = getTicks();
    unsigned  time = old;
    //    set_mypalette();
    COLORFONDO = 128;

    ClearScreen(COLORFONDO);

    speccy_corner();


#if defined(IPHONE)
    y = 3;
    v_putcad((40-38)/2,y,130,"iXpectrum v1.3 by D.Valdeita (Seleuco)");y += 2;
    v_putcad((40-33)/2,y,130,"Using some iPhone code from ZodTTD");y += 2;
    v_putcad((40-32)/2,y,130,"iPad support & test by Ryosaebaa");y += 3;

    v_putcad((40-35)/2,y,132,"Based on GP2XPectrum 1.9.2 work of:");y += 2;
    v_putcad((40-17)/2,y,132,"Hermes/PS2Reality");y += 2;
    v_putcad((40-33)/2,y,132,"Seleuco & Metalbrain & SplinterGU");y += 2;
    v_putcad((40-14)/2,y,132,"rlyeh / fZX32");y += 2;
    v_putcad((40-27)/2,y,132,"Santiago Romero / ASpectrum");y += 2;
    v_putcad((40-33)/2,y,132,"Philip Kendall / FUSE+libspectrum");y += 2;
    v_putcad((40-18)/2,y,132,"James McKay / X128");y += 2;
    v_putcad((40-24)/2,y,132,"Ulrich Doewich / Caprice");y += 2;
    v_putcad((40-22)/2,y,132,"Sergey Bulba /  AY2SNA");y += 2;
    v_putcad((40-27)/2,y,132,"and others (thanks for all)");//y += 2;
#elif defined (ANDROID)
    y = 4;
    v_putcad((40-40)/2,y,130," Xpectroid v1.2a by D.Valdeita(Seleuco)");y += 2;
    v_putcad((40-40)/2,y,130,".....................................");y += 3;

    v_putcad((40-35)/2,y,132,"Based on GP2XPectrum 1.9.2 work of:");y += 2;
    v_putcad((40-17)/2,y,132,"Hermes/PS2Reality");y += 2;
    v_putcad((40-33)/2,y,132,"Seleuco & Metalbrain & SplinterGU");y += 2;
    v_putcad((40-14)/2,y,132,"rlyeh / fZX32");y += 2;
    v_putcad((40-27)/2,y,132,"Santiago Romero / ASpectrum");y += 2;
    v_putcad((40-33)/2,y,132,"Philip Kendall / FUSE+libspectrum");y += 2;
    v_putcad((40-18)/2,y,132,"James McKay / X128");y += 2;
    v_putcad((40-24)/2,y,132,"Ulrich Doewich / Caprice");y += 2;
    v_putcad((40-22)/2,y,132,"Sergey Bulba /  AY2SNA");y += 2;
    v_putcad((40-27)/2,y,132,"and others (thanks for all)");//y += 2;
#else
    y=1;
    v_putcad((40-18)/2,y,130,"GP2Xpectrum v1.9.2");y += 2;
    v_putcad((40-21)/2,y,132,"Based on the work of:");y += 2;
    v_putcad((40-17)/2,y,132,"Hermes/PS2Reality");y += 2;
    v_putcad((40-33)/2,y,132,"Metalbrain & Seleuco & SplinterGU");y += 2;
    v_putcad((40-15)/2,y,132,"kounch, GnoStiC");y += 2;
    v_putcad((40-13)/2,y,132,"rlyeh / fZX32");y += 2;
    v_putcad((40-27)/2,y,132,"Santiago Romero / ASpectrum");y += 2;
    v_putcad((40-33)/2,y,132,"Philip Kendall / FUSE+libspectrum");y += 2;
    v_putcad((40-18)/2,y,132,"James McKay / X128");y += 2;
    v_putcad((40-24)/2,y,132,"Ulrich Doewich / Caprice");y += 2;
    v_putcad((40-22)/2,y,132,"Sergey Bulba /  AY2SNA");y += 2;
    v_putcad((40-27)/2,y,132,"and others (thanks for all)");//y += 2;
#endif

    SyncFreq2();
    dump_video();

    while(( time + 10000 > getTicks()) &&
          !(nKeys & (JOY_BUTTON_A | JOY_BUTTON_B | JOY_BUTTON_X | JOY_BUTTON_Y | JOY_BUTTON_MENU)))
    {
        nKeys = joystick_read();

        if(getTicks() - old > 20)
        {
           dump_video();
           old = getTicks();
        }
    }

    while(nKeys & (JOY_BUTTON_A | JOY_BUTTON_B | JOY_BUTTON_X | JOY_BUTTON_Y | JOY_BUTTON_MENU)) nKeys = joystick_read();
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
    for(n = 0;n<5;n++)
    {
        sprintf(poke_title[n],"POKE #%u",n+1);
        for(m = 0; m<10; m++)
        {
            poke_table[n][m][0] = 0;
            poke_table[n][m][1] = 0;
        }
    }
}

void write_pokefile(char *name)
{
    FILE *fp;
    int n,m;

    fp = fopen(name,"w");
    if (fp == 0) return;
    for(n = 0;n<5;n++)
    {
        fprintf(fp,"[%s]\n",poke_title[n]);
        for(m = 0;m<10;m++)
        {
            if (poke_table[n][m][0]>= 16384) fprintf(fp,"POKE %u,%u\n",poke_table[n][m][0],poke_table[n][m][1]);
        }
    }
    fclose(fp);
    sync();
}

void read_pokefile(char *name)
{
    FILE *fp;
    int mode = 0;
    int n = 0,m = 0,i;
    /*unsigned*/ char str_buf[256];


    default_pokes();

    fp = fopen(name,"r");
    if (fp == 0) return;

    while(1)
    {
        if (fgets(str_buf,255,fp) == 0) break;
        if (str_buf[0] == '[')
        {
            if (mode == 1)  n++;
            if (n>4) break; // demasiadas entradas
            i = 0;
            while(i<30 && str_buf[1+i]>= 32 && str_buf[1+i] != ']')
            {
                poke_title[n][i] = str_buf[1+i];
                i++;
            }
            poke_title[n][i] = 0;
            mode = 1;m = 0;
        }
        else
       {
            if (mode == 0) continue; //ignora lineas hasta el proximo titulo
            if (sscanf(str_buf,"POKE %u,%u",poke_table[n][m],&poke_table[n][m][1]) == 2)
            {
                m++;
                if (m>= 10)
                {
                    mode = 0;
                    n++;
                }
            }
            // else mode = 0; //ignora lineas hasta el proximo titulo
        }

    }

    fclose(fp);
}


static char get_char_digit(unsigned num,int dig)
{
    int n,m;
    static char cad[5];
    m = 10000;
    for(n = 0; n<5; n++)
    {
        cad[n] = 48+(num/m);
        num %= m;
        m /= 10;
    }

    return cad[4-dig];
}

static void change_digit1(unsigned *v,unsigned char ch,int pos)
{
    static char cad[5];
    unsigned m,n,num;
    m = 10000;
    num = *v;
    for(n = 0; n<5; n++)
    {
        cad[n] = 48+(num/m);
        num %= m;
        m /= 10;
    }
    cad[pos] = ch;
    m = 0;
    for(n = 0; n<5; n++)
    {
        m *= 10;
        m += cad[n]-48;
    }
    if (m<65536) *v = m;
}

static void change_digit2(unsigned *v,unsigned char ch,int pos)
{
    static char cad[5];
    unsigned m,n,num;
    m = 100;
    num = *v;
    for(n = 0;n<3;n++)
    {
        cad[n] = 48+(num/m);
        num %= m;
        m /= 10;
    }
    cad[pos] = ch;
    m = 0;
    for(n = 0;n<3;n++)
    {
        m *= 10;
        m += cad[n]-48;
    }
    if (m<256) *v = m;
}

extern void POKE(unsigned dir,unsigned char dat); // funcion de escritura en memoria en zx.c

int poke_manager()
{
    int old_speed;
    int n,m,y,f,g;
    int op = 0,op2 = 0;
    unsigned new_key = 0,old_key = 0;
    ///unsigned char cad[256];
    int ret = 0;
    int file_modified = 0;
    int editor_mode = 0;
    int poke_select = 0;
    char *mname;

    int pos = 0,keypos = 0;
    char str_key[30][4] = {
         "A","B","C","D","E","F","G","H","I","J","K","L","M","N","Ent",
         "O","P","Q","R","S","T","U","V","W","X","Y","Z"," ","/","Del"};
    char num_key[10][4] = {
         "1","2","3","4","5","6","7","8","9","0"};

    mname = get_name(MY_filename);
    // obten nombre sin extension
    n = 0;while(mname[n] != 0) n++;
    while(n>0)
    {
        if (mname[n] == '.')
        {
            mname[n] = 0;
            break;
        }
        n--;
    }
    //sprintf(photo_name,"saves/%s.pok",mname);
	sprintf(photo_name,"%s/saves/%s.pok",globalpath,mname);

    read_pokefile(photo_name);

    COLORFONDO = 128;
    while(nKeys & (JOY_BUTTON_A | JOY_BUTTON_B | JOY_BUTTON_X | JOY_BUTTON_Y | JOY_BUTTON_MENU)) nKeys = joystick_read();

    ClearScreen(COLORFONDO);

    g = 0;
    while(1)
    {
        COLORFONDO = 128;
#ifdef  CAPTURE
        if ((old_key & JOY_BUTTON_VOLUP) && (old_key & JOY_BUTTON_SELECT))
        {
	        char pathstring1[512];
			char pathstring2[512];
			char pathstring3[512];
			sprintf(pathstring1,"%s/img/poke_manager.bmp",globalpath);
	        sprintf(pathstring2,"%s/img/poke_edit_name.bmp",globalpath);
	        sprintf(pathstring3,"%s/img/poke_edit_values.bmp",globalpath);
            if(editor_mode==0) save_bmp(pathstring1,  video_screen8, 320, 240, 320);
	        if(editor_mode==1) save_bmp(pathstring2, video_screen8, 320, 240, 320);
	        if(editor_mode==2) save_bmp(pathstring3, video_screen8, 320, 240, 320);
        }
#endif
        ClearScreen(COLORFONDO);

        speccy_corner();

        if (editor_mode == 0)
        {
            COLORFONDO = 128;
            v_putcad(14,0,132,"POKE MANAGER");
            y = 4;
            if (op == 0) COLORFONDO = 129; else COLORFONDO = 128;
            v_putcad(5,y,130,&poke_title[0][0]);
            y += 2;
            if (op == 1) COLORFONDO = 129; else COLORFONDO = 128;
            v_putcad(5,y,130,&poke_title[1][0]);
            y += 2;
            if (op == 2) COLORFONDO = 129; else COLORFONDO = 128;
            v_putcad(5,y,130,&poke_title[2][0]);
            y += 2;
            if (op == 3) COLORFONDO = 129; else COLORFONDO = 128;
            v_putcad(5,y,130,&poke_title[3][0]);
            y += 2;
            if (op == 4) COLORFONDO = 129; else COLORFONDO = 128;
            v_putcad(5,y,130,&poke_title[4][0]);
            y += 2;

            if (op == 5) COLORFONDO = 129; else COLORFONDO = 128;
            v_putcad(5,y,130,"Return");y += 2;
            COLORFONDO = 128;
            y += 4;
            v_putcad(2,y,132,"Use A to edit, B to select"); y += 2;
            v_putcad(2,y,132,"Use X to abort");
        }

        if (editor_mode == 1)
        {
            COLORFONDO = 128;
            v_putcad(11,0,132,"POKE TITLE EDITOR");
            y = 4;
            COLORFONDO = 128;
            v_putcad(5,y,130,&poke_title[poke_select][0]);
            COLORFONDO = 129;
            if (poke_title[poke_select][pos]<32) v_putchar(5+pos,y,130,' ');
            else v_putchar(5+pos,y,130,poke_title[poke_select][pos]);

            y = 15; //teclado virtual
            n = 0;
            for(n = 0;n<30;n++)
            {
                if (keypos == n) COLORFONDO = 129; else COLORFONDO = 128;
                if (n == 0 || n == 15)  v_putcad(5,y+2*(n>= 15),130,&str_key[n][0]);
                else v_putcad(lastx,y+2*(n>= 15),130,&str_key[n][0]);
                lastx += 1;
            }

            y = 20;

            COLORFONDO = 128;
            v_putcad((40-34)>>1,y,132,"Use L/R to move the string cursor.");y += 2;
            v_putcad((40-34)>>1,y,132,"Use LEFT/RIGHT in Virtual Keyboard");y += 2;
            v_putcad((40-31)>>1,y,132,"Use A to edit poke, B to select");y += 2;
            v_putcad((40-14)>>1,y,132,"Use X to abort");y += 2;

        }

        if (editor_mode == 2)
        {
            COLORFONDO = 128;
            v_putcad(11,0,132,"POKE VALUE EDITOR");
            y = 4;

            for(n = 0;n<10;n++)
            {
                COLORFONDO = 128;
                v_putcad(5+16*(n & 1),y,130,"POKE ");
                for(m = 0;m<5;m++)
                {
                    if (pos == m && op2 == n) COLORFONDO = 129; else COLORFONDO = 128;
                    v_putchar(lastx,y,130,get_char_digit(poke_table[poke_select][n][0],4-m));lastx++;
                }
                COLORFONDO = 128;
                v_putcad(lastx,y,130,",");lastx++;
                for(m = 0;m<3;m++)
                {
                    if (pos == m+5 && op2 == n) COLORFONDO = 129; else COLORFONDO = 128;
                    v_putchar(lastx,y,130,get_char_digit(poke_table[poke_select][n][1],2-m));lastx++;
                }
                if (n & 1) y += 2;
            }

            y += 2;
            n = 0;
            for(n = 0;n<10;n++) // teclado virtual
            {
                if (keypos == n) COLORFONDO = 131; else COLORFONDO = 129;
                if (n == 0)  v_putcad(10,y,130,&num_key[n][0]);
                else v_putcad(lastx,y,130,&num_key[n][0]);
                v_putcad(lastx,y,130," ");
            }

            y += 3;

            COLORFONDO = 128;
            v_putcad((40-31)>>1,y,132,"Use UP/DOWN to select the POKE");y += 2;
            v_putcad((40-35)>>1,y,132,"Use L/R to move the numeric cursor.");y += 2;
            v_putcad((40-34)>>1,y,132,"Use LEFT/RIGHT in Virtual Keyboard");y += 2;
            v_putcad((40-31)>>1,y,132,"Use B to select and X to finish");y += 2;

        }

        SyncFreq2();
        dump_video();

        nKeys = joystick_read();

        new_key = nKeys & (~old_key);
        old_key = nKeys;

        if (editor_mode == 0)
        {
            if (!(old_key & ( JOY_BUTTON_UP | JOY_BUTTON_DOWN ))) g = 0;
            if (old_key & JOY_BUTTON_UP)
            {
                if (g == 0) g = 2;
                else if (g & 1) g = 2;
                else {g += 2;if (g>80) { g = 68; new_key |= JOY_BUTTON_UP;}}
            }
            if (old_key  & JOY_BUTTON_DOWN)
            {
                if (g == 0) g = 1;
                else if (!(g & 1)) g = 1;
                else {g += 2;if (g>81) {g = 69; new_key |= JOY_BUTTON_DOWN;}}
            }

            if (new_key & JOY_BUTTON_UP ) {op--;if (op<0) op = 5;}
            if (new_key & JOY_BUTTON_DOWN ) {op++;if (op>5) op = 0;}

            if (new_key & JOY_BUTTON_X) break;

            if ((new_key & JOY_BUTTON_A) && op<5) {editor_mode = 1; poke_select = op; pos = 0; keypos = 0;}
            if (new_key & JOY_BUTTON_B)
            {
                if (op>= 0 && op<= 4)
                {
                    m = 0;
                    for(n = 0;n<10;n++)
                    {
                        if (poke_table[op][n][0] != 0)
                        {
                            m = 1;
                            POKE(poke_table[op][n][0],poke_table[op][n][1]);
                        }
                    }
                    if (m)
                    {
                        for(n = 0;n<60;n++)
                        {
                            ClearScreen(COLORFONDO);
                            speccy_corner();
                            COLORFONDO = 128;
                            v_putcad(17,14,132,"Done!");
                            SyncFreq2();
                            dump_video();
                        }
                    }
                    else
                    {
                        for(n = 0;n<60;n++)
                        {
                            ClearScreen(COLORFONDO);
                            speccy_corner();
                            COLORFONDO = 128;
                            v_putcad(14,14,132,"Not defined!");
                            SyncFreq2();
                            dump_video();
                        }
                    }
                }
                if (op == 5) break;
            }
        }
        else
            if (editor_mode == 1)
            {
                if (new_key & JOY_BUTTON_X) { file_modified = 0; editor_mode = 0; }

                if (!(old_key  & ( JOY_BUTTON_LEFT | JOY_BUTTON_RIGHT ))) g = 0;
                if (old_key  & JOY_BUTTON_LEFT)
                {
                    if (g == 0) g = 2;
                    else if (g & 1) g = 2;
                    else {g += 2;if (g>80) {g = 68; new_key |= JOY_BUTTON_LEFT;}}
                }
                if (old_key & JOY_BUTTON_RIGHT)
                {
                    if (g == 0) g = 1;
                    else if (!(g & 1)) g = 1;
                    else {g += 2;if (g>81) {g = 69; new_key |= JOY_BUTTON_RIGHT;}}
                }
                if (new_key & JOY_BUTTON_LEFT)
                {
                    keypos--;
                    if (keypos < 0) keypos = 29;
                }
                if (new_key & JOY_BUTTON_RIGHT)
                {
                    keypos++;
                    if (keypos > 29) keypos = 0;
                }
                if (new_key & JOY_BUTTON_UP)
                {
                    keypos -= 15;
                    if (keypos < 0) keypos = 29;
                }
                if (new_key & JOY_BUTTON_DOWN)
                {
                    keypos += 15;
                    if (keypos > 29) keypos = 0;
                }

                if (new_key & JOY_BUTTON_L) {pos--;if (pos<0) pos = 0;}
                if (new_key & JOY_BUTTON_R) {if (poke_title[poke_select][pos] != 0) pos++;}
                if (new_key & ( JOY_BUTTON_B | JOY_BUTTON_A ) )
                {
                    if (keypos == 14 || (new_key & JOY_BUTTON_A)) // enter
                    {
                        editor_mode = 2;
                        op2 = 0;
                        keypos = 0;
                        pos = 0;
                    }
                    else
                        if (keypos == 29) // del
                        {
                            if (pos != 0)
                            {
                                m = strlen(&poke_title[poke_select][0])+1;
                                for(n = pos-1;n<m;n++) poke_title[poke_select][n] = poke_title[poke_select][n+1];
                                pos--;
                                if (pos<0) pos = 0;
                                file_modified = 1;
                            }
                            else
                            {
                                m = strlen(&poke_title[poke_select][0])+1;
                                for(n = pos;n<m;n++) poke_title[poke_select][n] = poke_title[poke_select][n+1];
                                file_modified = 1;
                            }
                        }
                        else
                        {
                            if ((m = strlen(&poke_title[poke_select][0]))<29)
                            {
                                m++;
                                for(n = m;n>= pos;n--) poke_title[poke_select][n+1] = poke_title[poke_select][n];
                                poke_title[poke_select][pos] = str_key[keypos][0];
                                /*if (poke_title[poke_select][pos+1] != 0) */
                                pos++;
                                file_modified = 1;
                            }
                        }
                }
            }
            else
                if (editor_mode == 2)
                {
                    if (new_key & JOY_BUTTON_X) { file_modified = 1; editor_mode = 0; }

                    if (!(old_key & (JOY_BUTTON_LEFT | JOY_BUTTON_RIGHT))) g = 0;
                    if (old_key & JOY_BUTTON_LEFT)
                    {
                        if (g == 0) g = 2;
                        else if (g & 1) g = 2;
                        else {g += 2;if (g>80) {g = 68; new_key |= JOY_BUTTON_LEFT;}}
                    }
                    if (old_key  & JOY_BUTTON_RIGHT)
                    {
                        if (g == 0) g = 1;
                        else if (!(g & 1)) g = 1;
                        else {g += 2;if (g>81) {g = 69; new_key |= JOY_BUTTON_RIGHT;}}
                    }
                    if (new_key & JOY_BUTTON_LEFT)
                    {
                        keypos--;
                        if (keypos < 0) keypos = 9;
                    }
                    if (new_key & JOY_BUTTON_RIGHT)
                    {
                        keypos++;
                        if (keypos > 9) keypos = 0;
                    }
                    if (new_key & JOY_BUTTON_UP)
                    {
                        pos = 0;
                        op2--;
                        if (op2 < 0) op2 = 9;
                    }
                    if (new_key & JOY_BUTTON_DOWN)
                    {
                        pos = 0;
                        op2++;
                        if (op2 > 9) op2 = 0;
                    }

                    if (new_key & JOY_BUTTON_L) {pos--;if (pos<0) {pos = 7;op2--;} if (op2<0) op2 = 0;}
                    if (new_key & JOY_BUTTON_R) {pos++;if (pos>7) {pos = 0;op2++;} if (op2>9) op2 = 0;}
                    if (new_key & JOY_BUTTON_B)
                    {
                        if (pos<5)
                        {
                            file_modified = 1;
                            change_digit1(&poke_table[poke_select][op2][0],num_key[keypos][0],pos);
                        }
                        else
                        {
                            file_modified = 1;
                            change_digit2(&poke_table[poke_select][op2][1],num_key[keypos][0],pos-5);
                        }

                        pos++;
                        if (pos>7)
                        {
                            pos = 0;
                            op2++;
                        }
                        if (op2>9) op2 = 0;
                    }
                }
    }

    if (file_modified) write_pokefile(photo_name);

    while(nKeys & (JOY_BUTTON_A | JOY_BUTTON_B | JOY_BUTTON_X | JOY_BUTTON_Y | JOY_BUTTON_MENU)) nKeys = joystick_read();

    COLORFONDO = 128;
    ClearScreen(COLORFONDO);

    return ret;
}

/****************************************************************************************************************/
// disk manager
/****************************************************************************************************************/

int disk_manager()
{
    int old_speed;
    int n,m,y,f,g;
    int op = 0,op2 = 0;
    unsigned new_key = 0,old_key = 0;
    /*unsigned*/ char cad[256] = "";
    int ret = 0;

    char *mname;

    int pos = 0,keypos = 0;
    char str_key[30][4] = {
        "A","B","C","D","E","F","G","H","I","J","K","L","M","N","Ent",
        "O","P","Q","R","S","T","U","V","W","X","Y","Z"," ","_","Del"
    };

    read_pokefile(photo_name);

    while(nKeys & (JOY_BUTTON_A | JOY_BUTTON_B | JOY_BUTTON_X | JOY_BUTTON_Y | JOY_BUTTON_MENU)) nKeys = joystick_read();

    pos = 0;
    keypos = 0;
    g = 0;
    while(1)
    {
        COLORFONDO = 128;
#ifdef  CAPTURE
        if ((old_key & JOY_BUTTON_VOLUP) && (old_key & JOY_BUTTON_SELECT))
        {
			char pathstring[512];
			sprintf(pathstring,"%s/img/disk_manager.bmp",globalpath);
            save_bmp(pathstring, video_screen8, 320, 240, 320);
        }
#endif
        ClearScreen(COLORFONDO);
        speccy_corner();

        COLORFONDO = 128;
        v_putcad(11,0,132,"DISK TITLE EDITOR");
        y = 4;
        COLORFONDO = 128;
        v_putcad(12,y,130,&cad[0]);
        COLORFONDO = 129;
        if (cad[pos]<32) v_putchar(12+pos,y,130,' ');
        else v_putchar(12+pos,y,130,cad[pos]);

        y = 15; //teclado virtual
        n = 0;
        for(n = 0;n<30;n++)
        {
            if (keypos == n) COLORFONDO = 129; else COLORFONDO = 128;
            if (n == 0 || n == 15)  v_putcad(5,y+2*(n>= 15),130,&str_key[n][0]);
            else v_putcad(lastx,y+2*(n>= 15),130,&str_key[n][0]);
            lastx += 1;
        }

        y = 20;

        COLORFONDO = 128;
        v_putcad((40-34)>>1,y,132,"Use L/R to move the string cursor.");y += 2;
        v_putcad((40-34)>>1,y,132,"Use LEFT/RIGHT in Virtual Keyboard");y += 2;
        v_putcad((40-34)>>1,y,132,"Use B to select and X to abort");y += 2;

        SyncFreq2();
        dump_video();

        nKeys = joystick_read();

        new_key = nKeys & (~old_key);
        old_key = nKeys;

        if (new_key & JOY_BUTTON_X) break;

        if (!(old_key & (JOY_BUTTON_LEFT | JOY_BUTTON_RIGHT))) g = 0;
        if (old_key & JOY_BUTTON_LEFT)
        {
            if (g == 0) g = 2;
            else if (g & 1) g = 2;
            else {g += 2;if (g>80) {g = 68; new_key |= JOY_BUTTON_LEFT;}}
        }
        if (old_key  & JOY_BUTTON_RIGHT)
        {
            if (g == 0) g = 1;
            else if (!(g & 1)) g = 1;
            else {g += 2;if (g>81) {g = 69; new_key |= JOY_BUTTON_RIGHT;}}
        }

        if (new_key & JOY_BUTTON_LEFT)
        {
            keypos--;
            if (keypos < 0) keypos = 29;
        }
        if (new_key & JOY_BUTTON_RIGHT)
        {
            keypos++;
            if (keypos > 29) keypos = 0;
        }
        if (new_key & JOY_BUTTON_UP)
        {
            keypos -= 15;
            if (keypos < 0) keypos = 29;
        }
        if (new_key & JOY_BUTTON_DOWN)
        {
            keypos += 15;
            if (keypos > 29) keypos = 0;
        }

        if (new_key & JOY_BUTTON_L) {pos--;if (pos<0) pos = 0;}
        if (new_key & JOY_BUTTON_R) {if (cad[pos] != 0) pos++;}
        if (new_key & ( JOY_BUTTON_B | JOY_BUTTON_A ))
        {
            if (keypos == 14 || (new_key & JOY_BUTTON_A)) // enter
            {
                op2 = 0;keypos = 0;pos = 0;
                m = -1;
                if (cad[0] != 0)
                {
					sprintf(photo_name,"%s/saves/%s.dsk",globalpath,cad);
                    m = dsk_save(photo_name);sync();
                }

                for(n = 0;n<60;n++)
                {
                    ClearScreen(COLORFONDO);
                    speccy_corner();
                    COLORFONDO = 128;
                    if (m == 0) v_putcad(17,14,132,"Done!"); else v_putcad(17,14,132,"Fail!");
                    SyncFreq2();
                    dump_video();
                }
                break;
            }
            else
                if (keypos == 29) // del
                {
                    if (pos != 0)
                    {
                        m = strlen(&cad[0])+1;
                        for(n = pos-1;n<m;n++) cad[n] = cad[n+1];
                        pos--;if (pos<0)  pos = 0;
                    }
                    else
                    {
                        m = strlen(&cad[0])+1;
                        for(n = pos;n<m;n++) cad[n] = cad[n+1];
                    }
                }
                else
                {
                    if ((m = strlen(&cad[0]))<16)
                    {
                        m++;
                        for(n = m;n>= pos;n--) cad[n+1] = cad[n];
                        cad[pos] = str_key[keypos][0];
                        /*if (cad[pos+1] != 0) */
                        pos++;
                    }
                }
        }
    }

    while(nKeys & (JOY_BUTTON_A | JOY_BUTTON_B | JOY_BUTTON_X | JOY_BUTTON_Y | JOY_BUTTON_MENU)) nKeys = joystick_read();

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
    static int op = 25;
    unsigned new_key = 0,old_key = 0;
    unsigned char cad[256];
    int ret = 0;
    char *mname;
    while(nKeys & (JOY_BUTTON_A | JOY_BUTTON_B | JOY_BUTTON_X | JOY_BUTTON_Y | JOY_BUTTON_MENU))
    	nKeys = joystick_read();

//    set_mypalette()


	MCONFIG oldmconfig;
	memcpy(&oldmconfig,&mconfig,sizeof(MCONFIG));

    g = 0;
    while(1)
    {
        COLORFONDO = 128;
#ifdef  CAPTURE

        if (old_key & (JOY_BUTTON_VOLUP | JOY_BUTTON_SELECT))
        {
            char pathstring[512];
			sprintf(pathstring,"%s/img/config.bmp",globalpath);
            save_bmp(pathstring, video_screen8, 320, 240, 320);
        }
#endif

        ClearScreen(COLORFONDO);speccy_corner();

        v_putcad(14,1,132,"CONFIGURATION");
        if (model == ZX_PLUS3) y = 3; else y = 4;

        //opcion 0
        if (op == 0) COLORFONDO = 129; else COLORFONDO = 128;
        v_putcad(10,y,130,"Load Game State");y += 1;

        //opcion 1
        if (op == 1) COLORFONDO = 129; else COLORFONDO = 128;
        v_putcad(10,y,130,"Save Keyboard");y += 1;

        //opcion 2
        if (op == 2) COLORFONDO = 129; else COLORFONDO = 128;
        v_putcad(10,y,130,"Save Game State");y += 1;

        y += 1;///SALTO

        //opcion 7
        if (op == 3) COLORFONDO = 129; else COLORFONDO = 128;
        v_putcad(10,y,130,"POKE Manager");y += 1;

        if (op == 4) COLORFONDO = 129; else COLORFONDO = 128;
        if (mconfig.ula64 == 0)
        {
            v_putcad(10,y,130,"Ula+64 Disabled");y += 1;
        }
        else if (mconfig.ula64 == 1)
        {
            v_putcad(10,y,130,"Ula+64 with Color Reset");y += 1;
        }
        else if (mconfig.ula64 == 2)
        {
            v_putcad(10,y,130,"Ula+64 without Color Reset");y += 1;
        }

        //if (mconfig.ula64_reset) v_putcad(10,y,130,"Ula64 Reset ON");
        //else v_putcad(10,y,130,"Ula64 Reset OFF"); y += 1;

        //opciones 10,11,12 saltarse si no plus3
        if (model == ZX_PLUS3)
        {
            //y += 1;///SALTO
            if (op == 9) COLORFONDO = 129; else COLORFONDO = 128;
            v_putcad(10,y,130,"New +3 Disk");y += 1;
            if (op == 10) COLORFONDO = 129; else COLORFONDO = 128;
            if (dsk_flipped) v_putcad(10,y,130,"Use SIDE B for +3 Disk");
            else v_putcad(10,y,130,"Use SIDE A for +3 Disk");
            y += 1;
            if (op == 11) COLORFONDO = 129; else COLORFONDO = 128;
            v_putcad(10,y,130,"Save +3 Disk");y += 1;
        }

        //opcion 5
        if (op == 12) COLORFONDO = 129; else COLORFONDO = 128;
        if (mconfig.show_fps) v_putcad(10,y,130,"Show FPS ON");
        else v_putcad(10,y,130,"Show FPS OFF");

        y += 1;

        //y += 1;///SALTO
        //opcion 13
        if (op == 13) COLORFONDO = 129; else COLORFONDO = 128;

        if (mconfig.contention)
        {
            sprintf(menustring,"Contention ON");
        }
        else
        {
            sprintf(menustring,"Contention OFF");
        }
        v_putcad(10,y,130,menustring);
        y += 1;
        //opcion 14
        if (op == 14) COLORFONDO = 129; else COLORFONDO = 128;
        sprintf(menustring,"Emulation Speed %i\%%",mconfig.speed_mode);
        v_putcad(10,y,130,menustring);y += 1;

        //opcion 15
        if (op == 15) COLORFONDO = 129; else COLORFONDO = 128;
        //sprintf(menustring,"CPU Speed %i MHz",mconfig.cpu_freq);
        //v_putcad(10,y,130,menustring);
        if (mconfig.frameskip==2)
        	 v_putcad(10,y,130,"Auto Skip Frames");
        else if (mconfig.frameskip == 1)
        	 v_putcad(10,y,130,"Draw 1/2 Frames");
        else v_putcad(10,y,130,"Draw All Frames");
        y += 1;



        y += 1;///SALTO
        //opcion 16
        if (op == 16) COLORFONDO = 129; else COLORFONDO = 128;
        if (mconfig.sound_mode == 0)
        {
            v_putcad(10,y,130,"Sound OFF");y += 1;
        }
        else if (mconfig.sound_mode == 1)
        {
            v_putcad(10,y,130,"Sound Mono");y += 1;
        }
        else if (mconfig.sound_mode == 2)
        {
            v_putcad(10,y,130,"Sound Stereo Beeper");y += 1;
        }
        else if (mconfig.sound_mode == 3)
        {
            v_putcad(10,y,130,"Sound Stereo ABC AY");y += 1;
        }
        else if (mconfig.sound_mode == 4)
        {
            v_putcad(10,y,130,"Sound Stereo ALL");y += 1;
        }
        //opcion 17
        if (op == 17) COLORFONDO = 129; else COLORFONDO = 128;
        sprintf(menustring,"Sound Rate %i KHz",mconfig.sound_freq);
        v_putcad(10,y,130,menustring);y += 1;

        y += 1;///SALTO
        //opcion 18
        if (op == 18) COLORFONDO = 129; else COLORFONDO = 128;
        if (mconfig.speed_loading)
        {
            v_putcad(10,y,130,"Fast Loading ON");y += 1;
        }
        else
        {
            v_putcad(10,y,130,"Fast Loading OFF");y += 1;
        }

        if (op == 19) COLORFONDO = 129; else COLORFONDO = 128;
        if (mconfig.flash_loading)
        {
            v_putcad(10,y,130,"Flash Loading ON");y += 1;
        }
        else
        {
            v_putcad(10,y,130,"Flash Loading OFF");y += 1;
        }

        if (op == 20) COLORFONDO = 129; else COLORFONDO = 128;
        if (mconfig.edge_loading)
        {
            v_putcad(10,y,130,"Edge Loading ON");y += 1;
        }
        else
        {
            v_putcad(10,y,130,"Edge Loading OFF");y += 1;
        }

        if (op == 21) COLORFONDO = 129; else COLORFONDO = 128;
        if (mconfig.auto_loading)
        {
            v_putcad(10,y,130,"Auto Tape Play/Stop ON");y += 1;
        }
        else
        {
            v_putcad(10,y,130,"Auto Tape Play/Stop OFF");y += 1;
        }

        if (op == 22) COLORFONDO = 129; else COLORFONDO = 128;
        {
            v_putcad(10,y,130,"Tape Browser");y += 1;
        }

        //opcion 23
#if !defined(IPHONE) && !defined(ANDROID)
        if (op == 23) COLORFONDO = 129; else COLORFONDO = 128;
        if (mconfig.zx_screen_mode) v_putcad(10, y,130, "Full Screen");
        else v_putcad(10, y,130, "Spectrum Screen");
        y += 1;
#endif
        y += 1;///SALTO
        //opcion 24
        if (op == 24) COLORFONDO = 129; else COLORFONDO = 128;
        v_putcad(10,y,130,"Exit from Game");y += 1;

        //opcion 25
        if (op == 25) COLORFONDO = 129; else COLORFONDO = 128;
        v_putcad(10,y,130,"Return to Game");y += 2;

        SyncFreq2();
        dump_video();

        //usleep(10000);

        nKeys = joystick_read();

        new_key = nKeys & (~old_key);
        old_key = nKeys;

        if (!(old_key & (JOY_BUTTON_UP | JOY_BUTTON_DOWN))) g = 0;
        if (old_key & JOY_BUTTON_UP)
        {
            if (g == 0) g = 2;
            else if (g & 1) g = 2;
            else {g += 2;if (g>80) {g = 68; new_key |= JOY_BUTTON_UP;}}
        }
        if (old_key  & JOY_BUTTON_DOWN)
        {
            if (g == 0) g = 1;
            else if (!(g & 1)) g = 1;
            else {g += 2;if (g>81) {g = 69; new_key |= JOY_BUTTON_DOWN;}}
        }
        if (new_key & JOY_BUTTON_UP) {op--;if (model != ZX_PLUS3 && op == 11) op = 4; if (model == ZX_PLUS3 && op == 8) op = 4;if (op<0) op = 25;
#if defined(IPHONE) || defined(ANDROID)
        if(op==23)op--;
#endif
        }

        if (new_key & JOY_BUTTON_DOWN) {op++;if (model != ZX_PLUS3 && op == 5) op = 12; if (model == ZX_PLUS3 && op == 5) op = 9; if (op>25) op = 0;
#if defined(IPHONE) || defined(ANDROID)
        if(op==23)op++;
#endif
        }

        if (new_key & JOY_BUTTON_LEFT)
        {
            if (op == 14)
            {
                mconfig.speed_mode -= 5;
                if (mconfig.speed_mode<25)
                    mconfig.speed_mode = 25;
                int factor = (20 * 100 ) / mconfig.speed_mode;
                delayvalue =  factor +(mconfig.frameskip * factor);
                sound_end();
                sound_init(-1,-1);
                if (tape_playing && mconfig.speed_loading)
                    sound_pause();
            }
/*
            if (op == 15)
            {
            	mconfig.cpu_freq -= 5;
                if (mconfig.cpu_freq<180)
                mconfig.cpu_freq = 180;
            }
*/
            if (op == 16)
            {
                sound_end();
                sound_close();
                mconfig.sound_mode -= 1;
                if (mconfig.sound_mode<0)
                    mconfig.sound_mode = 0;

                if (mconfig.sound_mode != 0)
                {
                    sound_open(mconfig.sound_freq,16,mconfig.sound_mode >=  2);
                    sound_init(-1,-1);
                    if (tape_playing && mconfig.speed_loading)
                        sound_pause();
                }
            }

            if (op == 17)
            {
                if (mconfig.sound_freq == 44100)
                    mconfig.sound_freq = 32000;
                else if (mconfig.sound_freq == 32000)
                    mconfig.sound_freq = 22050;
                else if (mconfig.sound_freq == 22050)
                    mconfig.sound_freq = 11025;

                sound_end();
                sound_close();
                sound_open(mconfig.sound_freq,16,mconfig.sound_mode >=  2);
                sound_init(-1,-1);
                if (tape_playing && mconfig.speed_loading)
                    sound_pause();
            }
        }

        if (new_key & JOY_BUTTON_RIGHT)
        {
            if (op == 14)
            {
                mconfig.speed_mode += 5;
                if (mconfig.speed_mode>175)
                    mconfig.speed_mode = 175;
                int factor = (20 * 100 ) / mconfig.speed_mode;
                delayvalue =  factor +(mconfig.frameskip * factor);
                sound_end();
                sound_init(-1,-1);
                if (tape_playing && mconfig.speed_loading)
                    sound_pause();
            }
/*
            if (op == 15)
            {

                mconfig.cpu_freq += 5;
                if (mconfig.cpu_freq>300)
                    mconfig.cpu_freq = 300;
            }
*/
            if (op == 16)
            {
                sound_end();
                sound_close();
                mconfig.sound_mode += 1;
                if (mconfig.sound_mode>4)
                    mconfig.sound_mode = 4;

                if (mconfig.sound_mode != 0)
                {
                    sound_open(mconfig.sound_freq,16,mconfig.sound_mode >=  2);
                    sound_init(-1,-1);
                    if (tape_playing && mconfig.speed_loading)
                        sound_pause();
                }
            }

            if (op == 17)
            {
                if (mconfig.sound_freq == 11025)
                    mconfig.sound_freq = 22050;
                else if (mconfig.sound_freq == 22050)
                    mconfig.sound_freq = 32000;
                else if (mconfig.sound_freq == 32000)
                    mconfig.sound_freq = 44100;

                sound_end();
                sound_close();
                sound_open(mconfig.sound_freq,16,mconfig.sound_mode >=  2);
                sound_init(-1,-1);
                if (tape_playing && mconfig.speed_loading)
                    sound_pause();
            }
        }

        if (new_key & JOY_BUTTON_MENU || new_key & JOY_BUTTON_X) break;

        if (new_key & JOY_BUTTON_B)
        {
            if (op == 0) {load_state(0);break;}
            if (op == 1)
            {
                mname = get_name(MY_filename);
                // obten nombre sin extension
                n = 0;while(mname[n] != 0) n++;
                while(n>0) {if (mname[n] == '.') {mname[n] = 0;break;} n--;}
	            sprintf(photo_name,"%s/saves/%s.key",globalpath,mname);
                write_keyfile(photo_name);break;
            }
            if (op == 2)  {save_state(0);break;}

            //if (op == 3) mconfig.zx_screen_mode ^= 1;
            //if (op == 4){mconfig.wait_vsync ^= 1;}

            if (op == 3) {poke_manager();}

            if (op == 4){
                mconfig.ula64 += 1;
                if (mconfig.ula64 > 2)
                    mconfig.ula64 = 0;
            }

            if (op == 9) {load_empty_dsk();dsk_load((void *) DSK);break;}
            if (op == 10) {if (driveA.sides) {dsk_flipped ^= 1;driveA.flipped = dsk_flipped;} else driveA.flipped = 0;}
            if (op == 11) {disk_manager();}

            if (op == 12){mconfig.show_fps ^= 1;}
            if (op == 13){mconfig.contention ^= 1;}
            if (op == 15){
                       mconfig.frameskip = (mconfig.frameskip + 1) % 3;
                       int factor = (20 * 100 ) / mconfig.speed_mode;
                       delayvalue =  factor +(mconfig.frameskip * factor);
                       sound_end();
                       sound_init(-1,-1);
                       if (tape_playing && mconfig.speed_loading)
                           sound_pause();
            }


            if (op == 18)
            {
                mconfig.speed_loading ^= 1;
                if (mconfig.speed_loading && tape_playing)
                    sound_pause();
                else
                    sound_unpause();
            }
            if (op == 19)
            {
                mconfig.flash_loading ^= 1;
                if (mconfig.flash_loading)
                    ZX_Patch_ROM();
                else
                    ZX_Unpatch_ROM();
            }

            if (op == 20){mconfig.edge_loading ^= 1;}

            if (op == 21){mconfig.auto_loading ^= 1;}

            if (op == 22){tape_browser();}

            if (op == 23){mconfig.zx_screen_mode ^= 1;}

            if (op == 24){

            	 ret = 1;
            	 break;
            }

            if (op == 25) break;
        }
    }

//    set_emupalette();

    while(nKeys & (JOY_BUTTON_A | JOY_BUTTON_B | JOY_BUTTON_X | JOY_BUTTON_Y | JOY_BUTTON_MENU)) nKeys = joystick_read();

	//comprobar si cambio y grabar
    //printf("miro si grabo!\n");
	if(memcmp(&mconfig,&oldmconfig,sizeof(MCONFIG))!=0)
	{
	    //printf("mconfig modificado, lo grabo\n!");
		save_mconfig();
	}

    return ret;
}


/*************************************************************************************************************************/
//
// SPECTRUM interface
//
//
/**************************************************************************************************************************/

// spectrum
unsigned new_key = 0,old_key = 0;

unsigned char * Picture;
unsigned char *LCDs8;


int keyb_num[4][10] = {
    {SPECKEY_1, SPECKEY_2, SPECKEY_3, SPECKEY_4, SPECKEY_5,SPECKEY_6, SPECKEY_7, SPECKEY_8, SPECKEY_9, SPECKEY_0},
    {SPECKEY_Q,SPECKEY_W,SPECKEY_E,SPECKEY_R, SPECKEY_T,SPECKEY_Y,SPECKEY_U,SPECKEY_I,SPECKEY_O, SPECKEY_P},
    {SPECKEY_A,SPECKEY_S,SPECKEY_D, SPECKEY_F, SPECKEY_G, SPECKEY_H,SPECKEY_J, SPECKEY_K, SPECKEY_L,SPECKEY_ENTER},
    {SPECKEY_SHIFT,SPECKEY_Z,SPECKEY_X, SPECKEY_C, SPECKEY_V,SPECKEY_B, SPECKEY_N, SPECKEY_M, SPECKEY_SYMB,SPECKEY_SPACE}
};

char keyb_str[8][10][5] = {
    {" 1 "," 2 "," 3 "," 4 "," 5 "," 6 "," 7 "," 8 "," 9 "," 0 "},
    {" Q "," W "," E "," R "," T "," Y "," U "," I "," O "," P "},
    {" A "," S "," D "," F "," G "," H "," J "," K "," L ","ENT"},
    {"CAP"," Z "," X "," C "," V "," B "," N "," M ","SYM","SPC"},
    {" ! "," @ "," # "," $ "," % "," & "," ' "," ( "," ) "," _ "},
    {" <= "," <>"," >= "," < "," > "," &&"," ||"," at"," ; "," \42 "},
    {" sp"," no"," st"," to"," th"," ^ "," - "," + "," = ","ENT"},
    {"CAP","  :","  l"," ? "," / "," * "," , "," . ","SYM","SPC"}
};


/*
fill_mask(int  x,int  y,int anc,int  alt)
{
int   n,m;
volatile unsigned char *p,v;
for(n = 0;n<alt;n++)
    {
    p = &video_screen88[(y+n)*320+x];
    for(m = 0;m<anc;m++)
        {
        v = *p;if (v<17) v += 18;*p++ = v;

        }
    }
}
*/
int keyboard_on = 0;
int keyboard_press = 0;
int autoload = 0;
int mJoystick  = 1;//DAV: cambio defecto a kemston
                //left/right/up/down/a,b,x,y/L/R
int map_keys[14] = {SPECKEY_O,SPECKEY_P,SPECKEY_Q,SPECKEY_A,SPECKEY_M,SPECKEY_SPACE,SPECKEY_ENTER,SPECKEY_0,SPECKEY_SHIFT,SPECKEY_SYMB,DEFAULT_SPEED_NOFS,DEFAULT_SPEED_FS1,DEFAULT_SPEED_NOFSNC,DEFAULT_SPEED_FS1NC};

int multiple_key[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int nmultikey = 0;

/****************************************************************************************************************/
// POKE MANAGER
/****************************************************************************************************************/

void write_keyfile(char *name)
{
    FILE *fp;
    fp = fopen(name,"w");
    if (fp == 0) return;
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
    fclose(fp);
    sync();
}

void read_keyfile(char *name)
{
    FILE *fp;
    int mode = 0;
    int n = 0,m = 0,i;
    unsigned char str_buf[512];

    default_pokes();

    fp = fopen(name,"r");
    if (fp == 0)
    {
        //seteamos keys por defecto.
    	mJoystick = 1;
    	map_keys[0] = SPECKEY_O;
    	map_keys[1] = SPECKEY_P;
    	map_keys[2] = SPECKEY_Q;
    	map_keys[3] = SPECKEY_A;
    	map_keys[4] = SPECKEY_M;
    	map_keys[5] = SPECKEY_SPACE;
    	map_keys[6] = SPECKEY_ENTER;
    	map_keys[7] = SPECKEY_0;
    	map_keys[8] = SPECKEY_SHIFT;
    	map_keys[9] = SPECKEY_SYMB;
    	return;
    }
    if (fscanf(fp,"mJoystick: %i\n",&m) == 1) mJoystick = m;
    if (fscanf(fp,"LEFT: %i\n",&m) == 1) map_keys[0] = m;
    if (fscanf(fp,"RIGHT: %i\n",&m) == 1) map_keys[1] = m;
    if (fscanf(fp,"UP: %i\n",&m) == 1) map_keys[2] = m;
    if (fscanf(fp,"DOWN: %i\n",&m) == 1) map_keys[3] = m;
    if (fscanf(fp,"A: %i\n",&m) == 1) map_keys[4] = m;
    if (fscanf(fp,"B: %i\n",&m) == 1) map_keys[5] = m;
    if (fscanf(fp,"X: %i\n",&m) == 1) map_keys[6] = m;
    if (fscanf(fp,"Y: %i\n",&m) == 1) map_keys[7] = m;
    if (fscanf(fp,"L: %i\n",&m) == 1) map_keys[8] = m;
    else map_keys[8] = SPECKEY_SHIFT;
    if (fscanf(fp,"R: %i\n",&m) == 1) map_keys[9] = m;
    else map_keys[9] = SPECKEY_SYMB;
    fclose(fp);
}


static int program_mode = 0;
/*static int unprogram = 0;*/
static int menu_mode = 0;


static int redrawmask = 1;
int display_keyboard()
{
    int n,m,ret,x;
    static int curx = 0;
    static int cury = 0;
    static int f1 = 0,f2 = 0,f3 = 0,f4 = 0;
    static int sub_men = 0;
    static int flip = 0;

    while(nKeys & JOY_BUTTON_MENU) nKeys = joystick_read();

    if (menu_mode == 0)
    {
        if (program_mode && cury<4)
        {
            if ((new_key & JOY_BUTTON_LEFT))  {map_keys[0] = keyb_num[cury][curx];program_mode = 0;redrawmask = 1;return -1;}
            if ((new_key & JOY_BUTTON_RIGHT)) {map_keys[1] = keyb_num[cury][curx];program_mode = 0;redrawmask = 1;return -1;}
            if ((new_key & JOY_BUTTON_UP))    {map_keys[2] = keyb_num[cury][curx];program_mode = 0;redrawmask = 1;return -1;}
            if ((new_key & JOY_BUTTON_DOWN))  {map_keys[3] = keyb_num[cury][curx];program_mode = 0;redrawmask = 1;return -1;}
            if ((new_key & JOY_BUTTON_A))     {map_keys[4] = keyb_num[cury][curx];program_mode = 0;redrawmask = 1;return -1;}
            if ((new_key & JOY_BUTTON_B))     {map_keys[5] = keyb_num[cury][curx];program_mode = 0;redrawmask = 1;return -1;}
            if ((new_key & JOY_BUTTON_X))     {map_keys[6] = keyb_num[cury][curx];program_mode = 0;redrawmask = 1;return -1;}
            if ((new_key & JOY_BUTTON_Y))     {map_keys[7] = keyb_num[cury][curx];program_mode = 0;redrawmask = 1;return -1;}
            if ((new_key & JOY_BUTTON_L))     {map_keys[8] = keyb_num[cury][curx];program_mode = 0;redrawmask = 1;return -1;}
            if ((new_key & JOY_BUTTON_R))     {map_keys[9] = keyb_num[cury][curx];program_mode = 0;redrawmask = 1;return -1;}
        }

        if (!program_mode)
        {
            if (old_key  & JOY_BUTTON_UP)
            {
                if (f1 == 0) f1 = 1;
                else {f1++;if (f1>20) {f1 = 17; new_key |= JOY_BUTTON_UP;}}
            } else f1 = 0;
            if (old_key  & JOY_BUTTON_DOWN)
            {
                if (f2 == 0) f2 = 1;
                else {f2++;if (f2>20) {f2 = 17; new_key |= JOY_BUTTON_DOWN;}}
            } else f2 = 0;
            if (old_key  & JOY_BUTTON_LEFT)
            {
                if (f3 == 0) f3 = 1;
                else {f3++;if (f3>20) {f3 = 17; new_key |= JOY_BUTTON_LEFT;}}
            } else f3 = 0;
            if (old_key  & JOY_BUTTON_RIGHT)
            {
                if (f4 == 0) f4 = 1;
                else {f4++;if (f4>20) {f4 = 17; new_key |= JOY_BUTTON_RIGHT;}}
            } else f4 = 0;

            if (new_key & JOY_BUTTON_UP) {redrawmask = 1;cury--;if (cury<0) cury = 4;if (cury == 4) if (curx>4)  curx = 4;}
            if (new_key & JOY_BUTTON_DOWN) {redrawmask = 1;cury++;if (cury>4) cury = 0;if (cury == 4) if (curx>4)  curx = 4;}
            if (new_key & JOY_BUTTON_LEFT) {redrawmask = 1;curx--;if (curx<0) curx = 9;if (cury == 4) if (curx>4)  curx = 4;}
            if (new_key & JOY_BUTTON_RIGHT) {redrawmask = 1;curx++;if (curx>9) curx = 0;if (cury == 4) if (curx>4)  curx = 0;}

            if ((new_key & JOY_BUTTON_A) && cury<4)
            {
                if (nmultikey<8) multiple_key[nmultikey++] = keyb_num[cury][curx];
                redrawmask = 1;
            }

            if (new_key & JOY_BUTTON_X)
            {
                keyboard_on = 0;
                while(nKeys & JOY_BUTTON_X) nKeys = joystick_read(); // para quieto!!
                return;
            }
        }

        if ((new_key & JOY_BUTTON_MENU) && cury != 4) {
        	redrawmask = 1;
        	program_mode ^= 1;
        	while(nKeys & JOY_BUTTON_MENU) nKeys = joystick_read();
        }

        if (program_mode) redrawmask = 1;
        else
        {
            if (old_key & JOY_BUTTON_L) {redrawmask = 1;}
            if (old_key & JOY_BUTTON_R) {redrawmask = 1;}
        }
        if (redrawmask)
        {
            memset(mask_keyb,0,32*8*11*8);
            for(n = 0;n<4;n++)
                for(m = 0;m<10;m++)
                {
                    if (program_mode)
                    {
                        if (curx == m && cury == n) {if (flip & 2) COLORFONDO = 252; else COLORFONDO = 255;} else COLORFONDO = 255;
                    }
                    else
                    {
                        if (curx == m && cury == n) COLORFONDO = 252; else COLORFONDO = 255;
                        if (n == 3 && m == 0 && (old_key & JOY_BUTTON_L)) {redrawmask = 1;COLORFONDO = 254;}
                        if (n == 3 && m == 8 && (old_key & JOY_BUTTON_R)) {redrawmask = 1;COLORFONDO = 254;}
                    }
                    if (n == 3 && m == 9) {COLORFONDO = 252+(COLORFONDO == 255);mask_putcad(5+m*3,n*2+10,253,&keyb_str[n][m][0]);}
                    else mask_putcad(5+m*3,n*2+10,255,&keyb_str[n+4*((old_key & JOY_BUTTON_R) != 0)][m][0]);
                }
            if (curx == 0 && cury == n) COLORFONDO = 252; else COLORFONDO = 255;
            mask_putcad(5,n*2+10,255,"RESET ");

            if (curx == 1 && cury == n) COLORFONDO = 252; else COLORFONDO = 255;
            mask_putcad(lastx,n*2+10,255,"LOAD ");

            if (curx == 2 && cury == n) COLORFONDO = 252; else COLORFONDO = 255;

            if (mJoystick == 1) mask_putcad(lastx,n*2+10,255,"KEMPSTON ");
            if (mJoystick == 2) mask_putcad(lastx,n*2+10,255,"INTERFII ");
            if (mJoystick == 3) mask_putcad(lastx,n*2+10,255,"INTERF I ");
            if (mJoystick == 4) mask_putcad(lastx,n*2+10,255,"CURSOR ");
            if (mJoystick == 5) mask_putcad(lastx,n*2+10,255,"FULLER ");
            if (mJoystick == 0) mask_putcad(lastx,n*2+10,255,"KEYBOARD ");

            if (curx == 3 && cury == n) COLORFONDO = 252; else COLORFONDO = 255;
            {
                char txt[10];
                sprintf ( txt, "MKEY%c ", '0' + nmultikey );
                mask_putcad(lastx,n*2+10,255,txt);
            }

            if (curx == 4 && cury == n) COLORFONDO = 252; else COLORFONDO = 255;
            mask_putcad(lastx,n*2+10,255,"TAPE ");

            redrawmask = 0;
        }// redraw

        if (!program_mode)
        {
            if (old_key & JOY_BUTTON_L) {redrawmask = 1;}
            if (old_key & JOY_BUTTON_R) {redrawmask = 1;}
        }

        mask_out(4*8,9*8);
        //fill_mask(4*8,9*8,32*8,11*8);

        flip++;
        COLORFONDO = 128;
        if (cury == 4 && !program_mode && !menu_mode)
        {
            if (new_key & JOY_BUTTON_B)
            {
                program_mode = 0;redrawmask = 1;
                if (curx == 0) {sub_men = 0;menu_mode = 1;} //ZX_Reset(ZX_48);
                if (curx == 1) autoload = 1;
                if (curx == 2) {mJoystick++;if (mJoystick>5) mJoystick = 0;}
                if (curx == 3)
                {
                    for(n = 0; n<nmultikey; n++) ZXKey(multiple_key[n]);
                    nmultikey = 0;
                }
                if (curx == 4) {
                    tape_browser();
                    keyboard_on = 0;
                }
            }
            return -1;
        }
    }

    if (menu_mode == 1)
    {
        //fill_mask(4*8,9*8,32*8,11*8);
        if (redrawmask)
        {
            memset(mask_keyb,0,32*8*11*8);
            COLORFONDO = 254;
            mask_putcad(7,12,254,"SELECT SPECTRUM EMULATION");
            if (sub_men == 0) COLORFONDO = 252; else COLORFONDO = 255;
            mask_putcad(6,14,255,"48K ");
            if (sub_men == 1) COLORFONDO = 252; else COLORFONDO = 255;
            mask_putcad(lastx,14,255,"128K ");
            if (sub_men == 2) COLORFONDO = 252; else COLORFONDO = 255;
            mask_putcad(lastx,14,255,"PLUS2 ");
            if (sub_men == 3) COLORFONDO = 252; else COLORFONDO = 255;
            mask_putcad(lastx,14,255,"PLUS2A ");
            if (sub_men == 4) COLORFONDO = 252; else COLORFONDO = 255;
            mask_putcad(lastx,14,255,"PLUS3");
            redrawmask = 0;
        }// redraw

        mask_out(4*8,9*8);

        if (new_key & JOY_BUTTON_LEFT) {redrawmask = 1;sub_men--;if (sub_men<0) sub_men = 4;}
        if (new_key & JOY_BUTTON_RIGHT) {redrawmask = 1;sub_men++;if (sub_men>4) sub_men = 0;}
        if (new_key & JOY_BUTTON_X) {redrawmask = 1;menu_mode = 0;}
        if (new_key & JOY_BUTTON_B)
        {
            redrawmask = 1;
            switch(sub_men)
            {
                case 1:ZX_Reset(ZX_128);break;
                case 2:ZX_Reset(ZX_PLUS2);break;
                case 3:ZX_Reset(ZX_PLUS2A);break;
                case 4:ZX_Reset(ZX_PLUS3);dsk_load((void *) DSK);break;
                default:ZX_Reset(ZX_48);break;
            }
            menu_mode = 0;
        }
        return -1;
    }
    ret = keyb_num[cury][curx];
    return ret;
}

#ifdef USE_ZLIB

struct zip_files
{
    char file[256];
} zip_files[MAX_ENTRY];

int zip_load(char *name)
{

    unzFile uf;
    unz_global_info gi;
    unz_file_info file_info;

    int n,m,idx;
    int err;

    uf = unzOpen(name);
    if(uf == NULL) return -1;

    err = unzGetGlobalInfo (uf,&gi);

    if (err!=UNZ_OK || gi.number_entry<=0 )
    {
    	unzClose(uf);
    	return -2;
    }
    m = gi.number_entry;
    ////seleccionar

    int  zip_pos_file[MAX_ENTRY];
    int  zip_num_files = 0;
    int y = 0;
    char filename_inzip[256];

    for(idx = 0;idx<m;idx++)
    {

    	err = unzGetCurrentFileInfo(uf,&file_info,&filename_inzip[0],256,NULL,0,NULL,0);

        if (err!=UNZ_OK)
        {
            continue;
        }

        if ((

        	  is_ext(filename_inzip,".z80") ||
              is_ext(filename_inzip,".sna") ||
              is_ext(filename_inzip,".tzx") ||
              is_ext(filename_inzip,".tap") ||
              is_ext(filename_inzip,".sp")  ||
              is_ext(filename_inzip,".dsk")

            ))
        {

        	strcpy(zip_files[zip_num_files].file,&filename_inzip[0]);

            zip_pos_file[zip_num_files] = idx;
            zip_num_files++;
        }

        if ((idx+1)<gi.number_entry)
        {
            err = unzGoToNextFile(uf);
            if (err!=UNZ_OK)
            {
                break;
            }
        }
    }

    char cad[256];
    int posfile = 0;
    unsigned new_key = 0,old_key = 0;
    int f = 0;

    if (zip_num_files != 1)
    {
        COLORFONDO = 128;
        dump_video();
        while(nKeys & JOY_BUTTON_B) nKeys = joystick_read(); // para quieto!!
        while(1)
        {
            ClearScreen(COLORFONDO);
            sprintf(cad,"ZIP LIST (%u)",zip_num_files);
            v_putcad(0,1,132,cad);
            m = 0;

            for(n = posfile-10;n<posfile+24;n++)
            {
                if (n<0) continue;
                if (n>= zip_num_files) break;
                if (m>23) break;
                if (n == posfile) COLORFONDO = 129; else COLORFONDO = 134;
                v_breakcad = 38;v_forcebreakcad = 1;
                v_putcad(1,m+3,130,zip_files[n].file);
                v_breakcad = 40; v_forcebreakcad = 0;
                COLORFONDO = 128;
                m++;
            }

            for(;m<24;n++)//lo que falta
            {
                COLORFONDO = 134;
                v_breakcad = 38;v_forcebreakcad = 1;
                v_putcad(1,m+3,130," ");
                v_breakcad = 40;v_forcebreakcad = 0;
                COLORFONDO = 128;
                m++;
            }
            v_putcad(1,28,132,"Press B to Select ZIP Entry");

        /*
            int y = 6;
            v_putcad(30,y,132,"Press A/X");y += 2;
            v_putcad(30,y,132,"to Play");y += 2;
            */

            SyncFreq2();
            dump_video();

            nKeys = joystick_read();
            new_key = nKeys & (~old_key);
            old_key = nKeys;

            if (!(old_key & (JOY_BUTTON_UP | JOY_BUTTON_DOWN))) f = 0;
            if (old_key & JOY_BUTTON_UP )
            {
                if (f == 0) f = 2;
                else if (f & 1) f = 2;
                else {f += 2;if (f>40) {f = 34; new_key |= JOY_BUTTON_UP;}}
            }
            if (old_key & JOY_BUTTON_DOWN)
            {
                if (f == 0) f = 1;
                else if (!(f & 1)) f = 1;
                else {f += 2;if (f>41) {f = 35; new_key |= JOY_BUTTON_DOWN;}}
            }

            if (new_key & JOY_BUTTON_B) break;

            if ((new_key & JOY_BUTTON_L) || (new_key & JOY_BUTTON_LEFT)) {posfile -= 25;if (posfile<0) posfile = 0;}
            if ((new_key & JOY_BUTTON_R) || (new_key & JOY_BUTTON_RIGHT)) {posfile += 25;if (posfile>= zip_num_files) posfile = zip_num_files-1;}

            if ((new_key & JOY_BUTTON_UP)) {posfile--;if (posfile<0) posfile = zip_num_files-1;}
            if ((new_key & JOY_BUTTON_DOWN)){posfile++;if (posfile>= zip_num_files) posfile = 0;}
        }
    }
    else
    {
        posfile = 0;
    }



    if (unzLocateFile(uf,zip_files[posfile].file,0)!=UNZ_OK)
    {
    	unzClose(uf);
    	return -2;
    }

	err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
    if (err!=UNZ_OK)
    {
    	unzClose(uf);
    	return -2;
    }

    GAME_size = file_info.uncompressed_size;
    err = unzOpenCurrentFile(uf);
    if (err!=UNZ_OK)
    {
         unzClose(uf);
         return -2;
    }

    m = 0;
    while(1)
    {
        n = unzReadCurrentFile(uf,GAME+m,GAME_size-m);
        if (n>0) m += n; else break;
    }
    if (n<0 || m != GAME_size) GAME_size = 0;

    unzCloseCurrentFile(uf);
    unzClose(uf);
}

#endif

#ifdef USE_ZIP

int zip_load(char *name)
{
    struct zip *za;
    struct zip_file * fp;
    struct zip_stat st;

    int n,m,idx;
    int err;

    if ((za = zip_open(name, 0, &err)) ==  NULL)     return -1;
    m =  zip_get_num_files(za);
    if (m<= 0) {zip_close(za);return -2;}

    ////seleccionar

    char *zip_files[MAX_ENTRY];
    int  zip_pos_file[MAX_ENTRY];
    int  zip_num_files = 0;
    int y = 0;

    for(idx = 0;idx<m/*,zip_num_files<MAX_ENTRY*/;idx++)
    {
        zip_stat_index(za, idx , 0, &st);
        if (( is_ext(st.name,".z80") ||
              is_ext(st.name,".sna") ||
              is_ext(st.name,".tzx") ||
              is_ext(st.name,".tap") ||
              is_ext(st.name,".sp")  ||
              is_ext(st.name,".dsk")
            ))
        {
            zip_files[zip_num_files] = (char *)st.name;
            //zip_files[zip_num_files] = "kk";
            zip_pos_file[zip_num_files] = idx;
            zip_num_files++;

            //strcpy(&zip_files[zip_num_files++][0],st.name);
            //strcpy(kk,st.name);
        }
    }


    char cad[256];
    int posfile = 0;
    unsigned new_key = 0,old_key = 0;
    int f = 0;

    if (zip_num_files != 1)
    {
        COLORFONDO = 128;
        dump_video();
        while(nKeys & JOY_BUTTON_B) nKeys = joystick_read(); // para quieto!!
        while(1)
        {
            ClearScreen(COLORFONDO);
            sprintf(cad,"ZIP LIST (%u)",zip_num_files);
            v_putcad(0,1,132,cad);
            m = 0;

            for(n = posfile-10;n<posfile+24;n++)
            {
                if (n<0) continue;
                if (n>= zip_num_files) break;
                if (m>23) break;
                if (n == posfile) COLORFONDO = 129; else COLORFONDO = 134;
                v_breakcad = 38;v_forcebreakcad = 1;
                v_putcad(1,m+3,130,zip_files[n]);
                v_breakcad = 40; v_forcebreakcad = 0;
                COLORFONDO = 128;
                m++;
            }

            for(;m<24;n++)//lo que falta
            {
                COLORFONDO = 134;
                v_breakcad = 38;v_forcebreakcad = 1;
                v_putcad(1,m+3,130," ");
                v_breakcad = 40;v_forcebreakcad = 0;
                COLORFONDO = 128;
                m++;
            }
            v_putcad(1,28,132,"Press B to Select ZIP Entry");

        /*
            int y = 6;
            v_putcad(30,y,132,"Press A/X");y += 2;
            v_putcad(30,y,132,"to Play");y += 2;
            */

            SyncFreq2();
            dump_video();

            nKeys = joystick_read();
            new_key = nKeys & (~old_key);
            old_key = nKeys;

            if (!(old_key & (JOY_BUTTON_UP | JOY_BUTTON_DOWN))) f = 0;
            if (old_key & JOY_BUTTON_UP )
            {
                if (f == 0) f = 2;
                else if (f & 1) f = 2;
                else {f += 2;if (f>40) {f = 34; new_key |= JOY_BUTTON_UP;}}
            }
            if (old_key & JOY_BUTTON_DOWN)
            {
                if (f == 0) f = 1;
                else if (!(f & 1)) f = 1;
                else {f += 2;if (f>41) {f = 35; new_key |= JOY_BUTTON_DOWN;}}
            }

            if (new_key & JOY_BUTTON_B) break;

            if ((new_key & JOY_BUTTON_L) || (new_key & JOY_BUTTON_LEFT)) {posfile -= 25;if (posfile<0) posfile = 0;}
            if ((new_key & JOY_BUTTON_R) || (new_key & JOY_BUTTON_RIGHT)) {posfile += 25;if (posfile>= zip_num_files) posfile = zip_num_files-1;}

            if ((new_key & JOY_BUTTON_UP)) {posfile--;if (posfile<0) posfile = zip_num_files-1;}
            if ((new_key & JOY_BUTTON_DOWN)){posfile++;if (posfile>= zip_num_files) posfile = 0;}
        }
    }
    else
    {
        posfile = 0;
    }

    ////seleccionar
    idx = zip_pos_file[posfile];

    //if (idx == m) {zip_close(za);return -2;}

    zip_stat_index(za, idx , 0, &st);

    GAME_size = st.size;
    //st.crc;

    fp = zip_fopen_index(za, idx,0);

    m = 0;
    while(1)
    {
        n = zip_fread(fp,GAME+m,GAME_size-m);
        if (n>0) m += n; else break;
    }
    if (n<0 || m != GAME_size) GAME_size = 0;
    zip_fclose(fp);
    zip_close(za);
}

#endif


void load_empty_dsk()
{
    int len_d = 1*1024*1024;

    BZ_API(BZ2_bzBuffToBuffDecompress) ((void *)DSK, (void *)&len_d,(void *)empty_dsk, size_empty_dsk,0,0);
}

int load_game(char *name)
{
    FILE *fp;

    GAME_size = 0;

    MY_filename = name;
    fp = fopen(name,"rb");
    if (fp == NULL) return 1;


#if defined( USE_ZIP) || defined( USE_ZLIB)
    if (is_ext (name, ".zip"))
    {
        zip_load(name); return 0;
    }
#endif
    if (is_ext (name, ".bz2"))
    {
        BZFILE * my_bzip;
        int bzip_err = 0;
        int n,m;

        my_bzip = BZ2_bzReadOpen( &bzip_err, fp, 0, 1, NULL, 0 );

        GAME_size = BZ2_bzRead (&bzip_err, my_bzip, GAME, 1*1024*1024);

        BZ2_bzReadClose (&bzip_err, my_bzip);
        fclose(fp);
        return 0;
    }
    fseek(fp,0,SEEK_END);
    GAME_size = ftell(fp);
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
    int bzip_err = 0;
    char *temp;
    FILE *fp, *fp_bzip;
    strcpy(filename,name);

    temp = malloc(1024*2048);
    if (!temp) return -3;

    strcat(filename,".bz2");

    fp = fopen(name, "rb");
    if (fp ==  NULL) {free(temp);return -1;}

    fp_bzip = fopen(filename, "wb");
    if (fp_bzip ==  NULL) {free(temp);fclose(fp);return -2;}

    fseek(fp,0,SEEK_END);
    m = ftell(fp);
    fseek(fp,0,SEEK_SET);
    if (m>(1024*2048)) {free(temp);fclose(fp);fclose(fp_bzip);return -3;}
    n = fread(temp,1,m,fp);
    fclose(fp);
    if (n<m) {free(temp);fclose(fp_bzip);return -4;}

    my_bzip = BZ2_bzWriteOpen( &bzip_err, fp_bzip, 4, 0,30);
    if (bzip_err != BZ_OK)  {free(temp);fclose(fp_bzip);return -5;}

    BZ2_bzWrite (&bzip_err, my_bzip, temp,  m);
    i = bzip_err;
    BZ2_bzWriteClose (&bzip_err, my_bzip,0,(void*)&n,(void*)&m);
    fclose(fp_bzip);
    free(temp);
    sync();
    if (i != BZ_OK || bzip_err != BZ_OK)
    {
        remove(filename);
        return -5;
    }
    else remove(name);

    return 0;
}

#define SIZEOFZ80REGS sizeof(Z80Regs)

t_track  track_temp;
#define RESTA_PUNT(a) if ((int)a != 0)  a -= (unsigned) DSK; else a = (void*)/*(unsigned)*/ -1
#define SUMA_PUNT(a) if ((int) a != -1)  a += (unsigned) DSK; else a = (void*)/*(unsigned)*/ 0
int save_state(int st)
{
    FILE * fp; unsigned char *mem;
    int count,n,m;
    char *mname;
    char savefile[256];
    char old_savefile[256];
    BZFILE * my_bzip;
    int bzip_err = 0;
    int file_error = 0;

    mname = get_name(MY_filename);
    // obten nombre sin extension
    n = 0;while(mname[n] != 0) n++;
    while(n>0) {if (mname[n] == '.') {mname[n] = 0;break;} n--;}

	sprintf(photo_name,"%s/saves/%s.tmp",globalpath,mname);
    sprintf(savefile,"%s/saves/%s.sav",globalpath,mname);
    sprintf(old_savefile,"%s/saves/%s-old.sav",globalpath,mname);
    fp = fopen(photo_name,"wb");
    if (fp == NULL) return 0;

    my_bzip = BZ2_bzWriteOpen( &bzip_err, fp, 4, 0,30);
    if (bzip_err != BZ_OK)  {fclose(fp);return 0;}

    mem = (unsigned char*) malloc(16384*16+SIZEOFZ80REGS+100);
    if (mem == NULL) {BZ2_bzWriteClose (&bzip_err, my_bzip,0,(void*)&n,(void*)&m);fclose(fp);return 0;}

    state_header.header = 0x12345678;

    state_header.mJoystick = mJoystick;
    map_keys[10] = 0;
    map_keys[11] = 0;
    map_keys[12] = 0;
    map_keys[13] = 0;
    for(n = 0;n<14;n++) state_header.map_keys[n] = map_keys[n];
    if (!is_ext(MY_filename,".sav")) strcpy((void*)state_header.filename,MY_filename);

    if (model == ZX_PLUS3) state_header.have_fd_info = 1; else state_header.have_fd_info = 0;


    if(zx_ula64_enabled)
    	strcpy((void*)state_header.ulaplus,"ula64");
    else
    	strcpy((void*)state_header.ulaplus,"nope");

    BZ2_bzWrite (&bzip_err, my_bzip, (void *)&state_header,  sizeof(state_header));


    if (bzip_err != BZ_OK) {file_error = 1;}
    else
    {
        if(zx_ula64_enabled)
        {
        	  BZ2_bzWrite (&bzip_err, my_bzip, (void *)&zx_ula64_palette,64);
        }

    	if (state_header.have_fd_info == 1)
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
            if (active_track) memcpy(&track_temp,active_track,sizeof (t_track)); else memset(&track_temp,0,sizeof (t_track));
            RESTA_PUNT(track_temp.data);
            for(n = 0;n<track_temp.sectors;n++)
            {
                RESTA_PUNT(track_temp.sector[n].data);
            }
            BZ2_bzWrite (&bzip_err, my_bzip, (void *) &track_temp,  sizeof (t_track));
            //if (bzip_err != BZ_OK) {file_error = 1;}
        }
        if (!file_error)
        {
            count = ZX_SaveState(mem);

            BZ2_bzWrite (&bzip_err, my_bzip, (void *)mem,  count);
            if (bzip_err != BZ_OK) {file_error = 1;}
        }
    }

    BZ2_bzWriteClose (&bzip_err, my_bzip,0,(void*)&n,(void*)&m);
    if (bzip_err != BZ_OK) {file_error = 1;}
    fclose(fp);
    if (!file_error)
    {

        //remove(savefile);
    	remove(old_savefile);
    	rename(savefile,old_savefile);
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
    int  ula64 = 0;
    byte ula64Colors[64];

    char *mname;

    BZFILE * my_bzip;
    int bzip_err = 0;
    int n;
    mname = get_name(MY_filename);
    // obten nombre sin extension
    n = 0;while(mname[n] != 0) n++;
    while(n>0) {if (mname[n] == '.') {mname[n] = 0;break;} n--;}

	sprintf(photo_name,"%s/saves/%s.sav",globalpath,mname);

    fp = fopen(photo_name,"rb");
    if (fp == NULL) return 0;

    my_bzip = BZ2_bzReadOpen( &bzip_err, fp, 0, 1, NULL, 0 );

    size = BZ2_bzRead (&bzip_err, my_bzip, (void *)&state_header, sizeof(state_header));

    if (state_header.header != 0x12345678) {BZ2_bzReadClose (&bzip_err, my_bzip);fclose(fp);return 0;}

    if(state_header.ulaplus[0]=='u' && state_header.ulaplus[1]=='l' && state_header.ulaplus[2]=='a'
       && state_header.ulaplus[3]=='6' && state_header.ulaplus[4]=='4')
    {
    	ula64=1;
    	BZ2_bzRead (&bzip_err, my_bzip, (void *)&ula64Colors, 64);
    }

    if (state_header.have_fd_info == 1)
    {
        BZ2_bzRead (&bzip_err, my_bzip, (void *) &disk_temp.current_track,  4);
        BZ2_bzRead (&bzip_err, my_bzip, (void *) &disk_temp.current_side,  4);
        BZ2_bzRead (&bzip_err, my_bzip, (void *) &disk_temp.current_sector,  4);
        BZ2_bzRead (&bzip_err, my_bzip, (void *) &disk_temp.flipped,  4);
        BZ2_bzRead (&bzip_err, my_bzip, (void *) &FDC_temp,  sizeof (t_FDC));
        BZ2_bzRead (&bzip_err, my_bzip, (void *) &track_temp,  sizeof (t_track));
        SUMA_PUNT(track_temp.data);
        for(n = 0;n<track_temp.sectors;n++)
        {
            SUMA_PUNT(track_temp.sector[n].data);
        }
    }

    mem = (unsigned char*) malloc(16384*16+SIZEOFZ80REGS+100);
    if (mem == NULL) {BZ2_bzReadClose (&bzip_err, my_bzip);fclose(fp);return 0;}

    size = BZ2_bzRead (&bzip_err, my_bzip, (void *)mem, 16384*16+SIZEOFZ80REGS+100);

    BZ2_bzReadClose (&bzip_err, my_bzip);fclose(fp);
    mJoystick = state_header.mJoystick;
    for(n = 0;n<12;n++) map_keys[n] = state_header.map_keys[n];
    if (map_keys[8] == 0) map_keys[8] = SPECKEY_SHIFT;
    if (map_keys[9] == 0) map_keys[9] = SPECKEY_SYMB;
    if (map_keys[10] == 0) map_keys[10] = DEFAULT_SPEED_NOFS;
    if (map_keys[11] == 0) map_keys[11] = DEFAULT_SPEED_FS1;
    //gp2x_speed = map_keys[10]; // no cambiamos la velocidad para evitar cagarla y mantener compatilidad con savestates de la GP2X
    //mconfig.cpu_freq = gp2x_speed;
    //set_speed_clock(gp2x_speed);
    MY_filename = (char*)state_header.filename ;

#ifdef DEBUG_MSG
    printf("Load State:\n%s\n",state_header.filename);
#endif

    memset(DSK,0,1*1024*1024);
    load_empty_dsk(); // disco comprimido formateado por defecto

    if (!is_ext((char*)state_header.filename,".sav"))
        load_game((char*)state_header.filename );
    else {load_game("fake_name.fak");}
    ZX_LoadGame(ZX_128, /*MyGameCRC*/0, 0/*mQuick*/);

    {
        int code = 0;
        code = ZX_LoadState(mem);

        free(mem);

        if(ula64)
        {
        	//if((!zx_ula64_enabled  && mconfig.ula64!=0)|| ( mconfig.ula64==1))
        	if(!(zx_ula64_enabled  && mconfig.ula64==2))//if a save state has ula i put it but hw ula could be disabled
        	{
        		memcpy(zx_ula64_palette,ula64Colors,64);
        		zx_ula64_enabled = 1;
        		set_emupalette();
        	}
        }

        if (state_header.have_fd_info == 1)
        {
            dsk_load(DSK);
            SUMA_PUNT(FDC_temp.buffer_ptr);
            SUMA_PUNT(FDC_temp.buffer_endptr);
            FDC_temp.cmd_handler = FDC.cmd_handler;
            FDC = FDC_temp;
            driveA.current_track = disk_temp.current_track;
            driveA.current_side = disk_temp.current_side;
            driveA.current_sector = disk_temp.current_sector;
            driveA.flipped = disk_temp.flipped;

            active_track = &track_temp;
        }
        return (code?1:0);
    }
}


int
tape_browser()
{
    char cad[256];
    int posfile = 0;
    unsigned new_key = 0,old_key = 0;
    int n = 0;
    int m = 0;
    int f = 0;

    int max_blocks = 1024;
    char block_entries[max_blocks][256];
    int num_entries = tape_blocks_entries(block_entries,38);

    posfile = tape_get_current_block();

    COLORFONDO = 128;
    dump_video();
    while(nKeys & JOY_BUTTON_B) nKeys = joystick_read(); // para quieto!!
    while(1)
    {
        ClearScreen(COLORFONDO);//speccy_corner();
        sprintf(cad,"TAPE BLOCK LIST (%u) TAPE:%s",num_entries,tape_playing?"ON":"OFF");
        v_putcad(0,1,132,cad);
        m = 0;

        for(n = posfile-10;n<posfile+22;n++)
        {
            if (n<0) continue;
            if (n>= num_entries) break;
            if (m>21) break;
            int col = 130;
            if (n == tape_get_current_block())
            col = tape_playing ? 132 : 131 ;

            if (n == posfile) COLORFONDO = 129;
            else COLORFONDO = 134;
            v_breakcad = 38;v_forcebreakcad = 1;
            v_putcad(1,m+3,col,block_entries[n]);
            v_breakcad = 40;v_forcebreakcad = 0;
            if (n == tape_get_current_block()) v_putcad(6,m+3,col,"*");
            COLORFONDO = 128;
            m++;
        }

        for(;m<22;n++)//lo que falta
        {
            COLORFONDO = 134;
            v_breakcad = 38;v_forcebreakcad = 1;
            v_putcad(1,m+3,130," ");
            v_breakcad = 40;v_forcebreakcad = 0;
            COLORFONDO = 128;
            m++;
        }

        v_putcad(1,26,132,"Press X to Exit, A to New Tape");
        v_putcad(1,28,132,"B to Toggle Tape Play or Change Block");

        SyncFreq2();
        dump_video();

        nKeys = joystick_read();
        new_key = nKeys & (~old_key);
        old_key = nKeys;

        if (new_key & JOY_BUTTON_B)
        {
            if (posfile == tape_get_current_block())
            {
               tape_toggle_play(1);//cuidado que avanza
               if (posfile == tape_get_current_block())
                    tape_select_block(posfile);
            }
            else
                tape_select_block(posfile);
        }

        if (new_key & JOY_BUTTON_X) break;

        if (new_key & JOY_BUTTON_A)
        {
            int r = get_rom(1);
            while(nKeys & (JOY_BUTTON_B | JOY_BUTTON_X)) nKeys = joystick_read(); // para quieto!!
            if (r == -1) continue;
            int ret = 0;
            ret = load_game(files[r].file);
            char *mname = get_name(MY_filename);
            // obten nombre sin extension
            n = 0;while(mname[n] != 0) n++;
            while(n>0) {if (mname[n] == '.') {mname[n] = 0;break;} n--;}
			    sprintf(photo_name,"%s/saves/%s.key",globalpath,mname);
            read_keyfile(photo_name);
            ZX_LoadGame(-1,0,0);
            while(nKeys & (JOY_BUTTON_B | JOY_BUTTON_X)) nKeys = joystick_read(); // para quieto!!
            num_entries = tape_blocks_entries(block_entries,38);
            posfile = tape_get_current_block();
            //TODO chequear esto
        }

        if ((new_key & JOY_BUTTON_L) || (new_key & JOY_BUTTON_LEFT)) {posfile -= 24;if (posfile<0) posfile = 0;}
        if ((new_key & JOY_BUTTON_R) || (new_key & JOY_BUTTON_RIGHT)) {posfile += 24;if (posfile>= num_entries) posfile = num_entries-1;}

        if (!(old_key & (JOY_BUTTON_UP | JOY_BUTTON_DOWN))) f = 0;
        if (old_key & JOY_BUTTON_UP )
        {
           if (f == 0) f = 2;
           else if (f & 1) f = 2;
           else {f += 2;if (f>40) {f = 34; new_key |= JOY_BUTTON_UP;}}
        }
        if (old_key  & JOY_BUTTON_DOWN)
        {
           if (f == 0) f = 1;
           else if (!(f & 1)) f = 1;
           else {f += 2;if (f>41) {f = 35; new_key |= JOY_BUTTON_DOWN;}}
        }

        if ((new_key & JOY_BUTTON_UP)) {posfile--;if (posfile<0) posfile = num_entries-1;}
        if ((new_key & JOY_BUTTON_DOWN)){posfile++;if (posfile>= num_entries) posfile = 0;}
    }

    while(nKeys & (JOY_BUTTON_A | JOY_BUTTON_B | JOY_BUTTON_X | JOY_BUTTON_Y | JOY_BUTTON_MENU)) nKeys = joystick_read();
}


unsigned oldtime = 0;
unsigned fpstime = 0;
unsigned autoskiptime = 0;

unsigned prev_measure=0,this_frame_base,prev;
static int speed = 100;
unsigned curr,last=0;
int frameskipadjust = 0;

////////////////////////////
//NEW AUTO SYNC STUFF

#define FRAMESKIP_LEVELS 12
#define FRAMESKIP_FACTOR 1

int frameskip = 0;//Nos indica el nivel de frameskip
static int frameskip_counter = 0;//nos indica el skip actual

int skip_this_frame(void)
{
	static const int skiptable[FRAMESKIP_LEVELS][FRAMESKIP_LEVELS] =
	{
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,1 },
		{ 0,0,0,0,0,1,0,0,0,0,0,1 },
		{ 0,0,0,1,0,0,0,1,0,0,0,1 },
		{ 0,0,1,0,0,1,0,0,1,0,0,1 },
		{ 0,1,0,0,1,0,1,0,0,1,0,1 },
		{ 0,1,0,1,0,1,0,1,0,1,0,1 },
		{ 0,1,0,1,1,0,1,0,1,1,0,1 },
		{ 0,1,1,0,1,1,0,1,1,0,1,1 },
		{ 0,1,1,1,0,1,1,1,0,1,1,1 },
		{ 0,1,1,1,1,1,0,1,1,1,1,1 },
		{ 0,1,1,1,1,1,1,1,1,1,1,1 }
	};
	return skiptable[frameskip][frameskip_counter /* % 12*/];
}

void presync()
{

	float t_frame = (1000.0/50.0) * (100.0/(float)mconfig.speed_mode);

	if (prev_measure==0)
	{
		prev_measure = getTicks() - (FRAMESKIP_LEVELS * FRAMESKIP_FACTOR) * (int)t_frame;
		last = getTicks();
	}

	if (frameskip_counter == 0)
		this_frame_base = prev_measure + (FRAMESKIP_LEVELS * FRAMESKIP_FACTOR)* (int)t_frame;

	curr = getTicks();
	if ((curr - last) > 300)
	{
		frameskip_counter = 0;
		frameskip = 0;
		prev_measure = 0;
		return;
	}
	last = curr;

	if (skip_this_frame() == 0)
	{
		unsigned target;

		if ( mconfig.sound_mode == 0 && !(mconfig.speed_loading && tape_playing))
		{
			target = this_frame_base + frameskip_counter * (int)t_frame;
			if ((curr < target) && (target-curr<1000))
			{
				do
				{
					curr = getTicks();
				} while ((curr < target) && (target-curr<1000));
			}
		}
		if (frameskip_counter == 0)
		{
			float divdr;
			//divdr = (float)((float)((float)(1000) / 50) * (float)(FRAMESKIP_LEVELS * FRAMESKIP_FACTOR)) / (float)(curr - prev_measure);
			divdr = (float)((float) t_frame * (float)(FRAMESKIP_LEVELS * FRAMESKIP_FACTOR)) / (float)(curr - prev_measure);
			speed = (int)(divdr * 100.0);

			//int divdr;
			//divdr = 50 * (curr - prev_measure) / (100 * FRAMESKIP_LEVELS * FRAMESKIP_FACTOR);
			//speed = (1000 + divdr/2) / divdr;

			prev_measure = curr;
		}

		prev = curr;

		if(frameskip_counter == 0)
		{
			if (speed >= 98)
			{
				frameskipadjust++;
				if (frameskipadjust >= 3)
				{
					frameskipadjust = 0;

					if (frameskip > 0) frameskip--;
				}
			}
			else
			{
				if (speed < 87)
				{
					frameskipadjust -= (90 - speed) / 5;
				}
				else
				{

					if (frameskip < 8)
						frameskipadjust--;
				}

				while (frameskipadjust <= -2)
				{
					frameskipadjust += 2;
					if (frameskip < FRAMESKIP_LEVELS-1) frameskip++;
				}
			}
		}
	}


	frameskip_counter = (frameskip_counter + 1) % (FRAMESKIP_LEVELS * FRAMESKIP_FACTOR);
}

////////////////////////////


#if defined(IPHONE)
int iphone_main(int argc, char *argv[])
#elif defined(ANDROID)
int android_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    int i,r,n,ret;
    int dclock;
    int sel_key = 0;
    int fd;

    unsigned time;
    int nvol = 0;
    Z80Regs regs_z80;
    char *mname;
    FILE *fp;

    int count_fps = 0;
    int fpsseg = 0;
    long tape_stop_delay = 0;

    int count_fps_draw = 0;
    int fpsseg_draw = 0;

#ifdef DEBUG_MSG
    printf("Iniciando xpectrum\n");
#endif

    microlib_init();

    set_emupalette();

    cur_frame = 0;

    numKeys = 0;
    isShiftKey  = 0;
    isSymbolKey = 0;

    MY_filename = "default_name.fak";
    spectrumZ80 = &regs_z80;

    //load_mconfig();//error no esta puesto el global path
    //mount("-t vfat -o remount,sync,iocharset = utf8 /dev/mmcsd/disc0/part1 /mnt/sd");


	char pathstring[512];
#ifndef ANDROID
    mkdir(globalpath, S_IREAD | S_IWRITE | S_IEXEC );
#endif
    sprintf(pathstring,"%s/saves/",globalpath);
#ifndef ANDROID
    mkdir(pathstring, S_IREAD | S_IWRITE | S_IEXEC );
#endif
#ifdef CAPTURE
    sprintf(pathstring,"%s/img/",globalpath);
    mkdir(pathstring, S_IREAD | S_IWRITE | S_IEXEC );
#endif

	sprintf(actual_roms_dir,"%s",globalpath);

    credits();
    //volume = 50;

    load_mconfig();

    sound_open(mconfig.sound_freq,16,mconfig.sound_mode >=  2);
    sound_volume(volume,volume);//FIX sin sonido se arranca no se pone el volumen

    if(mconfig.sound_mode==0)
    	sound_close();

    tape_init();

    ZX_Init();



    while(1)
    {
        while(nKeys & (JOY_BUTTON_A | JOY_BUTTON_B | JOY_BUTTON_X | JOY_BUTTON_Y | JOY_BUTTON_MENU)) nKeys = joystick_read();
        memset(DSK,0,1*1024*1024);
        load_empty_dsk(); // disco comprimido formateado por defecto
        nvol = 0;
        dsk_flipped = 0;
        r = get_rom(0);
        ret = 0;

        if (r == -1)
        {
#if !defined(IPHONE) && !defined(ANDROID)
        break;
#else
        tape_init();
        ZX_Reset(ZX_128);
        ret=666;
#endif
        }

        if (r>= 0)
        {
            if (is_ext (files[r].file, ".sav"))
            {
                MY_filename = files[r].file;
                load_state(0);
                ret = 666;
            }
            else
            {
                ret = load_game(files[r].file);
                mname = get_name(MY_filename);
                // obten nombre sin extension
                n = 0;
                while(mname[n] != 0) n++;
                while(n>0)
                {
                    if (mname[n] == '.')
                    {
                        mname[n] = 0;
                        break;
                    }
                    n--;
                }
		        sprintf(photo_name,"%s/saves/%s.key",globalpath,mname);
                read_keyfile(photo_name);
            }
        }

        // inicializa variables de teclado virtual
        keyboard_on = 0;
        keyboard_press = 0;
        autoload = 0;
        program_mode = 0;
        menu_mode = 0;
        redrawmask = 1;

        SyncFreq2();
        dump_video();

        ClearScreen(COLORFONDO);
//        set_emupalette();

        //GAME_size = MyGameSize;
        if (ret != 666) ZX_LoadGame(ZX_128, /*MyGameCRC*/0, 0/*mQuick*/);

        skip = 0;
        count_fps = 0;
        count_fps_draw = 0;
        emulating = 1;
        while(1)
        {
#ifdef  CAPTURE
            if (old_key &( JOY_BUTTON_VOLUP |JOY_BUTTON_SELECT))
            {
				char pathstring[512];
				sprintf(pathstring,"%s/img/ingame.bmp",globalpath);
                save_bmp(pathstring, video_screen8, 320-64*full_screen, 240-48*full_screen, 320);
            }
#endif
            Picture = video_screen8;
            full_screen = tape_playing ? 0 : mconfig.zx_screen_mode;
            //full_screen =  mconfig.zx_screen_mode;
            ZX_Frame(skip);

            Sound_Loop();

            if (keyboard_on == 0 && (nKeys & JOY_BUTTON_MENU))
            {
                //printf("Inicio llamada a ConfigSCR\n");
                //TODO sound pause!
            	emulating = 0;
            	if (Config_SCR() == 1) break;
            	//printf("Salida llamada a ConfigSCR\n");
            	emulating = 1;
                skip = 0;
                prev_measure = 0;
                frameskip = 0;
                frameskip_counter=0;
            }

            nKeys = joystick_read();

            new_key = nKeys & (~old_key);
            old_key = nKeys;
#if !defined(IPHONE) && !defined(ANDROID)
    	    if((old_key & JOY_BUTTON_VOLUP) && (nvol % 3 == 0))
	            {volume += 1; if (volume > 100) volume = 100; sound_volume(volume,volume); nvol = 50*5;}
	        if((old_key & JOY_BUTTON_VOLDOWN) && (nvol % 3 == 0))
			    {volume -= 1; if (volume < 0) volume = 0; sound_volume(volume,volume); nvol = 50*5;}

            if (nvol != 0)
            {
                int x = 0, y;

                char result[4];
                int tmp = COLORFONDO; /* if (!full_screen) COLORFONDO = 134; */

                COLORFONDO = 134;
                v_breakcad = 34;
                v_forcebreakcad = 1;
//                v_putcad(1,27,130," ");

                sprintf( result, "%3d", volume );
                v_putcad( 2,27,129,"VOLUME:");

                v_breakcad = 40;
                v_forcebreakcad = 0;
                v_putcad(35,27,129,result);

                for ( x = 0; x < volume; x++ )
                {
                    for ( y = 217; y < 223; y++ )
                    {
                        video_screen8[72+y*320+x/2*4  ] = 129;
                        video_screen8[72+y*320+x/2*4+1] = 129;
                        video_screen8[72+y*320+x/2*4+2] = 129;
                    }
                }
                COLORFONDO = tmp;
                nvol--;
            }
#endif
            if (mconfig.show_fps)
            {
                if (getTicks() - fpstime > 1000)
                {
                    fpsseg = count_fps;
                    count_fps = 0;
                    fpsseg_draw = count_fps_draw;
                    count_fps_draw = 0;
                    fpstime = getTicks();
                }
                count_fps++;

                char result[30];
                if (mconfig.frameskip==2)
                {
                	//sprintf( result, "FPS2: %d/%d", fpsseg, fpsseg_draw);
        			int fps;
        			char buf[30];
        			int divdr;
        			divdr = 100 * FRAMESKIP_LEVELS;
        			fps = (50 * (FRAMESKIP_LEVELS - frameskip) * speed + (divdr / 2)) / divdr;
        			sprintf(result,"%s%2d%4d%%%4d/%d fps",1?"auto":"fskp",frameskip,speed,fps,(int)(50+0.5));
                }
                else  if (mconfig.frameskip==1)
                {
                	sprintf( result, "FPS: %d/%d", fpsseg/2,fpsseg );
                }
                else
                    sprintf( result, "FPS: %d", fpsseg );

                int tmp = COLORFONDO; /* if (!full_screen) COLORFONDO = 134; */
                COLORFONDO = 134;
#if defined(IPHONE) || defined(ANDROID)
                v_putcad(4,3,129,result);
#else
                v_putcad(0,0,129,result);
#endif
                COLORFONDO = tmp;
            }

            cur_frame++;

            if (tape_playing && mconfig.speed_loading)
            {
                if (!init_speed_loading)
                {
                    init_speed_loading = 1;
                    sound_pause();
                }
                skip = !(cur_frame % 4 ==  0);//1 de 3 frames
            }
            else
            {
                if (init_speed_loading)
                {
                    init_speed_loading = 0;
                    skip = 0;
                    sound_unpause();
                }
                //skip ^= mconfig.frameskip;

                if(mconfig.frameskip == 2)
                {
                	//skip = !(cur_frame % 3 ==  0);
                	presync();
                	skip = skip_this_frame();
                    //if(!skip)count_fps_draw++;
                }
                else if (mconfig.frameskip == 1)
                {
                	skip = !(cur_frame % 2 ==  0);
                }
                else skip = 0;

            }

            if (tape_playing)
            {
                int tmp = COLORFONDO;

                if (fast_edge_loading != 2)
                {
                   COLORFONDO = 134;
#if defined(IPHONE) || defined(ANDROID)
                   v_putcad(320/8-9,240/8-5,132,"PLAY");
#else
                   v_putcad(320/8-5,240/8-2,132,"PLAY");
#endif
                }
                else
                {

                	COLORFONDO = 134;
#if defined(IPHONE) || defined(ANDROID)
                   v_putcad(320/8-9,240/8-5,129,"PLAY");
#else
                   v_putcad(320/8-5,240/8-2,129,"PLAY");
#endif
                }

                COLORFONDO = tmp;
                tape_stop_delay = 3000;
            }
            else if (tape_stop_delay >= 0)
            {
                tape_stop_delay  -=  20;
                int tmp = COLORFONDO;

                int x = full_screen ? 256 : 320;
                int y = full_screen ? 192 : 240;
                COLORFONDO = 134;
#if defined(IPHONE) || defined(ANDROID)
                v_putcad(x/8-9,y/8-5,131,"STOP");
#else
                v_putcad(x/8-5,y/8-2,131,"STOP");
#endif
                COLORFONDO = tmp;
            }

            if (new_key & JOY_BUTTON_SELECT)
            {
                keyboard_on ^= 1;
                program_mode = 0;
                menu_mode = 0;
            }

            //external keyboard
            int jj = 0;
            for(jj=0; jj< numKeys;jj++)
               ZXKey(keyboardKeys[jj]);

            if(isShiftKey != 0)
               ZXKey(SPECKEY_SHIFT);
            if(isSymbolKey != 0)
               ZXKey(SPECKEY_SYMB);

            if (keyboard_on)
            {
                if (old_key & JOY_BUTTON_B)
                {
                    ZXKey(sel_key);
                }
                if (autoload)
                {
                    if (autoload == 1 || autoload == 2)
                    {
                        ZXKey(SPECKEY_J);
                    }
                    if (autoload == 11 || autoload == 12 || autoload == 21|| autoload == 22)
                    {
                        ZXKey(SPECKEY_SYMB);
                        ZXKey(SPECKEY_P);
                    }
                    if (autoload == 30)
                    {
                        ZXKey(SPECKEY_ENTER);
                    }
                    if (autoload == 31)
                    {
                        ZXKey(SPECKEY_ENTER);
                        autoload = -1;
                    }
                    autoload++;
                }
            }
            else
            {
                //left/right/up/down/a,b,x,y
                switch (mJoystick)
                {
                    case 1:
                        //kempston
                        if (old_key & JOY_BUTTON_LEFT)  kempston |= 2;
                        if (old_key & JOY_BUTTON_RIGHT) kempston |= 1;
                        if (old_key & JOY_BUTTON_UP)    kempston |= 8;
                        if (old_key & JOY_BUTTON_DOWN)  kempston |= 4;
                        if (old_key & JOY_BUTTON_B)     kempston |= 16;
                        break;

                    case 2:
                        //Sinclair Joystick 1,Interface II (port 1)
                        if (old_key & JOY_BUTTON_LEFT)  ZXKey(SPECKEY_6);
                        if (old_key & JOY_BUTTON_RIGHT) ZXKey(SPECKEY_7);
                        if (old_key & JOY_BUTTON_UP)    ZXKey(SPECKEY_9);
                        if (old_key & JOY_BUTTON_DOWN)  ZXKey(SPECKEY_8);
                        if (old_key & JOY_BUTTON_B)     ZXKey(SPECKEY_0);
                        break;

                    case 3:
                        //Sinclair Joystick 2,Interface II (port 2)
                        if (old_key & JOY_BUTTON_LEFT)  ZXKey(SPECKEY_1);
                        if (old_key & JOY_BUTTON_RIGHT) ZXKey(SPECKEY_2);
                        if (old_key & JOY_BUTTON_UP)    ZXKey(SPECKEY_4);
                        if (old_key & JOY_BUTTON_DOWN)  ZXKey(SPECKEY_3);
                        if (old_key & JOY_BUTTON_B)     ZXKey(SPECKEY_5);
                        break;

                    case 4:
                        // Cursor
                        if (old_key & JOY_BUTTON_LEFT)  ZXKey(SPECKEY_5);
                        if (old_key & JOY_BUTTON_RIGHT) ZXKey(SPECKEY_8);
                        if (old_key & JOY_BUTTON_UP)    ZXKey(SPECKEY_7);
                        if (old_key & JOY_BUTTON_DOWN)  ZXKey(SPECKEY_6);
                        if (old_key & JOY_BUTTON_B)     ZXKey(SPECKEY_0);
                        break;

                    case 5:
                        //fuller
                        if (old_key & JOY_BUTTON_LEFT)  fuller &= 0xFF - 4;
                        if (old_key & JOY_BUTTON_RIGHT) fuller &= 0xFF - 8;
                        if (old_key & JOY_BUTTON_UP)    fuller &= 0xFF - 1;
                        if (old_key & JOY_BUTTON_DOWN)  fuller &= 0xFF - 2;
                        if (old_key & JOY_BUTTON_B)     fuller &= 0xFF - 128;
                        break;

                    default:
                        // keyboard
                        if (old_key & JOY_BUTTON_LEFT)  ZXKey(map_keys[0]);
                        if (old_key & JOY_BUTTON_RIGHT) ZXKey(map_keys[1]);
                        if (old_key & JOY_BUTTON_UP)    ZXKey(map_keys[2]);
                        if (old_key & JOY_BUTTON_DOWN)  ZXKey(map_keys[3]);
                        if (old_key & JOY_BUTTON_B)     ZXKey(map_keys[5]);
                        break;
                }

                if (old_key & JOY_BUTTON_A) ZXKey(map_keys[4]);
                if (old_key & JOY_BUTTON_X) ZXKey(map_keys[6]);
                if (old_key & JOY_BUTTON_Y) ZXKey(map_keys[7]);
            }
            if (old_key & JOY_BUTTON_L) ZXKey(map_keys[8]);
            if (old_key & JOY_BUTTON_R) ZXKey(map_keys[9]);

            if (keyboard_on)
            {
                if(ext_keyboard)
                	keyboard_on = 0;
                else
            	    sel_key = display_keyboard();
/*
                if ((old_key & JOY_BUTTON_Y)&&(unprogram == 0))
                {
                    ZXKey(sel_key);
                    keyboard_on = 0;
                    keyboard_press = 4;
                }
*/
            }
            else
            {
                if (keyboard_press)
                {
                    ZXKey(sel_key);
                    keyboard_press--;
                }
            }
/*
            if (unprogram)
            {
                unprogram--;
            }
*/

            if (skip == 0)
            {
            	  dump_video();
                //Seleuco: Esta temporizacion solamente se tiene que hacer si no se reproduce Audio. Para evitar underuns dejar que temporice el audio si existe sonido.
                if (mconfig.sound_mode == 0 && !(mconfig.speed_loading && tape_playing) && !(mconfig.frameskip == 2))
                {
                    SyncFreq();
                }

            }
        }

        sound_end();
    }

    save_mconfig();

    tape_finish();

    sound_volume(70,70);
    sound_close();

    microlib_end();

    return(0);
}
