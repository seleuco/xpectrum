/*=====================================================================
  snaps.c    -> This file includes all the snapshot handling functions
        for the emulator, called from the main loop and when
        the user wants to load/save snapshots files.

    also routines to manage files, i.e. rom files, search in several sites and so on...

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

 Copyright (c) 2008 Seleuco  (tape code)
 Copyright (c) 2006-2008 Metalbrain  (snapshot fixes)
 Copyright (c) 2006 Philip Kendall  (loader detection code)
 Copyright (c) 2005 rlyeh
 Copyright (c) 2000 Santiago Romero Iglesias.
 ======================================================================*/

//rlyeh, based on ASpectrum & X128

#include "shared.h"

extern MCONFIG mconfig;

extern void msg(char *);
#define debugmsg(a,b) { char text[255]; gm_sprintf(text,"%s = %04x",a,b); msg(text); }



    //byte  *TAP_sof,*TAP_eof,*TAP_pof;
    //byte  *TZX_sof,*TZX_eof,*TZX_pof;


/*-----------------------------------------------------------------
 char LoadSP( Z80Regs *regs, void *fp );
 This loads a .SP file from disk to the Z80 registers/memory.
------------------------------------------------------------------*/
char
LoadSP (Z80Regs * regs, void *fp)
{
  unsigned short length, start, sword;
  int f;
  byte buffer[80], *source=(byte *)fp;

  ZX_Reset(ZX_48);

  memcpy(buffer, source, 38); source+=38;

  /* load the .SP header: */
  length = (buffer[3] << 8) + buffer[2];
  start = (buffer[5] << 8) + buffer[4];
  regs->BC.B.l = buffer[6];
  regs->BC.B.h = buffer[7];
  regs->DE.B.l = buffer[8];
  regs->DE.B.h = buffer[9];
  regs->HL.B.l = buffer[10];
  regs->HL.B.h = buffer[11];
  regs->AF.B.l = buffer[12];
  regs->AF.B.h = buffer[13];
  regs->IX.B.l = buffer[14];
  regs->IX.B.h = buffer[15];
  regs->IY.B.l = buffer[16];
  regs->IY.B.h = buffer[17];
  regs->BCs.B.l = buffer[18];
  regs->BCs.B.h = buffer[19];
  regs->DEs.B.l = buffer[20];
  regs->DEs.B.h = buffer[21];
  regs->HLs.B.l = buffer[22];
  regs->HLs.B.h = buffer[23];
  regs->AFs.B.l = buffer[24];
  regs->AFs.B.h = buffer[25];
  regs->R = buffer[26];
  regs->I = buffer[27];
  regs->SP.B.l = buffer[28];
  regs->SP.B.h = buffer[29];
  regs->PC.B.l = buffer[30];
  regs->PC.B.h = buffer[31];
  /*regs->*/BorderColor = buffer[34];
  sword = (buffer[37] << 8) | buffer[36];

/*
  ASprintf("\nSP_PC = %04X, SP_START =  %d,  SP_LENGTH = %d\n", regs->PC, start, length);
*/

  /* interrupt mode */
  regs->IFF1 = regs->IFF2 = 0;
  if (sword & 0x4)
    regs->IFF2 = 1;
  if (sword & 0x8)
    regs->IM = 0;
  else
    {
      if (sword & 0x2)
    regs->IM = 2;
      else
    regs->IM = 1;
    }
  if (sword & 0x1)
    regs->IFF1 = 1;

  /*
  if (sword & 0x16){
    ASprintf("\n\nPENDING INTERRUPT!!\n\n");
  } else {
    ASprintf("\n\nno pending interrupt.\n\n");
  }
  */

  for (f = 0; f <= length; f++)
    if (start + f < 65536)
      Z80WriteMem_notiming(start+f,*source++);

  return (1);
}





/*-----------------------------------------------------------------
 char LoadSNA( Z80Regs *regs, void *fp, int cmodel );
 This loads a .SNA 48/128 file from disk to the Z80 registers/memory.
------------------------------------------------------------------*/
char
LoadSNA (Z80Regs * regs, void * fp, int cmodel)
{
  byte buffer[27];
  byte *source=(byte *)fp;

  if(cmodel!=ZX_48)
   if(cmodel!=ZX_128)
    if(cmodel!=ZX_PLUS2)
    return (0);

  ZX_Reset(cmodel);

  /* load the .SNA header */
  memcpy(buffer, source, 27); source+=27;

  regs->I       = buffer[0];
  regs->HLs.B.l = buffer[1];
  regs->HLs.B.h = buffer[2];
  regs->DEs.B.l = buffer[3];
  regs->DEs.B.h = buffer[4];
  regs->BCs.B.l = buffer[5];
  regs->BCs.B.h = buffer[6];
  regs->AFs.B.l = buffer[7];
  regs->AFs.B.h = buffer[8];
  regs->HL.B.l  = buffer[9];
  regs->HL.B.h  = buffer[10];
  regs->DE.B.l  = buffer[11];
  regs->DE.B.h  = buffer[12];
  regs->BC.B.l  = buffer[13];
  regs->BC.B.h  = buffer[14];
  regs->IY.B.l  = buffer[15];
  regs->IY.B.h  = buffer[16];
  regs->IX.B.l  = buffer[17];
  regs->IX.B.h  = buffer[18];
//regs->IFF1    = regs->IFF2 = (buffer[19] & 0x04) >> 2;
  regs->IFF1    = (buffer[19] & 0x03) > 0 ? 1 : 0;
  regs->IFF2    = (buffer[19] & 0x04) > 0 ? 1 : 0;
  regs->R       = buffer[20];
  regs->AF.B.l  = buffer[21];
  regs->AF.B.h  = buffer[22];
  regs->SP.B.l  = buffer[23];
  regs->SP.B.h  = buffer[24];
  regs->IM      = buffer[25];
//regs->BorderColor = buffer[26];
  /*regs->*/BorderColor = buffer[26] & 7;

/*
  regs->PC.B.l = Z80ReadMem_notiming(regs->SP.W);
  regs->SP.W++;
  regs->PC.B.h = Z80ReadMem_notiming(regs->SP.W);
  regs->SP.W++;

  Z80WriteMem_notiming (regs->SP.W - 1 , 0); // Rui Ribeiro's fix as told at CSS FAQ
  Z80WriteMem_notiming (regs->SP.W - 2 , 0);
*/

  if(cmodel==ZX_48)
  {
   int x;
   for(x=0;x<0x4000*3;x++) Z80WriteMem_notiming(x+16384,*source++);
  }
  else
  {
   int x;

   port_0x7ffd(source[0x4000*3+2]); //set pagination_128

   for(x=0;x<0x4000;x++) RAM_pages[0x4000*5+x]=*source++;
   for(x=0;x<0x4000;x++) RAM_pages[0x4000*2+x]=*source++;
   for(x=0;x<0x4000;x++) RAM_pages[0x4000*(pagination_128&7)+x]=*source++;

   regs->PC.B.l = *source++;
   regs->PC.B.h = *source++;

   source++; //skip pagination_128

   source++; //trdos on/off

   if((pagination_128&7)!=0) for(x=0;x<0x4000;x++) RAM_pages[0x4000*0+x]=*source++;
   if((pagination_128&7)!=1) for(x=0;x<0x4000;x++) RAM_pages[0x4000*1+x]=*source++;
   if((pagination_128&7)!=3) for(x=0;x<0x4000;x++) RAM_pages[0x4000*3+x]=*source++;
   if((pagination_128&7)!=4) for(x=0;x<0x4000;x++) RAM_pages[0x4000*4+x]=*source++;
   if((pagination_128&7)!=6) for(x=0;x<0x4000;x++) RAM_pages[0x4000*6+x]=*source++;
   if((pagination_128&7)!=7) for(x=0;x<0x4000;x++) RAM_pages[0x4000*7+x]=*source++;
  }

  // Rui Ribeiro's fix as told at CSS FAQ :
  regs->PC.B.l = Z80ReadMem_notiming(regs->SP.W); Z80WriteMem_notiming (regs->SP.W, 0); regs->SP.W++;
  regs->PC.B.h = Z80ReadMem_notiming(regs->SP.W); Z80WriteMem_notiming (regs->SP.W, 0); regs->SP.W++;

  return (1);
}

