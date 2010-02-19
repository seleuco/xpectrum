//rlyeh, based on AY2SNA

/*

- AY-file format (thanks to Patrik Rak for help).

  Originally this format was supporting on Amiga's DeliTracker (DeliAY plug-in).
  Therefore, all word data is in Motorola order (from hi to lo). Also, all
  pointers are signed and relative (from current position in file to pointed
  data). So, AY-file is sequences of records. First record is header. Here you
  are:

  TAYFileHeader=packed record
   FileID,			//'ZXAY'
   TypeID:longword;		//'EMUL'
   FileVersion,			//Version of song file (release version)
   PlayerVersion:byte;		//Required player version. Micro Speccy
				//works as player of version 3.
				//Now available values:
				//0       Use zero, if you do not
				//        know what player version
				//        you need
				//All other player versions and short
				//description of these players next:
				//1       initial player version
				//2       first 256 bytes is
				//        filled with 0xC9 (ret),
				//        not just last init byte
				//3       PC rewrite, different
				//        Z80 init routine, full
				//        Z80 emulation, supports
				//        48k tunes.
   PSpecialPlayer,		//It's for tunes which contained "custom
				//player" in m68k assembly. As Patrik Rak
				//is saying, exists only one AY-file of this
				//kind. So, can be simply ignored.
   PAuthor,PMisc:smallint;	//Relative pointers to the
				//Author and Misc NTStrings
   NumOfSongs,FirstSong:byte;	//Number of songs and first song values, both
				//are decreased by 1
   PSongsStructure:smallint;	//Relative pointer to song structure
  end;

  Next is song structure (repeated NumOfSongs + 1 times):

  TSongStructure=packed record
   PSongName,PSongData:smallint;//Relative pointers to NTStr song
				//name and song data
   end;

  All described above is rightly for any AY-file. Following data only for
  TypeID = 'EMUL'. Next is song data:
 
  TSongData=packed record
   AChan,BChan,
   CChan,Noise:byte;		//These are actually four bytes which specify
				//which Amiga channels will play which AY
				//channel (A,B,C and Noise, respectively). The
				//legal value is any permutation of 0,1,2,3.
				//The most common is 0,1,2,3. Not used in
				//Micro Speccy, of course
   SongLength:word;		//Song duration in 1/50 of second. If ZERO then
				//length is unknown (infinitely). Not used in
				//Micro Speccy
   FadeLength:word;		//Duration of volume fade after end of song in
				//1/50 of second. Not used in Micro Speccy
   HiReg,LoReg:byte;		//HiReg and LoReg for all common registers:
				//AF,AF',HL,HL',DE,DE',BC,BC',IX and IY
   PPoints,			//Relative pointer to points data
   PAddresses:smallint;		//Relative pointer to blocks data
  end;

  Next is points data:

  TPoints = packed record
   Stack,Init,Inter:word;	//Values for SP, INIT and INTERRUPT addresses
  end;

  Next is blocks data (words, offsets are signed):

      Address1			//Address of 1st block in Z80 memory
      Length1			//Length of 1st block
      Offset1			//Relative offset in AY-file to this block
      Address2                  //and for 2nd block
      Length2
      Offset2
      ...
      AddressN			//and for Nth (last) block
      LengthN
      OffsetN
      ENDWORD                   //ZERO word (end of blocks data)
				//so block can't have ZERO
				//address

  Notes from Patrik Rak:
  The word values should be always aligned at 2 byte offsets. Otherwise the
  DeliAY will crash (it's the limitation of the m68000).

  In case Address+Length > 65536, DeliAY decreases the size to make it
  == 65536.

  From me:
  In case CurrPositionInAYFile+Offset+Length > AYFileSize, MicroSpeccy
  and AYPlay decreases the size to make it == AYFileSize. I must to say,
  that if it is then this is broken AY-file. But there are many that ones
  in the world.





*/

#include "shared.h"

struct ay_track_tag
  {
  unsigned char *namestr,*data;
  unsigned char *data_stacketc,*data_memblocks;
  int fadestart,fadelen;
  };

struct aydata_tag
  {
  unsigned char *filedata;
  int filelen;
  struct ay_track_tag *tracks;
  
  int filever,playerver;
  unsigned char *authorstr,*miscstr;
  int num_tracks;
  int first_track;
  };

struct aydata_tag aydata;


