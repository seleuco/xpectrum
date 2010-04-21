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

switch (opcode)
  {
  case RLC_xIXY:
    RLC (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case RRC_xIXY:
    RRC (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case RL_xIXY:
    RL (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case RR_xIXY:
    RR (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case SLA_xIXY:
    SLA (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case SRA_xIXY:
    SRA (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case SLL_xIXY:
    SLL (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case SRL_xIXY:
    SRL (r_meml);
    Z80WriteMem_notiming (tmpreg.W, r_meml, regs);
    AddCycles(23);
    break;
  case 0x40:
  case 0x41:
  case 0x42:
  case 0x43:
  case 0x44:
  case 0x45:
  case 0x47:
  case BIT_0_xIXY:
    BIT_BIT_XY (0, r_meml, r_memh);
    AddCycles(20);
    break;

  case 0x48:
  case 0x49:
  case 0x4a:
  case 0x4b:
  case 0x4c:
  case 0x4d:
  case 0x4f:
  case BIT_1_xIXY:
    BIT_BIT_XY (1, r_meml, r_memh);
    AddCycles(20);
    break;

  case 0x50:
  case 0x51:
  case 0x52:
  case 0x53:
  case 0x54:
  case 0x55:
  case 0x57:
  case BIT_2_xIXY:
    BIT_BIT_XY (2, r_meml, r_memh);
    AddCycles(20);
    break;

  case 0x58:
  case 0x59:
  case 0x5a:
  case 0x5b:
  case 0x5c:
  case 0x5d:
  case 0x5f:
  case BIT_3_xIXY:
    BIT_BIT_XY (3, r_meml, r_memh);
    AddCycles(20);
    break;

  case 0x60:
  case 0x61:
  case 0x62:
  case 0x63:
  case 0x64:
  case 0x65:
  case 0x67:
  case BIT_4_xIXY:
    BIT_BIT_XY (4, r_meml,r_memh);
    AddCycles(20);
    break;

  case 0x68:
  case 0x69:
  case 0x6a:
  case 0x6b:
  case 0x6c:
  case 0x6d:
  case 0x6f:
  case BIT_5_xIXY:
    BIT_BIT_XY (5, r_meml, r_memh);
    AddCycles(20);
    break;

  case 0x70:
  case 0x71:
  case 0x72:
  case 0x73:
  case 0x74:
  case 0x75:
  case 0x77:
  case BIT_6_xIXY:
    BIT_BIT_XY (6, r_meml, r_memh);
    AddCycles(20);
    break;
  case 0x78:
  case 0x79:
  case 0x7a:
  case 0x7b:
  case 0x7c:
  case 0x7d:
  case 0x7f:
  case BIT_7_xIXY:
    BIT_BIT7_XY (r_meml, r_memh);
    AddCycles(20);
    break;

  case RES_0_xIXY:
    BIT_RES_mem_NC (0, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_1_xIXY:
    BIT_RES_mem_NC (1, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_2_xIXY:
    BIT_RES_mem_NC (2, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_3_xIXY:
    BIT_RES_mem_NC (3, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_4_xIXY:
    BIT_RES_mem_NC (4, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_5_xIXY:
    BIT_RES_mem_NC (5, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_6_xIXY:
    BIT_RES_mem_NC (6, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case RES_7_xIXY:
    BIT_RES_mem_NC (7, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_0_xIXY:
    BIT_SET_mem_NC (0, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_1_xIXY:
    BIT_SET_mem_NC (1, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_2_xIXY:
    BIT_SET_mem_NC (2, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_3_xIXY:
    BIT_SET_mem_NC (3, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_4_xIXY:
    BIT_SET_mem_NC (4, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_5_xIXY:
    BIT_SET_mem_NC (5, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_6_xIXY:
    BIT_SET_mem_NC (6, tmpreg.W, r_meml);
    AddCycles(23);
    break;
  case SET_7_xIXY:
    BIT_SET_mem_NC (7, tmpreg.W, r_meml);
    AddCycles(23);
    break;


/*
 I must still include the undocumented opcodes such as:
 LD B, RLC(REGISTER+dd)       and so on ...

*/
  default:
//    exit(1);
    if (regs->DecodingErrors)
      {
	printf ("z80 core: Unknown instruction: ");
	if (regs->we_are_on_ddfd == WE_ARE_ON_DD)
	  printf ("DD");
	else
	  printf ("FD");
	printf ("CB %02Xh %02Xh at PC=%04Xh.\n",
		Z80ReadMem_notiming (r_PC - 2), Z80ReadMem_notiming (r_PC - 1), r_PC - 4);
      }
    break;
  }