/*
    SNA128

    Offset   Size   Description
    ------------------------------------------------------------------------
    0        27     bytes  SNA header (see above)
    27       16Kb   bytes  RAM bank 5 \
    16411    16Kb   bytes  RAM bank 2  } - as standard 48Kb SNA file
    32795    16Kb   bytes  RAM bank n / (currently paged bank)
    49179    2      word   PC
    49181    1      byte   port 0x7ffd setting
    49182    1      byte   TR-DOS rom paged (1) or not (0)
    49183    16Kb   bytes  remaining RAM banks in ascending order
      ...
    ------------------------------------------------------------------------
    Total: 131103 or 147487 bytes
*/


//
//char TAP_loadblock2(Z80Regs * regs)
//{
//  unsigned int  where, bytes, amount, skip, error;
//  byte crc = 0;
//
//  where  = regs->IX.W ;
//  amount = regs->DE.W ;
//  skip   = 0;
//  error  = 0;
//
//  // auto tape-rewind function on end-of-file
//  if( TAP_pof > TAP_eof )  TAP_pof = TAP_sof;
//
//  {
//  unsigned char blow, bhi;
//  memcpy((unsigned char *)&blow, TAP_pof, 1); TAP_pof++;
//  memcpy((unsigned char *)&bhi,  TAP_pof, 1); TAP_pof++;
//  bytes = (bhi << 8) | blow;  //size of header
//  }
//
//  switch(bytes)
//  {
//   case 1:   TAP_pof++;    //error
//   case 0:   return 0;     //error
//   default:  bytes-=2;     //ok
//  }
//
//  {
//  unsigned char id_tape;
//
//  memcpy((unsigned char *)&id_tape, TAP_pof, 1); TAP_pof++; /* read flag */
//
//  crc^=id_tape;
//
//  if(id_tape != (regs->AFs.B.h))     //if id not ok then
//   { TAP_pof+=bytes+2-1; return 0; } //skip block+checksum and return error
//
//  }
//
//  /* determine how many bytes to read. If there are less bytes in
//     the tap block than the requested DE bytes, generate the read
//     error by setting later on the Z/C_FLAG on F...
//
//     if there are more bytes in the tap than requested (ie, Gregory
//     Loses His Clock.tap has a header of 26 bytes instead of 13) then
//     read only the initial bytes and skip later the other bytes.
//
//  */
//
//
//  if (amount > bytes)
//  {
//       skip   = amount - bytes;
//       amount = bytes;
//       error  = 1;
//  }
//
//  regs->IX.W += amount;
//  regs->DE.W  = 0;
//
//
//  {
//   unsigned int f;
//   byte temp;
//
//    for (f = 0; f < amount+skip; f++)
//    {
//     crc^=(temp=*TAP_pof); TAP_pof++;
//     if(f < amount)
//     {
//      if (!(regs->AFs.B.l & C_FLAG))  //if verify (carry==0)...
//       error+=(Z80ReadMem_notiming(where + f) != temp);
//      else                            //else read
//       Z80WriteMem_notiming (where + f, temp);
//     }
//    }
//
//  TAP_pof += bytes - amount; /*skip unused bytes (if any!)*/
//
//  regs->HL.B.l=temp;    //util?
//
//  {
//   unsigned char checksum;
//
//   memcpy((unsigned char *)&checksum, TAP_pof, 1); TAP_pof++; /* read flag */
//
//   //if(!error)                      /* if all went ok, return last byte read */
//   {
//    regs->HL.B.h=checksum;
//
//    //util?
//    //if(crc!=checksum) error=1; /* but if cheksum error, return error */
//   }
//  }
//  }
//
//  return (error ? 0 : 1); //return error/ok
//}


//char LoadTAP (Z80Regs * regs)
//{
// word x;
//
///* x128 v0.93 */
//
//  /* fuse 0.7.0:
//   *
//   * On exit:
//   *  A = calculated parity byte if parity checked, else 0 (CHECKME)
//   *  F : if parity checked, all flags are modified
//   *      else carry only is modified (FIXME)
//   *  B = 0xB0 (success) or 0x00 (failure)
//   *  C = 0x01 (confirmed), 0x21, 0xFE or 0xDE (CHECKME)
//    * DE : decremented by number of bytes loaded or verified
//   *  H = calculated parity byte or undefined
//   *  L = last byte read, or 1 if none
//    * IX : incremented by number of bytes loaded or verified
//    * A' = unchanged on error + no flag byte, else 0x01
//    * F' = 0x01      on error + no flag byte, else 0x45
//   *  R = no point in altering it :-)
//    * Other registers unchanged.
//   */
//
//  regs->BC.W=0xCB01;
//  regs->HL.W=0;
//
////  debugmsg("AFs = ",regs->AFs.W);
//
//  if(TAP_loadblock2(regs)) //if ok, return inversed
//  {
////   regs->AF.B.l |= (C_FLAG);
////   regs->AF.B.l |= (Z_FLAG);
//   regs->AF.B.l = (regs->AF.B.l & (255-64) ) | 1;
//   regs->AF.B.h=0;
//
////   regs->AFs.B.h=0x1;     //util?
////   regs->AFs.B.l=0x45;    //util?
//
//   regs->PC.W=0x05e2;
//
//   return 1;
//  }
//  else
//  {
////   regs->AF.B.l &= ~(C_FLAG);
////   regs->AF.B.l &= ~(Z_FLAG);
//   regs->AF.B.l = (regs->AF.B.l | 64 );
//   regs->AF.B.h=0;
//
////   regs->AFs.B.l=0x1;     //util?
//
//   regs->PC.W=0x05e2;
//
//   return 0;
//  }
//
//
//
//#if 0
//  /* x128 v0.61 */
//  //trap at 1386,1387
//
//  if(!TAP_loadblock2(regs,fp)) //if ok, return inversed
//  {
//   regs->AFs.B.l &= (0xFF^(C_FLAG));
//   regs->AFs.B.l &= (0xFF^(Z_FLAG));
//   regs->PC.W=1506;
//
//   x=regs->AF.W; regs->AF.W=regs->AFs.W; regs->AFs.W=x;
//
//   return 1;
//  }
//  else
//  {
//   regs->AFs.B.l |= (Z_FLAG);
//   regs->PC.W=1506;
//
//   x=regs->AF.W; regs->AF.W=regs->AFs.W; regs->AFs.W=x;
//
//   return 0;
//  }
//
//
//  /* x128 v0.93 */
//  //trap at 0x56c...
//
//  regs->BC.W=51969;
//  regs->HL.W=0;
//
//  if(TAP_loadblock2(regs,fp)) //if ok, return inversed
//  {
//   regs->AF.B.l |= (C_FLAG);
//   regs->AF.B.l |= (Z_FLAG);
//   regs->AF.B.h=0;
//   regs->PC.W=1506;
//
//   return 1;
//  }
//  else
//  {
//   regs->AF.B.l &= ~(C_FLAG);
//   regs->AF.B.l &= ~(Z_FLAG);
//   regs->AF.B.h=0;
//   regs->PC.W=1506;
//
//   return 0;
//  }
//
//#endif
//
//
//
///*
//  LOAD x128 traps
//
//  type_fast:
//  [1386]=237 [1387]=255       //rom 0 (48), 0+1 (128), 3 (+3)
//  [1386]=237 [1387]=255
//
//  type_edge:
//  [1511]=64
//
//  SAVE x128 traps
//
//  [1240]=237 [1241]=254       //rom 0 (48), 0+1 (128), 3 (+3)
//*/
//}