void play_track(Z80Regs *regs, int track)
{
    unsigned char intz[]=
    {
    0xf3,	/* di             */
    0xcd,0,0,	/* call init      */
    0xed,0x5e,	/* loop: im 2     */
    0xfb,	/* ei             */
    0x76,	/* halt           */
    0x18,0xfa	/* jr loop        */
    };

    unsigned char intnz[]=
    {
    0xf3,	/* di             */
    0xcd,0,0,	/* call init      */
    0xed,0x56,	/* loop: im 1     */
    0xfb,	/* ei             */
    0x76,	/* halt           */
    0xcd,0,0,	/* call interrupt */
    0x18,0xf7	/* jr loop        */
    };

    int init,ay_1st_block,ourinit,interrupt;

    #define GETWORD(x) (((*(x))<<8)|(*(x+1)))

    init=GETWORD(aydata.tracks[track].data_stacketc+2);
    interrupt=GETWORD(aydata.tracks[track].data_stacketc+4);
    ay_1st_block=GETWORD(aydata.tracks[track].data_memblocks);

//    m) Load to PC ZERO value
//    n) Disable Z80 interrupts and set IM0 mode
//    o) Emulate resetting of AY chip
//    p) Start Z80 emulation

    ZX_Reset(ZX_128);

    memset(&ROM_pages[0x0000],0xC9,0x0100);
    memset(&ROM_pages[0x0100],0xFF,0x3F00);
    memset(&ROM_pages[0x4000],0x00,0xC000);
    ROM_pages[0x38]=0xfb; /* EI */

    /* call first AY block if no init */
    ourinit=(init?init:ay_1st_block);

    if(!interrupt) memcpy(&ROM_pages[0],intz,sizeof(intz));
    else  {
	   memcpy(&ROM_pages[0],intnz,sizeof(intnz));
	   ROM_pages[ 9]=interrupt%256;
	   ROM_pages[10]=interrupt/256;
	  }

    ROM_pages[2]=ourinit%256; /* it patches call init */
    ROM_pages[3]=ourinit/256; 

//  h) Load all blocks for this song

    /* now put the memory blocks in place */
    {
    unsigned char *ptr;
    int addr,len,ofs;

    ptr=aydata.tracks[track].data_memblocks;
    while((addr=GETWORD(ptr))!=0)
    { 
     len=GETWORD(ptr+2);
     ofs=GETWORD(ptr+4); 
     if(ofs>=0x8000) ofs=-0x10000+ofs;

     /* range check */
     if(ptr-4-aydata.filedata+ofs>=aydata.filelen || ptr-4-aydata.filedata+ofs<0)
     {
      ptr+=6;
      continue;
     }
 
     /* fix any broken length */
     if(ptr+4+ofs+len>=aydata.filedata+aydata.filelen)
       len=aydata.filedata+aydata.filelen-(ptr+4+ofs);
     if(addr+len>0x10000)
       len=0x10000-addr;
   
     memcpy(&ROM_pages[addr],ptr+4+ofs,len);
     ptr+=6;
     }
    }


//    i) Load all common lower registers with LoReg value (including AF register)
//    j) Load all common higher registers with HiReg value

    regs->BC.B.l = *(aydata.tracks[track].data+9);
    regs->BC.B.h = *(aydata.tracks[track].data+8);

    regs->DE.W = 
    regs->HL.W = 
    regs->AF.W =

    regs->BCs.W = 
    regs->DEs.W = 
    regs->HLs.W = 
    regs->AFs.W =

    regs->IX.W =
    regs->IY.W = regs->BC.W;

//  k) Load into I register 3 (this player version)

    regs->I = 3;

//  l) load to SP stack value from points data of this song

    regs->SP.W = GETWORD(aydata.tracks[track].data_stacketc+0);

}



int read_ay_file(Z80Regs *regs, void *fp, int ay_size)
{
unsigned char *data,*ptr,*ptr2;
int data_len=ay_size;
int tmp, f;

#define READWORD(x)   (x)=256*(*ptr++); (x)|=*ptr++
//#define READWORD(x)     (x)=(*ptr++); (x)|=256*(*ptr++)
#define READWORDPTR(x)	READWORD(tmp); if(tmp>=0x8000) tmp=-0x10000+tmp; if(ptr-data-2+tmp>=data_len || ptr-data-2+tmp<0)  return(0); (x)=ptr-2+tmp
#define CHECK_ASCIIZ(x) for(f=0,tmp=0;tmp<data+data_len-(x);tmp++) if(*(x+tmp)==0) f=1; if(!f) return(0); 

aydata.filedata=data=(unsigned char *)fp;
aydata.tracks=(struct ay_track_tag *)&data[100*1024]; //at least 100k+100k required
aydata.filelen=ay_size;

//if(memcmp(data,"ZXAYEMUL",8)!=0) return(0);

ptr=data+8;
aydata.filever=*ptr++;
aydata.playerver=*ptr++; if(aydata.playerver!=3) return(0);
ptr+=2;		/* skip custom player stuff */
READWORDPTR(aydata.authorstr);
//CHECK_ASCIIZ(aydata.authorstr);
READWORDPTR(aydata.miscstr);
//CHECK_ASCIIZ(aydata.miscstr);
aydata.num_tracks=1+*ptr++;
aydata.first_track=*ptr++;

/* skip to track info */
READWORDPTR(ptr2);
ptr=ptr2;

/*
if((aydata.tracks=malloc(aydata.num_tracks*sizeof(struct ay_track_tag)))==NULL)
  {
   return(0);
  }
  */

for(f=0;f<aydata.num_tracks;f++)
  {
   READWORDPTR(aydata.tracks[f].namestr);
   //CHECK_ASCIIZ(aydata.tracks[f].namestr);
   READWORDPTR(aydata.tracks[f].data);
  }

for(f=0;f<aydata.num_tracks;f++)
  {
  if(aydata.tracks[f].data-data+10>data_len-4)
    {
    //free(aydata.tracks);
    return(0);
    }

  ptr=aydata.tracks[f].data+10;
  READWORDPTR(aydata.tracks[f].data_stacketc);
  READWORDPTR(aydata.tracks[f].data_memblocks);
  
  ptr=aydata.tracks[f].data+4;
  READWORD(aydata.tracks[f].fadestart);
  READWORD(aydata.tracks[f].fadelen);
  }

/* ok then, that's as much parsing as we do here. */

return(1);
}

