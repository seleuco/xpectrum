/*=====================================================================
  Macros.c -> Macros used on the opcode execution.

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

/* defines for the registers: faster access to them when coding... */

#define   r_PC    regs->PC.W
#define   r_PCl   regs->PC.B.l
#define   r_PCh   regs->PC.B.h
#define   r_SP    regs->SP.W
#define   r_IFF1  regs->IFF1
#define   r_IFF2  regs->IFF2
#define   r_R     regs->R
#define   r_R7    regs->R7

#define   r_AF    regs->AF.W
#define   r_A     regs->AF.B.h
#define   r_F     regs->AF.B.l
#define   r_BC    regs->BC.W
#define   r_B     regs->BC.B.h
#define   r_C     regs->BC.B.l
#define   r_DE    regs->DE.W
#define   r_D     regs->DE.B.h
#define   r_E     regs->DE.B.l
#define   r_HL    regs->HL.W
#define   r_H     regs->HL.B.h
#define   r_L     regs->HL.B.l
#define   r_IX    regs->IX.W
#define   r_IXh   regs->IX.B.h
#define   r_IXl   regs->IX.B.l
#define   r_IY    regs->IY.W
#define   r_IYh   regs->IY.B.h
#define   r_IYl   regs->IY.B.l

#define   r_AFs   regs->AFs.W
#define   r_As    regs->AFs.B.h
#define   r_Fs    regs->AFs.B.l
#define   r_BCs   regs->BCs.W
#define   r_Bs    regs->BCs.B.h
#define   r_Cs    regs->BCs.B.l
#define   r_DEs   regs->DEs.W
#define   r_Ds    regs->DEs.B.h
#define   r_Es    regs->DEs.B.l
#define   r_HLs   regs->HLs.W
#define   r_Hs    regs->HLs.B.h
#define   r_Ls    regs->HLs.B.l
#define   r_IXs   regs->IX.W
#define   r_IXhs  regs->IX.B.h
#define   r_IXls  regs->IX.B.l
#define   r_IYs   regs->IY.W
#define   r_IYhs  regs->IY.B.h
#define   r_IYls  regs->IY.B.l

#define   r_op    ops.W
#define   r_oph   ops.B.h
#define   r_opl   ops.B.l
#define   r_tmp   tmpreg2.W
#define   r_tmph  tmpreg2.B.h
#define   r_tmpl  tmpreg2.B.l
#define   r_mem   mread.W
#define   r_memh  mread.B.h
#define   r_meml  mread.B.l

#ifndef _DISASM_
/*--- Flag tables by Philip Kendall, taken from it's fuse emulator -*/
/*--- I was having headache trying to emulate correctly the FLAGS,
      so I finished using the FLAG tables used by P. Kendall. ------*/
#define FLAG_C  0x01
#define FLAG_N  0x02
#define FLAG_P  0x04
#define FLAG_V  FLAG_P
#define FLAG_3  0x08
#define FLAG_H  0x10
#define FLAG_5  0x20
#define FLAG_Z  0x40
#define FLAG_S  0x80


#endif


/*--- Memory Read from the A address on no bank machines -------------*/
//#define Z80ReadMem(A)   ((regs->RAM[(A)]))

/* macros to change the ICount register */
#define AddCycles(n) { regs->ICount-=(n); }

#define SubCycles(n) regs->ICount+=(n)

//#define AddR(n) r_R = (r_R+(n))
#define AddR(n) r_R = r_R + n;
#define SubR(n) r_R = r_R - n;

//#define AddR(n) r_R |= ((r_R+(n)) & 0x7f)
//#define SubR(n) r_R |= ((r_R-(n)) & 0x7f)

/* setting and resetting the flag bits: */
#define SET_FLAG(flag)        (r_F |= (flag))

#define RESET_FLAG(flag)      (r_F &= ~(flag))

#define TEST_FLAG(flag)       (r_F & (flag))


/* store a given register in the stack (hi and lo bytes) */
#define PUSH(rreg)                              \
  Z80WriteMem( --(r_SP), regs->rreg.B.h, regs); \
  Z80WriteMem( --(r_SP), regs->rreg.B.l, regs)

#define PUSH_NC(rreg)                              \
  Z80WriteMem_notiming( --(r_SP), regs->rreg.B.h); \
  Z80WriteMem_notiming( --(r_SP), regs->rreg.B.l)
  
