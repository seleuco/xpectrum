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

#include <fcntl.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <unistd.h>
#include <usbjoy.h>


// usb joystick vars
unsigned long ExKey;
struct usbjoy * joys [4];
struct usbjoy * joy;
int num_of_joys;


// joystick defines
#define  GP2X_UP               0x1         
#define	 GP2X_LEFT             0x2         
#define	 GP2X_DOWN             0x4    
#define	 GP2X_RIGHT            0x8

#define  GP2X_START           0x10   
#define  GP2X_SELECT          0x20      
#define  GP2X_L               0x40    
#define  GP2X_R               0x80

#define  GP2X_A              0x100        
#define  GP2X_B              0x200          
#define  GP2X_X              0x400      
#define  GP2X_Y              0x800

#define  GP2X_VOL_UP        0x1000           
#define  GP2X_VOL_DOWN      0x2000 
#define  GP2X_PUSH          0x4000

// frame buffer
unsigned char  *gp2x_screen;

//init,end, Seleuco
void gp2x_init();
void gp2x_end();
 
//sound stuff, OSS, Seleuco.
extern int            gp2x_open_sound(int rate, int bits, int stereo);
extern int            gp2x_close_sound();
extern void           gp2x_volume(int, int);

//http://wiki.gp2x.org/wiki/Writing_to_the_framebuffer_device, Seleuco
//framebuffer stuff
extern void           gp2x_waitvsync(void);
extern void           gp2x_flip(void);
extern void           gp2x_noflip_buffering(void);

//palette stuff, Seleuco
//http://wiki.gp2x.org/wiki/Setting_the_8_bit_palette_from_Linux
extern void           gp2x_set_palette_color (int, int, int, int);
extern void           gp2x_set_palette(void);

//scaling stuff-tv-out, kounch, Rlyeh & wiki
//http://wiki.gp2x.org/wiki/TV-out_support
extern void           gp2x_set_scaling(int, int);
extern void			  gp2x_width_screen(int);

//joystick stuff Seleuco,GnoStiC
//http://wiki.gp2x.org/wiki/GPIO_Reference
extern unsigned long  gp2x_joystick_poll(void);
extern void           gp2x_usbjoy_update(void);

//timer stuff, Seleuco
extern unsigned long  gp2x_timer_poll(void);

#endif
