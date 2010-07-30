/* zxtape.c: tape handling routines
   Copyright (c) 1999-2005 Philip Kendall, Darren Salt, Witold Filipczyk
   Copyright (c) 2008 Seleuco

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

   Author contact information:

   E-mail: philip-fuse@shadowmagic.org.uk
*/

#include "shared.h"

#include "mylibspectrum/libspectrum.h"
#include "mylibspectrum/tape_block.h"

extern MCONFIG mconfig;

static libspectrum_tape *tape;//la cinta como abstraccion
int tape_playing;
int tape_microphone;
int tape_edge_tstates_target;
int tape_edge_tstates_current;


static int
trap_load_block( libspectrum_tape_block *block, Z80Regs * regs );

int tape_init( void )
{
  libspectrum_error error;
  
  error = libspectrum_tape_alloc( &tape );
    
  if( !tape ) return error;

  tape_playing = 0;
  tape_microphone = 0;
  tape_edge_tstates_target = 0;
  
  return 0;
}

int tape_open(void *fp, int size, const char *filename, int autoload )
{

  int error;

  error = tape_read_buffer( fp, size, LIBSPECTRUM_ID_UNKNOWN,
			    filename, autoload );
			    
  sound_beeper_1(tape_microphone,0);
			  
  if( error ) {return error; }

  return 0;
}

int
tape_read_buffer( unsigned char *buffer, size_t length, libspectrum_id_t type,
		  const char *filename, int autoload )
{
  int error;

  if( libspectrum_tape_present( tape ) ) {
    error = tape_close(); if( error ) return error;
  }

  error = libspectrum_tape_read( tape, buffer, length, type, filename );
    
  if( error ) return error;

  //ui_tape_browser_update( UI_TAPE_BROWSER_NEW_TAPE, NULL );
   
  /*
  if( autoload ) {
    error = tape_autoload( machine_current->machine );
    if( error ) return error;
  }
  */

  return 0;
}

int tape_close( void )
{
  int error;

  /* Stop the tape if it's currently playing */
  if( tape_playing ) {
    error = tape_stop();
    if( error ) return error;
  }

  /* And then remove it from memory */
  error = libspectrum_tape_clear( tape );
  if( error ) return error;

  //ui_tape_browser_update( UI_TAPE_BROWSER_NEW_TAPE, NULL );
  
  return 0;
}

int tape_finish(){
	
	int error;
	
	error = tape_close();
	if( error ) return error;
	
	error =  libspectrum_tape_free(tape);
    if( error ) return error;
}

static int
tape_play( int autoplay )
{
  libspectrum_tape_block* block;

  int error;
    
  if( !libspectrum_tape_present( tape ) ) return 1;
  
  ZX_Unpatch_ROM();//unpatch rom so some protection loader schemes works
    
  tape_playing = 1;
  //tape_autoplay = autoplay;
  tape_microphone = 0;

  /* Update the status bar */
  //ui_statusbar_update( UI_STATUSBAR_ITEM_TAPE, UI_STATUSBAR_STATE_ACTIVE );

  //if( ( !( machine_current->timex ) ) && settings_current.sound_load )
    //sound_beeper( 1, tape_microphone );

  //loader_tape_play();

  //el edge lo cojera directamente el Z80patch
   
   int edge_tstates;
   int flag;
   error = tape_next_edge( NULL, &edge_tstates,&flag);
   
   tape_edge_tstates_target = edge_tstates;
      
   tape_edge_tstates_current=0; 
    
   if( error ) return error;
  
  return 0;
}

int
tape_do_play( int autoplay )
{
  if( !tape_playing ) {
    return tape_play( autoplay );
  } else {
    return 0;
  }
}

int tape_toggle_play( int autoplay )
{
  if( tape_playing ) {
    return tape_stop();
  } else {
    return tape_play( autoplay );
  }
}

