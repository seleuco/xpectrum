/*=====================================================================
  graphics.c -> This file includes all the graphical functions

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
 Copyright (c) 2005 rlyeh
 Copyright (c) 2006-2007 Metalbrain
 ======================================================================*/
#define BORDERDELAY 5

#include "shared.h"

extern Z80Regs *spectrumZ80;

extern MCONFIG mconfig;

extern int memwrites;
extern int memwriteaddr[];
extern int memwritetime[];
extern int memwritevalue[];

extern int outwrites;
extern int outwritetime[];
extern int outwritevalue[];

extern int pagewrites;
extern int pagewritetime[];
extern int pagewritevalue[];

int full_screen=0;

//hardware definitions for spectrum 48K
tipo_hwopt hwopt = { 0xFF, 24, 128, 24, 48, 224, 16, 48, 192, 48, 8,
  224, 64, 192, 56, 24, 128, 72
};


int scan_convert[192] = {
   0<<5,8<<5,16<<5,24<<5,32<<5,40<<5,48<<5,56<<5,
   1<<5,9<<5,17<<5,25<<5,33<<5,41<<5,49<<5,57<<5,
   2<<5,10<<5,18<<5,26<<5,34<<5,42<<5,50<<5,58<<5,
   3<<5,11<<5,19<<5,27<<5,35<<5,43<<5,51<<5,59<<5,
   4<<5,12<<5,20<<5,28<<5,36<<5,44<<5,52<<5,60<<5,
   5<<5,13<<5,21<<5,29<<5,37<<5,45<<5,53<<5,61<<5,
   6<<5,14<<5,22<<5,30<<5,38<<5,46<<5,54<<5,62<<5,
   7<<5,15<<5,23<<5,31<<5,39<<5,47<<5,55<<5,63<<5,
		   
   64<<5,72<<5,80<<5,88<<5,96<<5,104<<5,112<<5,120<<5,
   65<<5,73<<5,81<<5,89<<5,97<<5,105<<5,113<<5,121<<5,
   66<<5,74<<5,82<<5,90<<5,98<<5,106<<5,114<<5,122<<5,
   67<<5,75<<5,83<<5,91<<5,99<<5,107<<5,115<<5,123<<5,
   68<<5,76<<5,84<<5,92<<5,100<<5,108<<5,116<<5,124<<5,
   69<<5,77<<5,85<<5,93<<5,101<<5,109<<5,117<<5,125<<5,
   70<<5,78<<5,86<<5,94<<5,102<<5,110<<5,118<<5,126<<5,
   71<<5,79<<5,87<<5,95<<5,103<<5,111<<5,119<<5,127<<5,

   128<<5,136<<5,144<<5,152<<5,160<<5,168<<5,176<<5,184<<5,
   129<<5,137<<5,145<<5,153<<5,161<<5,169<<5,177<<5,185<<5,
   130<<5,138<<5,146<<5,154<<5,162<<5,170<<5,178<<5,186<<5,
   131<<5,139<<5,147<<5,155<<5,163<<5,171<<5,179<<5,187<<5,
   132<<5,140<<5,148<<5,156<<5,164<<5,172<<5,180<<5,188<<5,
   133<<5,141<<5,149<<5,157<<5,165<<5,173<<5,181<<5,189<<5,
   134<<5,142<<5,150<<5,158<<5,166<<5,174<<5,182<<5,190<<5,
   135<<5,143<<5,151<<5,159<<5,167<<5,175<<5,183<<5,191<<5
};

char SpectrumFlashFlag=0;

/*-----------------------------------------------------------------
 CreateVideoTables ( void );
 Creates tables for direct access to videomemory pixels and attr.
------------------------------------------------------------------*/

unsigned short Pixeles[192],Atributos[192];

void CreateVideoTables ( void )
{
   int y;

   for( y=0; y < 192; y++)
   {
    Pixeles[y]   = ((y & 0xC0) << 5) + ((y & 0x7) << 8) + ((y & 0x38) << 2);
    Atributos[y] = 6144+(32 * (y >> 3));
   }
}

byte ToBeDrawn[6912*2];

