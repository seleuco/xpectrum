/*  cpuctrl for GP2X
    Copyright (C) 2005-2006  Hermes/PS2Reality 
    Some part from Robster's sdk2x (Copyright (C) 2005 by Rob Brown rob@cobbleware.com)    

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
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/


/****************************************************************************************************************************************/
// CPU CONTROL
/****************************************************************************************************************************************/

#include <sys/mman.h>

extern unsigned long   memfd ;
extern unsigned short *memregs16;

// system registers
static struct 
{
unsigned short SYSCLKENREG,SYSCSETREG,FPLLVSETREG,DUALINT920,DUALINT940,DUALCTRL940;
}
system_reg;

static unsigned short dispclockdiv;

volatile unsigned short *MEM_REG;

#define SYS_CLK_FREQ 7372800

unsigned MDIV,PDIV,SCALE;

void wait_vsync()
{MEM_REG[0x2846>>1]=(MEM_REG[0x2846>>1] | 0x20) & ~2;
while(!(MEM_REG[0x2846>>1] & 2));
}

void cpuctrl_init()
{
MEM_REG=&memregs16[0];
}
void save_system_regs()
{
system_reg.SYSCSETREG=MEM_REG[0x91c>>1];
system_reg.FPLLVSETREG=MEM_REG[0x912>>1];

system_reg.SYSCLKENREG=MEM_REG[0x904>>1];

system_reg.DUALINT920=MEM_REG[0x3B40>>1];
system_reg.DUALINT940=MEM_REG[0x3B42>>1];

system_reg.DUALCTRL940=MEM_REG[0x3B48>>1];
dispclockdiv=MEM_REG[0x924>>1];
}

void load_system_regs()
{

MEM_REG[0x91c>>1]=system_reg.SYSCSETREG;
MEM_REG[0x910>>1]=system_reg.FPLLVSETREG;



MEM_REG[0x3B40>>1]=system_reg.DUALINT920;
MEM_REG[0x3B42>>1]=system_reg.DUALINT940;

MEM_REG[0x3B48>>1]=system_reg.DUALCTRL940;
MEM_REG[0x904>>1]=system_reg.SYSCLKENREG;
MEM_REG[0x924>>1]=dispclockdiv;
}


unsigned get_display_clock_div()
{
return ((MEM_REG[0x924>>1]>>8));
}
void set_display_clock_div(unsigned div)
{
div=(div & 0xff)<<8;
MEM_REG[0x924>>1]=(MEM_REG[0x924>>1] & ~(255<<8)) | div;
}
unsigned get_pixel_clock_div()
{
return ((MEM_REG[0x926>>1]>>8) & 63);
}

void set_FCLK(unsigned MHZ)
{
unsigned v;
unsigned mdiv,pdiv=3,scale=0;
MHZ*=1000000;
mdiv=(MHZ*pdiv)/SYS_CLK_FREQ;

mdiv=((mdiv-8)<<8) & 0xff00;
pdiv=((pdiv-2)<<2) & 0xfc;
scale&=3;
v=mdiv | pdiv | scale;
MEM_REG[0x910>>1]=v;
}

unsigned get_freq_UCLK()
{
unsigned i;
unsigned reg,mdiv,pdiv,scale;

i = MEM_REG[0x900>>1];
i = ((i >> 7) & 1) ;
if(i) return 0;

reg=MEM_REG[0x916>>1];
mdiv = ((reg & 0xff00) >> 8) + 8;
pdiv = ((reg & 0xfc) >> 2) + 2;
scale = reg & 3;

		
return ((SYS_CLK_FREQ * mdiv) / (pdiv << scale));
}

unsigned get_freq_ACLK()
{
unsigned i;
unsigned reg,mdiv,pdiv,scale;

i = MEM_REG[0x900>>1];
i = ((i >> 8) & 1) ;
if(i) return 0;

reg=MEM_REG[0x918>>1];
mdiv = ((reg & 0xff00) >> 8) + 8;
pdiv = ((reg & 0xfc) >> 2) + 2;
scale = reg & 3;

		
return ((SYS_CLK_FREQ * mdiv) / (pdiv << scale));
}

unsigned get_freq_920_CLK()
{
unsigned i;
unsigned reg,mdiv,pdiv,scale;
reg=MEM_REG[0x912>>1];
mdiv = ((reg & 0xff00) >> 8) + 8;
pdiv = ((reg & 0xfc) >> 2) + 2;
scale = reg & 3;
MDIV=mdiv;
PDIV=pdiv;
SCALE=scale;

i = (MEM_REG[0x91c>>1] & 7)+1;
return ((SYS_CLK_FREQ * mdiv) / (pdiv << scale))/i;
}


unsigned get_freq_940_CLK()
{
unsigned i;
unsigned reg,mdiv,pdiv,scale;
reg=MEM_REG[0x912>>1];
mdiv = ((reg & 0xff00) >> 8) + 8;
pdiv = ((reg & 0xfc) >> 2) + 2;
scale = reg & 3;

i = ((MEM_REG[0x91c>>1]>>3) & 7)+1;
return ((SYS_CLK_FREQ * mdiv) / (pdiv << scale))/i;
}

unsigned get_freq_DCLK()
{
unsigned i;
unsigned reg,mdiv,pdiv,scale;
reg=MEM_REG[0x912>>1];
mdiv = ((reg & 0xff00) >> 8) + 8;
pdiv = ((reg & 0xfc) >> 2) + 2;
scale = reg & 3;

i = ((MEM_REG[0x91c>>1]>>6) & 7)+1;
return ((SYS_CLK_FREQ * mdiv) / (pdiv << scale))/i;
}