#define POP(rreg)\
  regs->rreg.B.l = Z80ReadMem(r_SP);r_SP++;\
  regs->rreg.B.h = Z80ReadMem(r_SP);r_SP++

#define POP_NC(rreg)\
  regs->rreg.B.l = Z80ReadMem_notiming(r_SP);r_SP++;\
  regs->rreg.B.h = Z80ReadMem_notiming(r_SP);r_SP++

#define PUSH_IXYr() \
  Z80WriteMem( --(r_SP), REGH, regs); \
  Z80WriteMem( --(r_SP), REGL, regs)

#define PUSH_IXYr_NC() \
  Z80WriteMem_notiming( --(r_SP), REGH); \
  Z80WriteMem_notiming( --(r_SP), REGL)

#define POP_IXYr()\
  REGL = Z80ReadMem(r_SP);r_SP++; \
  REGH = Z80ReadMem(r_SP);r_SP++

#define POP_IXYr_NC()\
  REGL = Z80ReadMem_notiming(r_SP);r_SP++; \
  REGH = Z80ReadMem_notiming(r_SP);r_SP++

#define RST(rstval)  PUSH(PC); r_PC=(rstval)

#define RST_NC(rstval)  PUSH_NC(PC); r_PC=(rstval)

/*--- Move data to mem or regs --------------------------------------*/
#define LD_r_r(dreg, sreg)  (dreg) = (sreg)

#define STORE_r(daddreg, sreg)  Z80WriteMem((daddreg), (sreg), regs);

#define STORE_r_NC(daddreg, sreg)  Z80WriteMem_notiming((daddreg), (sreg));


#define STORE_nn_rr(dreg) \
                        r_opl = Z80ReadMem(r_PC);r_PC++;\
                        r_oph = Z80ReadMem(r_PC);r_PC++;\
                        r_tmp = dreg; \
                        Z80WriteMem((r_op),r_tmpl, regs); \
                        Z80WriteMem((r_op+1),r_tmph, regs);


#define STORE_nn_rr_NC(dreg) \
                        r_opl = Z80ReadMem_notiming(r_PC);r_PC++;\
                        r_oph = Z80ReadMem_notiming(r_PC);r_PC++;\
                        r_tmp = dreg; \
                        Z80WriteMem_notiming((r_op),r_tmpl); \
                        Z80WriteMem_notiming((r_op+1),r_tmph);


#define STORE_nn_r(sreg) \
                        r_opl = Z80ReadMem(r_PC); r_PC++; \
                        r_oph = Z80ReadMem(r_PC); r_PC++; \
                        Z80WriteMem((r_op),(sreg), regs);
                        
#define STORE_nn_r_NC(sreg) \
                        r_opl = Z80ReadMem_notiming(r_PC); r_PC++; \
                        r_oph = Z80ReadMem_notiming(r_PC); r_PC++; \
                        Z80WriteMem_notiming((r_op),(sreg));

#define LOAD_r(dreg, saddreg)   (dreg)=Z80ReadMem((saddreg))

#define LOAD_r_NC(dreg, saddreg)   (dreg)=Z80ReadMem_notiming((saddreg))


#define LOAD_rr_nn(dreg)   r_opl = Z80ReadMem(r_PC); r_PC++; \
                           r_oph = Z80ReadMem(r_PC); r_PC++; \
                           r_tmpl = Z80ReadMem(r_op); \
                           r_tmph = Z80ReadMem((r_op)+1);\
                           dreg=r_tmp

#define LOAD_rr_nn_NC(dreg)   r_opl = Z80ReadMem_notiming(r_PC); r_PC++; \
                           r_oph = Z80ReadMem_notiming(r_PC); r_PC++; \
                           r_tmpl = Z80ReadMem_notiming(r_op); \
                           r_tmph = Z80ReadMem_notiming((r_op)+1);\
                           dreg=r_tmp

#define LOAD_r_nn(dreg)    r_opl = Z80ReadMem(r_PC); r_PC++; \
                           r_oph = Z80ReadMem(r_PC); r_PC++; \
                           dreg = Z80ReadMem(r_op);

#define LOAD_r_nn_NC(dreg)    r_opl = Z80ReadMem_notiming(r_PC); r_PC++; \
                           r_oph = Z80ReadMem_notiming(r_PC); r_PC++; \
                           dreg = Z80ReadMem_notiming(r_op);

