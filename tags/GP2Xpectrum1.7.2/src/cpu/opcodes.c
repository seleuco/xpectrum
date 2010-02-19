/*====================================================================
  opcodes.c -> This file executes the single-byte Z80 opcodes.

  The CPU fetchs the byte pointed by the PC register (Program Counter)
  into de IR (Instruction Register) and decodes it. The value of this
  fetched byte (opcode) determines what operation the CPU must do.
  On Z80 (which uses 8 bit for the IW register) this means that we
  can have 256 (2^8) different opcodes. The z80 uses a simple trick
  called PREFIXES to obtain more opcodes by using more than one byte
  in the decoding (see opcodes_cb.c to know how it does it).

  This file executes the whole list of single-byte opcodes.

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
 Copyright (c) 2008 Seleuco (Tape traps)
 =====================================================================*/

/* About the AddCycles(4) -> Remember that reading from memory
                             takes 3 cycles plus 1 of the decoding.
                             Add 3 cycles for each operand fetch, and
                             3 more for each memory write/read. */


case NOP:
break;

case LD_BC_NN:
LD_rr_nn (r_BC);
break;

case LD_xBC_A:
STORE_r (r_BC, r_A);
break;

case INC_BC:
contend_read_byte_x2();
r_BC++;
break;

case INC_B:
INC (r_B);
break;

case DEC_B:
DEC (r_B);
break;

case LD_B_N:
LD_r_n (r_B);
break;

case EX_AF_AF:
EX_WORD (r_AF, r_AFs);
break;

case LD_A_xBC:
LOAD_r (r_A, r_BC);
break;

case DEC_BC:
contend_read_byte_x2();
r_BC--;
break;

case INC_C:
INC (r_C);
break;

case DEC_C:
DEC (r_C);
break;

case LD_C_N:
LD_r_n (r_C);
break;

case LD_DE_NN:
LD_rr_nn (r_DE);
break;

case LD_xDE_A:
STORE_r (r_DE, r_A);
break;

case INC_DE:
contend_read_byte_x2();
r_DE++;
break;

case INC_D:
INC (r_D);
break;

case DEC_D:
DEC (r_D);
break;

case LD_D_N:
LD_r_n (r_D);
break;

case ADD_HL_BC:
contend_read_byte_x7();
ADD_WORD (r_HL, r_BC);
break;

case ADD_HL_DE:
contend_read_byte_x7();
ADD_WORD (r_HL, r_DE);
break;

case ADD_HL_HL:
contend_read_byte_x7();
ADD_WORD (r_HL, r_HL);
break;

case ADD_HL_SP:
contend_read_byte_x7();
ADD_WORD (r_HL, r_SP);
break;

case LD_A_xDE:
LOAD_r (r_A, r_DE);
break;

case DEC_DE:
contend_read_byte_x2();
r_DE--;
break;

case INC_E:
INC (r_E);
break;

case DEC_E:
DEC (r_E);
break;

case LD_E_N:
LD_r_n (r_E);
break;

case LD_HL_NN:
LD_rr_nn (r_HL);
break;

case LD_xNN_HL:
STORE_nn_rr (r_HL);
break;

case INC_HL:
contend_read_byte_x2();
r_HL++;
break;

case INC_H:
INC (r_H);
break;

case DEC_H:
DEC (r_H);
break;

case LD_H_N:
LD_r_n (r_H);
break;

case LD_HL_xNN:
LOAD_rr_nn (r_HL);
break;

case DEC_HL:
contend_read_byte_x2();
r_HL--;
break;

case INC_L:
INC (r_L);
break;

case DEC_L:
DEC (r_L);
break;

case LD_L_N:
LD_r_n (r_L);
break;

case LD_SP_NN:
LD_rr_nn (r_SP);
break;

case LD_xNN_A:
STORE_nn_r (r_A);
break;

case INC_SP:
contend_read_byte_x2();
r_SP++;
break;

case LD_xHL_N:
r_meml = Z80ReadMem (r_PC);r_PC++;
STORE_r (r_HL, r_meml);
break;

case LD_A_xNN:
LOAD_r_nn (r_A);
break;

