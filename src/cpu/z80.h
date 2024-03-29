/*=====================================================================
  z80.c -> Header file related to the Z80 emulation code.

   Please read documentation files to know how this works :)

   Thanks to Marat Fayzullin for writing the "How to write a Computer
   Eemulator" HOWTO. This emulator is based on his tutorials and the
   code organization (very readable!) of his "Z80 Portable Emulator".
   I've learnt a lot from it, and I've taken some ideas of his code
   to write this emulator.I think that almost all of the undocumented
   Z80 opcodes are covered on this emulator. I also asked Marat
   Fayzullin (by email) about ideas and so on (his Z80 emulator is
   quite good, so go check it :-).

   Of course, I can't forget Ra�l Gomez (he answered me thousands
   of email questions) and Philip Kendall. Whitout his ___kind___
   people surely you won't be reading this file now...

   "Programming the Z80" (from Rodnay Zaks) and the comp.sys.sinclair
   FAQ were another excelent sources of info!

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

 Copyright (c) 2000-2002 Santiago Romero Iglesias  (for ASpectrum)
 Copyright (c) 2004 rlyeh  (adapted for fzx32, also used by GP2Xpectrum)
 Copyright (c) 2006-2007 Metalbrain  (modifications for GP2Xpectrum)
 some small parts may be taken from or inspired by FUSE's equivalent file, which is
 Copyright (c) 1999-2006 Philip Kendall
 ======================================================================*/  
  

#ifndef Z80_H
#define Z80_H
  

#undef byte
#undef word
#undef dword
#undef offset

/*=== Some common standard data types: ==============================*/ 
typedef unsigned char byte;

typedef unsigned short word;

typedef unsigned long dword;

typedef signed char offset;



/*--- Thanks to Philip Kendall for it's help using the flags --------*/ 
extern byte halfcarry_add_table[];

extern byte halfcarry_sub_table[];

extern byte overflow_add_table[];

extern byte overflow_sub_table[];

extern byte sz53_table[];

extern byte sz53p_table[];

extern byte parity_table[];

/*=====================================================================
   Z80 Flag Register:       ---------------------------------
                            | 7   6   5   4   3   2   1   0 |
                            ---------------------------------
                            | S   Z   x   H   x  O/P  N   C |
                            ---------------------------------
   If (1) means that:           S   =  Negative result.
                                Z   =  Zero result.
                                x   =  special cases (by opcode)
                                H   =  Halfcarry/borrow.
                                O/P =  Overflow/Parity Flag.
                                N   =  Substraction.
                                C   =  Carry/borrow.
 ====================================================================*/ 
#define  S_FLAG    0x80
#define  Z_FLAG    0x40
#define  H_FLAG    0x10
#define  P_FLAG    0x04
#define  O_FLAG    0x04
#define  N_FLAG    0x02
#define  C_FLAG    0x01
  

/*
 Defines for interrupts and special Z80Hardware() codes:
=======================================================================
 INT_QUIT   =    Exit the emulation (for Z80Run())
 INT_NOINT  =    No interrupt required
 INT_IRQ    =    Standard RST 38h interrupt
 INT_NMI    =    Non-maskerable interrupt
*/ 
#define  INT_QUIT   0xFFFE
#define  INT_NOINT  0xFFFF
#define  INT_IRQ    0x0038
#define  INT_NMI    0x0066
  


/*=== A register is defined as follows: =============================*/ 
typedef union 
{
word W;
  
  struct 
  {
   #ifdef LOW_ENDIAN
   byte l, h;
   #else
   byte h, l;
   #endif
  }
  B;

  
 
}
eword;



#define  WE_ARE_ON_DD  1
#define  WE_ARE_ON_FD  2


/*=== Now we define the Z80 registers using the previous definition =*/ 
typedef struct 
{
int we_are_on_ddfd; 
/* the following is to take care of cycle counting */ 
  int IPeriod, ICount, IBackup, IIntTime;

word TrapAddress;
word IRequest;

 /* general and shadow z80 registers */ 
    eword AF, BC, DE, HL, IX, IY, PC, SP, 
AFs, BCs, DEs, HLs;

char machine_type;
  
   
   

    /* IFF and I registers, used on interrupts. */ 
    byte IFF1, IFF2, I, halted, R, R7;
   
char IM;
   
   

    
   

    /* DecodingErrors = set this to 1 for debugging purposes in order
       to trap undocumented or non implemented opcodes.
       Trace          = set this to 1 to start tracing. It's also set
       when PC reaches TrapAddress value.             */ 
byte DecodingErrors;
   

   
byte Trace, dobreak;
   
//byte BorderColor;
 

}
Z80Regs;



/*====================================================================
   Function declarations, read the .c file to know what they do.
 ===================================================================*/ 
void Z80Reset (register Z80Regs * regs, int, int);

// void Z80Interrupt (register Z80Regs *, register word);
// void Z80Interrupt_NC (register Z80Regs *, register word);

//void Z80Interrupt (register Z80Regs *);
void Z80Interrupt_NC (register Z80Regs *);

word Z80Run (register Z80Regs *, int);
word Z80Run_NC (register Z80Regs *, int);
word Z80Run_NCNI (register Z80Regs *, int);

void Z80Patch (register Z80Regs *);
void inline loader (register Z80Regs *);//TODO juntarla con la otra

byte Z80Debug (register Z80Regs *);

//word Z80Hardware (register Z80Regs *);

byte inline Z80ReadMem (register word);
byte inline Z80ReadMem_notiming (register word);

void inline Z80WriteMem (register word, register byte);
void inline Z80WriteMem_notiming (register word, register byte);

byte Z80InPort (register word);

void Z80OutPort (register word, register byte);


void Z80FlagTables (void);

word ParseOpcode (char *, char *, char *, word, Z80Regs *);

word Z80Dissasembler (Z80Regs *, char *, char *);


#endif	/* 
 */