#define LD_r_n(reg) (reg) = Z80ReadMem(r_PC++);

#define LD_r_n_NC(reg) (reg) = Z80ReadMem_notiming(r_PC++);

#define LD_rr_nn(reg)   r_opl = Z80ReadMem(r_PC); r_PC++; \
                        r_oph = Z80ReadMem(r_PC); r_PC++; \
                        reg = r_op

#define LD_rr_nn_NC(reg)   r_opl = Z80ReadMem_notiming(r_PC); r_PC++; \
                        r_oph = Z80ReadMem_notiming(r_PC); r_PC++; \
                        reg = r_op

#define EX(reg1,reg2)        r_opl=(reg1); (reg1)=(reg2); (reg2)=r_opl

#define EX_WORD(reg1,reg2)   r_op=(reg1); (reg1)=(reg2); (reg2)=r_op

/*--- Increments/Decrements -----------------------------------------*/
#define INC(reg)            (reg)++;                        \
   r_F = ( r_F & FLAG_C ) | ( (reg)==0x80 ? FLAG_V : 0 ) |  \
  ( (reg)&0x0f ? 0 : FLAG_H ) | ( (reg) ? 0 : FLAG_Z ) |    \
   sz53_table[(reg)]

#define DEC(reg)                                                  \
   r_F = ( r_F & FLAG_C ) | ( (reg)&0x0f ? 0 : FLAG_H ) | FLAG_N; \
   (reg)--;                                                       \
   r_F |= ( (reg)==0x7f ? FLAG_V : 0 ) | sz53_table[(reg)]

// COMMENTS:
// it was:
//   r_F |= ( (reg)==0x79 ? FLAG_V : 0 ) | sz53_table[(reg)]
// But Kak pointed my was not 0x79 -> 0x7F, changed 7-3-2001


/*--- Bit operations ------------------------------------------------*/
#define BIT_RES(b,reg) reg &= ~(0x1<<b)

#define BIT_SET(b,reg) reg |= (0x1<<b)

#define BIT_mem_RES(b,addr) r_opl = Z80ReadMem(addr);\
                            r_opl &= ~(0x1<<b);contend_read(addr);       \
                            Z80WriteMem(addr, r_opl, regs)

#define BIT_mem_RES_NC(b,addr) r_opl = Z80ReadMem_notiming(addr);	\
                            r_opl &= ~(0x1<<b);			       	\
                            Z80WriteMem_notiming(addr, r_opl)

#define BIT_mem_SET(b,addr) r_opl = Z80ReadMem(addr); \
                            r_opl |= (0x1<<b);contend_read(addr);        \
                            Z80WriteMem(addr, r_opl, regs)

#define BIT_mem_SET_NC(b,addr) r_opl = Z80ReadMem_notiming(addr); 	\
                            r_opl |= (0x1<<b);		        \
                            Z80WriteMem_notiming(addr, r_opl)

#define BIT_RES_mem(b,addr,reg)  reg &= ~(0x1<<b);    \
                                 Z80WriteMem((addr), (reg), regs)
                                 
#define BIT_RES_mem_NC(b,addr,reg)  reg &= ~(0x1<<b);    \
                                 Z80WriteMem_notiming((addr), (reg))

#define BIT_SET_mem(b,addr,reg) reg |= (0x1<<b);        \
                                Z80WriteMem((addr), (reg), regs)

#define BIT_SET_mem_NC(b,addr,reg) reg |= (0x1<<b);        \
                                Z80WriteMem_notiming((addr), (reg))

#define BIT_BIT(b,reg)     r_F = ( r_F & FLAG_C ) | \
                           ( (reg) & ( FLAG_3 | FLAG_5 ) ) |\
                           (((reg) & ( 0x01 << b ) ) ? FLAG_H : \
                           (FLAG_P|FLAG_H|FLAG_Z ) )

#define BIT_BIT_XY(b,reg,reg2)  r_F = ( r_F & FLAG_C ) | \
                           ( (reg2) & ( FLAG_3 | FLAG_5 ) ) |\
                           (((reg) & ( 0x01 << b ) ) ? FLAG_H : \
                           (FLAG_P|FLAG_H|FLAG_Z ) )

