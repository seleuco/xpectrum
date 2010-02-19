#define  u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long
#define  s8  signed char
#define s16  signed short
#define s32  signed long

#include "port.h"

#include "main.h"

#include "z80_tables.h"
#include "z80_macros.h"
#include "z80.h"

extern Z80Regs * spectrumZ80;

#include "sound.h"
#include "snaps.h"
#include "graphics.h"
//#include "monofnt.h"
#include "zx.h"
#include "ay8910.h"

#include "fdc.h"

#include "zxtape.h"

typedef struct
{
unsigned id;
int zx_screen_mode;
int battery_icon;
int reserved[30];
int frameskip;
int contention;
int sound_volume;
int sound_gain;
int sound_mode;
int sound_freq;
int speed_mode;
int wait_vsync;
int show_fps;
int speed_loading;
int flash_loading;
int edge_loading;
int auto_loading;
}
MCONFIG;

extern long cur_frame;
