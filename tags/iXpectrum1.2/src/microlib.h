/* 
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
   
   Copyright (c) 2008 Seleuco. This microlib is based in the findings of  wiki.gp2x.org, 
   GnoStiC, kounch, Hermes, Rlyeh, and others... thanks to all.
   
*/

#ifndef __MICROLIB_H__
#define __MICROLIB_H__

typedef struct _color{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}color_t;

typedef color_t palette_t[256] ;

void set_palette(palette_t palette);

unsigned long getTicks();

// joystick defines

enum  { JOY_BUTTON_UP=(1<<0),
        JOY_BUTTON_UPLEFT=(1<<1),
        JOY_BUTTON_LEFT=(1<<2),
        JOY_BUTTON_DOWNLEFT=(1<<3),
        JOY_BUTTON_DOWN=(1<<4),
        JOY_BUTTON_DOWNRIGHT=(1<<5),
        JOY_BUTTON_RIGHT=(1<<6),
        JOY_BUTTON_UPRIGHT=(1<<7),
        JOY_BUTTON_MENU=(1<<8),
        JOY_BUTTON_SELECT=(1<<9),
        JOY_BUTTON_L=(1<<10),
        JOY_BUTTON_R=(1<<11),
        JOY_BUTTON_A=(1<<12),
        JOY_BUTTON_B=(1<<13),
        JOY_BUTTON_X=(1<<14),
        JOY_BUTTON_Y=(1<<15),
        JOY_BUTTON_VOLUP=(1<<16),
        JOY_BUTTON_VOLDOWN=(1<<17),
        JOY_BUTTON_CLICK=(1<<18) 
      };

long joystick_read();

// video
extern char  *video_screen8;

void dump_video();

//init,end, Seleuco
void microlib_init();
void microlib_end();
 
//sound stuff, OSS, Seleuco.
extern int            sound_open(int rate, int bits, int stereo);
extern int            sound_close();
extern void           sound_volume(int, int);
extern int            sound_send(void *samples,int nsamples);

#endif
