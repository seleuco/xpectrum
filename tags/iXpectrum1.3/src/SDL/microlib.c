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

   Copyright (c) 2009 SplinterGU.

*/

#include "microlib.h"

#include "SDL.h"

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <unistd.h>

#include <termios.h>
#include <time.h>

#include <sys/soundcard.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include <linux/keyboard.h>

char * video_screen8 = NULL;

SDL_Surface * screen = NULL ;
SDL_Joystick * joy = NULL;

/* RIPPED FROM THE SDL LIBS */

static int keyboard_fd = -1, saved_kbd_mode = -1, current_vt = -1, saved_vt = -1;
static microlib_inited = 0;
struct termios saved_kbd_termios;

#define SDL_arraysize(array)	(sizeof(array)/sizeof(array[0]))

//sound
unsigned long mixerfd =0;
unsigned long dspfd=0;

void set_palette(palette_t palette){

	SDL_Color sdlpalette[256];
	int i;
	for(i=0;i < 256;i++)
	{
		sdlpalette[i].r = palette[i].r;
		sdlpalette[i].g = palette[i].g;
		sdlpalette[i].b = palette[i].b;
	}
	SDL_SetColors( screen, sdlpalette, 0, 256 );
}

unsigned long getTicks(){
	return SDL_GetTicks();
}

int OpenKeyboard(void)
{
	/* Open only if not already opened */
 	if ( keyboard_fd < 0 ) {
		static const char * const tty0[] = { "/dev/tty0", "/dev/vc/0", NULL };
		static const char * const vcs[] = { "/dev/vc/%d", "/dev/tty%d", NULL };
		int i, tty0_fd;

		/* Try to query for a free virtual terminal */
		tty0_fd = -1;
		for ( i=0; tty0[i] && (tty0_fd < 0); ++i ) {
			tty0_fd = open(tty0[i], O_WRONLY, 0);
		}
		if ( tty0_fd < 0 ) {
			tty0_fd = dup(0); /* Maybe stdin is a VT? */
		}
		ioctl(tty0_fd, VT_OPENQRY, &current_vt);
		close(tty0_fd);

		if ( (geteuid() == 0) && (current_vt > 0) ) {
			for ( i=0; vcs[i] && (keyboard_fd < 0); ++i ) {
				char vtpath[12];

				snprintf(vtpath, SDL_arraysize(vtpath), vcs[i], current_vt);
				keyboard_fd = open(vtpath, O_RDWR, 0);

				/* This needs to be our controlling tty
				   so that the kernel ioctl() calls work
				*/
				if ( keyboard_fd >= 0 ) {
					tty0_fd = open("/dev/tty", O_RDWR, 0);
					if ( tty0_fd >= 0 ) {
						ioctl(tty0_fd, TIOCNOTTY, 0);
						close(tty0_fd);
					}
				}
			}
		}
 		if ( keyboard_fd < 0 ) {
			/* Last resort, maybe our tty is a usable VT */
			struct vt_stat vtstate;

			keyboard_fd = open("/dev/tty", O_RDWR);

			if ( ioctl(keyboard_fd, VT_GETSTATE, &vtstate) == 0 ) {
				current_vt = vtstate.v_active;
			} else {
				current_vt = 0;
			}
 		}

		/* Make sure that our input is a console terminal */
		{
		  int dummy;
		  if ( ioctl(keyboard_fd, KDGKBMODE, &dummy) < 0 ) {
			close(keyboard_fd);
			keyboard_fd = -1;
			printf("Unable to open a console terminal\n");
		  }
		}

 	}
 	return(keyboard_fd);
}

int InGraphicsMode(void)
{
	return((keyboard_fd >= 0) && (saved_kbd_mode >= 0));
}

void LeaveGraphicsMode(void)
{
	if ( InGraphicsMode() ) {
		ioctl(keyboard_fd, KDSETMODE, KD_TEXT);
		ioctl(keyboard_fd, KDSKBMODE, saved_kbd_mode);
		tcsetattr(keyboard_fd, TCSAFLUSH, &saved_kbd_termios);
		saved_kbd_mode = -1;

		/* Head back over to the original virtual terminal */
		ioctl(keyboard_fd, VT_UNLOCKSWITCH, 1);
		if ( saved_vt > 0 ) {
			ioctl(keyboard_fd, VT_ACTIVATE, saved_vt);
		}
	}
}