#define BIT_mem_BIT(b,reg)  r_opl = Z80ReadMem(reg); \
                            r_F = ( r_F & FLAG_C ) | \
                            (((r_opl) & ( 0x01 << b ) ) ? FLAG_H : \
                            (FLAG_P|FLAG_H|FLAG_Z ) )

#define BIT_mem_BIT_NC(b,reg)  r_opl = Z80ReadMem_notiming(reg); \
                            r_F = ( r_F & FLAG_C ) | \
                            (((r_opl) & ( 0x01 << b ) ) ? FLAG_H : \
                            (FLAG_P|FLAG_H|FLAG_Z ) )

#define BIT_BIT7(reg)        r_F = ( r_F & FLAG_C ) | ( (reg) & \
                         ( FLAG_3 | FLAG_5 ) ) |\
                         (((reg) & 0x80 ) ? ( FLAG_H | FLAG_S ) :\
                         ( FLAG_P | FLAG_H | FLAG_Z ) )


#define BIT_BIT7_XY(reg,reg2) r_F = ( r_F & FLAG_C ) | ( (reg2) & \
                         ( FLAG_3 | FLAG_5 ) ) |\
                         (((reg) & 0x80 ) ? ( FLAG_H | FLAG_S ) :\
                         ( FLAG_P | FLAG_H | FLAG_Z ) )

#define BIT_mem_BIT7(reg)    r_opl = Z80ReadMem(reg); \
                         r_F = ( r_F & FLAG_C ) | \
                         (((r_opl) & 0x80 ) ? ( FLAG_H | FLAG_S ) :\
                         ( FLAG_P | FLAG_H | FLAG_Z ) )


#define BIT_mem_BIT7_NC(reg)    r_opl = Z80ReadMem_notiming(reg); \
                         r_F = ( r_F & FLAG_C ) | \
                         (((r_opl) & 0x80 ) ? ( FLAG_H | FLAG_S ) :\
                         ( FLAG_P | FLAG_H | FLAG_Z ) )

#define RLC(reg)  (reg) = ( (reg)<<1 ) | ( (reg)>>7 );          \
                  r_F = ( (reg) & FLAG_C ) | sz53p_table[(reg)]

#define RRC(reg)  r_F = (reg) & FLAG_C;               \
                  (reg) = ( (reg)>>1 ) | ( (reg)<<7 );\
                   r_F |= sz53p_table[(reg)]

#define RL(reg)   r_opl = (reg);                         \
                 (reg) = ( (reg)<<1 ) | ( r_F & FLAG_C );  \
                  r_F = ( r_opl >> 7 ) | sz53p_table[(reg)]

#define RR(reg)   r_opl = (reg);                          \
                  (reg) = ( (reg)>>1 ) | ( r_F << 7 );\
                  r_F = ( r_opl & FLAG_C ) | sz53p_table[(reg)]

#define SLA(reg)  r_F = (reg) >> 7;\
                  (reg) <<= 1;\
                   r_F |= sz53p_table[(reg)]

#define SRA(reg)  r_F = (reg) & FLAG_C; \
                 (reg) = ( (reg) & 0x80 ) | ( (reg) >> 1 );\
                  r_F |= sz53p_table[(reg)]

#define SLL(reg)  r_F = (reg) >> 7;\
                  (reg) = ( (reg) << 1 ) | 0x01;\
                   r_F |= sz53p_table[(reg)]

#define SRL(reg)  r_F = (reg) & FLAG_C;\
                  (reg) >>= 1;\
                  r_F |= sz53p_table[(reg)]



/*--- JP operations -------------------------------------------------*/
#define JP_nn()  r_opl = Z80ReadMem(r_PC); \
                 r_PC++;                   \
                 r_oph = Z80ReadMem(r_PC); \
                 r_PC = r_op

#define JP_nn_NC()  r_opl = Z80ReadMem_notiming(r_PC); \
                 r_PC++;                   \
                 r_oph = Z80ReadMem_notiming(r_PC); \
                 r_PC = r_op

#define JR_n()   r_PC += (offset) (Z80ReadMem(r_PC));contend_read_x5(r_PC); r_PC++;

#define JR_n_NC()   r_PC += (offset) (Z80ReadMem_notiming(r_PC));r_PC++;

#define RET_nn()   r_PCl = Z80ReadMem (r_SP);r_SP++; \
                   r_PCh = Z80ReadMem (r_SP);r_SP++;
                   
