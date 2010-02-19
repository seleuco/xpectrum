/*====================================================================
  opcodes_ed.c -> This file executes the ED opcodes.

  Another prefix that "creates" new instructions. This prefix also
  introduces some undocumented opcodes that we've tried to include
  here. Maybe their implementation it's wrong: if you can find any
  mistake about how we have implemented/interpreted them, please
  let us know.

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

/* 8 clock cycles minimum = ED opcode = 4 + 4 */

opcode = Z80ReadMem_notiming (r_PC);
r_PC++;

switch (opcode)
  {
  case TRAPLOAD:
	 r_PC++;
	Z80Patch (regs);
	break;
  case LD_BC_xNNe:
    LOAD_rr_nn_NC (r_BC);
    AddCycles(20);
    break;
  case LD_DE_xNNe:
    LOAD_rr_nn_NC (r_DE);
    AddCycles(20);
    break;
  case LD_HL_xNNe:
    LOAD_rr_nn_NC (r_HL);
    AddCycles(20);
    break;
  case LD_SP_xNNe:
    LOAD_rr_nn_NC (r_SP);
    AddCycles(20);
    break;

  case LD_xNNe_BC:
    STORE_nn_rr_NC (r_BC);
    AddCycles(20);
    break;
  case LD_xNNe_DE:
    STORE_nn_rr_NC (r_DE);
    AddCycles(20);
    break;
  case LD_xNNe_HL:
    STORE_nn_rr_NC (r_HL);
    AddCycles(20);
    break;
  case LD_xNNe_SP:
    STORE_nn_rr_NC (r_SP);
    AddCycles(20);
    break;

  case NEG:
  case ED_5C:
  case ED_74:
  case ED_7C:
  case ED_6C:
  case ED_54:
  case ED_4C:
  case ED_64:
    NEG_A ();
    AddCycles(8);
    break;

  case RETI:
  case RETN:
  case ED_65:
  case ED_6D:
  case ED_75:
  case ED_7D:
  case ED_5D:
  case ED_55:
    r_IFF1 = r_IFF2;
    RET_nn_NC ();
    AddCycles(14);
    break;

  case IM_0:
  case ED_4E:			/* * IM 0/1 */
  case ED_6E:
  case ED_66:
    regs->IM = 0;
    AddCycles(8);
    break;			/* * IM 0 */


  case IM_1:
  case ED_76:
    regs->IM = 1;
    AddCycles(8);
    break;

  case IM_2:
  case ED_7E:
    regs->IM = 2;
    AddCycles(8);
    break;

  case ED_77:
  case ED_7F:
    AddCycles(8);
    break;			/* * NOP */

  case OUT_xC_B:
    AddCycles(9);
    Z80OutPort (regs, r_BC, r_B);
    AddCycles(3);
    break;
  case OUT_xC_C:
    AddCycles(9);
    Z80OutPort (regs, r_BC, r_C);
    AddCycles(3);
    break;
  case OUT_xC_D:
    AddCycles(9);  
    Z80OutPort (regs, r_BC, r_D);
    AddCycles(3);
    break;
  case OUT_xC_E:
    AddCycles(9);
    Z80OutPort (regs, r_BC, r_E);
    AddCycles(3);
    break;
  case OUT_xC_H:
    AddCycles(9);
    Z80OutPort (regs, r_BC, r_H);
    AddCycles(3);
    break;
  case OUT_xC_L:
    AddCycles(9);
    Z80OutPort (regs, r_BC, r_L);
    AddCycles(3);
    break;
  case OUT_xC_A:
    AddCycles(9);
    Z80OutPort (regs, r_BC, r_A);
    AddCycles(3);
    break;
    /* * OUT (C), 0 */
  case ED_71:
    AddCycles(9);
    Z80OutPort (regs, r_BC, 0);
    AddCycles(3);
    break;

  case IN_B_xC:
    IN_PORT_NC(r_B, r_BC);
    AddCycles(12);
    break;
  case IN_C_xC:
    IN_PORT_NC (r_C, r_BC);
    AddCycles(12);
    break;
  case IN_D_xC:
    IN_PORT_NC (r_D, r_BC);
    AddCycles(12);
    break;
  case IN_E_xC:
    IN_PORT_NC (r_E, r_BC);
    AddCycles(12);
    break;
  case IN_L_xC:
    IN_PORT_NC (r_L, r_BC);
    AddCycles(12);
    break;
  case IN_H_xC:
    IN_PORT_NC (r_H, r_BC);
    AddCycles(12);
    break;
  case IN_A_xC:
    IN_PORT_NC (r_A, r_BC);
    AddCycles(12);
    break;
  case IN_F_xC:
    IN_PORT_NC (r_meml, r_BC);
    AddCycles(12);
    break;

  case LD_A_I:
    r_A = regs->I;
    r_F = (r_F & FLAG_C) | sz53_table[r_A] | (regs->IFF2 ? FLAG_V : 0);
    AddCycles(9);
    break;

  case LD_I_A:
    regs->I = r_A;
    AddCycles(9);
    break;

  case LD_A_R:
    r_A = (regs->R.W & 0x7f) | (regs->R.W & 0x80);
    r_F = (r_F & FLAG_C) | sz53_table[r_A] | (regs->IFF2 ? FLAG_V : 0);
    AddCycles(9);
    break;

  case LD_R_A:
    regs->R.W = r_A;
    AddCycles(9);
    break;


  case ADC_HL_BC:
    ADC_WORD (r_BC);
    AddCycles(15);
    break;
  case ADC_HL_DE:
    ADC_WORD (r_DE);
    AddCycles(15);
    break;
  case ADC_HL_HL:
    ADC_WORD (r_HL);
    AddCycles(15);
    break;
  case ADC_HL_SP:
    ADC_WORD (r_SP);
    AddCycles(15);
    break;

  case SBC_HL_BC:
    SBC_WORD (r_BC);
    AddCycles(15);
    break;
  case SBC_HL_DE:
    SBC_WORD (r_DE);
    AddCycles(15);
    break;
  case SBC_HL_HL:
    SBC_WORD (r_HL);
    AddCycles(15);
    break;
  case SBC_HL_SP:
    SBC_WORD (r_SP);
    AddCycles(15);
    break;

  case RRD:
    r_meml = Z80ReadMem_notiming (r_HL);
    Z80WriteMem_notiming (r_HL, (r_A << 4) | (r_meml >> 4));
    r_A = (r_A & 0xf0) | (r_meml & 0x0f);
    r_F = (r_F & FLAG_C) | sz53p_table[r_A];
    AddCycles(18);
    break;

  case RLD:
    r_meml = Z80ReadMem_notiming (r_HL);
    Z80WriteMem_notiming (r_HL, (r_meml << 4) | (r_A & 0x0f));
    r_A = (r_A & 0xf0) | (r_meml >> 4);
    r_F = (r_F & FLAG_C) | sz53p_table[r_A];
    AddCycles(18);
    break;

  case LDI:
    r_meml = Z80ReadMem_notiming (r_HL);
    r_HL++;
    Z80WriteMem_notiming (r_DE, r_meml);
    r_DE++;
    r_BC--;
    r_meml += r_A;
    r_F = (r_F & (FLAG_C | FLAG_Z | FLAG_S)) |
      (r_BC ? FLAG_V : 0) | (r_meml & FLAG_3) |
      ((r_meml & 0x02) ? FLAG_5 : 0);
    AddCycles(16);
    break;

  case LDIR:
    r_meml = Z80ReadMem_notiming (r_HL);
    r_HL++;
    Z80WriteMem_notiming (r_DE, r_meml);
    r_DE++;
    r_BC--;
    r_meml += r_A;
    r_F = (r_F & (FLAG_C | FLAG_Z | FLAG_S)) |
      (r_BC ? FLAG_V : 0) | (r_meml & FLAG_3) |
      ((r_meml & 0x02) ? FLAG_5 : 0);
    AddCycles(16);
    if (r_BC)
      {
      r_PC -= 2;
      AddCycles(5);
      }
    break;

  case LDD:
    r_meml = Z80ReadMem_notiming (r_HL);
    r_HL--;
    Z80WriteMem_notiming (r_DE, r_meml);
    r_DE--;
    r_BC--;
    r_meml += r_A;
    r_F = (r_F & (FLAG_C | FLAG_Z | FLAG_S)) |
      (r_BC ? FLAG_V : 0) | (r_meml & FLAG_3) |
      ((r_meml & 0x02) ? FLAG_5 : 0);
    AddCycles(16);
    break;


  case LDDR:
    r_meml = Z80ReadMem_notiming (r_HL);
    Z80WriteMem_notiming (r_DE, r_meml);
    r_HL--;
    r_DE--;
    r_BC--;
    r_meml += r_A;
    r_F = (r_F & (FLAG_C | FLAG_Z | FLAG_S)) |
      (r_BC ? FLAG_V : 0) | (r_meml & FLAG_3) |
      ((r_meml & 0x02) ? FLAG_5 : 0);
    AddCycles(16);
    if (r_BC)
      {
      r_PC -= 2;
      AddCycles(5);
      }
    break;

    // I had lots of problems with CPI, INI, CPD, IND, OUTI, OUTD and so...
    // Thanks a lot to Philip Kendall for letting me to take a look to his
    // fuse emulator and allowing me to use their flag routines :-)
  case CPI:		// "Inspired" by YAZE
    r_meml = Z80ReadMem_notiming (r_HL);
    r_memh = r_A - r_meml;
    r_opl = r_A ^ r_meml ^ r_memh;
    r_HL++;
    r_F = (r_F & FLAG_C) | (r_memh & FLAG_S) | (!(r_memh & 0xff)<<6)|
        (((r_memh - ((r_opl&16)>>4))&2) << 4) | (r_opl & 16) |
	((r_memh - ((r_opl >> 4) & 1)) & 8) |
	((--r_BC & 0xffff) != 0) << 2 | 2;
    if ((r_memh & 15) == 8 && (r_opl & 16) != 0)
    	r_F &= ~8;
    AddCycles(16);
    break;

  case CPIR:
    r_meml = Z80ReadMem_notiming (r_HL);
    r_memh = r_A - r_meml;
    r_opl = r_A ^ r_meml ^ r_memh;
    r_HL++;
    r_F = (r_F & FLAG_C) | (r_memh & FLAG_S) | (!(r_memh & 0xff)<<6)|
        (((r_memh - ((r_opl&16)>>4))&2) << 4) | (r_opl & 16) |
	((r_memh - ((r_opl >> 4) & 1)) & 8) |
	((--r_BC & 0xffff) != 0) << 2 | 2;
    if ((r_memh & 15) == 8 && (r_opl & 16) != 0)
    	r_F &= ~8;
    AddCycles(16);
    if ((r_F & (FLAG_V | FLAG_Z)) == FLAG_V)
      {
      r_PC -= 2;
      AddCycles(5);
      }
    break;

  case CPD:
    r_meml = Z80ReadMem_notiming (r_HL);
    r_memh = r_A - r_meml;
    r_opl = ((r_A & 0x08) >> 3) |
      (((r_meml) & 0x08) >> 2) | ((r_memh & 0x08) >> 1);
    r_HL--;
    r_BC--;
    r_F = (r_F & FLAG_C) |
      (r_BC ? (FLAG_V | FLAG_N) : FLAG_N) |
      halfcarry_sub_table[r_opl] | (r_memh ? 0 : FLAG_Z) | (r_memh & FLAG_S);
    if (r_F & FLAG_H)
      r_memh--;
    r_F |= (r_memh & FLAG_3) | ((r_memh & 0x02) ? FLAG_5 : 0);
    AddCycles(16);
    break;

  case CPDR:
    r_meml = Z80ReadMem_notiming (r_HL);
    r_memh = r_A - r_meml;
    r_opl = ((r_A & 0x08) >> 3) |
      (((r_meml) & 0x08) >> 2) | ((r_memh & 0x08) >> 1);
    r_HL--;
    r_BC--;
    r_F = (r_F & FLAG_C) |
      (r_BC ? (FLAG_V | FLAG_N) : FLAG_N) |
      halfcarry_sub_table[r_opl] | (r_memh ? 0 : FLAG_Z) | (r_memh & FLAG_S);
    if (r_F & FLAG_H)
      r_memh--;
    r_F |= (r_memh & FLAG_3) | ((r_memh & 0x02) ? FLAG_5 : 0);
    AddCycles(16);
    if ((r_F & (FLAG_V | FLAG_Z)) == FLAG_V)
      {
      r_PC -= 2;
      AddCycles(5);
      }
    break;

  case IND:
    r_meml = Z80InPort (regs, (r_BC));
    r_memh = 0;
    r_F = (r_F & FLAG_C) | ((r_B) & 0x0f ? 0 : FLAG_H) | FLAG_N;
    (r_B)--;
    r_F |= ((r_B) == 0x7f ? FLAG_V : 0) | sz53_table[(r_B)];
    r_F &= 0xE8;
    Z80WriteMem_notiming (r_HL, r_meml);
    r_F |= ((r_meml & 0x80) >> 6);

    r_op = r_meml+((r_C-1)&0xff);
    if (r_op>0xff) r_F |= 0x11;
    r_opl = (r_op & 7) ^ r_B;
    r_F |= parity_table[r_opl]<<2;
   
    r_HL--;
    AddCycles(16);
    break;

  case INDR:
    r_meml = Z80InPort (regs, (r_BC));
    r_memh = 0;
    r_F = (r_F & FLAG_C) | ((r_B) & 0x0f ? 0 : FLAG_H) | FLAG_N;
    (r_B)--;
    r_F |= ((r_B) == 0x7f ? FLAG_V : 0) | sz53_table[(r_B)];
    r_F &= 0xE8;
    Z80WriteMem_notiming (r_HL, r_meml);
    r_F |= ((r_meml & 0x80) >> 6);
    
    r_op = r_meml+((r_C-1)&0xff);
    if (r_op>0xff) r_F |= 0x11;
    r_opl = (r_op & 7) ^ r_B;
    r_F |= parity_table[r_opl]<<2;

    r_HL--;
    AddCycles(16);
    if (r_B)
      {
      r_PC -= 2;
      AddCycles(5);
      }
    break;

  case INI:
    r_meml = Z80InPort (regs, (r_BC));
    r_memh = 0;
    r_F = (r_F & FLAG_C) | ((r_B) & 0x0f ? 0 : FLAG_H) | FLAG_N;
    (r_B)--;
    r_F |= ((r_B) == 0x7f ? FLAG_V : 0) | sz53_table[(r_B)];
    r_F &= 0xE8;
    Z80WriteMem_notiming (r_HL, r_meml);
    r_F |= ((r_meml & 0x80) >> 6);

    r_op = r_meml+((r_C+1)&0xff);
    if (r_op>0xff) r_F |= 0x11;
    r_opl = (r_op & 7) ^ r_B;
    r_F |= parity_table[r_opl]<<2;
    r_HL++;
    AddCycles(16);
    break;


  case INIR:
    r_meml = Z80InPort (regs, (r_BC));
    r_memh = 0;
    r_F = (r_F & FLAG_C) | ((r_B) & 0x0f ? 0 : FLAG_H) | FLAG_N;
    (r_B)--;
    r_F |= ((r_B) == 0x7f ? FLAG_V : 0) | sz53_table[(r_B)];
    r_F &= 0xE8;
    Z80WriteMem_notiming (r_HL, r_meml);
    r_F |= ((r_meml & 0x80) >> 6);

    r_op = r_meml+((r_C+1)&0xff);
    if (r_op>0xff) r_F |= 0x11;
    r_opl = (r_op & 7) ^ r_B;
    r_F |= parity_table[r_opl]<<2;
    AddCycles(16);
    r_HL++;
    if (r_B)
      {
      r_PC -= 2;
      AddCycles(5);
      }
    break;

  case OUTI:
    r_meml = Z80ReadMem_notiming (r_HL);
    r_memh = 0;
    r_F = (r_F & FLAG_C) | ((r_B) & 0x0f ? 0 : FLAG_H) | FLAG_N;
    (r_B)--;
    r_F |= ((r_B) == 0x7f ? FLAG_V : 0) | sz53_table[(r_B)];
    r_F &= 0xE8;
    AddCycles(13);
    Z80OutPort (regs, r_BC, r_meml);
    r_F |= ((r_meml & 0x80) >> 6);


    r_op = r_meml+((r_L)&0xff);
    if (r_op>0xff) r_F |= 0x11;
    r_opl = (r_op & 7) ^ r_B;
    r_F |= parity_table[r_opl]<<2;

    r_HL++;
    AddCycles(3);
    break;

  case OTIR:
    r_meml = Z80ReadMem_notiming (r_HL);
    r_memh = 0;
    r_F = (r_F & FLAG_C) | ((r_B) & 0x0f ? 0 : FLAG_H) | FLAG_N;
    (r_B)--;
    r_F |= ((r_B) == 0x7f ? FLAG_V : 0) | sz53_table[(r_B)];
    r_F &= 0xE8;
    AddCycles(13);
    Z80OutPort (regs, r_BC, r_meml);
    r_F |= ((r_meml & 0x80) >> 6);

    r_op = r_meml+((r_L)&0xff);
    if (r_op>0xff) r_F |= 0x11;
    r_opl = (r_op & 7) ^ r_B;
    r_F |= parity_table[r_opl]<<2;

    r_HL++;
    AddCycles(3);
    if (r_B)
      {
      r_PC -= 2;
      AddCycles(5);
      }
    break;


  case OUTD:
    r_meml = Z80ReadMem_notiming (r_HL);
    r_memh = 0;
    r_F = (r_F & FLAG_C) | ((r_B) & 0x0f ? 0 : FLAG_H) | FLAG_N;
    (r_B)--;
    r_F |= ((r_B) == 0x7f ? FLAG_V : 0) | sz53_table[(r_B)];
    r_F &= 0xE8;
    AddCycles(13);
    Z80OutPort (regs, r_BC, r_meml);
    r_F |= ((r_meml & 0x80) >> 6);

    r_op = r_meml+((r_L)&0xff);
    if (r_op>0xff) r_F |= 0x11;
    r_opl = (r_op & 7) ^ r_B;
    r_F |= parity_table[r_opl]<<2;

    r_HL--;
    AddCycles(3);
    break;

  case OTDR:
    r_meml = Z80ReadMem_notiming (r_HL);
    r_memh = 0;
    r_F = (r_F & FLAG_C) | ((r_B) & 0x0f ? 0 : FLAG_H) | FLAG_N;
    (r_B)--;
    r_F |= ((r_B) == 0x7f ? FLAG_V : 0) | sz53_table[(r_B)];
    r_F &= 0xE8;
    AddCycles(13);
    Z80OutPort (regs, r_BC, r_meml);
    r_F |= ((r_meml & 0x80) >> 6);

    r_op = r_meml+((r_L)&0xff);
    if (r_op>0xff) r_F |= 0x11;
    r_opl = (r_op & 7) ^ r_B;
    r_F |= parity_table[r_opl]<<2;

    r_HL--;
    AddCycles(3);
    if (r_B)
      {
      r_PC -= 2;
      AddCycles(5);
      }
    break;

  case PREFIX_ED:
    AddCycles (12);              /* ED ED xx = 12 cycles min = 4+8 */
    r_PC--;
    break;

  default:
// exit(1);
    if (regs->DecodingErrors)
      printf ("z80 core: Unknown instruction: ED %02Xh at PC=%04Xh.\n",
	      Z80ReadMem_notiming (r_PC - 1), r_PC - 2);
    break;
  }