case DEC_SP:
contend_read_byte_x2();
r_SP--;
break;

case INC_A:
INC (r_A);
break;

case DEC_A:
///////// EDGE LOADING TRAP
if(fast_edge_loading==2)
{
   if
   (
      Z80ReadMem_notiming(r_PC+1) == 0xFD &&    
      Z80ReadMem_notiming(r_PC) == 0x20  //JR NZ,-1
   )
   {
      r_A = 0x01;
   }
}
//////////////////////////
DEC (r_A);
break;

case LD_A_N:
LD_r_n (r_A);
break;

case LD_B_B:
LD_r_r (r_B, r_B);
break;

case LD_B_C:
LD_r_r (r_B, r_C);
break;

case LD_B_D:
LD_r_r (r_B, r_D);
break;

case LD_B_E:
LD_r_r (r_B, r_E);
break;

case LD_B_H:
LD_r_r (r_B, r_H);
break;

case LD_B_L:
LD_r_r (r_B, r_L);
break;

case LD_B_xHL:
LOAD_r (r_B, r_HL);
break;

case LD_B_A:
LD_r_r (r_B, r_A);
break;

case LD_C_B:
LD_r_r (r_C, r_B);
break;

case LD_C_C:
LD_r_r (r_C, r_C);
break;

case LD_C_D:
LD_r_r (r_C, r_D);
break;

case LD_C_E:
LD_r_r (r_C, r_E);
break;

case LD_C_H:
LD_r_r (r_C, r_H);
break;

case LD_C_L:
LD_r_r (r_C, r_L);
break;

case LD_C_xHL:
LOAD_r (r_C, r_HL);
break;

case LD_C_A:
LD_r_r (r_C, r_A);
break;

case LD_D_B:
LD_r_r (r_D, r_B);
break;

case LD_D_C:
LD_r_r (r_D, r_C);
break;

case LD_D_D:
LD_r_r (r_D, r_D);
break;

case LD_D_E:
LD_r_r (r_D, r_E);
break;

case LD_D_H:
LD_r_r (r_D, r_H);
break;

case LD_D_L:
LD_r_r (r_D, r_L);
break;

case LD_D_xHL:
LOAD_r (r_D, r_HL);
break;

case LD_D_A:
LD_r_r (r_D, r_A);
break;

case LD_E_B:
LD_r_r (r_E, r_B);
break;

case LD_E_C:
LD_r_r (r_E, r_C);
break;

case LD_E_D:
LD_r_r (r_E, r_D);
break;

case LD_E_E:
LD_r_r (r_E, r_E);
break;

case LD_E_H:
LD_r_r (r_E, r_H);
break;

case LD_E_L:
LD_r_r (r_E, r_L);
break;

case LD_E_xHL:
LOAD_r (r_E, r_HL);
break;

case LD_E_A:
LD_r_r (r_E, r_A);
break;

case LD_H_B:
LD_r_r (r_H, r_B);
break;

case LD_H_C:
LD_r_r (r_H, r_C);
break;

case LD_H_D:
LD_r_r (r_H, r_D);
break;

case LD_H_E:
LD_r_r (r_H, r_E);
break;

case LD_H_H:
LD_r_r (r_H, r_H);
break;

case LD_H_L:
LD_r_r (r_H, r_L);
break;

case LD_H_xHL:
LOAD_r (r_H, r_HL);
break;

case LD_H_A:
LD_r_r (r_H, r_A);
break;

case LD_L_B:
LD_r_r (r_L, r_B);
break;

case LD_L_C:
LD_r_r (r_L, r_C);
break;

case LD_L_D:
LD_r_r (r_L, r_D);
break;

case LD_L_E:
LD_r_r (r_L, r_E);
break;

case LD_L_H:
LD_r_r (r_L, r_H);
break;

case LD_L_L:
LD_r_r (r_L, r_L);
break;

case LD_L_xHL:
LOAD_r (r_L, r_HL);
break;

case LD_L_A:
LD_r_r (r_L, r_A);
break;

case LD_xHL_B:
STORE_r (r_HL, r_B);
break;

case LD_xHL_C:
STORE_r (r_HL, r_C);
break;

