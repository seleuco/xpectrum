/* Caprice32 - Amstrad CPC Emulator
   (c) Copyright 1997-2004 Ulrich Doewich

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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#if 1

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>



// FDC constants
#define DSK_BPTMAX      8192
#define DSK_TRACKMAX    102   // max amount that fits in a DSK header
#define DSK_SIDEMAX     2
#define DSK_SECTORMAX   29    // max amount that fits in a track header

#define FDC_TO_CPU      0
#define CPU_TO_FDC      1

#define CMD_PHASE       0
#define EXEC_PHASE      1
#define RESULT_PHASE    2

#define SKIP_flag       1     // skip sectors with DDAM/DAM
#define SEEKDRVA_flag   2     // seek operation has finished for drive A
#define SEEKDRVB_flag   4     // seek operation has finished for drive B
#define RNDDE_flag      8     // simulate random DE sectors
#define OVERRUN_flag    16    // data transfer timed out
#define SCAN_flag       32    // one of the three scan commands is active
#define SCANFAILED_flag 64    // memory and sector data does not match
#define STATUSDRVA_flag 128   // status change of drive A
#define STATUSDRVB_flag 256   // status change of drive B

typedef struct {
   int timeout;
   int motor;
   int led;
   int flags;
   int phase;
   int byte_count;
   int buffer_count;
   int cmd_length;
   int res_length;
   int cmd_direction;
   void (*cmd_handler)(void);
   unsigned char *buffer_ptr;
   unsigned char *buffer_endptr;
   unsigned char command[12];
   unsigned char result[8];
} t_FDC;

typedef struct {
   unsigned char CHRN[4]; // the CHRN for this sector
   unsigned char flags[4]; // ST1 and ST2 - reflects any possible error conditions
   unsigned int size; // sector size in bytes
   unsigned char *data; // pointer to sector data
} t_sector;

typedef struct {
   unsigned int sectors; // sector count for this track
   unsigned int size; // track size in bytes
   unsigned char *data; // pointer to track data
   t_sector sector[DSK_SECTORMAX]; // array of sector information structures
} t_track;

typedef struct {
   unsigned int tracks; // total number of tracks
   unsigned int current_track; // location of drive head
   unsigned int sides; // total number of sides
   unsigned int current_side; // side being accessed
   unsigned int current_sector; // sector being accessed
   unsigned int altered; // has the image been modified?
   unsigned int write_protected; // is the image write protected?
   unsigned int random_DEs; // sectors with Data Errors return random data?
   unsigned int flipped; // reverse the side to access?
   t_track track[DSK_TRACKMAX][DSK_SIDEMAX]; // array of track information structures
} t_drive;

//#define DEBUG
//#define DEBUG_CRTC
//#define DEBUG_FDC
//#define DEBUG_GA
//#define DEBUG_NO_VIDEO
//#define DEBUG_TAPE
//#define DEBUG_Z80

#ifndef _MAX_PATH
 #ifdef _POSIX_PATH_MAX
 #define _MAX_PATH _POSIX_PATH_MAX
 #else
 #define _MAX_PATH 256
 #endif
#endif

#define CYCLE_COUNT_INIT 80000 // 4MHz divided by 50Hz = number of CPU cycles per frame

#define CPC_SCR_WIDTH 1024 // max width
#define CPC_SCR_HEIGHT 312 // max height
#define CPC_VISIBLE_SCR_WIDTH 384 // visible width: 4+40+4 * 8
#define CPC_VISIBLE_SCR_HEIGHT 272 // visible height: 5+25+4 * 8

#define ICN_DISK_WIDTH 14
#define ICN_DISK_HEIGHT 16
#define ICN_TAPE_WIDTH 18
#define ICN_TAPE_HEIGHT 13

#define VOC_THRESHOLD 128

// CRTC flags
#define VS_flag      1
#define HS_flag      2
#define HDT_flag     4
#define VDT_flag     8
#define HT_flag      16
#define VT_flag      32
#define MR_flag      64
#define VTadj_flag   128
#define VSf_flag     256

// Multiface 2 flags
#define MF2_ACTIVE      1
#define MF2_RUNNING     2
#define MF2_INVISIBLE   4



typedef struct {
   char id[8];
   char unused1[8];
   unsigned char version;
   unsigned char AF[2];
   unsigned char BC[2];
   unsigned char DE[2];
   unsigned char HL[2];
   unsigned char R;
   unsigned char I;
   unsigned char IFF0;
   unsigned char IFF1;
   unsigned char IX[2];
   unsigned char IY[2];
   unsigned char SP[2];
   unsigned char PC[2];
   unsigned char IM;
   unsigned char AFx[2];
   unsigned char BCx[2];
   unsigned char DEx[2];
   unsigned char HLx[2];
   unsigned char ga_pen;
   unsigned char ga_ink_values[17];
   unsigned char ga_ROM_config;
   unsigned char ga_RAM_config;
   unsigned char crtc_reg_select;
   unsigned char crtc_registers[18];
   unsigned char upper_ROM;
   unsigned char ppi_A;
   unsigned char ppi_B;
   unsigned char ppi_C;
   unsigned char ppi_control;
   unsigned char psg_reg_select;
   unsigned char psg_registers[16];
   unsigned char ram_size[2];
// version 2 info follows
   unsigned char cpc_model;
   unsigned char last_interrupt;
   unsigned char scr_modes[6];
// version 3 info follows
   unsigned char drvA_DOSfilename[13];
   unsigned char drvB_DOSfilename[13];
   unsigned char cart_DOSfilename[13];
   unsigned char fdc_motor;
   unsigned char drvA_current_track;
   unsigned char drvB_current_track;
   unsigned char drvC_current_track;
   unsigned char drvD_current_track;
   unsigned char printer_data;
   unsigned char psg_env_step;
   unsigned char psg_env_direction;
   unsigned char crtc_type;
   unsigned char crtc_addr[2];
   unsigned char crtc_scanline[2];
   unsigned char crtc_char_count[2];
   unsigned char crtc_line_count;
   unsigned char crtc_raster_count;
   unsigned char crtc_vt_adjust_count;
   unsigned char crtc_hsw_count;
   unsigned char crtc_vsw_count;
   unsigned char crtc_flags[2];
   unsigned char ga_int_delay;
   unsigned char ga_sl_count;
   unsigned char z80_int_pending;
   unsigned char unused2[75];
} t_SNA_header;

typedef struct {
   char id[34];
   char unused1[14];
   unsigned char tracks;
   unsigned char sides;
   unsigned char unused2[2];
   unsigned char track_size[DSK_TRACKMAX*DSK_SIDEMAX];
} t_DSK_header;

typedef struct {
   char id[12];
   char unused1[4];
   unsigned char track;
   unsigned char side;
   unsigned char unused2[2];
   unsigned char bps;
   unsigned char sectors;
   unsigned char gap3;
   unsigned char filler;
   unsigned char sector[DSK_SECTORMAX][8];
} t_track_header;







typedef struct {
   unsigned char label[40]; // label to display in options dialog
   unsigned int tracks; // number of tracks
   unsigned int sides; // number of sides
   unsigned int sectors; // sectors per track
   unsigned int sector_size; // sector size as N value
   unsigned int gap3_length; // GAP#3 size
   unsigned char filler_byte; // default byte to use
   unsigned char sector_ids[2][16]; // sector IDs
} t_disk_format;




#endif

// fdc.c
void fdc_write_data(unsigned char val);
unsigned char fdc_read_status(void);
unsigned char fdc_read_data(void);



#define ERR_OUT_OF_MEMORY_        9
#define ERR_DSK_INVALID          21
#define ERR_DSK_SIDES            22
#define ERR_DSK_SECTORS          23
#define ERR_DSK_WRITE            24
#define MSG_DSK_ALTERED          25

void fdc_init (int a, int b);
void fdc_motor(unsigned char on);
int dsk_load (void *pchFileName);