void CloseKeyboard(void)
{
	if ( keyboard_fd >= 0 ) {
		LeaveGraphicsMode();
		if ( keyboard_fd > 0 ) {
			close(keyboard_fd);
		}
	}
	keyboard_fd = -1;
}

int EnterGraphicsMode(void)
{
	struct termios keyboard_termios;

	/* Set medium-raw keyboard mode */
	if ( (keyboard_fd >= 0) && !InGraphicsMode() ) {

		/* Switch to the correct virtual terminal */
		if ( current_vt > 0 ) {
			struct vt_stat vtstate;

			if ( ioctl(keyboard_fd, VT_GETSTATE, &vtstate) == 0 ) {
				saved_vt = vtstate.v_active;
			}
			if ( ioctl(keyboard_fd, VT_ACTIVATE, current_vt) == 0 ) {
				ioctl(keyboard_fd, VT_WAITACTIVE, current_vt);
			}
		}

		/* Set the terminal input mode */
		if ( tcgetattr(keyboard_fd, &saved_kbd_termios) < 0 ) {
			printf("ERROR: Unable to get terminal attributes\n");
			if ( keyboard_fd > 0 ) {
				close(keyboard_fd);
			}
			keyboard_fd = -1;
			return(-1);
		}
		if ( ioctl(keyboard_fd, KDGKBMODE, &saved_kbd_mode) < 0 ) {
			printf("ERROR: Unable to get current keyboard mode\n");
			if ( keyboard_fd > 0 ) {
				close(keyboard_fd);
			}
			keyboard_fd = -1;
			return(-1);
		}
		keyboard_termios = saved_kbd_termios;
		keyboard_termios.c_lflag &= ~(ICANON | ECHO | ISIG);
		keyboard_termios.c_iflag &= ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON);
		keyboard_termios.c_cc[VMIN] = 0;
		keyboard_termios.c_cc[VTIME] = 0;
		if (tcsetattr(keyboard_fd, TCSAFLUSH, &keyboard_termios) < 0) {
			CloseKeyboard();
			printf("ERROR: Unable to set terminal attributes\n");
			return(-1);
		}
		/* This will fail if we aren't root or this isn't our tty */
		if ( ioctl(keyboard_fd, KDSKBMODE, K_MEDIUMRAW) < 0 ) {
			CloseKeyboard();
			printf("ERORR: Unable to set keyboard in raw mode\n");
			return(-1);
		}
		if ( ioctl(keyboard_fd, KDSETMODE, KD_GRAPHICS) < 0 ) {
			CloseKeyboard();
			printf("ERROR: Unable to set keyboard in graphics mode");
			return(-1);
		}
		/* Prevent switching the virtual terminal */
		ioctl(keyboard_fd, VT_LOCKSWITCH, 1);
	}
	return(keyboard_fd);
}


//SOUND

void sound_volume(int left, int rigth)
{
    if(mixerfd ==0)return;
    left=(((left*0x50)/100)<<8)|((rigth*0x50)/100);
    ioctl(mixerfd , SOUND_MIXER_WRITE_VOLUME, &left);
}

int sound_open(int rate, int bits, int stereo){

	if(!dspfd)  dspfd = open("/dev/dsp",   O_WRONLY /*| O_NONBLOCK*/);

	ioctl(dspfd, SNDCTL_DSP_SETFMT, &bits);

    int res = ioctl(dspfd, SNDCTL_DSP_STEREO, &stereo);
    if(res<0)exit(-1);
    ioctl(dspfd, SNDCTL_DSP_SPEED,  &rate);

    int frag = stereo ? (0x40000|13) : (0x40000|12);

    if(rate<16500) frag--;
    if(rate<33000) frag--;

    ioctl(dspfd, SNDCTL_DSP_SETFRAGMENT,  &frag);

}

int sound_close(){

	if(dspfd)
	{
	    close(dspfd);
	    dspfd=0;
	}
}