case LD_xHL_D:
STORE_r (r_HL, r_D);
break;

case LD_xHL_E:
STORE_r (r_HL, r_E);
break;

case LD_xHL_H:
STORE_r (r_HL, r_H);
break;

case LD_xHL_L:
STORE_r (r_HL, r_L);
break;

case LD_xHL_A:
STORE_r (r_HL, r_A);
break;

case LD_A_B:
LD_r_r (r_A, r_B);
break;

case LD_A_C:
LD_r_r (r_A, r_C);
break;

case LD_A_D:
LD_r_r (r_A, r_D);
break;

case LD_A_E:
LD_r_r (r_A, r_E);
break;

case LD_A_H:
LD_r_r (r_A, r_H);
break;

case LD_A_L:
LD_r_r (r_A, r_L);
break;

case LD_A_xHL:
LOAD_r (r_A, r_HL);
break;

case LD_A_A:
LD_r_r (r_A, r_A);
break;

case LD_SP_HL:
contend_read_byte_x2();
LD_r_r (r_SP, r_HL);
break;

case ADD_B:
ADD (r_B);
break;

case ADD_C:
ADD (r_C);
break;

case ADD_D:
ADD (r_D);
break;

case ADD_E:
ADD (r_E);
break;

case ADD_H:
ADD (r_H);
break;

case ADD_L:
ADD (r_L);
break;

case ADD_xHL:
r_meml = Z80ReadMem (r_HL);
ADD (r_meml);
break;

case ADD_A:
ADD (r_A);
break;

case ADC_B:
ADC (r_B);
break;

case ADC_C:
ADC (r_C);
break;

case ADC_D:
ADC (r_D);
break;

case ADC_E:
ADC (r_E);
break;

case ADC_H:
ADC (r_H);
break;

case ADC_L:
ADC (r_L);
break;

case ADC_xHL:
r_meml = Z80ReadMem (r_HL);
ADC (r_meml);
break;

case ADC_A:
ADC (r_A);
break;

case ADC_N:
r_meml = Z80ReadMem (r_PC);
r_PC++;
ADC (r_meml);
break;

case SUB_A:
SUB (r_A);
break;

case SUB_B:
SUB (r_B);
break;

case SUB_C:
SUB (r_C);
break;

case SUB_D:
SUB (r_D);
break;

case SUB_E:
SUB (r_E);
break;

case SUB_H:
SUB (r_H);
break;

case SUB_L:
SUB (r_L);
break;

case SUB_xHL:
r_meml = Z80ReadMem (r_HL);
SUB (r_meml);
break;

case SUB_N:
r_meml = Z80ReadMem (r_PC);
r_PC++;
SUB (r_meml);
break;

case SBC_A:
SBC (r_A);
break;

case SBC_B:
SBC (r_B);
break;

case SBC_C:
SBC (r_C);
break;

case SBC_D:
SBC (r_D);
break;

case SBC_E:
SBC (r_E);
break;

case SBC_H:
SBC (r_H);
break;

case SBC_L:
SBC (r_L);
break;

case SBC_xHL:
r_meml = Z80ReadMem (r_HL);
SBC (r_meml);
break;

case SBC_N:
r_meml = Z80ReadMem (r_PC);
r_PC++;
SBC (r_meml);
break;

case AND_B:
AND (r_B);
break;

case AND_C:
AND (r_C);
break;

case AND_D:
AND (r_D);
break;

case AND_E:
AND (r_E);
break;

case AND_H:
AND (r_H);
break;

case AND_L:
AND (r_L);
break;

case AND_xHL:
AND_mem (r_HL);
break;

case AND_A:
AND (r_A);
break;

case XOR_B:
XOR (r_B);
break;

case XOR_C:
XOR (r_C);
break;

case XOR_D:
XOR (r_D);
break;

case XOR_E:
XOR (r_E);
break;

case XOR_H:
XOR (r_H);
break;

case XOR_L:
XOR (r_L);
break;

case XOR_xHL:
XOR_mem (r_HL);
break;

case XOR_A:
XOR (r_A);
break;

case OR_B:
OR (r_B);
break;

case OR_C:
OR (r_C);
break;

