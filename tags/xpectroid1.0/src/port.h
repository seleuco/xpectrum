#define LOW_ENDIAN
#define USING_ALLEGRO
  
#define DEBUG
#define _DEV_DEBUG_                         /* development debugging */
#define LOW_ENDIAN
/*#define HI_ENDIAN */ 

/* Used by the Z80Debug() function */ 
#define DEBUG_OK       1
#define DEBUG_QUIT     0

#define video vscreen




//#define INLINE     static __inline




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>




#undef byte
#undef word
#undef dword

/*
#undef byte
#undef word

typedef unsigned char byte;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef un16 word;
typedef word addr;
*/


extern unsigned char *LCDs8;

//#define pixel(x,y) ((unsigned char *)0x0c7b4000+(240-y-1+x*240))
//#define sync_pixels() 

extern unsigned char *Picture;
extern void FlipScreen(int,unsigned char);
//#define pixel(x,y) ((unsigned char *)Picture+(240-y-1+x*240))
#define pixel(x,y) ((unsigned char *)Picture+(y*320+x))
//#define sync_pixels() FlipScreen(1,0)
#define sync_pixels() 

#define pixel_inc  (1)
#define scanl_inc  (320)


#define RGB2INT(r,g,b) (((r>>3)<<11)|((g>>3)<<6)|((b>>3)<<1))
void SetColor(unsigned char N,unsigned char R,unsigned char G,unsigned char B);

#define SOUND_SIGN(a) (a)
//#define SOUND_SIGN(a) (a^0x80)


//#define SOUND_X128
//#define SOUND_CAPRICE
#define SOUND_BULBA
#define DISPLAY_BY_SCANLINE 
#define DISPLAY_BY_CYCLE
//CSS FAQ timings:
/*
#define TIMING_48   14335  
#define TIMING_128  14361
*/
//ramsoft timings
//#define TIMING_48   14347  
//#define TIMING_128  14368

//mio
/*
#define TIMING_48   14335
#define TIMING_128  (14356-5)
*/


#define TIMING_48   14335  
#define TIMING_128  14361


#define FALSE 0
#define TRUE 1

#define max_tracksize 5990

//#define inline