int sound_send(void *samples,int nsamples)
{
	if(dspfd)
	   return write(dspfd,samples,nsamples<<1);
	else
	   return -1;
}

void microlib_end(void)
{
    if ( microlib_inited )
    {
        LeaveGraphicsMode();
        CloseKeyboard();

        sound_close();

        close(mixerfd);

        if ( screen ) SDL_FreeSurface( screen ) ;

        if ( joy ) SDL_JoystickClose( joy ) ;
        if ( SDL_WasInit( SDL_INIT_JOYSTICK ) ) SDL_QuitSubSystem( SDL_INIT_JOYSTICK );
        if ( SDL_WasInit( SDL_INIT_VIDEO ) ) SDL_QuitSubSystem( SDL_INIT_VIDEO );
        if ( SDL_WasInit( SDL_INIT_EVENTTHREAD ) ) SDL_QuitSubSystem( SDL_INIT_EVENTTHREAD );

        fcloseall();

        microlib_inited = 0;
    }
}

void microlib_init()
{
    if ( !microlib_inited )
    {
    	printf("KEYBOARD = %d\n", OpenKeyboard());
    	printf("GRAPHICS = %d\n", EnterGraphicsMode());
    	printf("entering init()\n"); fflush(stdout);

        if ( !SDL_WasInit( SDL_INIT_EVENTTHREAD ) ) SDL_InitSubSystem( SDL_INIT_EVENTTHREAD );
        if ( !SDL_WasInit( SDL_INIT_VIDEO ) ) SDL_InitSubSystem( SDL_INIT_VIDEO );

#ifdef __ARM__
        screen = SDL_SetVideoMode( 320, 240, 8, SDL_HWPALETTE | SDL_DOUBLEBUF | SDL_HWSURFACE );
#else
        screen = SDL_SetVideoMode( 640, 480, 8, SDL_HWPALETTE | SDL_DOUBLEBUF | SDL_HWSURFACE );
#endif
        if ( !screen ) return;
        SDL_ShowCursor( 0 ) ;

        if ( !SDL_WasInit( SDL_INIT_JOYSTICK ) )
        {
            SDL_InitSubSystem( SDL_INIT_JOYSTICK );
            SDL_JoystickEventState( SDL_ENABLE ) ;
        }

        joy = SDL_JoystickOpen( 0 ) ;
        SDL_JoystickUpdate() ;

        video_screen8 = malloc( 320 * 240 );

    	mixerfd  = open("/dev/mixer", O_RDWR);

    	microlib_inited = 1;
        atexit(microlib_end);
    }
}


void dump_video()
{
#ifdef __ARM__
    if ( SDL_MUSTLOCK( screen ) ) SDL_LockSurface( screen ) ;
    memmove( screen->pixels, video_screen8, 320 * 240 );
    if ( SDL_MUSTLOCK( screen ) ) SDL_UnlockSurface( screen ) ;
    SDL_Flip( screen ) ;
#else
    int height = 240, width = 320, w;
    int incr = width * 2;
    char * q, *p, *s;

    if ( SDL_MUSTLOCK( screen ) ) SDL_LockSurface( screen ) ;

    s = video_screen8;
    p = screen->pixels;
    q = p + incr;

    while ( height-- )
    {
        int w = width;
        while ( w-- )
        {
            *(q++) = *(p++) = *s;
            *(q++) = *(p++) = *s;
            s++;
        }
        q += incr ;
        p += incr ;
    }
    if ( SDL_MUSTLOCK( screen ) ) SDL_UnlockSurface( screen ) ;
    SDL_Flip( screen ) ;
#endif
}

