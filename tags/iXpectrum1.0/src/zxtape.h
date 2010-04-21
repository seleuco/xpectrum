/*=====================================================================
  zxtape.h    -> Header file for zxtape.c.

 Copyright (c) 1999-2004 Philip Kendall
 Copyright (c) 2008 Seleuco.

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

 
 ======================================================================*/  

#ifndef TAPE_H
#define TAPE_H

int tape_init( void );
int tape_finish( void );
int tape_open(void *fp, int size, const char *filename, int autoload );
int tape_close( void );
int tape_do_play( int autoplay );
int tape_toggle_play( int autoplay );
int tape_next_edge(Z80Regs *regs, int *edge_tstates,int *flag);
int tape_load_trap(Z80Regs *regs);
int tape_select_block(int n );
int tape_is_tape(void);
int tape_blocks_entries(char entries[][256],int length);
int tape_get_current_block( void );

extern int tape_microphone;
extern int tape_playing;
extern int tape_edge_tstates_target;
extern int tape_edge_tstates_current;


#endif
