/* aylet 0.2, a .AY music file player.
 * Copyright (C) 2001 Russell Marks and Ian Collier. See main.c for licence.
 *
 * sound.h
 */

extern int sound_enabled;
extern int sound_freq;
extern int sound_stereo;
extern int sound_stereo_beeper;
extern int sound_stereo_ay;
extern int sound_stereo_ay_abc;
extern int sound_stereo_ay_narrow;

extern void sound_init(int cpc_type, int model);
extern void sound_end(void);

extern void sound_pause(void);
extern void sound_unpause(void);

extern int sound_frame_16(void *blah, short  *ptr, int len);

extern void sound_ay_reset(void);
extern void sound_ay_write(int reg,int val,unsigned long tstates);
extern void sound_beeper_0(int on, unsigned long tstates/*1, unsigned long tstates2*/);
extern void sound_beeper_1(int on, unsigned long tstates);

extern void Sound_Init();
extern void Sound_Loop();