#define RET_nn_NC()   r_PCl = Z80ReadMem_notiming (r_SP);r_SP++; \
                   r_PCh = Z80ReadMem_notiming (r_SP);r_SP++;

#define CALL_nn()  r_opl = Z80ReadMem (r_PC); r_PC++; \
                   r_oph = Z80ReadMem (r_PC); contend_read(r_PC); r_PC++; \
                   Z80WriteMem( --(r_SP), r_PCh, regs ); \
                   Z80WriteMem( --(r_SP), r_PCl, regs ); \
                   r_PC = r_op

#define CALL_nn_NC()  r_opl = Z80ReadMem_notiming (r_PC); r_PC++; \
                   r_oph = Z80ReadMem_notiming (r_PC); r_PC++; \
                   Z80WriteMem_notiming( --(r_SP), r_PCh); \
                   Z80WriteMem_notiming( --(r_SP), r_PCl); \
                   r_PC = r_op


/*--- ALU operations ------------------------------------------------*/
#define AND(reg)     r_A &= (reg); \
                     r_F = FLAG_H | sz53p_table[r_A]

#define OR(reg)      r_A |= (reg); \
                     r_F = sz53p_table[r_A]

#define XOR(reg)     r_A ^= (reg); \
                     r_F = sz53p_table[r_A]

#define AND_mem(raddress)     r_opl = Z80ReadMem(raddress); \
                              r_A &= (r_opl);              \
                              r_F = FLAG_H | sz53p_table[r_A]

#define AND_mem_NC(raddress)     r_opl = Z80ReadMem_notiming(raddress); \
                              r_A &= (r_opl);              \
                              r_F = FLAG_H | sz53p_table[r_A]

#define OR_mem(raddress)      r_opl = Z80ReadMem(raddress); \
                              r_A |= (r_opl);               \
                              r_F = sz53p_table[r_A]

#define OR_mem_NC(raddress)      r_opl = Z80ReadMem_notiming(raddress); \
                              r_A |= (r_opl);               \
                              r_F = sz53p_table[r_A]

#define XOR_mem(raddress)     r_opl = Z80ReadMem(raddress); \
                              r_A ^= (r_opl);               \
                              r_F = sz53p_table[r_A]

#define XOR_mem_NC(raddress)     r_opl = Z80ReadMem_notiming(raddress); \
                              r_A ^= (r_opl);               \
                              r_F = sz53p_table[r_A]

#define ADD(val)   tempword = r_A + (val);                    \
                   r_oph = ((r_A&0x88)>>3)|(((val)&0x88)>>2) | \
                   ( (tempword & 0x88) >> 1 );                \
                   r_A = tempword;                            \
                   r_F = ( tempword & 0x100 ? FLAG_C : 0 ) |  \
                   halfcarry_add_table[ r_oph & 0x07] |        \
                   overflow_add_table[ r_oph >> 4] |           \
                   sz53_table[r_A]

#define ADD_WORD(value1,value2)                                   \
                   tempdword = (value1) + (value2);               \
                   r_oph = ( ( (value1) & 0x0800 ) >> 11 ) |     \
                   ( ( (value2) & 0x0800 ) >> 10 ) |              \
                   ( ( tempdword & 0x0800 ) >> 9 );               \
                   (value1) = tempdword;                            \
                   r_F = ( r_F & ( FLAG_V | FLAG_Z | FLAG_S ) ) | \
                   ( tempdword & 0x10000 ? FLAG_C : 0 ) |         \
                   (( tempdword >> 8 ) & ( FLAG_3 | FLAG_5 ) ) | \
                   halfcarry_add_table[r_oph]

#define ADC(value)                                                 \
                   tempword = r_A + (value) + ( r_F & FLAG_C );      \
            r_oph = ( (r_A & 0x88) >> 3 ) | ( ( (value) & 0x88 ) >> 2 ) |\
                   ( (tempword & 0x88) >> 1 );                       \
                    r_A = tempword;                                  \
                    r_F = ( tempword & 0x100 ? FLAG_C : 0 ) |        \
                   halfcarry_add_table[r_oph & 0x07] |            \
                   overflow_add_table[r_oph >> 4] |               \
                   sz53_table[r_A]

