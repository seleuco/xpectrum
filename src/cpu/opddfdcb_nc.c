/*====================================================================
  opcodes_ddfdcb.c -> This file executes the DD/FD CB PREFIX opcodes.

  Those are the double prefix opcodes. We found the DD prefix, which
  means that we must treat HL as IX, and then we found the CB prefix,
  so we must apply this rule to the CB PREFIX list of opcodes. A
  signed byte displacement is also added, and it's located BEFORE
  the DD CB opcode:

     ie:     CB 2E        =  SRA (HL)
             DD CB xx 2E  =  SRA (IX+xx)

  (or...)

  Those are the double prefix opcodes. We found the FD prefix, which
  means that we must treat HL as IY, and then we found the CB prefix,
  so we must apply this rule to the CB PREFIX list of opcodes. A
  signed byte displacement is also added, and it's located BEFORE
  the FD CB opcode:

     ie:     CB 2E        =  SRA (HL)
             FD CB xx 2E  =  SRA (IY+xx)

 Call here using something like #define REGISTER regs->IX

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
 =====================================================================*/

/* 15 clock cycles minimum = FD/DD CB xx opcode = 4 + 4 + 3 + 4 */

tmpreg.W = REGISTER.W + (offset) Z80ReadMem_notiming (r_PC);
r_PC++;
opcode = Z80ReadMem_notiming (r_PC);
r_PC++;
r_meml = Z80ReadMem_notiming (tmpreg.W);
r_memh = tmpreg.W>>8;

switch (opcode & 0xf8)
  {
  case RLC_xIXY & 0xf8:
    RLC (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case RRC_xIXY & 0xf8:
    RRC (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case RL_xIXY & 0xf8:
    RL (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case RR_xIXY & 0xf8:
    RR (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case SLA_xIXY & 0xf8:
    SLA (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case SRA_xIXY & 0xf8:
    SRA (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case SLL_xIXY & 0xf8:
    SLL (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case SRL_xIXY & 0xf8:
    SRL (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;

  case BIT_0_xIXY & 0xf8:
    BIT_BIT_XY (0, r_meml, r_memh);
    AddCycles(20);
    break;

  case BIT_1_xIXY & 0xf8:
    BIT_BIT_XY (1, r_meml, r_memh);
    AddCycles(20);
    break;

  case BIT_2_xIXY & 0xf8:
    BIT_BIT_XY (2, r_meml, r_memh);
    AddCycles(20);
    break;

  case BIT_3_xIXY & 0xf8:
    BIT_BIT_XY (3, r_meml, r_memh);
    AddCycles(20);
    break;

  case BIT_4_xIXY & 0xf8:
    BIT_BIT_XY (4, r_meml,r_memh);
    AddCycles(20);
    break;

  case BIT_5_xIXY & 0xf8:
    BIT_BIT_XY (5, r_meml, r_memh);
    AddCycles(20);
    break;

  case BIT_6_xIXY & 0xf8:
    BIT_BIT_XY (6, r_meml, r_memh);
    AddCycles(20);
    break;
  case BIT_7_xIXY & 0xf8:
    BIT_BIT7_XY (r_meml, r_memh);
    AddCycles(20);
    break;

  case RES_0_xIXY & 0xf8:
    BIT_RES_mem_NC (0, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_1_xIXY & 0xf8:
    BIT_RES_mem_NC (1, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_2_xIXY & 0xf8:
    BIT_RES_mem_NC (2, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_3_xIXY & 0xf8:
    BIT_RES_mem_NC (3, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_4_xIXY & 0xf8:
    BIT_RES_mem_NC (4, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_5_xIXY & 0xf8:
    BIT_RES_mem_NC (5, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_6_xIXY & 0xf8:
    BIT_RES_mem_NC (6, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_7_xIXY & 0xf8:
    BIT_RES_mem_NC (7, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_0_xIXY & 0xf8:
    BIT_SET_mem_NC (0, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_1_xIXY & 0xf8:
    BIT_SET_mem_NC (1, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_2_xIXY & 0xf8:
    BIT_SET_mem_NC (2, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_3_xIXY & 0xf8:
    BIT_SET_mem_NC (3, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_4_xIXY & 0xf8:
    BIT_SET_mem_NC (4, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_5_xIXY & 0xf8:
    BIT_SET_mem_NC (5, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_6_xIXY & 0xf8:
    BIT_SET_mem_NC (6, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_7_xIXY & 0xf8:
    BIT_SET_mem_NC (7, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  default:
    break;
  }

if((opcode & 0xC0)!=0x40)
  {
  switch (opcode & 0x7)
    {
    case 0:	// LD B,
    	r_B = r_meml;
    	break;
    case 1:	// LD C,
    	r_C = r_meml;
    	break;
    case 2:	// LD D,
    	r_D = r_meml;
    	break;
    case 3:	// LD E,
    	r_E = r_meml;
    	break;
    case 4:	// LD H,
    	r_H = r_meml;
    	break;
    case 5:	// LD L,
    	r_L = r_meml;
    	break;
    case 6:
    	break;
    case 7:	// LD A,
    	r_A = r_meml;
    	break;
    }
  }