long joystick_read()
{
    int i;
#ifdef __ARM__
    long button = 0;
#else
    static long button = 0;
#endif

    SDL_Event event;
    /* Remove all pendings events */

    /* We can't return -1, just return 0 (no event) on error */
    while ( SDL_PeepEvents( &event, 1, SDL_GETEVENT, SDL_ALLEVENTS ) > 0 );

    /* Get new events */
    SDL_PumpEvents();
#ifdef __ARM__
    for (i = 0; i < 19; i++)
    {
        button  |=  ( SDL_JoystickGetButton(joy,i)  !=  0 ) ? 1 << i : 0 ;
    }

    if ( button & JOY_BUTTON_UPLEFT    ) button |= JOY_BUTTON_UP   | JOY_BUTTON_LEFT ;
    if ( button & JOY_BUTTON_DOWNLEFT  ) button |= JOY_BUTTON_DOWN | JOY_BUTTON_LEFT ;
    if ( button & JOY_BUTTON_UPRIGHT   ) button |= JOY_BUTTON_UP   | JOY_BUTTON_RIGHT ;
    if ( button & JOY_BUTTON_DOWNRIGHT ) button |= JOY_BUTTON_DOWN | JOY_BUTTON_RIGHT ;
#else
    while ( SDL_PeepEvents( &event, 1, SDL_GETEVENT, SDL_EVENTMASK(SDL_KEYDOWN)|SDL_EVENTMASK(SDL_KEYUP) ) > 0 )
    {
        switch ( event.type )
        {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case    SDLK_UP:
                            button  |= JOY_BUTTON_UP;
                            break;

                    case    SDLK_LEFT:
                            button  |= JOY_BUTTON_LEFT;
                            break;

                    case    SDLK_RIGHT:
                            button  |= JOY_BUTTON_RIGHT;
                            break;

                    case    SDLK_DOWN:
                            button  |= JOY_BUTTON_DOWN;
                            break;

                    case    SDLK_RETURN:
                            button  |= JOY_BUTTON_MENU;
                            break;

                    case    SDLK_SPACE:
                            button  |= JOY_BUTTON_SELECT;
                            break;

                    case    SDLK_a:
                            button  |= JOY_BUTTON_A;
                            break;

                    case    SDLK_s:
                            button  |= JOY_BUTTON_X;
                            break;

                    case    SDLK_d:
                            button  |= JOY_BUTTON_B;
                            break;

                    case    SDLK_q:
                            button  |= JOY_BUTTON_L;
                            break;

                    case    SDLK_w:
                            button  |= JOY_BUTTON_Y;
                            break;

                    case    SDLK_e:
                            button  |= JOY_BUTTON_R;
                            break;

                    case    SDLK_MINUS:
                    case    SDLK_KP_MINUS:
                            button  |= JOY_BUTTON_VOLDOWN;
                            break;

                    case    SDLK_PLUS:
                    case    SDLK_KP_PLUS:
                            button  |= JOY_BUTTON_VOLUP;
                            break;
                }
                break ;

            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                    case    SDLK_UP:
                            button  &= ~JOY_BUTTON_UP;
                            break;

                    case    SDLK_LEFT:
                            button  &= ~JOY_BUTTON_LEFT;
                            break;

                    case    SDLK_RIGHT:
                            button  &= ~JOY_BUTTON_RIGHT;
                            break;

                    case    SDLK_DOWN:
                            button  &= ~JOY_BUTTON_DOWN;
                            break;

                    case    SDLK_RETURN:
                            button  &= ~JOY_BUTTON_MENU;
                            break;

                    case    SDLK_SPACE:
                            button  &= ~JOY_BUTTON_SELECT;
                            break;

                    case    SDLK_a:
                            button  &= ~JOY_BUTTON_A;
                            break;

                    case    SDLK_s:
                            button  &= ~JOY_BUTTON_X;
                            break;

                    case    SDLK_d:
                            button  &= ~JOY_BUTTON_B;
                            break;

                    case    SDLK_q:
                            button  &= ~JOY_BUTTON_L;
                            break;

                    case    SDLK_w:
                            button  &= ~JOY_BUTTON_Y;
                            break;

                    case    SDLK_e:
                            button  &= ~JOY_BUTTON_R;
                            break;

                    case    SDLK_MINUS:
                    case    SDLK_KP_MINUS:
                            button  &= ~JOY_BUTTON_VOLDOWN;
                            break;

                    case    SDLK_PLUS:
                    case    SDLK_KP_PLUS:
                            button  &= ~JOY_BUTTON_VOLUP;
                            break;
                }
                break ;
        }
    }
#endif
    return button;
}