case OR_D:
OR (r_D);
break;

case OR_E:
OR (r_E);
break;

case OR_H:
OR (r_H);
break;

case OR_L:
OR (r_L);
break;

case OR_xHL:
OR_mem (r_HL);
break;

case OR_A:
OR (r_A);
break;

case CP_A:
CP (r_A);
break;

case CP_B:
CP (r_B);
break;

case CP_C:
CP (r_C);
break;

case CP_D:
CP (r_D);
break;

case CP_E:
CP (r_E);
break;

case CP_H:
CP (r_H);
break;

case CP_L:
CP (r_L);
break;

case CP_xHL:
r_meml = Z80ReadMem (r_HL);
CP (r_meml);
break;

case CP_N:
r_meml = Z80ReadMem (r_PC);
r_PC++;
CP (r_meml);
break;

case RET_Z:
contend_read_byte();
if (TEST_FLAG (Z_FLAG))
  {
  RET_nn ();
  }
break;

case RET_C:
contend_read_byte();
if (TEST_FLAG (C_FLAG))
  {
  RET_nn ();
  }
break;

case RET_M:
contend_read_byte();
if (TEST_FLAG (S_FLAG))
  {
  RET_nn ();
  }
break;

case RET_PE:
contend_read_byte();
if (TEST_FLAG (P_FLAG))
  {
  RET_nn ();
  }
break;

case RET_PO:
contend_read_byte();
if (!TEST_FLAG (P_FLAG))
  {
  RET_nn ();
  }
break;

case RET_P:
contend_read_byte();
if (!TEST_FLAG (S_FLAG))
  {
  RET_nn ();
  }
break;

case RET:
RET_nn ();
break;

case RET_NZ:
contend_read_byte();
if (!TEST_FLAG (Z_FLAG))
  {
  RET_nn ();
  }
break;

case RET_NC:
contend_read_byte();
if (!TEST_FLAG (C_FLAG))
  {
  RET_nn ();
  }
break;

case ADD_N:
r_meml = Z80ReadMem (r_PC);
r_PC++;
ADD (r_meml);
break;

case JR:
JR_n ();
break;

case JR_NZ:
if (TEST_FLAG (Z_FLAG))
  {
  contend_read_jr();
  }
else
  {
  JR_n ();
  }

break;

case JR_Z:
if (TEST_FLAG (Z_FLAG))
  {
  JR_n ();
  }
else
  {
  contend_read_jr();
  }

break;

case JR_NC:
if (TEST_FLAG (C_FLAG))
  {
  contend_read_jr();
  }
else
  {
  JR_n ();
  }

break;

case JR_C:
if (TEST_FLAG (C_FLAG))
  {
  JR_n ();
  }
else
  {
  contend_read_jr();
  }

break;

case JP_NZ:
if (TEST_FLAG (Z_FLAG))
  {
  contend_read_jr();
  contend_read_jr();
  }
else
  {
  JP_nn ();
  }
break;

case JP:
JP_nn ();
break;

case JP_Z:
if (TEST_FLAG (Z_FLAG))
  {
  JP_nn ();
  }
else
  {
  contend_read_jr();
  contend_read_jr();
  }
break;

case JP_NC:
if (TEST_FLAG (C_FLAG))
  {
  contend_read_jr();
  contend_read_jr();
  }
else
  {
  JP_nn ();
  }
break;

case JP_C:
if (TEST_FLAG (C_FLAG))
  {
  JP_nn ();
  }
else
  {
  contend_read_jr();
  contend_read_jr();
  }
break;

case JP_PO:
if (TEST_FLAG (P_FLAG))
  {
  contend_read_jr();
  contend_read_jr();
  }
else
  {
  JP_nn ();
  }
break;

case JP_PE:
if (TEST_FLAG (P_FLAG))
  {
  JP_nn ();
  }
else
  {
  contend_read_jr();
  contend_read_jr();
  }
break;

case JP_P:
if (TEST_FLAG (S_FLAG))
  {
  contend_read_jr();
  contend_read_jr();
  }
else
  {
  JP_nn ();
  }
break;