/* Rebobina una Cinta .TAP */
//void TAP_rewind(void)
//{
//        TAP_pof=TAP_sof;
//}
//
//void TAP_init(void *fp, int siz)
//{
// TAP_pof=TAP_sof=(byte *)fp;
//
// TAP_eof=(byte *)fp+siz;
//}


/*-----------------------------------------------------------------
 char LoadZ80( Z80Regs *regs, FILE *fp );
 This loads a .Z80 file from disk to the Z80 registers/memory.

 void UncompressZ80 (int tipo, int pc, Z80Regs *regs, FILE *fp)
 This load and uncompres a Z80 block to pc adress memory.

 The Z80 Load Routine is (C) 2001 Alvaro Alea Fdz.
 e-mail: ALEAsoft@yahoo.com  Distributed under GPL2
------------------------------------------------------------------*/
#define Z80BL_V1UNCOMP 0
#define Z80BL_V1COMPRE 1
#define Z80BL_V2UNCOMP 3
#define Z80BL_V2COMPRE 4

void
UncompressZ80 (byte *dest, int tipo, int tam, Z80Regs * regs, void * fp)
{
  byte c, c2;
  int  limit, pc=0, count=0;
  byte *source=(byte *)fp;

  if (tipo == Z80BL_V1UNCOMP || tipo == Z80BL_V2UNCOMP)
  {
    limit = (tipo == Z80BL_V1UNCOMP ? 0xc000 : 0x4000 );

    memset(dest,0,limit);

    while (pc < limit) dest[ pc++ ] = *source++;

    return;
  }
  else
  if (tipo == Z80BL_V1COMPRE)
  {
   memset(dest,0,0xc000);
   limit = 0 + 0xc000;
  }
  else
  //if (tipo == Z80BL_V2COMPRE)
  {
   memset(dest,0,0x4000);
   limit = 0 + tam; //0x4000; //(tam>0x4000?0x4000:tam); //ultimo!
  }


    {
    while(1)
    {
        c=source[count++];

        if(c==237) /* is ED... */
        {
            c2=source[count++];

            if (c2==237) /* is ED ED code */
            {
                byte i, val; int x;

                i=source[count++];
                val=source[count++];

                for(x=0;x<i;x++)
                {
                 dest[pc++]=val;
                }
            }
            else
            {
                dest[pc++]=c; /* is ED ?? */
                dest[pc++]=c2;
            }
        }
        else
        {
            dest[pc++]=c;   /* not ED... */
        }

#if 0
        if(pc>16385 && (tipo != Z80BL_V1UNCOMP) )
        {
            /* The above is a scam to get 3D Stock Cars */
            /* snaps to work, the encoding routine on V2 */
            /* and V3 files seems to save 'page 4' in such */
            /* a way that it decompresses to a length of */
            /* 16385, it does this with snaps saved on both */
            /* Z80 and X128, so it could be a fault with */
            /* the encoding system, or a fault with James' McKay */
            /* decoding routine */

            return ; /* fail */
        }
#endif

      if (tipo == Z80BL_V1COMPRE)    if(pc>=limit) return;
      if (tipo == Z80BL_V2COMPRE) if(count>=limit) return;

    } //while (count<limit); //ultimo! pc<limit

    }
}