void set_920_Div(unsigned short div)
{
	
unsigned short v;
v = MEM_REG[0x91c>>1] & (~0x3);
MEM_REG[0x91c>>1] = (div & 0x7) | v; 

}

unsigned short get_920_Div()
{
return (MEM_REG[0x91c>>1] & 0x7); 
}

void set_940_Div(unsigned short div)
{
	
unsigned short v;
v = (unsigned short)( MEM_REG[0x91c>>1] & (~(0x7 << 3)) );
MEM_REG[0x91c>>1] = ((div & 0x7) << 3) | v; 

}

unsigned short get_940_Div()
{
return ((MEM_REG[0x91c>>1] >> 3) & 0x7); 
}

void set_DCLK_Div( unsigned short div )
{

unsigned short v;
v = (unsigned short)( MEM_REG[0x91c>>1] & (~(0x7 << 6)) );
MEM_REG[0x91c>>1] = ((div & 0x7) << 6) | v; 
}

unsigned short get_DCLK_Div()
{
return ((MEM_REG[0x91c>>1] >> 6) & 0x7); 
}


unsigned short Disable_Int_920()
{
unsigned short ret;
ret=MEM_REG[0x3B40>>1];
MEM_REG[0x3B40>>1]=0;
MEM_REG[0x3B44>>1]=0xffff;	
return ret;	
}
unsigned short Disable_Int_940()
{
unsigned short ret;
ret=MEM_REG[0x3B42>>1];
MEM_REG[0x3B42>>1]=0;
MEM_REG[0x3B46>>1]=0xffff;	
return ret;	
}

void Enable_Int_920(unsigned short flag)
{
MEM_REG[0x3B40>>1]=flag;
}

void Enable_Int_940(unsigned short flag)
{
MEM_REG[0x3B42>>1]=flag;
}

void Disable_940()
{
Disable_Int_940();
MEM_REG[0x3B48>>1]|= (1 << 7);
MEM_REG[0x904>>1]&=0xfffe;
}



volatile unsigned  *arm940code;

void Load_940_code(unsigned *code,int size)
{
	unsigned *cp;
	int i;
 arm940code=(unsigned  *)mmap(0, 0x100000, PROT_READ|PROT_WRITE, MAP_SHARED, memfd , 0x03000000);



Disable_940();
cp=(unsigned *) code;;
 for (i = 0; i < size/4; i ++)
	{
		arm940code[i] = cp[i];
	}
for (i = 0; i < 64; i ++)
	{
		arm940code[0x3FC0+i] = 0;
	}
MEM_REG[0x3B48>>1]=(MEM_REG[0x3B48>>1]  & 0xFF00) | 0x03; // allow 940

}

void clock_940_off()
{
MEM_REG[0x904>>1]&=0xfffe;
}

void clock_940_on()
{
MEM_REG[0x904>>1]|=1;
}




// based in Robster's sdk2x method

void set_display(int mode) // 1- display on 0-display off
{
int i;
if(mode)
	{
	
	MEM_REG[0x1062>>1] &= ~0x0c;
	MEM_REG[0x1066>>1] &= ~0x10;
	MEM_REG[0x106E>>1] &= ~0x06;
	MEM_REG[0x106E>>1] |= 0x08;

	MEM_REG[0x280A>>1]= 0xffff;
	MEM_REG[0x280C>>1]= 0xffff;
	MEM_REG[0x280E>>1]= 0xffff;
	MEM_REG[0x2802>>1]= 0;
	MEM_REG[0x2800>>1]|= 0x0001;

	for (i=0;i<1000000;i++);
    
	MEM_REG[0x1062>>1] |= 0x0c;
	MEM_REG[0x1066>>1] |= 0x10;
	MEM_REG[0x106E>>1] &= ~0x08;
	MEM_REG[0x106E>>1] |= 0x06;

	}
else
	{
	MEM_REG[0x106E>>1]&=~4;
	MEM_REG[0x280A>>1]= 0;
	MEM_REG[0x280C>>1]= 0;
	MEM_REG[0x280E>>1]= 0;
	MEM_REG[0x2800>>1]= 4;
	MEM_REG[0x2802>>1]= 1;
	}
}


void set_battery_led(int on)
{
on^=1;
on&=1;
MEM_REG[0x106E>>1]&=(~16);
MEM_REG[0x106E>>1]|=on<<4;
}


unsigned get_status_UCLK()
{
unsigned i;
unsigned reg,mdiv,pdiv,scale;

i = MEM_REG[0x900>>1];
i = ((i >> 7) & 1) ;
if(i) return 0;
return 1;
}

unsigned get_status_ACLK()
{
unsigned i;
unsigned reg,mdiv,pdiv,scale;

i = MEM_REG[0x900>>1];
i = ((i >> 8) & 1) ;
if(i) return 0;

return 1;
}

void set_status_UCLK(unsigned s)
{
if(s==0) MEM_REG[0x900>>1]|=128;
else MEM_REG[0x900>>1]&=~128;
}

void set_status_ACLK(unsigned s)
{
unsigned i;
unsigned reg,mdiv,pdiv,scale;

if(s==0) MEM_REG[0x900>>1]|=256;
else MEM_REG[0x900>>1]&=~256;

//return 1;
}