case JP_M:
if (TEST_FLAG (S_FLAG))
  {
  JP_nn ();
  }
else
  {
  contend_read_jr();
  contend_read_jr();
  }
break;

case JP_xHL:
r_PC = r_HL;
break;

case CPL:
r_A ^= 0xFF;
r_F = (r_F & (FLAG_C | FLAG_P | FLAG_Z | FLAG_S)) |
  (r_A & (FLAG_3 | FLAG_5)) | (FLAG_N | FLAG_H);
break;

case INC_xHL:
r_meml = Z80ReadMem (r_HL);
INC (r_meml);contend_read(r_HL);
Z80WriteMem (r_HL, r_meml, regs);
break;

case DEC_xHL:
r_meml = Z80ReadMem (r_HL);
DEC (r_meml);contend_read(r_HL);
Z80WriteMem (r_HL, r_meml, regs);
break;

case SCF:
r_F &= FLAG_Z | FLAG_S | FLAG_P;
r_F |= (r_A & (FLAG_3 | FLAG_5));
r_F |= FLAG_C;
break;

case CCF:
r_F = (r_F & (FLAG_P | FLAG_Z | FLAG_S)) |
  ((r_F & FLAG_C) ? FLAG_H : FLAG_C) | (r_A & (FLAG_3 | FLAG_5));
break;

case HALT:
regs->halted = 1;
break;

case POP_BC:
POP (BC);
break;

case PUSH_BC:
contend_read_byte();
PUSH (BC);
break;

case POP_HL:
POP (HL);
break;

case PUSH_HL:
contend_read_byte();
PUSH (HL);
break;

case POP_AF:
POP (AF);
break;

case PUSH_AF:
contend_read_byte();
PUSH (AF);
break;

case POP_DE:
POP (DE);
break;

case PUSH_DE:
contend_read_byte();
PUSH (DE);
break;

case RLCA:
r_A = (r_A << 1) | (r_A >> 7);
r_F = (r_F & (FLAG_P | FLAG_Z | FLAG_S)) | (r_A & (FLAG_C | FLAG_3 | FLAG_5));
break;

case RRCA:
r_F = (r_F & (FLAG_P | FLAG_Z | FLAG_S)) | (r_A & FLAG_C);
r_A = (r_A >> 1) | (r_A << 7);
r_F |= (r_A & (FLAG_3 | FLAG_5));
break;

case DJNZ:
///////// EDGE LOADING TRAP
if(fast_edge_loading==2)
{
	  if(Z80ReadMem_notiming(r_PC) == 0xFE)
	  {
		   r_B=0x00;
		   r_PC++;
		   break;	   		        	
	  }	
}	
/////////////////////////
contend_read_byte();
r_B--;
if (r_B)
  {
  JR_n ();
  }
else
  {
  contend_read_jr();
  }

break;

case RLA:
r_meml = r_A;
r_A = (r_A << 1) | (r_F & FLAG_C);
r_F = (r_F & (FLAG_P | FLAG_Z | FLAG_S)) |
  (r_A & (FLAG_3 | FLAG_5)) | (r_meml >> 7);
break;

case RRA:
r_meml = r_A;
r_A = (r_A >> 1) | (r_F << 7);
r_F = (r_F & (FLAG_P | FLAG_Z | FLAG_S)) |
  (r_A & (FLAG_3 | FLAG_5)) | (r_meml & FLAG_C);
break;

case DAA:
r_meml = 0;
r_memh = (r_F & FLAG_C);
if ((r_F & FLAG_H) || ((r_A & 0x0f) > 9))
  r_meml = 6;
if (r_memh || (r_A > 0x99))
  r_meml |= 0x60;
if (r_A > 0x99)
  r_memh = 1;
if (r_F & FLAG_N)
  {
    if ((r_F & FLAG_H)&&((r_A & 0x0f) < 6))
      r_memh|=FLAG_H;
    SUB (r_meml);
  }
else
  {
    if((r_A & 0x0f) > 9)
      r_memh|=FLAG_H;
    ADD (r_meml);
  }

r_F = (r_F & ~(FLAG_C | FLAG_P | FLAG_H)) | r_memh | parity_table[r_A];
break;