int
LoadZ80 (Z80Regs * regs, byte * fp, byte * fp_end)
{
    int f, tam, sig;
    byte buffer[86+10], pag, ver = 0;
    byte *source=(byte *)fp, *source_=(byte *)fp;

    ZX_Reset(ZX_48);

    memcpy(buffer, source, /*86*/87);//fix by MetalBrain

    if (buffer[12]==255) buffer[12]=1; /*as told in CSS FAQ / .z80 section */

    // Check file version
    if ((buffer[6] != 0)||(buffer[7] != 0))
    {
        // .z80 v1.45 or earlier
        source=source_; source+=30;
        UncompressZ80 (&RAM_pages[0x4000*5],(buffer[12] & 0x20 ? Z80BL_V1COMPRE : Z80BL_V1UNCOMP), 0, regs, (void *)source);

        regs->PC.B.l = buffer[6]; //set PC
        regs->PC.B.h = buffer[7];
    }
    else
    {
        switch((u16) buffer[30])
        {
            case 23:   ver = 2; break;
            case 54:
            case 55:   ver = 3; break;
            default:            break; //?
        }

        // version>=2 more hardware allowed
        if(ver==2)
            switch (buffer[34])       /* is it 48K? */
            {
                //OK!
                case 0: if(buffer[37]&0x80) ZX_Reset(ZX_16); else ZX_Reset(ZX_48);
                        break;

                //MISSING 48+IF1
                case 1: if(buffer[37]&0x80) ZX_Reset(ZX_16); else ZX_Reset(ZX_48);
                        break;

                //MISSING 48+SAMRAM
                case 2: if(buffer[37]&0x80) ZX_Reset(ZX_16); else ZX_Reset(ZX_48);
                        //leer 2 bloques de 16k
                        //saltar 16k*2 en el fichero y poenr mem a 0
                        //leer 1 bloques de 16k
                        break;

                //OK!
                case 3: if(buffer[37]&0x80) ZX_Reset(ZX_PLUS2); else ZX_Reset(ZX_128);
                        break;

                //MISSING 128+IF1:
                case 4: if(buffer[37]&0x80) ZX_Reset(ZX_PLUS2); else ZX_Reset(ZX_128);
                        break;
            }

        else
            switch(buffer[34])
            {
                //OK!
                case 0: if(buffer[37]&0x80) ZX_Reset(ZX_16); else ZX_Reset(ZX_48);
                        break;

                //MISSING 16+IF1 & 48+IF1:
                case 1: if(buffer[37]&0x80) ZX_Reset(ZX_16); else ZX_Reset(ZX_48);
                        break;

                //MISSING 16+SAMRAM & 48+SAMRAM:
                case 2: if(buffer[37]&0x80) ZX_Reset(ZX_16); else ZX_Reset(ZX_48);
                        //leer 2 bloques de 16k
                        //saltar 16k*2 en el fichero y poenr mem a 0
                        //leer 1 bloques de 16k
                        break;

                //MISSING 16+MGT & 48+MGT:
                case 3: if(buffer[37]&0x80) ZX_Reset(ZX_16); else ZX_Reset(ZX_48);
                        break;

                //OK!
                case 4: if(buffer[37]&0x80) ZX_Reset(ZX_PLUS2); else ZX_Reset(ZX_128);
                        break;

                //MISSING +2+IF1 & 128+IF1 :
                case 5: if(buffer[37]&0x80) ZX_Reset(ZX_PLUS2); else ZX_Reset(ZX_128);
                        break;

                //MISSING 128 + MGT:
                case 6: if(buffer[37]&0x80) ZX_Reset(ZX_PLUS2); else ZX_Reset(ZX_128);
                        break;
            }

        switch(buffer[34]) //common extended values to ver2 & ver3
        {
            //OK!
            case 7: if(buffer[37]&0x80) ZX_Reset(ZX_PLUS2A); else ZX_Reset(ZX_PLUS3);
                    break;

            //OK!
            case 8: if(buffer[37]&0x80) ZX_Reset(ZX_PLUS2A); else ZX_Reset(ZX_PLUS3);
                    break;

            case 9: //          ASprintf("Pentagon 128k\n");
                    return (0);

            case 10: //          ASprintf("Scorpion 256k\n");
                    return (0);

            case 11: //          ASprintf("Didaktik-Kompakt\n");
                    return (0);

            //OK!
            case 12: ZX_Reset(ZX_PLUS2);
                    break;

            //OK!
            case 13: ZX_Reset(ZX_PLUS2A);
                    break;

            case 14: //          ASprintf("TC2048\n");
                    return (0);

            case 15: //          ASprintf("TC2068\n");
                    return (0);

            case 16: //          ASprintf("TS2068\n");
                    return (0);

            default: //          ASprintf("Unknown hardware, %x\n", buffer[34]);
                    //return (0);
                    ;
       }

        if(model>=ZX_128)
        {
            int psg;

            for(psg=0;psg<16;psg++)
            {
                port_0xfffd (psg); port_0xbffd(buffer[39+psg]); }

                port_0xfffd(buffer[38]);

                if ((u16) buffer[30] == 55) port_0x1ffd(buffer[86]);

                port_0x7ffd(buffer[35]);
            }

            sig = 30 + 2 + buffer[30];

            for (f = 0; f < 16 ; f++) //up 16 pages (ZS Scorpion)
            {
                byte *target=NULL;

                source=source_; source+=sig;

                tam = *source++;
                tam = tam + ((*source++) << 8);
                pag = *source++;
                sig += 3 + tam;
                if (tam==65535) sig-=49151;

                if(model<ZX_128)
                switch(pag)
                {
                    case 1:     break; //Interface I/disciple/plus D rom
                    case 11:    break; //multiface rom

                    //case 0:  target=(byte *)&ROM_pages[0x4000*0]; break;
                    case 8:     target=(byte *)&RAM_pages[0x4000*5]; break;
                    case 4:     target=(byte *)&RAM_pages[0x4000*6]; break;
                    case 5:
                    default:    target=(byte *)&RAM_pages[0x4000*7]; break;
                }
                else
                    switch(pag)
                    {
                        case 1:     break; //Interface I/disciple/plus D rom
                        case 11:    break; //multiface rom

                        //case 0:  target=(byte *)&ROM_pages[0x4000*1]; break;
                        //case 2:  target=(byte *)&ROM_pages[0x4000*0]; break;
                        default:    if((pag>=3) && (pag<=18))
                                        target=(byte *)&RAM_pages[0x4000*(pag-3)]; break;
                    }

                if(target!=NULL)
                    UncompressZ80(target, (tam == 0xffff ? Z80BL_V2UNCOMP : Z80BL_V2COMPRE), tam, regs, source);

                if(source_+sig>=(byte *)fp_end) break;

            }
            regs->PC.B.l    = buffer[32];
            regs->PC.B.h    = buffer[33];
        }

        regs->AF.B.h    = buffer[0];
        regs->AF.B.l    = buffer[1];
        regs->BC.B.l    = buffer[2];
        regs->BC.B.h    = buffer[3];
        regs->HL.B.l    = buffer[4];
        regs->HL.B.h    = buffer[5];
        regs->SP.B.l    = buffer[8];
        regs->SP.B.h    = buffer[9];
        regs->I         = buffer[10];

        regs->R   	= buffer[11] & 0x7F;
        regs->R   	|= (buffer[12] & 1 ) << 7;

        /*regs->*/BorderColor = (buffer[12] >> 1) & 7;
        regs->DE.B.l    = buffer[13];
        regs->DE.B.h    = buffer[14];
        regs->BCs.B.l   = buffer[15];
        regs->BCs.B.h   = buffer[16];
        regs->DEs.B.l   = buffer[17];
        regs->DEs.B.h   = buffer[18];
        regs->HLs.B.l   = buffer[19];
        regs->HLs.B.h   = buffer[20];
        regs->AFs.B.h   = buffer[21];
        regs->AFs.B.l   = buffer[22];
        regs->IY.B.l    = buffer[23];
        regs->IY.B.h    = buffer[24];
        regs->IX.B.l    = buffer[25];
        regs->IX.B.h    = buffer[26];

        regs->IFF1      = buffer[27] > 0 ? 1 : 0;
        regs->IFF2      = buffer[28] > 0 ? 1 : 0;

        regs->IM        = buffer[29] & 0x03;

/*
        debugmsg("PC",regs->PC.W);
        debugmsg("SP",regs->SP.W);
        debugmsg("IX",regs->IX.W);
        debugmsg("IY",regs->IY.W);
        debugmsg("HL",regs->HL.W);
        debugmsg("DE",regs->DE.W);
        debugmsg("BC",regs->BC.W);
        debugmsg("AF",regs->AF.W);
        debugmsg("HLs",regs->HLs.W);
        debugmsg("DEs",regs->DEs.W);
        debugmsg("BCs",regs->BCs.W);
        debugmsg("AFs",regs->AFs.W);
        debugmsg("I",regs->I);
        debugmsg("R",regs->R);
        debugmsg("IFF1",regs->IFF1);
        debugmsg("IFF2",regs->IFF2);
        debugmsg("IM",regs->IM);
        debugmsg("7ffd",pagination_128);
*/
        //issue2
        //double int freq
        //highvideo,lowvideo,normal
        //joystick type
/*
        if(flag&4)
        {
            keyboard_issue=255;
        }
        else
        {
            keyboard_issue=191;
        }
        joy_type=(flag&0xC0)>>6;

#ifndef NON_WINDOWS
        printf("Joystick : ");
        switch(joy_type)
        {
            case 0 : printf("Cursor\n");break;
            case 1 : printf("Kempston\n");break;
            case 2 : printf("Sinclair 1\n");break;
            case 3 : printf("Sinclair 2\n");break;
        }
*/

#if 0
        #define SPEC_16
        #define SPEC_16IF1 //EXISTE?
        #define SPEC_16MGT //EXISTE?
        #define SPEC_48
        #define SPEC_48IF1
        #define SPEC_SAMRAM
        #define SPEC_48MGT
        #define SPEC_128
        #define SPEC_128IF1
        #define SPEC_128MGT
        #define SPEC_PLUS2IF1 //EXISTE?
        #define SPEC_PLUS2MGT //EXISTE?
        #define SPEC_PLUS3
        #define SPEC_PLUS2
        #define SPEC_PLUS2A
        #define SPEC_PENTAGON
        #define SPEC_SCORPION
        #define SPEC_DIDAKTIK
        #define SPEC_TC2048
        #define SPEC_TC2068
        #define SPEC_TS2068

        int version[4][16]=  {
        /*v2,bit7=0*/{ SPEC_48, SPEC_48IF1, SPEC_SAMRAM, SPEC_128, SPEC_128IF1, SPEC_128 (?), SPEC_128 (?), SPEC_PLUS3, SPEC_PLUS3, SPEC_PENTAGON, SPEC_SCORPION, SPEC_DIDAKTIK, SPEC_PLUS2, SPEC_PLUS2A, SPEC_TC2048, SPEC_TC2068 },
        /*v3,bit7=0*/{ SPEC_48, SPEC_48IF1, SPEC_SAMRAM, SPEC_48MGT, SPEC_128, SPEC_128IF1, SPEC128_MGT, SPEC_PLUS3, SPEC_PLUS3, SPEC_PENTAGON, SPEC_SCORPION, SPEC_DIDAKTIK, SPEC_PLUS2, SPEC_PLUS2A, SPEC_TC2048, SPEC_TC2068 },
        /*v2,bit7=1*/{ SPEC_16, SPEC_16IF1, SPEC_SAMRAM, SPEC_PLUS2, SPEC_PLUS2IF1, SPEC_PLUS2 (?), SPEC_PLUS2 (?), SPEC_PLUS2A, SPEC_PLUS2A, SPEC_PENTAGON, SPEC_SCORPION, SPEC_DIDAKTIK, SPEC_PLUS2, SPEC_PLUS2A, SPEC_TC2048, SPEC_TC2068 },
        /*v3,bit7=1*/{ SPEC_16, SPEC_16IF1, SPEC_SAMRAM, SPEC_16MGT, SPEC_PLUS2, SPEC_PLUS2IF1, SPECPLUS2_MGT, SPEC_PLUS2A, SPEC_PLUS2A, SPEC_PENTAGON, SPEC_SCORPION, SPEC_DIDAKTIK, SPEC_PLUS2, SPEC_PLUS2A, SPEC_TC2048, SPEC_TC2068 }
        }

        model=(val!=128 ? version[(ver-2)+((buffer[37]>>7)&1)*2][val&0x0F] : SPEC_TS2068);
#endif

    return (1);
}