/*-----------------------------------------------------------------
 Redraw the entire screen from the speccy's VRAM.
 It reads the from the 16384 memory address the bytes of the
 VideoRAM needed to rebuild the screen and the border.
------------------------------------------------------------------*/

void
JustRun(Z80Regs * regs, int do_skip)
{

int target_tstate, current_tstate,memindex,outindex,direccion,charx,chary,i,repeat;

int scanl,x,startbytes,startattr;
int idxout,idxmem,idxpage,border,page;
volatile byte *gp32offset;
register byte fg,bg,attr,bytevalue;

outwrites=0;
outwritetime[outwrites]=spectrumZ80->ICount;
outwritevalue[outwrites++]=BorderColor;

Z80Run_NC (spectrumZ80, 50 ); // Trying interruption

target_tstate = ( model<ZX_128 ? TIMING_48+1 : TIMING_128+1 ) - 23 ; // 23=max execution time of uncontended instruction
Z80Run_NCNI (spectrumZ80, target_tstate - (spectrumZ80->IPeriod - spectrumZ80->ICount) ); // upper border

memwrites=0;
pagewrites=0;
if(!do_skip)
	{
	register byte *VRAM5,*VRAM7;
	VRAM5   = (byte *)&RAM_pages[0x4000*5];
	VRAM7    = (byte *)&RAM_pages[0x4000*7];
       	memcpy(ToBeDrawn,VRAM5,6912);
       	memcpy(ToBeDrawn+6912,VRAM7,6912);
	pagewritetime[pagewrites]=spectrumZ80->ICount;
	pagewritevalue[pagewrites++]=pagination_128 & 8;
	page=pagination_128 & 8;
	}
target_tstate+=192*hwopt.ts_line+23;	// All screen
if (mconfig.contention==1 && !(mconfig.speed_loading && tape_playing))
	{
        Z80Run (spectrumZ80, target_tstate - (spectrumZ80->IPeriod - spectrumZ80->ICount) );
	}
else	{
    	Z80Run_NCNI (spectrumZ80, target_tstate - (spectrumZ80->IPeriod - spectrumZ80->ICount) );
	}
pagewritetime[pagewrites]=spectrumZ80->ICount;
memwritetime[memwrites]=spectrumZ80->ICount;

Z80Run_NCNI (spectrumZ80, spectrumZ80->ICount-73 ); // Lower border

outwritetime[outwrites]=spectrumZ80->ICount;

Z80Run_NC (spectrumZ80, spectrumZ80->ICount);       // End & Try interrupt?

if(!do_skip)
	{
	if(!full_screen)
		{
		idxout=0;
		idxpage=0;
		idxmem=0;

		target_tstate = ( model<ZX_128 ? (TIMING_48 - 16) : (TIMING_128 - 16) ) - 24 * hwopt.ts_line ;
		do	{
			border=outwritevalue[idxout];
			idxout++;
			}
		while((spectrumZ80->IPeriod - outwritetime[idxout])<target_tstate+BORDERDELAY);
		
		gp32offset=(byte *)Picture;

		for (scanl = 0; scanl < 24; scanl++)
			{
			for(x=0;x<40;x++)
				{
				if((spectrumZ80->IPeriod - outwritetime[idxout])<target_tstate+BORDERDELAY)
					{
					border=outwritevalue[idxout++];
					}
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				target_tstate+=4;
				}
			target_tstate+=hwopt.ts_line-160;
			while((spectrumZ80->IPeriod - outwritetime[idxout])<target_tstate+BORDERDELAY)
				{
				border=outwritevalue[idxout++];
				}
			}

		for (scanl = 0; scanl < 192; scanl++)
			{
			startbytes=Pixeles[scanl];
			startattr=Atributos[scanl];

			for(x=0;x<4;x++)
				{
				if((spectrumZ80->IPeriod - outwritetime[idxout])<target_tstate+BORDERDELAY)
					{
					border=outwritevalue[idxout++];
					}
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				target_tstate+=4;
				}
			while((spectrumZ80->IPeriod - pagewritetime[idxpage]) < target_tstate)
				{
				page=864*pagewritevalue[idxpage++];
				}
			while((spectrumZ80->IPeriod - memwritetime[idxmem])  < target_tstate)
				{
				ToBeDrawn[memwriteaddr[idxmem]]=memwritevalue[idxmem];
				idxmem++;
				}
			bytevalue=ToBeDrawn[startbytes+page];
			attr=ToBeDrawn[startattr+page];
			if(SpectrumFlashFlag)
				{
				if(attr&0x80)
			   		{
   					bg=(attr&0x7)+((attr>>3)&0x8); fg=((attr>>3)&0x0F);
   					}
  				else   	{
	  				fg=(attr&0x7)+((attr>>3)&0x8); bg=((attr>>3)&0x0F);
  					}
	 			*(gp32offset++)=bytevalue & 0x80 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x40 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x20 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x10 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x8  ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x4  ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x2  ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x1  ? fg : bg;
				target_tstate+=4;
				for(x=1;x<32;x++)
					{
					if((spectrumZ80->IPeriod - memwritetime[idxmem] ) < target_tstate)
						{
						ToBeDrawn[memwriteaddr[idxmem]]=memwritevalue[idxmem];
						idxmem++;
						}
					if((spectrumZ80->IPeriod - pagewritetime[idxpage]) < target_tstate)
						{
						page=864*pagewritevalue[idxpage++];
						}
					bytevalue=ToBeDrawn[startbytes+x+page];
					attr=ToBeDrawn[startattr+x+page];
					if(attr&0x80)
				   		{
   						bg=(attr&0x7)+((attr>>3)&0x8); fg=((attr>>3)&0x0F);
   						}
  					else   	{
		  				fg=(attr&0x7)+((attr>>3)&0x8); bg=((attr>>3)&0x0F);
  						}
	  				*(gp32offset++)=bytevalue & 0x80 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x40 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x20 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x10 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x8  ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x4  ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x2  ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x1  ? fg : bg;
					target_tstate+=4;
 					}
				}
			else	{
				fg=(attr&0x7)+((attr>>3)&0x8); bg=((attr>>3)&0x0F);
	  			*(gp32offset++)=bytevalue & 0x80 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x40 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x20 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x10 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x8  ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x4  ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x2  ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x1  ? fg : bg;
				target_tstate+=4;
				for(x=1;x<32;x++)
					{
					if((spectrumZ80->IPeriod - memwritetime[idxmem] ) < target_tstate)
						{
						ToBeDrawn[memwriteaddr[idxmem]]=memwritevalue[idxmem];
						idxmem++;
						}
					if((spectrumZ80->IPeriod - pagewritetime[idxpage]) < target_tstate)
						{
						page=864*pagewritevalue[idxpage++];
						}
					bytevalue=ToBeDrawn[Pixeles[scanl]+x+page];
					attr=ToBeDrawn[Atributos[scanl]+x+page];
					fg=(attr&0x7)+((attr>>3)&0x8); bg=((attr>>3)&0x0F);
	  				*(gp32offset++)=bytevalue & 0x80 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x40 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x20 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x10 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x8  ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x4  ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x2  ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x1  ? fg : bg;
					target_tstate+=4;
 					}
				}

			for(x=0;x<4;x++)
				{
				while((spectrumZ80->IPeriod - outwritetime[idxout])<target_tstate+BORDERDELAY)
					{
					border=outwritevalue[idxout++];
					}
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				target_tstate+=4;
				}
			target_tstate+=hwopt.ts_line-160;
			}

		for (scanl = 0; scanl < 24; scanl++)
			{
			while((spectrumZ80->IPeriod - outwritetime[idxout])<target_tstate+BORDERDELAY)
				{
				border=outwritevalue[idxout];
				idxout++;
				}
			*(gp32offset++)=border;
			*(gp32offset++)=border;
			*(gp32offset++)=border;
			*(gp32offset++)=border;
			*(gp32offset++)=border;
			*(gp32offset++)=border;
			*(gp32offset++)=border;
			*(gp32offset++)=border;
			target_tstate+=4;
			for(x=1;x<40;x++)
				{
				if((spectrumZ80->IPeriod - outwritetime[idxout])<target_tstate+BORDERDELAY)
					{
					border=outwritevalue[idxout++];
					}
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				*(gp32offset++)=border;
				target_tstate+=4;
				}
			target_tstate+=hwopt.ts_line-160;
			}
		}
	else	{
		idxpage=0;
		idxmem=0;

		target_tstate = ( model<ZX_128 ? TIMING_48+1 : TIMING_128+1 );
		gp32offset=(byte *)Picture;
		for (scanl = 0; scanl < 192; scanl++)
			{
			startbytes=Pixeles[scanl];
			startattr=Atributos[scanl];
			while((spectrumZ80->IPeriod - pagewritetime[idxpage]) < target_tstate)
				{
				page=864*pagewritevalue[idxpage++];
				}
			while((spectrumZ80->IPeriod - memwritetime[idxmem])  < target_tstate)
				{
				ToBeDrawn[memwriteaddr[idxmem]]=memwritevalue[idxmem];
				idxmem++;
				}
			bytevalue=ToBeDrawn[startbytes+page];
			attr=ToBeDrawn[startattr+page];
			if(SpectrumFlashFlag)
				{
				if(attr&0x80)
			   		{
   					bg=(attr&0x7)+((attr>>3)&0x8); fg=((attr>>3)&0x0F);
   					}
  				else   	{
	  				fg=(attr&0x7)+((attr>>3)&0x8); bg=((attr>>3)&0x0F);
  					}
	 			*(gp32offset++)=bytevalue & 0x80 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x40 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x20 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x10 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x8  ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x4  ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x2  ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x1  ? fg : bg;
				target_tstate+=4;
				for(x=1;x<32;x++)
					{
					if((spectrumZ80->IPeriod - memwritetime[idxmem] ) < target_tstate)
						{
						ToBeDrawn[memwriteaddr[idxmem]]=memwritevalue[idxmem];
						idxmem++;
						}
					if((spectrumZ80->IPeriod - pagewritetime[idxpage]) < target_tstate)
						{
						page=864*pagewritevalue[idxpage++];
						}
					bytevalue=ToBeDrawn[startbytes+x+page];
					attr=ToBeDrawn[startattr+x+page];
					if(attr&0x80)
				   		{
   						bg=(attr&0x7)+((attr>>3)&0x8); fg=((attr>>3)&0x0F);
   						}
  					else   	{
		  				fg=(attr&0x7)+((attr>>3)&0x8); bg=((attr>>3)&0x0F);
  						}
	  				*(gp32offset++)=bytevalue & 0x80 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x40 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x20 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x10 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x8  ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x4  ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x2  ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x1  ? fg : bg;
					target_tstate+=4;
 					}
				}
			else	{
				fg=(attr&0x7)+((attr>>3)&0x8); bg=((attr>>3)&0x0F);
	  			*(gp32offset++)=bytevalue & 0x80 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x40 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x20 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x10 ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x8  ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x4  ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x2  ? fg : bg;
  				*(gp32offset++)=bytevalue & 0x1  ? fg : bg;
				target_tstate+=4;
				for(x=1;x<32;x++)
					{
					if((spectrumZ80->IPeriod - memwritetime[idxmem] ) < target_tstate)
						{
						ToBeDrawn[memwriteaddr[idxmem]]=memwritevalue[idxmem];
						idxmem++;
						}
					if((spectrumZ80->IPeriod - pagewritetime[idxpage]) < target_tstate)
						{
						page=864*pagewritevalue[idxpage++];
						}
					bytevalue=ToBeDrawn[startbytes+x+page];
					attr=ToBeDrawn[startattr+x+page];
					fg=(attr&0x7)+((attr>>3)&0x8); bg=((attr>>3)&0x0F);
	  				*(gp32offset++)=bytevalue & 0x80 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x40 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x20 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x10 ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x8  ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x4  ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x2  ? fg : bg;
  					*(gp32offset++)=bytevalue & 0x1  ? fg : bg;
					target_tstate+=4;
 					}
				}
			target_tstate+=hwopt.ts_line-128;
			gp32offset+=64;
			}
		}
	}
}