#define ADC_WORD(value)                                            \
              tempdword= r_HL + (value) + ( r_F & FLAG_C );            \
              r_oph = ( ( r_HL & 0x8800 ) >> 11 ) |               \
              ( ( (value) & 0x8800 ) >> 10 ) |                     \
              ( ( tempdword & 0x8800 ) >> 9 );                         \
              r_HL = tempdword;                                        \
              r_F = ( tempdword & 0x10000 ? FLAG_C : 0 )|              \
              overflow_add_table[r_oph >> 4] |                    \
              ( r_H & ( FLAG_3 | FLAG_5 | FLAG_S ) ) |             \
              halfcarry_add_table[ r_oph & 0x07 ]|                \
              ( r_HL ? 0 : FLAG_Z )

#define SUB(value)                                                 \
              tempword = r_A - (value);\
              r_opl = ( (r_A & 0x88) >> 3 ) |                     \
              ( ( (value) & 0x88 ) >> 2 ) |                        \
              ( (tempword & 0x88) >> 1 );                             \
              r_A = tempword;                                         \
              r_F = ( tempword & 0x100 ? FLAG_C : 0 ) | FLAG_N |      \
              halfcarry_sub_table[r_opl & 0x07] |                 \
              overflow_sub_table[r_opl >> 4] |                    \
              sz53_table[r_A]

#define SBC(value)                                                 \
              tempword = r_A - (value) - ( r_F & FLAG_C );            \
              r_opl = ( (r_A & 0x88) >> 3 ) |                     \
              ( ( (value) & 0x88 ) >> 2 ) |                        \
              ( (tempword & 0x88) >> 1 );                             \
              r_A = tempword;                                         \
              r_F = ( tempword & 0x100 ? FLAG_C : 0 ) | FLAG_N |      \
              halfcarry_sub_table[r_opl & 0x07] |                 \
              overflow_sub_table[r_opl >> 4] |                    \
              sz53_table[r_A]


#define SBC_WORD(Rg)      \
  tempword=r_F & C_FLAG; r_op=(r_HL-Rg-tempword)&0xFFFF;           \
  r_F=                                                   \
    N_FLAG|                                                    \
    (((long)r_HL-(long)Rg-(long)tempword)&0x10000? C_FLAG:0)| \
    ((r_HL^Rg)&(r_HL^r_op)&0x8000? O_FLAG:0)|        \
    ((r_HL^Rg^r_op)&0x1000? H_FLAG:0)|                  \
    (r_op? 0:Z_FLAG)|(r_oph&(S_FLAG|FLAG_3|FLAG_5));                            \
     r_HL=r_op


#define CP(value)                                                       \
  tempword = r_A - (value);\
  r_opl = ( (r_A & 0x88) >> 3 ) | ( ( (value) & 0x88 ) >> 2 ) |        \
       ( (tempword & 0x88) >> 1 );                                        \
  r_F = ( tempword & 0x100 ? FLAG_C : ( tempword ? 0 : FLAG_Z ) ) | FLAG_N |\
  halfcarry_sub_table[r_opl & 0x07] |                                  \
  overflow_sub_table[r_opl >> 4] |                                     \
  ( value & ( FLAG_3 | FLAG_5 ) ) |                                     \
  ( tempword & FLAG_S )

#define NEG_A()  r_opl = r_A; r_A=0; SUB(r_opl)

/*--- MISC operations -----------------------------------------------*/
#define IN_PORT(reg,port)    \
                 ula_contend_port_early(port); \
                 ula_contend_port_late(port); \
                 reg=Z80InPort(regs,port);  \
                 if( ( port & 0x8002 ) == 0 && ( model == ZX_128 || model == ZX_128_USR0 || model == ZX_PLUS2 ) ) \
                        {  \
                        Z80OutPort(regs,0x7ffd, reg ); \
                        } \
                 r_F = ( r_F & FLAG_C) | sz53p_table[(reg)];

#define IN_PORT_NC(reg,port)    \
                 reg=Z80InPort(regs,port);  \
                 if( ( port & 0x8002 ) == 0 && ( model == ZX_128 || model == ZX_128_USR0 || model == ZX_PLUS2 ) ) \
                        {  \
                        Z80OutPort(regs, 0x7ffd, reg ); \
                        } \
                 r_F = ( r_F & FLAG_C) | sz53p_table[(reg)];