/*

Each leader halfpulse takes 2,168 T
The first sync halfpulse takes 667 T
The second sync halfpulse takes 735 T
A '0' bit takes two 855 T halfpulses
A '1' bit takes two 1,710 T halfpulses (twice the length of a '0' bit)

1) leader tone, 808 hz
2) synchro impulse, 2500 hz

For saving data to a tape, the Spectrum uses a very simple method called
frequency modulation. Each byte is saved to tape bit by bit, where the most
significant bit comes first, and the least significant one comes last. Bit "0"
is coded with an impulse of the frequency of 2044 Hz, and bit "1" is coded
with an impulse of the frequency of 1022 Hz. It results in 1500 bits per
second (1500 baud) in the case of equal numbers of "ones" and "zeros".

In order to ease the search for the block start, it is necessary to have on
the beginning of each one a leader tone (808 Hz) that lasts for 3 or 5 seconds.
It is followed by a short impulse (about 2500 Hz) called the synchro impulse,
that specifies the start of the data block. The synchro impulse has the
positive half-period that is a bit longer than the negative one, which results
in a slight elevation of the signal on the very beginning of the block. That's
probably made to emphasize the impulse.

The very first byte written after the synchro impulse is so called flag byte
and specifies the block type. Usually 0 stands for header, and 255 specifies
data block.

*/
//
//
//byte  RAW_stage; //0=leader,1=transition,2=sync,3=transition,4=data,5=pause
//byte *RAW_pof, *RAW_sof, *RAW_eof;
//int   RAW_tstate, RAW_tstatesmax, RAW_tstatetarget;
//int   RAW_repeat, RAW_pulse;
//unsigned short RAW_bit;
//
///*
//tstates
//
//standard rom loader:
//
//P=half wave
//DP=full wave
//PILOT 8063x[P 2168] (header) or 3223x[P 2168] (data)
//SYNC standard P 667, P 735
//DATA
//bit 0 = DP 855
//bit 1 = DP 1710
//FLAG BYTE #00 (header), #FF (data)
//PARITY BYTE present
//
//*/
//
//extern byte mic;
//int  RAW_tstateprev=0,RAW_tstatecurr=0;
//int  RAW_length;
//
//byte LoadMIC(int tstates)
//{
// int tstate;
//
// //loop:;
//
// switch(RAW_stage)
// {
//  case 0: //PILOT 8063,3223
//
//          //length in bits
//          RAW_length=(*RAW_pof++);
//          RAW_length+=(*RAW_pof++)<<8;
//          RAW_length*=8;
//
//          RAW_repeat=((*RAW_pof) ? (3220/2) : (8064/2) ) / 1;  //data or header block? 2 or 5 seconds?
//          RAW_pulse=2168;
//
//          RAW_tstateprev=tstates;
//          RAW_tstatecurr=tstates;
//          RAW_tstatetarget=(tstates+RAW_pulse);
//
//          RAW_stage++;
//
//          RAW_bit = 0x80; //for DATA section
//          break;
//
//  case 2: //SYNC 1
//          RAW_repeat=1;
//          RAW_pulse=667;
//          RAW_stage++;
//          break;
//
//  case 4: //SYNC 2
//          RAW_repeat=1;
//          RAW_pulse=735;
//          RAW_stage++;
//          break;
//
//  case 6: //DATA
//
//          //if(queda octeto)
//          //  envia bit
//          //else
//          // if(queda bloque)
//          //  loop again
//          // else if(!quedan bloques)
//          //       exit
//          //      else
//          //       RAW_stage++; para pausa y siguiente bloque
//
//          RAW_repeat=2;
//          RAW_pulse=((*RAW_pof) & (RAW_bit) ? 1710 : 855);
//          RAW_stage++;
//
//          break;
//
//  case  8:
//          RAW_bit>>=1;
//
//          // init bit counter
//          if(RAW_bit==0x00)
//          {
//           RAW_bit = 0x80;
//
//           //advance tape
//           RAW_pof++;
//
//           // auto rewind if needed
//           if(RAW_pof > RAW_eof) RAW_pof=RAW_sof;
//          }
//
//          // if data loop again, else finish loop and set next pause
//          if(RAW_length--)
//          {
//           RAW_stage =  6;
//          }
//          else
//          {
//           RAW_repeat=1611; //1; //pause for 1 second (1611*2168t /69888tpf /50fps = 1s)
//           RAW_pulse=2168;  //3500;
//           RAW_stage++;
//          }
//          break;
//
//  case 10: RAW_stage = 0;
//           break;
//
// default: tstate=tstates-RAW_tstateprev;
//
//          if(tstate<0) tstate+=RAW_tstatesmax;
//
//          RAW_tstateprev=tstates;
//
//          RAW_tstatecurr+=tstate;
//
//          if(RAW_tstatecurr>=RAW_tstatetarget)
//          {
//           RAW_tstatetarget=RAW_tstatecurr+RAW_pulse;
//
//           mic^=64;
//
//           if(!(--RAW_repeat)) { RAW_stage++; /*  goto loop; */ }
//          }
//
//          break;
// }
//
//  return (RAW_stage==9 ? (mic = 0) : (mic) ); //if paused, return nothing
//}
//
//
// int LoadRAW(void * fp, int siz, int tstatesmax)
//{
// RAW_stage=0;
//
// RAW_sof=(byte *)fp;
// RAW_eof=RAW_sof+siz;
// RAW_pof=RAW_sof;
//
//// RAW_tstate=0;
//// RAW_bit=0x80;
// RAW_tstatesmax=tstatesmax;
//
//}


/*-----------------------------------------------------------------
 int SaveSCR( void *fp );
 This saves a .SCR file from the Spectrum videomemory into fp.
------------------------------------------------------------------*/
int
SaveSCR (void * fp)
{
  int i=6912;
  byte *dst=(byte *)fp;
  byte *VRAM=(byte *)&RAM_pages[(/*pagination_128 & 8 ? 0x4000*7 : */0x4000*5)];

  /* Save the contents of VideoRAM to file: write the 6912 bytes
   * starting on the memory address 16384 */

  while(i--) *dst++=*VRAM++;

  return (1);
}

/*-----------------------------------------------------------------
 void LoadSCR( void *fp );
 This loads a .SCR file from fp to the Spectrum videomemory.
 Patches PC & rom
------------------------------------------------------------------*/
void
LoadSCR (Z80Regs * regs, void * fp)
{
  int i=6912;
  byte *src=(byte *)fp;
  byte *VRAM;

  ZX_Reset(ZX_48);

  VRAM=(byte *)&RAM_pages[(/*pagination_128 & 8 ? 0x4000*7 : */0x4000*5)];

  regs->PC.W = 0;

  ROM_pages[0]=0xF3; //di
  ROM_pages[1]=0x00; //nop
  ROM_pages[2]=0x00; //nop
  ROM_pages[3]=0x76; //halt
  ROM_pages[4]=0x00; //nop
  ROM_pages[5]=0x00; //nop
  ROM_pages[6]=0x00; //nop

  while(i--) *VRAM++=*src++;
}