int tape_stop( void )
{
  if( tape_playing ) {

    tape_playing = 0;
    tape_edge_tstates_target=0;
    tape_edge_tstates_current=0;
    
    if(mconfig.flash_loading) ZX_Patch_ROM();//enable flash loading traps again
        
    //ui_statusbar_update( UI_STATUSBAR_ITEM_TAPE, UI_STATUSBAR_STATE_INACTIVE );
  }

  return 0;
}

int
tape_next_edge(Z80Regs *regs, int *edge_tstates, int *bit)
{
  int error; libspectrum_error libspec_error;
  libspectrum_tape_block *block;
  
  //libspectrum_dword edge_tstates;
  int flags;
  *edge_tstates = 0;
  *bit=-1;
  int one_value=0;
  /* If the tape's not playing, just return */
  if( ! tape_playing ) return 0;

  block = libspectrum_tape_current_block( tape );
  int type = libspectrum_tape_block_type( block );
  if(type==LIBSPECTRUM_TAPE_BLOCK_ROM || 
     type==LIBSPECTRUM_TAPE_BLOCK_TURBO || 
     type==LIBSPECTRUM_TAPE_BLOCK_PURE_DATA /*||
     type==LIBSPECTRUM_TAPE_BLOCK_RAW_DATA*/)
  { 
     int state = libspectrum_tape_state( tape );

     if(state==LIBSPECTRUM_TAPE_STATE_DATA1 || state==LIBSPECTRUM_TAPE_STATE_DATA2)
     {
        if(block->type==LIBSPECTRUM_TAPE_BLOCK_TURBO)
        {
           one_value = block->types.turbo.bit1_length;
        }
        else if(block->type==LIBSPECTRUM_TAPE_BLOCK_PURE_DATA)
        {
           one_value = block->types.pure_data.bit1_length;
        }
        else
        {
           one_value = 1710;
        }        
     }             
  }   

  /* Get the time until the next edge */
  libspectrum_dword tmp = 0;
  libspec_error = libspectrum_tape_get_next_edge(&tmp, &flags, tape);
  *edge_tstates = tmp;
  
  if(one_value!=0)
     *bit = *edge_tstates == one_value ? 1 : 0;
 
  //edge_tstates_target = edge_tstates_target+*edge_tstates; 
    
  if( libspec_error != LIBSPECTRUM_ERROR_NONE ) return libspec_error;

  /* Invert the microphone state */
  if( *edge_tstates || ( flags & LIBSPECTRUM_TAPE_FLAGS_STOP ) ) {

    if( flags & LIBSPECTRUM_TAPE_FLAGS_NO_EDGE ) {
      // Do nothing 
    } else if( flags & LIBSPECTRUM_TAPE_FLAGS_LEVEL_LOW ) {
      tape_microphone = 0;
    } else if( flags & LIBSPECTRUM_TAPE_FLAGS_LEVEL_HIGH ) {
      tape_microphone = 1;
    } else {
      tape_microphone = !tape_microphone;
    }

    if(regs!=NULL)
       sound_beeper_1(tape_microphone,regs->IPeriod-regs->ICount);//TODO: poner pref
  }

  /* If we've been requested to stop the tape, do so and then
     return without stacking another edge */
     
  int zx48=0;
  if(regs!=NULL)
    zx48=regs->IPeriod==69888;
         
  if((flags & LIBSPECTRUM_TAPE_FLAGS_STOP) || ( zx48 && (flags & LIBSPECTRUM_TAPE_FLAGS_STOP48) ) )
  {
    error = tape_stop(); if( error ) return error;
    return 0;
  }

  /* If that was the end of a block, update the browser */
  if( flags & LIBSPECTRUM_TAPE_FLAGS_BLOCK ) {

    //ui_tape_browser_update( UI_TAPE_BROWSER_SELECT_BLOCK, NULL );

    /* If the tape was started automatically, tape traps are active
       and the new block is a ROM loader, stop the tape and return
       without putting another event into the queue */
    block = libspectrum_tape_current_block( tape );    
    if( /*tape_autoplay &&*/ mconfig.flash_loading &&
	    libspectrum_tape_block_type( block ) == LIBSPECTRUM_TAPE_BLOCK_ROM
      ) {
      error = tape_stop(); if( error ) return error;
      return 0;//TODO: devolver valor que nos diga que nos diga que no hay que devolver otro edge?
    }
    
  }

  /* Otherwise, put this into the event queue; remember that this edge
     should occur 'edge_tstates' after the last edge, not after the
     current time (these will be slightly different as we only process
     events between instructions). */

  return 0;
}

