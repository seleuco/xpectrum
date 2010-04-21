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

   Copyright (c) 2010 Seleuco.

*/

#include "microlib.h"
#include <stdlib.h>
#include <stdio.h>

#include <android/log.h>

#include <pthread.h>

#define RGB2565L(R, G, B) ((R >> 3) << 11) | (( G >> 2) << 5 ) \
    | (( B >> 3 ) << 0 )

char local_buffer[320 * 240];
char * video_screen8 = NULL;
unsigned short screenbuffer[320 * 240];

unsigned long gp2x_pad_status = 0;

static microlib_inited = 0;
static soundInit = 0;

unsigned short local_palette[256];

extern char globalpath[];
extern int emulating;
extern int ext_keyboard;
extern int numKeys;
extern int keyboardKeys[];

static int __emulation_paused = 0;
static int isStereo = 0;


//android callbacks
int  (*dumpVideo_callback)(int emulating);
void (*initVideo_callback)(void *buffer);

void (*openSound_callback)(int rate,int stereo);
void (*dumpSound_callback)(void *buffer,int size);
void (*closeSound_callback)();

void setVideoCallbacks(void *init_video,void *dump_video)
{
	__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "setVideoCallbacks");

	initVideo_callback = init_video;
	dumpVideo_callback = dump_video;
}

void setAudioCallbacks(void *open_sound, void *dump_sound, void *close_sound)
{
    __android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "setAudioCallbacks");

    openSound_callback = open_sound;
    dumpSound_callback = dump_sound;
    closeSound_callback = close_sound;
}

void setGlobalPath(const char *path){
	__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "setGlobalPath %s",path);
	strcpy(globalpath,path);
}

void setPadStatus(unsigned long pad_status)
{
	gp2x_pad_status = pad_status;
	//__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "set_pad %ld",pad_status);
}

void setEmulationPause(unsigned char b)
{
	__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "setEmulationPause %d",b);

	__emulation_paused = b;
}

int isEmulating()
{
	__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "isEmulating  %d",emulating);
	return emulating;
}

void enabledExternalKeyboard(int value)
{
	__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "enabledExternalKeyboard %d",value);
	ext_keyboard = value;
}

void setExternalKeys(int keys[10], int n){
	//__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "setExternalKeys num:%d",n);

	numKeys = n;
	int i=0;

    for(i=0; i<n;i++)
      keyboardKeys[i]=keys[i];
}

void set_palette(palette_t palette){

	int i;
	for(i = 0; i < 256; i++)
	{
		local_palette[i] = RGB2565L(palette[i].r, palette[i].g, palette[i].b);
	}
	__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "set_palette");
}

unsigned long getTicks()
{
	struct timeval current_time;
	gettimeofday(&current_time, NULL);

	return ((unsigned long long)current_time.tv_sec * 1000LL + (current_time.tv_usec / 1000LL));
	//return (((unsigned long long)current_time.tv_sec * 1000000LL + current_time.tv_usec));
}


void dump_video()
{
	//__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "LLaman a dump_video %d",emulating);


	 while(__emulation_paused)
	 {
       usleep(100);
	 }

	 unsigned short *color;
	 int i=0;
	 for(i = 0; i < (320 * 240); i++)
	 {
		screenbuffer[i] = local_palette[(unsigned char)local_buffer[i]];
	 }

	 dumpVideo_callback(emulating);
}


long joystick_read()
{

	sched_yield();

	//Super importante para que el hilo del iphone responda a los eventos.
	// Mejor aqui que en el dump ya que existen muchos bucles que leen el teclado y se quedan atorados

	//gp2x_pad_status = 1<<14;//HACK!

	return gp2x_pad_status;
}

//SOUND

void sound_volume(int left, int rigth)
{

}


int sound_send(void *samples,int nsamples)
{
	//__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "LLaman a dump_audio");
	//dumpSound_callback(samples,isStereo ? nsamples*2 : nsamples);
	dumpSound_callback(samples,nsamples*2);
	//__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "-->FIN LLaman a dump_audio");
}


int sound_open(int rate, int bits, int stereo){

    if( soundInit == 1 )
    {
    	sound_close();
    }

	isStereo = stereo;

	__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "openSound rate:%d stereo:%d",rate,isStereo);

	openSound_callback(rate,isStereo);

	soundInit = 1;
}

int sound_close(){
	__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "closeSound");

	if( soundInit == 1 )
	{
   	   closeSound_callback();

   	   soundInit = 0;
	}
}

void microlib_end(void)
{
    if ( microlib_inited )
    {
  
        microlib_inited = 0;
        __android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "microlib_end");
    }
}

void microlib_init()
{
    if ( !microlib_inited )
    {
    	video_screen8  = (char *)&local_buffer;

        initVideo_callback((void *)&screenbuffer);

    	microlib_inited = 1;
    	__android_log_print(ANDROID_LOG_DEBUG, "libXpectrum.so", "microlib_init");
    }
}