case OUT_N_A:
r_meml=Z80ReadMem (r_PC);
r_memh=r_A;
ula_contend_port_early(r_mem);
Z80OutPort (regs, r_mem, r_A);
ula_contend_port_late(r_mem);
r_PC++;
break;

case IN_A_N:
r_meml=Z80ReadMem (r_PC);
r_memh=r_A;
ula_contend_port_early(r_mem);
ula_contend_port_late(r_mem);
r_A = Z80InPort (regs, r_mem);
r_PC++;
break;

case EX_HL_xSP:
r_meml = Z80ReadMem (r_SP);
r_memh = Z80ReadMem (r_SP + 1);contend_read(r_SP+1);
Z80WriteMem (r_SP + 1, r_H, regs);
Z80WriteMem (r_SP, r_L, regs);
contend_read_x2(r_SP);
r_L = r_meml;
r_H = r_memh;
break;

case EXX:
EX_WORD (r_BC, r_BCs);
EX_WORD (r_DE, r_DEs);
EX_WORD (r_HL, r_HLs);
break;

case EX_DE_HL:
EX_WORD (r_DE, r_HL);
break;

case AND_N:
AND_mem (r_PC);
r_PC++;
break;

case XOR_N:
XOR_mem (r_PC);
r_PC++;
break;

case OR_N:
OR_mem (r_PC);
r_PC++;
break;

case DI:
r_IFF1 = r_IFF2 = 0;
break;

case CALL:
CALL_nn ();
break;

case CALL_NZ:
if (TEST_FLAG (Z_FLAG))
  {
  contend_read_jr();
  contend_read_jr();
  }
else
  {
  CALL_nn ();
  }
break;

case CALL_NC:
if (TEST_FLAG (C_FLAG))
  {
  contend_read_jr();
  contend_read_jr();
  }
else
  {
  CALL_nn ();
  }

break;

case CALL_PO:
if (TEST_FLAG (P_FLAG))
  {
  contend_read_jr();
  contend_read_jr();
  }
else
  {
  CALL_nn ();
  }

break;

case CALL_P:
if (TEST_FLAG (S_FLAG))
  {
  contend_read_jr();
  contend_read_jr();
  }
else
  {
  CALL_nn ();
  }

break;


case CALL_Z:
if (TEST_FLAG (Z_FLAG))
  {
  CALL_nn ();
  }
else
  {
  contend_read_jr();
  contend_read_jr();
  }

break;

case CALL_C:
if (TEST_FLAG (C_FLAG))
  {
  CALL_nn ();
  }
else
  {
  contend_read_jr();
  contend_read_jr();
  }
break;

case CALL_PE:
if (TEST_FLAG (P_FLAG))
  {
  CALL_nn ();
  }
else
  {
  contend_read_jr();
  contend_read_jr();
  }

break;

case CALL_M:
if (TEST_FLAG (S_FLAG))
  {
  CALL_nn ();
  }
else
  {
  contend_read_jr();
  contend_read_jr();
  }
break;

case EI:
r_IFF1 = r_IFF2 = 1;
		    /*
		       Why Marat Fayzullin does this? ->

		       regs->IFF2 |= 0x01;
		       if( regs->IRequest != INT_NOINT )
		       {
		       regs->IBackup = regs->ICount;
		       regs->ICount = 0x1;
		       r_IFF |= 0x20;
		       } */
break;

case RST_00:
contend_read_byte();
RST (0x00);
break;

case RST_08:
contend_read_byte();
RST (0x08);
break;

case RST_10:
contend_read_byte();
RST (0x10);
break;

case RST_18:
contend_read_byte();
RST (0x18);
break;

case RST_20:
contend_read_byte();
RST (0x20);
break;

case RST_28:
contend_read_byte();
RST (0x28);
break;

case RST_30:
contend_read_byte();
RST (0x30);
break;

case RST_38:
contend_read_byte();
RST (0x38);
break;

default:
//    exit(1);
if (regs->DecodingErrors)
  printf ("z80 core: Unknown instruction: %02Xh at PC=%04Xh.\n",
	  Z80ReadMem (r_PC - 1), r_PC - 1);
break;