/* Load the next tape block into memory; returns 0 if a block was
   loaded (even if it had an tape loading error or equivalent) or
   non-zero if there was an error at the emulator level, or tape traps
   are not active */
int tape_load_trap(Z80Regs * regs)
{
  libspectrum_tape_block *block, *next_block;
  int error;

  /* Do nothing if tape traps aren't active, or the tape is already playing */
  if( !mconfig.flash_loading || tape_playing ) return 1;

  /* Do nothing if we're not in the correct ROM */
  //if( ! trap_check_rom() ) return 3;

  /* Return with error if no tape file loaded */
  if( !libspectrum_tape_present( tape ) ) return 1;

  block = libspectrum_tape_current_block( tape );

  /* Skip over any meta-data blocks */
  while( libspectrum_tape_block_metadata( block ) ) {
    block = libspectrum_tape_select_next_block( tape );
    if( !block ) return 1;
  }

  /* If this block isn't a ROM loader, start the block playing. After
     that, return with `error' so that we actually do whichever
     instruction it was that caused the trap to hit */
  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_ROM 
   || libspectrum_tape_state( tape ) != LIBSPECTRUM_TAPE_STATE_PILOT  
  ) {
    tape_play( 1 );
    return /*-1*/1;
  }

  /* We don't properly handle the case of partial loading, so don't run
     the traps in that situation *///Esto hace cascar algunos games
   
  /* 
  if( libspectrum_tape_block_data_length( block ) != regs->DE.W + 2 ) {
    tape_play( 1 );
    return 1;
  }
  */
  
  error = trap_load_block( block , regs);//carga el bloque
  if( error ) return -1;

  /* Peek at the next block. If it's a ROM block, move along, initialise
     the block, and return */
  next_block = libspectrum_tape_peek_next_block( tape );

  if( libspectrum_tape_block_type(next_block) == LIBSPECTRUM_TAPE_BLOCK_ROM ) {

    next_block = libspectrum_tape_select_next_block( tape );
    if( !next_block ) return 1;

    //ui_tape_browser_update( UI_TAPE_BROWSER_SELECT_BLOCK, NULL );    

    return 0;//FLASHLOADED!
  }

  /* If the next block isn't a ROM block, set ourselves up such that the
     next thing to occur is the pause at the end of the current block */
     
  libspectrum_tape_set_state( tape, LIBSPECTRUM_TAPE_STATE_PAUSE );//TODO ? autodetect, sino no tira?
  //libspectrum_tape_block_set_state( block, LIBSPECTRUM_TAPE_STATE_PAUSE );
  
  /*
  error = tape_play(1);
  if( error ) return -1;
  */
  return 0;
  
}

static int
trap_load_block( libspectrum_tape_block *block, Z80Regs * regs )
{    
    regs->BC.W=0xCB01;//??
    regs->HL.W=0;//??
    
    int where,flags,howmany,bytes,f;
    byte *src;

    src = libspectrum_tape_block_data( block );
    bytes = libspectrum_tape_block_data_length( block );
                                 
    where = regs->IX.W;
    flags= (*src++); /* read flag type and ignore it */
    howmany = regs->DE.W;
                                         				
    if (bytes - 2 < howmany)
    {
       howmany = bytes - 2;
       (regs->AF.B.l &= ~(C_FLAG));
       regs->IX.W += bytes - 2;
    }
    else
       regs->IX.W += regs->DE.W;

  	for (f = 0; f < howmany; f++)
    {
       Z80WriteMem_notiming(where + f, *src++);
    }
  				
    src++; /* read checksum (and ignore it :-) */
                                
  	/* if load was successful, reset C_FLAG */
  	if (howmany == regs->DE.W) regs->AF.B.l |= (C_FLAG);

  	regs->DE.W = 0;
  	
  	return 0;
}

