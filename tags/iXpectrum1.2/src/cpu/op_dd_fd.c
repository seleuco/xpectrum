/*====================================================================
  opcodes_dd_fd.c -> This file executes the DD/FD PREFIX opcodes.

  The DD prefix "creates" some new instructions by changing HL to IX
  on the opcode defined by the next byte on memory.

  The FD prefix "creates" some new instructions by changing HL to IY
  on the opcode defined by the next byte on memory.

  Change the REGISTER variable to IX or HY before including this file.
  Something like:

        #define REGISTER  regs->IX
            #include "op_dd_fd.c"
        #undef  REGISTER

 On this code, this REGISTER variable is used as REGISTER.W or
 REGISTER.B.h and REGISTER.B.l ...

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

/* 8 clock cycles minimum = DD opcode = FD opcode = 4 + 4 */

#define REG  REGISTER.W
#define REGL REGISTER.B.l
#define REGH REGISTER.B.h

opcode = Z80ReadMem (r_PC);
AddCycles(1);
r_PC++;

#define AddCycles2(x) AddCycles((x)-8)

switch (opcode)
  {
  case ADD_IXY_BC:
    contend_read_byte_x7();
    ADD_WORD (REG, r_BC);
    break;
  case ADD_IXY_DE:
    contend_read_byte_x7();
    ADD_WORD (REG, r_DE);
    break;
  case ADD_IXY_SP:
    contend_read_byte_x7();
    ADD_WORD (REG, r_SP);
    break;
  case ADD_IXY_IXY:
    contend_read_byte_x7();
    ADD_WORD (REG, REG);
    break;
  case DEC_IXY:
    contend_read_byte_x2();
    REG--;
    break;
  case INC_IXY:
    contend_read_byte_x2();
    REG++;
    break;

  case JP_IXY:
    r_PC = REG;
    break;
  case LD_SP_IXY:
    contend_read_byte_x2();
    r_SP = REG;
    break;

  case PUSH_IXY:
    contend_read_byte();
    PUSH_IXYr ();
    break;
  case POP_IXY:
    POP_IXYr ();
    break;

  case EX_IXY_xSP:
    r_meml = Z80ReadMem (r_SP);
    r_memh = Z80ReadMem (r_SP + 1);contend_read(r_SP+1);
    Z80WriteMem (r_SP + 1, REGH, regs);
    Z80WriteMem (r_SP, REGL, regs);
    contend_read_x2(r_SP);
    REGL = r_meml;
    REGH = r_memh;
    break;

  case LD_A_xIXY:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_A = Z80ReadMem (REG + ((offset) r_meml));
    r_PC++;
    break;
  case LD_B_xIXY:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_B = Z80ReadMem (REG + ((offset) r_meml));
    r_PC++;
    break;
  case LD_C_xIXY:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_C = Z80ReadMem (REG + ((offset) r_meml));
    r_PC++;
    break;
  case LD_D_xIXY:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_D = Z80ReadMem (REG + ((offset) r_meml));
    r_PC++;
    break;
  case LD_E_xIXY:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_E = Z80ReadMem (REG + ((offset) r_meml));
    r_PC++;
    break;

  case LD_xIXY_A:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    Z80WriteMem (REG + (offset) r_meml, r_A, regs);
    r_PC++;
    break;
  case LD_xIXY_B:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    Z80WriteMem (REG + (offset) r_meml, r_B, regs);
    r_PC++;
    break;
  case LD_xIXY_C:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    Z80WriteMem (REG + (offset) r_meml, r_C, regs);
    r_PC++;
    break;
  case LD_xIXY_D:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    Z80WriteMem (REG + (offset) r_meml, r_D, regs);
    r_PC++;
    break;
  case LD_xIXY_E:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    Z80WriteMem (REG + (offset) r_meml, r_E, regs);
    r_PC++;
    break;

  case INC_xIXY:
    r_mem = REG + (offset) Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_PC++;
    tmpreg.B.l = Z80ReadMem (r_mem);
    INC (tmpreg.B.l);contend_read(r_mem);
    Z80WriteMem (r_mem, tmpreg.B.l, regs);
    break;
    
  case DEC_xIXY:
    r_mem = REG + (offset) Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_PC++;
    tmpreg.B.l = Z80ReadMem (r_mem);
    DEC (tmpreg.B.l);contend_read(r_mem);
    Z80WriteMem (r_mem, tmpreg.B.l, regs);
    break;

  case ADC_xIXY:
    r_memh = Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_meml = Z80ReadMem (REG + (offset) r_memh);
    r_PC++;
    ADC (r_meml);
    break;

  case SBC_xIXY:
    r_memh = Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_meml = Z80ReadMem (REG + (offset) r_memh);
    r_PC++;
    SBC (r_meml);
    break;
  case ADD_xIXY:
    r_memh = Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_meml = Z80ReadMem (REG + (offset) r_memh);
    r_PC++;
    ADD (r_meml);
    break;
  case SUB_xIXY:
    r_memh = Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_meml = Z80ReadMem (REG + (offset) r_memh);
    r_PC++;
    SUB (r_meml);
    break;
  case AND_xIXY:
    r_memh = Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_meml = Z80ReadMem (REG + (offset) r_memh);
    r_PC++;
    AND (r_meml);
    break;
  case OR_xIXY:
    r_memh = Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_meml = Z80ReadMem (REG + (offset) r_memh);
    r_PC++;
    OR (r_meml);
    break;
  case XOR_xIXY:
    r_memh = Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_meml = Z80ReadMem (REG + (offset) r_memh);
    r_PC++;
    XOR (r_meml);
    break;

  case CP_xIXY:
    r_memh = Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_meml = Z80ReadMem (REG + (offset) r_memh);
    r_PC++;
    CP (r_meml);
    break;

  case LD_IXY_NN:
    REGL = Z80ReadMem (r_PC);
    r_PC++;
    REGH = Z80ReadMem (r_PC);
    r_PC++;
    break;

  case LD_xIXY_N:
    r_mem = REG + (offset) Z80ReadMem (r_PC);
    r_PC++;
    r_tmph=Z80ReadMem (r_PC);
    contend_read_x2(r_PC);
    Z80WriteMem (r_mem, r_tmph, regs);
    r_PC++;
    break;

  case LD_IXY_xNN:
    LOAD_rr_nn (REG);
    break;

  case LD_xNN_IXY:
    STORE_nn_rr (REG);
    break;


/* some undocumented opcodes: may be wrong: */
  case LD_A_IXYh:
    r_A = REGH;
    break;
  case LD_A_IXYl:
    r_A = REGL;
    break;
  case LD_B_IXYh:
    r_B = REGH;
    break;
  case LD_B_IXYl:
    r_B = REGL;
    break;
  case LD_C_IXYh:
    r_C = REGH;
    break;
  case LD_C_IXYl:
    r_C = REGL;
    break;
  case LD_D_IXYh:
    r_D = REGH;
    break;
  case LD_D_IXYl:
    r_D = REGL;
    break;
  case LD_E_IXYh:
    r_E = REGH;
    break;
  case LD_E_IXYl:
    r_E = REGL;
    break;
  case LD_IXYh_A:
    REGH = r_A;
    break;
  case LD_IXYh_B:
    REGH = r_B;
    break;
  case LD_IXYh_C:
    REGH = r_C;
    break;
  case LD_IXYh_D:
    REGH = r_D;
    break;
  case LD_IXYh_E:
    REGH = r_E;
    break;
  case LD_IXYh_IXYh:
    break;
  case LD_IXYh_IXYl:
    REGH = REGL;
    break;
  case LD_IXYl_A:
    REGL = r_A;
    break;
  case LD_IXYl_B:
    REGL = r_B;
    break;
  case LD_IXYl_C:
    REGL = r_C;
    break;
  case LD_IXYl_D:
    REGL = r_D;
    break;
  case LD_IXYl_E:
    REGL = r_E;
    break;
  case LD_IXYl_IXYh:
    REGL = REGH;
    break;
  case LD_IXYl_IXYl:
    break;
  case LD_IXYh_N:
    REGH = Z80ReadMem (r_PC);
    r_PC++;
    break;
  case LD_IXYl_N:
    REGL = Z80ReadMem (r_PC);
    r_PC++;
    break;


  case ADD_IXYh:
    ADD (REGH);
    break;
  case ADD_IXYl:
    ADD (REGL);
    break;
  case ADC_IXYh:
    ADC (REGH);
    break;
  case ADC_IXYl:
    ADC (REGL);
    break;
  case SUB_IXYh:
    SUB (REGH);
    break;
  case SUB_IXYl:
    SUB (REGL);
    break;
  case SBC_IXYh:
    SBC (REGH);
    break;
  case SBC_IXYl:
    SBC (REGL);
    break;
  case AND_IXYh:
    AND (REGH);
    break;
  case AND_IXYl:
    AND (REGL);
    break;
  case XOR_IXYh:
    XOR (REGH);
    break;
  case XOR_IXYl:
    XOR (REGL);
    break;
  case OR_IXYh:
    OR (REGH);
    break;
  case OR_IXYl:
    OR (REGL);
    break;
  case CP_IXYh:
    CP (REGH);
    break;
  case CP_IXYl:
    CP (REGL);
    break;
  case INC_IXYh:
    INC (REGH);
    break;
  case INC_IXYl:
    INC (REGL);
    break;
  case DEC_IXYh:
    DEC (REGH);
    break;
  case DEC_IXYl:
    DEC (REGL);
    break;

  case LD_xIXY_H:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_PC++;
    Z80WriteMem (REG + (offset) r_meml, r_H, regs);
    break;
  case LD_xIXY_L:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_PC++;
    Z80WriteMem (REG + (offset) r_meml, r_L, regs);
    break;
  case LD_H_xIXY:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_H = Z80ReadMem (REG + ((offset) r_meml));
    r_PC++;
    break;
  case LD_L_xIXY:
    r_meml=Z80ReadMem (r_PC);
    contend_read_x5(r_PC);
    r_L = Z80ReadMem (REG + ((offset) r_meml));
    r_PC++;
    break;

  case PREFIX_CB:
#ifndef CPP_COMPILATION
#include "opddfdcb.c"
#else
#include "opddfdcb.cpp"
#endif
    break;

  default:
    AddCycles2 (4);
    r_PC--;			/* decode it the next time :) */
    SubR (1);

//    exit(1);
//    if( regs->DecodingErrors )
//    {
//      printf("z80 core: Unknown instruction: ");
//      if ( regs->we_are_on_ddfd == WE_ARE_ON_DD )
//         printf("DD ");
//      else
//         printf("FD ");
//      printf("%02Xh at PC=%04Xh.\n", Z80ReadMem(r_PC-1), r_PC-2 );
//    }
    break;
  }

#undef AddCycles2

#undef REG
#undef REGL
#undef REGH