/*-----------------------------------------------------------------
 void LoadCart(Z80Regs * regs, void *fp);
 This loads a multiface II cart from fp to the Spectrum base ROM.
------------------------------------------------------------------*/
void
LoadCart (Z80Regs * regs, void * fp)
{
  int i;
  byte *src=(byte *)fp;

  ZX_Reset(ZX_48);

  regs->PC.W = 0;

  for(i=0;i<16384;i++) ROM_pages[i]=*src++;

  //trap_rom_loading=0;
}


/* Definiciones de variables que afectan a la carga de TZX */
/*
    int TZX_valid;
    int TZX_numofblocks;
    int TZX_actualblock;
    struct TZX_block_struct {
        int id;
        int sup;
        long offset;
    } ;
    struct TZX_block_struct TZX_index[65536];
*/

/* Inicializa una cinta .TZX */

//int LoadTZX(Z80Regs * regs)
//{
//
//   word x;
///* x128 v0.93 */
//
//  /* fuse 0.7.0:
//   *
//   * On exit:
//   *  A = calculated parity byte if parity checked, else 0 (CHECKME)
//   *  F : if parity checked, all flags are modified
//   *      else carry only is modified (FIXME)
//   *  B = 0xB0 (success) or 0x00 (failure)
//   *  C = 0x01 (confirmed), 0x21, 0xFE or 0xDE (CHECKME)
//    * DE : decremented by number of bytes loaded or verified
//   *  H = calculated parity byte or undefined
//   *  L = last byte read, or 1 if none
//    * IX : incremented by number of bytes loaded or verified
//    * A' = unchanged on error + no flag byte, else 0x01
//    * F' = 0x01      on error + no flag byte, else 0x45
//    *  R = no point in altering it :-)
//   * Other registers unchanged.
//   */
//
//  regs->BC.W=0xCB01;
//  regs->HL.W=0;
//
//  if(TZX_play_from_trap(regs)) //if ok, return inversed
//  {
//   regs->AF.B.l |= (C_FLAG);
//   regs->AF.B.l |= (Z_FLAG);
//   regs->AF.B.h=0;
//
//   //regs->BC.B.h=0xB0;
//
//   regs->AFs.B.l = 0x45;
//   regs->AFs.B.h = 0x01;
//
//   regs->PC.W=0x05e2;
//
//   return 1;
//  }
//  else
//  {
//   regs->AF.B.l &= ~(C_FLAG);
//   regs->AF.B.l &= ~(Z_FLAG);
//   regs->AF.B.h=0; //1?
//
//   //regs->BC.B.h=0x00;
//
//   regs->AFs.B.l = 0x01;
//   regs->AFs.B.h = regs->AFs.B.h;
//
//   regs->PC.W=0x05e2;
//   return 0;
//  }
//}


/* Rebobina una Cinta .TZX */
//int TZX_rewind()
//{
//      TZX_actualblock=0;
//      return 0;
//}