/*
static int
trap_load_block( libspectrum_tape_block *block, Z80Regs * regs )
{
  unsigned int  where, bytes, amount, skip, error;
  byte crc = 0;
  byte *src;
   
  where  = regs->IX.W ;
  amount = regs->DE.W ;
  skip   = 0;
  error  = 0;

  src = libspectrum_tape_block_data( block );
  bytes = libspectrum_tape_block_data_length( block );

  bytes-=2;

  {
  unsigned char id_tape;

  memcpy((unsigned char *)&id_tape, src, 1); src++; // read flag 

  crc^=id_tape;

  if(id_tape != (regs->AFs.B.h))     //if id not ok then 
  { 
   	 return 0; } //skip block+checksum and return error
  }

  if (amount > bytes)
  {
       skip   = amount - bytes;
       amount = bytes;
       error  = 1;
  }

  regs->IX.W += amount;
  regs->DE.W  = 0; 

  {
   unsigned int f;
   byte temp;

    for (f = 0; f < amount+skip; f++)
    {
     crc^=(temp=*src); src++;
     if(f < amount)
     {
      if (!(regs->AFs.B.l & C_FLAG))  //if verify (carry==0)...
       error+=(Z80ReadMem_notiming(where + f) != temp);
      else                            //else read
       Z80WriteMem_notiming (where + f, temp);
     }
    }

  src += bytes - amount; //skip unused bytes (if any!)

  regs->HL.B.l=temp;    //util?

  {
   unsigned char checksum;

   memcpy((unsigned char *)&checksum, src, 1); src++; // read flag 

   //if(!error)                      // if all went ok, return last byte read 
   {
    regs->HL.B.h=checksum;   

    //util?
    //if(crc!=checksum) error=1; // but if cheksum error, return error 
   }
  }
  }

  return (error ? 1 : 0); //return error/ok
}
*/

  /* On exit:
   *  A = calculated parity byte if parity checked, else 0 (CHECKME)
   *  F : if parity checked, all flags are modified
   *      else carry only is modified (FIXME)
   *  B = 0xB0 (success) or 0x00 (failure)
   *  C = 0x01 (confirmed), 0x21, 0xFE or 0xDE (CHECKME)
   * DE : decremented by number of bytes loaded or verified
   *  H = calculated parity byte or undefined
   *  L = last byte read, or 1 if none
   * IX : incremented by number of bytes loaded or verified
   * A' = unchanged on error + no flag byte, else 0x01
   * F' = 0x01      on error + no flag byte, else 0x45
   *  R = no point in altering it :-)
   * Other registers unchanged.
   */
/*
static int
trap_load_block( libspectrum_tape_block *block, Z80Regs *regs )
{
  libspectrum_byte parity, *data;
  int i = 0, length, read;

  data = libspectrum_tape_block_data( block );
  length = libspectrum_tape_block_data_length( block );

  // Number of bytes to load or verify 
  read = length - 1;
  if( read > regs->DE.W )
    read = regs->DE.W;

  // If there's no data in the block (!), set L then error exit.
  // We don't need to alter H, IX or DE here 
  if( !length ) {
    regs->HL.B.l =  regs->AFs.B.l = 1;
    regs->AF.B.l &= ~FLAG_C;
    return 0;
  }

  i = regs->AFs.B.h ; // i = A' (flag byte) 
  regs->AFs.W = 0x0145;
  regs->AF.B.h = 0;

  // Initialise the parity check and L to the block ID byte
  regs->HL.B.l = parity = *data++;

  // If the block ID byte != the flag byte, clear carry and return
  if( parity != i )
    goto error_ret;

  // Now set L to the *last* byte in the block 
  regs->HL.B.l = data[read - 1];

  // Loading or verifying determined by the carry flag of F'
  if( regs->AFs.B.l & FLAG_C ) {
    for( i = 0; i < read; i++ ) {
      parity ^= data[i];
      Z80WriteMem_notiming( regs->IX.W+i, data[i] );
    }
  } else {		// verifying 
    for( i = 0; i < read; i++ ) {
      parity ^= data[i];
      if( data[i] != Z80ReadMem_notiming(regs->IX.W+i) ) {
        // Verification failure 
        regs->HL.B.l = data[i];
	goto error_ret;
      }
    }
  }

  // At this point, i == number of bytes actually read or verified

  // If |DE| bytes have been read and there's more data, do the parity check
  if(  regs->DE.W == i && read + 1 < length ) {
    parity ^= data[read];
    regs->AF.B.h = parity;
    CP( 1 ); //parity check is successful if A==0
    regs->BC.B.h = 0xB0;
  } else {
    // Failure to read first bit of the next byte (ref. 48K ROM, 0x5EC) 
    regs->BC.B.h = 255;
    regs->HL.B.l = 1;
    INC( regs->BC.B.h );
error_ret:
    regs->AF.B.l &= ~FLAG_C;
  }

  // At this point, AF, AF', B and L are already modified 
  regs->BC.B.l = 1;
  regs->HL.B.h = parity;
  regs->DE.W -= i;
  regs->IX.W += i;
  return 0;
}
*/

