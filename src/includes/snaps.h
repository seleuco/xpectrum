/*=====================================================================
  snaps.h    -> Header file for snaps.c.

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

 Copyright (c) 2000 Santiago Romero Iglesias.
 Email: sromero@escomposlinux.org
 ======================================================================*/  

#define TYPE_NULL 0
#define TYPE_TZX 1
#define TYPE_TAP 2
#define TYPE_Z80 3
#define TYPE_SNA 4
#define TYPE_SP  5
#define TYPE_SCR 6

extern int fast_edge_loading;

char LoadSP (Z80Regs *, void *);
char LoadSNA48 (Z80Regs *, void *);
char LoadSNA128 (Z80Regs *, void *);

// RUTINAS DE CINTA: ESPECIFICAS
//void TAP_init(void *fp, int siz);
//void TAP_rewind(void);
//char LoadTAP(Z80Regs * regs);



int  LoadZ80 (Z80Regs *, byte *, byte *);

 int SaveSCR (void * fp);
void LoadSCR (Z80Regs * regs, void * fp);

void LoadCart(Z80Regs * regs, void * fp);



byte LoadMIC(int tstate);
int LoadRAW(void * fp, int siz, int tstatesmax);


 //int TZX_init(void *fp, int siz);
 //int TZX_genindex(void *fp, int siz);
 //int LoadTZX(Z80Regs * regs);
 //int TZX_rewind(void);



/*
 to do:

int typeoffile(char *);
char LoadSnapshot (Z80Regs * regs, char *filename);

char SaveSNA (Z80Regs *, FILE *);
char SaveSCR (Z80Regs *, FILE *);
char SaveSnapshot (Z80Regs * regs, char *filename);
char SaveScreenshot (Z80Regs * regs, char *filename);


// RUTINAS DE CINTA: GENERICAS 
FILE *InitTape(FILE *fp);

// busca el archivo en los sitios habituales y lo abre como rb 
FILE *findopen_file(char *file);
char *find_file(char *file);
*/