///* genera indices de bloques del archivo .TZX */
//int TZX_genindex(void *fp, int siz)
//{
///* ¿por que genero un indece de los TZX */
//
///* para empezar por culpa de los bloques de salto, call y loop
//   me ahorran el releer todo el tzx cada vez que encuentro uno
//   ademas me permiten comprobar de una tacada que estoy leyendo
//   bien la estructura de los tzx y que no me equivoco */
//
///* informacion de los bloques
//
//Los bloques no validos son ignorados directamente, los validos, son tratados
//por load_block (incluso aunque no tengan efecto).
//
//valido id(HEX) descripcion tamaño sin contar el ID
//*     SI 10 standar tap block (2,3)+4
//*     NO 11 turbo tap block  (f,10,11)+12
//*     NO 12 tono 4
//*     NO 13 pulsos 0*2+1
//*     NO 14 pure data (7,8,9)+10
//*     no 15 voc data (5,6,7)+8
//*     no 16 c64 data standar (0,1,2,3)
//*     no 17 c64 data turbo (0,1,2,3)
//*     no 20 pause/stop tape 2
//*     si 21 group start (0)
//*     si 22 group end 0
//*     si 23 jump 2
//*     si 24 loop 2
//*     si 25 loop end 0
//*     si 26 call (0,1)*2+2
//*     si 27 return 0
//*     si 28 selection block (0,1)+2  *** no comprendo para que es esto ***
//*     no 2a stop if in 48k 4
//*     si 30 texto (0)+1
//*     si 31 mensaje (1)+2
//*     si 32 informacion (0,1)+2
//*     si 33 hardware type (0)*3+1
//*     no 34 emulator info 8
//*     si 35 custon block (10,11,12,13)+14
//*     si 40 snapshot (1,2,4)+4
//*     no 5a header 9
//*     no ?? unsoported (0,1,2,3)
//*/
//      int size,offset,c,valido=0,funciona=1;
//
//        byte *src  =(byte *)fp;
//        byte *src_e=(byte *)fp+siz;
//
//      TZX_numofblocks=0;
//
//        TZX_pof=TZX_sof=(byte *)src;   //used in tzx_loadblock
//        TZX_eof=(byte *)src+siz;
//
//      //nos vamos al principio
//        //fseek(fp,0,SEEK_SET);
//      offset=0;
//
//      // leemos el indice y tratamos el bloque hasta que llegemos al final.
//        while (src<src_e) {
//                c=*src++;
//
//              TZX_index[TZX_numofblocks].id=c;
//              TZX_index[TZX_numofblocks].offset=offset;
//              switch (c) {
//                      case 0x10: //OK
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                src+=2;
//                                size = (*src++);
//                                size = size + (*src++)*0x100;
//                                src+=size;
//                              size += 4;
//                                //ASprintf("  Bloque 0x10, offset: %i, size %i, Standar Data \n",offset,size);
//                              offset+=size;
//                              valido=1;
//                              break;
//
//                      case 0x11: // REV
//                              TZX_index[TZX_numofblocks].sup=FALSE;
//                                src+=0x0F;
//                                size = (*src++);
//                                size = size + (*src++)*0x100;
//                                size = size + (*src++)*0x10000;
//                                src+=size;
//                              size += 18;
//                                //ASprintf("  Bloque 0x11, offset: %i, size %i, Turbo Data\n",offset,size);
//                              offset+=size;
//                              funciona=0;
//                              break;
//
//                      case 0x12: // OK
//                              TZX_index[TZX_numofblocks].sup=FALSE;
//                                src+=4;
//                                //ASprintf("  Bloque 0x12, offset: %i, size 4, Tono Puro\n",offset);
//                              offset+=4;
//                              funciona=0;
//                              break;
//
//                      case 0x13: // OK
//                              TZX_index[TZX_numofblocks].sup=FALSE;
//                                size = ((*src++)*2);
//                                src+=size;
//                              size+=1;
//                                //ASprintf("  Bloque 0x13, offset: %i, size %i, Pulsos\n",offset,size);
//                              offset+=size;
//                              funciona=0;
//                              break;
//
//                      case 0x14: // OK
//                              TZX_index[TZX_numofblocks].sup=FALSE;
//                                src+=0x07;
//                                size = (*src++);
//                                size = size + (*src++)*0x100;
//                                size = size + (*src++)*0x10000;
//                                src+=size;
//                              size+=10;
//                                //ASprintf("  Bloque 0x14, offset: %i, size %i, RAW square data\n",offset,size);
//                              offset+=size;
//                              funciona=0;
//                              break;
//
//                      case 0x15: // REV
//                              TZX_index[TZX_numofblocks].sup=FALSE;
//                                src+=0x05;
//                                size = (*src++);
//                                size = size + (*src++)*0x100;
//                                size = size + (*src++)*0x10000;
//                                src+=size;
//                              size+=8;
//                                //ASprintf("  Bloque 0x15, offset: %i, size %i, .VOC data\n",offset,size);
//                              offset+=size;
//                              funciona=0;
//                              break;
//
//                      case 0x20: // OK
//                              TZX_index[TZX_numofblocks].sup=FALSE;
//                                src+=2;
//                                //ASprintf("  Bloque 0x20, offset: %i, size 2, Pause/stop tape\n",offset);
//                              offset+=2;
//                              break;
//
//                      case 0x21: // OK
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                size=(*src++);
//                                src+=size;
//                              size+=1;
//                                //ASprintf("  Bloque 0x21, offset: %i, size %i, Group start\n",offset,size);
//                              offset+=size;
//                              break;
//
//                      case 0x22: // OK
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                //ASprintf("  Bloque 0x22, offset: %i, size 0, Group end\n",offset);
//                              offset+=0;
//                              break;
//
//                      case 0x23: // REV
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                src+=2;
//                                //ASprintf("  Bloque 0x23, offset: %i, size 2, Jump block\n",offset);
//                              offset+=2;
//                              break;
//
//                      case 0x24: // OK
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                src+=2;
//                                //ASprintf("  Bloque 0x24, offset: %i, size 2, loop block\n",offset);
//                              offset+=2;
//                              break;
//
//                      case 0x25: // ok
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                //ASprintf("  Bloque 0x25, offset: %i, size 0, loop end\n",offset);
//                              offset+=0;
//                              break;
//
//                      case 0x26:  // REV
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                size = (*src++);
//                                size = (size+(*src++)*0x100)*2  ;
//                                src+=size;
//                              size+=2;
//                                //ASprintf("  Bloque 0x26, offset: %i, size %i, call block\n",offset,size);
//                              offset+=size;
//                              break;
//
//
//                      case 0x27: // REV
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                //ASprintf("  Bloque 0x27, offset: %i, size 0, Return\n",offset);
//                              offset+=0;
//                              break;
//
//                      case 0x28: // REV
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                size = (*src++);
//                                size = size + (*src++)*0x100;
//                                src+=size;
//                              size+=2;
//                                //ASprintf("  Bloque 0x28, offset: %i, size %i, selection block\n",offset,size);
//                              offset+=size;
//                              break;
//
//                      case 0x2A: // REV
//                              TZX_index[TZX_numofblocks].sup=FALSE;
//                                src+=4;
//                                //ASprintf("  Bloque 0x2A, offset: %i, size 4, Stop Tape if in 48K\n",offset);
//                              offset+=4;
//                              break;
//
//                      case 0x2B: // MIO
//                              TZX_index[TZX_numofblocks].sup=FALSE;
//                                src+=5;
//                                //ASprintf("  Bloque 0x2B, offset: %i, size 4, Stop Tape if in 48K\n",offset);
//                              offset+=5;
//                              break;
//
//
//                      case 0x30: // OK
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                size = (*src++);
//                                src+=size;
//                              size+=1;
//                                //ASprintf("  Bloque 0x%02X, offset: %i, size %i, texto\n",c,offset,size);
//                              offset+=size;
//                              break;
//
//                      case 0x31: // REV
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                src+=1;
//                                size = (*src++);
//                                src+=size;
//                              size+=2;
//                                //ASprintf("  Bloque 0x%02X, offset: %i, size %i, mensaje\n",c,offset,size);
//                              offset+=size;
//                              break;
//
//                      case 0x32: // OK
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                size = (*src++);
//                                size = size + (*src++)*0x100;
//                                src+=size;
//                              size+=2;
//                                //ASprintf("  Bloque 0x%02X, offset: %i, size %i, informacion\n",c,offset,size);
//                              offset+=size;
//                              break;
//
//                      case 0x33: // REV
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                size = (*src++)*3;
//                                src+=size;
//                              size+=1;
//                                //ASprintf("  Bloque 0x%02X, offset: %i, size %i, hardware type\n",c,offset,size);
//                              offset+=size;
//                              break;
//
///*
//                      case 0x34: // REV
//                              TZX_index[TZX_numofblocks].sup=FALSE;
//                                src+=8;
//                                //ASprintf("  Bloque 0x34, offset: %i, size 8, Emul. Info.\n",offset);
//                              offset+=8;
//                              break;
//*/
//                      case 0x35: // REV
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                src+=0x0A;
//                                size = (*src++);
//                                size = size + (*src++)*0x100;
//                                size = size + (*src++)*0x10000;
//                                size = size + (*src++)*0x1000000;
//                                src+=size;
//                              size+=14;
//                                //ASprintf("  Bloque 0x%02X, offset: %i, size %i, custom block\n",c,offset,size);
//                                offset+=size;
//                              break;
///*
//                      case 0x40: // REV
//                              TZX_index[TZX_numofblocks].sup=TRUE;
//                                src+=0x01;
//                                size = (*src++);
//                                size = size + (*src++)*0x100;
//                                size = size + (*src++)*0x10000;
//                                src+=size;
//                              size+=4;
//                                //ASprintf("  Bloque 0x%02X, offset: %i, size %i, snapshot\n",c,offset,size);
//                              offset+=size;
//                              break;
//*/
//                      case 0x5a: // OK
//                              TZX_index[TZX_numofblocks].sup=FALSE;
//                                src+=0x09;
//                                //ASprintf("  Bloque 0x%02X, offset: %i, size 9, Cabecera\n",c,offset);
//                              offset+=9;
//                              break;
//
//                      case 0x16:  // REV
//                      case 0x17:
//                      default:
//                              TZX_index[TZX_numofblocks].sup=FALSE;
//                                size = (*src++);
//                                size = size + (*src++)*0x100;
//                                size = size + (*src++)*0x10000;
//                                size = size + (*src++)*0x1000000;
//                                src+=size;
//                              size+=4;
//                                //ASprintf("  Bloque 0x%02X, offset: %i, size %i, Desconocido o no soportado\n",c,offset,size);
//                              offset+=size;
//                              break;
//
//                      }
//              TZX_numofblocks++;
//              offset++;
//                };
//
//        //ASprintf("indice completo %i bloques,en offset: %i, %s es valido, %s.\n",TZX_numofblocks,offset,(valido==1?"":"no"),(funciona==1?"funcionara":"no funcionara"));
//      return valido;
//}

int tstates_prev_A = 0;
int bit=0;
int fast_edge_loading=0;

//int last_tstates_read = -100000;

//permitimos detectar 2 edge loaders, el rom, el turbo, o dos turbo
unsigned int _IN_A_hook[2][2];//={{0x05f1-2,0x0000};
short new_IN_A_pos=1;