/* Select the nth block on the tape; 0 => 1st block */
/* The same, but without updating the browser display */
int
tape_select_block(int n )
{
  //ui_tape_browser_update( UI_TAPE_BROWSER_SELECT_BLOCK, NULL );
  
  return libspectrum_tape_nth_block( tape, n );
}

/* Which block is current? */
int
tape_get_current_block( void )
{
  int n;
  libspectrum_error error;

  if( !libspectrum_tape_present( tape ) ) return -1;

  error = libspectrum_tape_position( &n, tape );
  if( error ) return -1;

  return n;
}

int
tape_is_tape(void)
{   
  return libspectrum_tape_present( tape );
}


static void
make_name( unsigned char *name, const unsigned char *data )
{
  size_t i;

  for( i = 0; i < 10; i++, name++, data++ ) {
    if( *data >= 32 && *data < 127 ) {
      *name = *data;
    } else {
      *name = '?';
    }
  }

  *name = '\0';
}

int
tape_block_details( char *buffer, size_t length,
		    libspectrum_tape_block *block )
{
  libspectrum_byte *data;
  const char *type; unsigned char name[11];
  int offset;

  buffer[0] = '\0';

  switch( libspectrum_tape_block_type( block ) ) {

  case LIBSPECTRUM_TAPE_BLOCK_ROM:
    /* See if this looks like a standard Spectrum header and if so
       display some extra data */
    if( libspectrum_tape_block_data_length( block ) != 19 ) goto normal;

    data = libspectrum_tape_block_data( block );

    /* Flag byte is 0x00 for headers */
    if( data[0] != 0x00 ) goto normal;

    switch( data[1] ) {
    case 0x00: type = "Program"; break;
    case 0x01: type = "Number array"; break;
    case 0x02: type = "Character array"; break;
    case 0x03: type = "Bytes"; break;
    default: goto normal;
    }

    make_name( name, &data[2] );

    snprintf( buffer, length, "%s: \"%s\"", type, name );

    break;

  normal:
    snprintf( buffer, length, "Data (%lu Bytes)",
	      (unsigned long)libspectrum_tape_block_data_length( block ) );
    break;

  case LIBSPECTRUM_TAPE_BLOCK_TURBO:
    snprintf( buffer, length, "Turbo Data (%lu Bytes)",
	      (unsigned long)libspectrum_tape_block_data_length( block ) );
    break;  
  case LIBSPECTRUM_TAPE_BLOCK_PURE_DATA:
    snprintf( buffer, length, "Pure Data (%lu Bytes)",
	      (unsigned long)libspectrum_tape_block_data_length( block ) );
    break;  
  case LIBSPECTRUM_TAPE_BLOCK_RAW_DATA:
    snprintf( buffer, length, "Raw Data (%lu Bytes)",
	      (unsigned long)libspectrum_tape_block_data_length( block ) );
    break;

  case LIBSPECTRUM_TAPE_BLOCK_PURE_TONE:
    snprintf( buffer, length, "Pure Tone (%lu tstates)",
	      (unsigned long)libspectrum_tape_block_pulse_length( block ) );
    break;

  case LIBSPECTRUM_TAPE_BLOCK_PULSES:
    snprintf( buffer, length, "Sequence Of %lu Pulses",
	      (unsigned long)libspectrum_tape_block_count( block ) );
    break;

  case LIBSPECTRUM_TAPE_BLOCK_PAUSE:
    if((unsigned long)libspectrum_tape_block_pause( block )==0)
      snprintf( buffer, length, "[Stop The Tape]");
    else 
      snprintf( buffer, length, "[Pause - %lu ms]",(unsigned long)libspectrum_tape_block_pause( block ));
    break;

  case LIBSPECTRUM_TAPE_BLOCK_GROUP_START:
  case LIBSPECTRUM_TAPE_BLOCK_COMMENT:
  case LIBSPECTRUM_TAPE_BLOCK_MESSAGE:
  case LIBSPECTRUM_TAPE_BLOCK_CUSTOM:
    snprintf( buffer, length, "[%s]", libspectrum_tape_block_text( block ) );
    break;

  case LIBSPECTRUM_TAPE_BLOCK_JUMP:
    offset = libspectrum_tape_block_offset( block );
    if( offset > 0 ) {
      snprintf( buffer, length, "[Forward %d Blocks]", offset );
    } else {
      snprintf( buffer, length, "[Backward %d Blocks]", -offset );
    }
    break;

  case LIBSPECTRUM_TAPE_BLOCK_LOOP_START:
    snprintf( buffer, length, "[Loop %lu Times]",
	      (unsigned long)libspectrum_tape_block_count( block ) );
    break;

  case LIBSPECTRUM_TAPE_BLOCK_SELECT:
    snprintf( buffer, length, "[Select %lu Options]",
	      (unsigned long)libspectrum_tape_block_count( block ) );
    break;

  case LIBSPECTRUM_TAPE_BLOCK_GENERALISED_DATA:
    snprintf( buffer, length, "[Generalised Data - %lu Data Symbols]",
	      (unsigned long)libspectrum_tape_generalised_data_symbol_table_symbols_in_block( (void*)libspectrum_tape_block_data_table( block ) ) );
    break;

  case LIBSPECTRUM_TAPE_BLOCK_RLE_PULSE:
    /* Could do something better with this one */
    break;

  case LIBSPECTRUM_TAPE_BLOCK_GROUP_END:
    snprintf( buffer, length, "[Group End]");
    break;
  case LIBSPECTRUM_TAPE_BLOCK_LOOP_END:
    snprintf( buffer, length, "[Loop End]");
    break;  
  case LIBSPECTRUM_TAPE_BLOCK_STOP48:
    snprintf( buffer, length, "[Stop The Tape If 48]");
    break;    
  case LIBSPECTRUM_TAPE_BLOCK_ARCHIVE_INFO:
    snprintf( buffer, length, "[Archive Info]");
    break;  
  case LIBSPECTRUM_TAPE_BLOCK_HARDWARE:
    snprintf( buffer, length, "[Hardware]");
    break;    
  case LIBSPECTRUM_TAPE_BLOCK_CONCAT:
    snprintf( buffer, length, "[Concat]");  
    break;

  }

  return 0;
}

int
tape_blocks_entries(char entries[][256],int length)
{
  libspectrum_tape_block *block;
  libspectrum_tape_iterator iterator;
  int num_entries=0;

  for( block = libspectrum_tape_iterator_init( &iterator, tape );
       block;
       block = libspectrum_tape_iterator_next( &iterator ),num_entries++)
  {     
      
       char *p=entries[num_entries];
       sprintf(p,"%4d: ",num_entries);
       tape_block_details(p+6,length-6,block );    
  }
  
  return num_entries;
}