void inline
loader (register Z80Regs *spectrumZ80)
{
    /*
    if(spectrumZ80->ICount <= 0)
    {
      if( last_tstates_read > -100000 ) {
       last_tstates_read -= spectrumZ80->IPeriod;
       }
    }
    */
  /*
    if( spectrumZ80->PC.W==0x056c)
    {
    if(Tape_load(spectrumZ80)!=0)
    {
    Z80Regs *regs=spectrumZ80;
    RET_nn_NC();
    }
    }  */

//lo saco fuera, ya que no con inline ni nada. ganamos rendimiento
/*
    if(!tape_playing)
      return;
*/
    /*
    if(Z80ReadMem_notiming(spectrumZ80->PC.W)==EI)
    {
       tape_stop();
       return;
    }
    */
    /*
    if(spectrumZ80->IFF1==1 && spectrumZ80->IFF2==1)
    {
      tape_stop();
      return;
    }
    */
    int tstates=spectrumZ80->IPeriod-spectrumZ80->ICount;//pasados!
    int tstates_elapsed = tstates - tstates_prev_A;
    if (tstates_elapsed<0) tstates_elapsed+=spectrumZ80->IPeriod;

    tstates_prev_A=tstates;
    tape_edge_tstates_current+=tstates_elapsed;

    if(fast_edge_loading==1)
    {
    if(spectrumZ80->PC.W==_IN_A_hook[0][0] ||spectrumZ80->PC.W==_IN_A_hook[1][0])
    {
       fast_edge_loading = 2;//consumimos el _IN del edge normal y hemos detectado un hook
    }
    }
    else if(fast_edge_loading==2)
    {
    if(spectrumZ80->PC.W==_IN_A_hook[0][0] ||spectrumZ80->PC.W==_IN_A_hook[1][0])
    {
       spectrumZ80->BC.B.h = (bit ?  0xfA : 0x05); //forces edge trigger zero or one
       int edge_tstates=0;
       tape_next_edge(spectrumZ80,&edge_tstates,&bit);
       if(bit==-1 || !mconfig.edge_loading)
          fast_edge_loading = 0;
       tape_edge_tstates_target = /*edge_tstates_current+*/edge_tstates;
       tape_edge_tstates_current=0;
    }
    }

    if(tape_edge_tstates_current>=tape_edge_tstates_target)
    {
        int edge_tstates=0;
        tape_next_edge(spectrumZ80,&edge_tstates,&bit);
        tape_edge_tstates_target = /*edge_tstates_target*//*edge_tstates_current+*/edge_tstates;//arregla las cintas largas
        tape_edge_tstates_current=0;

        fast_edge_loading = bit!=-1 && mconfig.edge_loading;
    }
}

int tstates_prev_B = 0;
int successive_reads = 0;
byte last_b_read = 0;

void loader_hook (register Z80Regs *spectrumZ80)
{
    if(!tape_is_tape())
      return;

    unsigned int _IN_pos = spectrumZ80->PC.W-1;

    if(mconfig.auto_loading)//detector de autoarranque de cinta
    {
    byte b_diff = spectrumZ80->BC.B.h - last_b_read;


    int tstates=spectrumZ80->IPeriod-spectrumZ80->ICount;//pasados!
        int tstates_elapsed = tstates - tstates_prev_B;
        if (tstates_elapsed<0) tstates_elapsed+=spectrumZ80->IPeriod;
        tstates_prev_B = tstates;

        /*
        int tstates=spectrumZ80->IPeriod-spectrumZ80->ICount;//pasados!
        int tstates_elapsed = tstates - last_tstates_read;
        last_tstates_read = tstates;
        */

        last_b_read = spectrumZ80->BC.B.h;

    ///FROM FUSE
    if(!tape_playing)
    {

        if( tstates_elapsed <= 500 && ( b_diff == 1 || b_diff == 0xff ) )
        {
               successive_reads++;
               if( successive_reads >= 11 ) {
                tape_do_play( 1 );
                successive_reads = 0;
               }
        }
        else
        {
               successive_reads = 0;
        }
    }
    else
    {
        if( (tstates_elapsed > 1000 || ( b_diff != 1 && b_diff != 0xff )) && !fast_edge_loading )
        {
           successive_reads++;
           if( successive_reads >= 3) {
               tape_stop();
               successive_reads = 0;
           }
        }
        else
        {
          successive_reads = 0;
        }
    }
    ///END FROM FUSE

    }

    if(tape_playing && mconfig.edge_loading)
    {

        if(_IN_A_hook[0][1] == _IN_pos || _IN_A_hook[1][1] == _IN_pos)
        return;

        if
        (
        Z80ReadMem_notiming(_IN_pos - 2) == 0x3e && //LD A,+x
        Z80ReadMem_notiming(_IN_pos - 3) == 0xc8 && //RET Z
        Z80ReadMem_notiming(_IN_pos - 4) == 0x04  //INC B
        )
        {
        _IN_A_hook[new_IN_A_pos][0]=_IN_pos-2;//justo antes del in, cambio de edge
        _IN_A_hook[new_IN_A_pos][1]=_IN_pos;

        new_IN_A_pos = (new_IN_A_pos + 1) % 2;
        }
        else if
        (
        Z80ReadMem_notiming(_IN_pos - 5) == 0x20 && //JR NZ,+x
        Z80ReadMem_notiming(_IN_pos - 6) == 0x04  //INC B
        )
        {
        _IN_A_hook[new_IN_A_pos][0]=_IN_pos-5;//justo antes del in, cambio de edge
        _IN_A_hook[new_IN_A_pos][1]=_IN_pos;

        new_IN_A_pos = (new_IN_A_pos + 1) % 2;
        }
        else if(
        Z80ReadMem_notiming(_IN_pos - 2) == 0x01 && //JR NZ,1
        Z80ReadMem_notiming(_IN_pos - 3) == 0x20 &&
        Z80ReadMem_notiming(_IN_pos - 4) == 0x04  //INC B
        )
        {
        _IN_A_hook[new_IN_A_pos][0]=_IN_pos-3;//justo antes del in, cambio de edge
        _IN_A_hook[new_IN_A_pos][1]=_IN_pos;

        new_IN_A_pos = (new_IN_A_pos + 1) % 2;
        }

        //TODO Otro algoritmo. mirar si crece, decrece, algun registro en menos de 500 tstates
        //y A no cambia. Mas generico. pero Perdemos control del temporizador de A. los dos.
    }
}

/* Inicializa una cinta .TZX */
int Tape_init(void *fp, int size)
{
    fast_edge_loading=0;
    tstates_prev_A=0;
    _IN_A_hook[0][0]=0x05f1-2;
    _IN_A_hook[0][1]=0x05f1;
    _IN_A_hook[1][0]=0x0000;
    _IN_A_hook[1][1]=0x0000;
    new_IN_A_pos=1;

    tape_open(fp,size,0,0);
    Tape_rewind();

    return 1;
}


Tape_close()
{
     tape_close();
     return 1;
}

int Tape_load(Z80Regs * regs)
{

   word x;

/* x128 v0.93 */

  /* fuse 0.7.0:
   *
   * On exit:
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

  //regs->BC.W=0xCB01;
  //regs->HL.W=0;

  int res = tape_load_trap(regs);

  if(res==0) //if ok, return inversed
  {

   //regs->AF.B.l = (regs->AF.B.l & (255-64) ) | 1;
   //regs->AF.B.h=0;

   regs->AF.B.l |= (C_FLAG);
   regs->AF.B.l |= (Z_FLAG);
   regs->AF.B.h=0;

   //regs->BC.B.h=0xB0;

   regs->AFs.B.l = 0x45;
   regs->AFs.B.h = 0x01;


   regs->PC.W=0x05e2;

   return 1;
  }
  else if(res==-1) //error de carga
  {
   regs->AF.B.l &= ~(C_FLAG);
   regs->AF.B.l &= ~(Z_FLAG);
   regs->AF.B.h=0; //1?

   //regs->BC.B.h=0x00;

   regs->AFs.B.l = 0x01;
   regs->AFs.B.h = regs->AFs.B.h;

   regs->PC.W=0x05e2;

   return 1;
  }
  else if(res==1)
  {
    //salida normal del trap sin hacer nada!
    //no modificamos pc ni nada.
    return 0;
  }

}

int Tape_rewind()
{
    tape_stop();
    tape_select_block(0);
    return 0;
}
