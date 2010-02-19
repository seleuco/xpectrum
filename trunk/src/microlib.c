#include "microlib.h"

//joy GPIO
#define GPIOMPINLVL 0x1198
#define GPIOCPINLVL 0x1184
#define GPIODPINLVL 0x1186
  
static volatile unsigned char palette_red[256];
static volatile unsigned char palette_green[256];
static volatile unsigned char palette_blue[256];

static volatile unsigned long   ticks = 0;
                  
volatile unsigned short        *memregs16;
static volatile unsigned long  *memregs32;
         
unsigned char  *gp2x_screen;

unsigned char   flip=0;
unsigned long   vram[2];
unsigned char  *buff[2];
static unsigned char ext_buff[320*240];
  

//GP2X DEVICES
//framebuffers
unsigned long fb0fd=0;
unsigned long fb1fd=0;
//mem regs
unsigned long memfd =0;
//sound                   
unsigned long mixerfd =0;
unsigned long dspfd=0;



//OK 
void gp2x_set_palette_color (int red, int green, int blue, int i)
{ 
  if(i>255)return;
  palette_red[i]=red;
  palette_green[i]=green;
  palette_blue[i]=blue;
  
}

//OK 
void gp2x_set_palette(void)
{
  int i=0;  	
  memregs16[0x2958>>1]=0;  // palette table address 0  
  for(i=0; i<=255; i++) {
		memregs16[0x295a>>1]=((palette_green[i])<<8)|palette_blue[i];
		memregs16[0x295a>>1]=palette_red[i];
  }    
}

//OK
void gp2x_set_scaling(int W, int H)
{

  float escalaw,escalah;
  int bpp=(memregs16[0x28DA>>1]>>9)&0x3;

  if(memregs16[0x2800>>1]&0x100) //Vemos si el TV-Out esta activado
  {
    escalaw=489.0; //Factor de escala RGB Horizontal para TV (comun a PAL y NTSC)
    if (memregs16[0x2818>>1]  == 287) //Comprobacion si la altura de la pantalla es para PAL
      escalah=274.0; //Factor de escala RGB Vertical para TV PAL
    else if (memregs16[0x2818>>1]  == 239) //Comprobacion si la altura de la pantalla es para NTSC
      escalah=331.0; //Factor de escala RGB Vertical para TV NTSC
  }
  else //Suponemos que esta en modo LCD (TV-Out desactivado)
  {
    escalaw=1024.0; //Escala RGB Horizontal para LCD
    escalah=320.0; //Escala RGB Vertical para LCD
  }

  // scale horizontal
  memregs16[0x2906>>1]=(unsigned short)((float)escalaw *(W/320.0));
  // scale vertical
  memregs32[0x2908>>2]=(unsigned long)((float)escalah *bpp *(H/240.0));
  
}

void gp2x_width_screen(int W)
{
   int bpp=(memregs16[0x28DA>>1]>>9)&0x3;
   memregs16[0x290C>>1]=W*bpp; 
}


//OK
void gp2x_waitvsync(void)
{
  while(memregs16[0x1182>>1]&(1<<4));
}

void gp2x_flip(void)
{
  unsigned long address=vram[flip];
 
  gp2x_screen =buff [flip^=1]; 
 
  memregs16[0x290E>>1]=(unsigned short)(address & 0xffff);
  memregs16[0x2910>>1]=(unsigned short)(address >> 16);
  memregs16[0x2912>>1]=(unsigned short)(address & 0xffff);
  memregs16[0x2914>>1]=(unsigned short)(address >> 16);
}

//no flip with externalbuffer
void gp2x_noflip_buffering(void)
{
  unsigned long address=vram[flip];	
	   
  gp2x_screen =buff [flip];
  
  memcpy((char*)gp2x_screen,(char*)ext_buff,320*240);
  gp2x_screen = ext_buff;
  
  memregs16[0x290E>>1]=(unsigned short)(address & 0xffff);
  memregs16[0x2910>>1]=(unsigned short)(address >> 16);
  memregs16[0x2912>>1]=(unsigned short)(address & 0xffff);
  memregs16[0x2914>>1]=(unsigned short)(address >> 16);
}

unsigned long gp2x_joystick_poll(void)
{
  unsigned long l=0;
  
  //leo GPIO-M 
  short s1 = memregs16[GPIOMPINLVL>>1];
  
  if(!(s1 & 0x0001))
   l |= GP2X_UP;
  if(!(s1 & 0x0002))
   l |= GP2X_UP | GP2X_LEFT ;
  if(!(s1 & 0x0004))
   l |= GP2X_LEFT ;
  if(!(s1 & 0x0008))
   l |= GP2X_LEFT | GP2X_DOWN;
  if(!(s1 & 0x0010))
   l |= GP2X_DOWN;
  if(!(s1 & 0x0020))
   l |= GP2X_DOWN | GP2X_RIGHT;
  if(!(s1 & 0x0040))
   l |=  GP2X_RIGHT;   
  if(!(s1 & 0x0080))
   l |=  GP2X_RIGHT | GP2X_UP;
   
   //leo GPIO-C  
  short s2 = memregs16[GPIOCPINLVL>>1];
  if(!(s2 & 0x0100))
   l |= GP2X_START ;  
  if(!(s2 & 0x0200))
   l |= GP2X_SELECT; 
  if(!(s2 & 0x0400))
   l |=  GP2X_L;
  if(!(s2 & 0x0800))
   l |= GP2X_R;      
  if(!(s2 & 0x1000))
   l |= GP2X_A;
  if(!(s2 & 0x2000))
   l |= GP2X_B;   
  if(!(s2 & 0x4000))
   l |=  GP2X_X ;   
  if(!(s2 & 0x8000))
   l |= GP2X_Y;
   
  //leo GPIO-D  
  short s3 = memregs16[GPIODPINLVL>>1];
  
  if(!(s3 & 0x0040))
   l |=  GP2X_VOL_DOWN;   
  if(!(s3 & 0x0080))
   l |= GP2X_VOL_UP;  
  if(!(s3 & 0x0800))
   l |= GP2X_PUSH;  
      
  if (num_of_joys > 0) {
	  //check the usb joy as well..
	  gp2x_usbjoy_update();
	  l |= gp2x_usbjoy_check(0);
  }
    
  return l;
}


unsigned long gp2x_timer_poll(void)
{
   return memregs32[0x0A00>>2]/(7372800/1000);
}

//SOUND

void gp2x_volume(int left, int rigth)
{
 if(mixerfd ==0)return;
 left=(((left*0x50)/100)<<8)|((rigth*0x50)/100);          
 ioctl(mixerfd , SOUND_MIXER_WRITE_PCM, &left); 
}

int gp2x_open_sound(int rate, int bits, int stereo){
	
	if(!dspfd)  dspfd = open("/dev/dsp",   O_WRONLY /*| O_NONBLOCK*/);
	
	ioctl(dspfd, SNDCTL_DSP_SETFMT, &bits);

    int res = ioctl(dspfd, SNDCTL_DSP_STEREO, &stereo);
    if(res<0)exit(-1);
    ioctl(dspfd, SNDCTL_DSP_SPEED,  &rate);
  
    int frag = stereo ? (0x40000|13) : (0x40000|12);
    
    if(rate<16500) frag--;
    if(rate<33000) frag--;
    
    ioctl(dspfd, SNDCTL_DSP_SETFRAGMENT,  &frag);

}

int gp2x_close_sound(){
	
	if(dspfd)
	{  
	    close(dspfd);
	    dspfd=0;
	}    
}

int gp2x_send_sound(void *samples,int nsamples)
{
	if(dspfd)
	   return write(dspfd,samples,nsamples<<1);
	else 
	   return -1;
}
 
void gp2x_end(void)
{
 
  close(fb0fd);
  close(fb1fd);
  close(memfd );
  if(dspfd!=0)
     close(dspfd);
  close(mixerfd );
   
  fcloseall();
  
  mmuunhack();
 
  chdir("/usr/gp2x");
  execl("gp2xmenu",NULL);
}
 
void gp2x_init()
{
  struct fb_fix_screeninfo fixed_info;
     
  fb0fd = open("/dev/fb0", O_RDWR);
  fb1fd = open("/dev/fb1", O_RDWR);  
  memfd  = open("/dev/mem", O_RDWR);  
  mixerfd  = open("/dev/mixer", O_RDWR);
  
  memregs16=(unsigned short *)mmap(0, 0x10000, PROT_READ|PROT_WRITE, MAP_SHARED, memfd  , 0xc0000000);
  memregs32=(unsigned long  *)mmap(0, 0x10000, PROT_READ|PROT_WRITE, MAP_SHARED, memfd  , 0xc0000000);
                         
  ioctl (fb0fd, FBIOGET_FSCREENINFO, &fixed_info);   
  gp2x_screen=buff[0]=(unsigned char *)mmap(0,  320*240*2, PROT_WRITE, MAP_SHARED, fb0fd, 0);
  vram[0]=fixed_info.smem_start;
 
  ioctl (fb1fd, FBIOGET_FSCREENINFO, &fixed_info);
  buff[1]=(unsigned char *)mmap(0,  320*240*2, PROT_WRITE, MAP_SHARED, fb1fd, 0);
  vram[1]=fixed_info.smem_start;
                  
  mmuhack();                
 
  //pongo RGB 8
  memregs16[0x28DA>>1]=(((8+1)/8)<<9)|0xAB;
  memregs16[0x290C>>1]=320*((8+1)/8);

}
 
/* Call this MMU Hack kernel module after doing mmap, and before doing memset*/
int mmuhack(void)
{
	int mmufd;
	system("/sbin/rmmod mmuhack");
	system("/sbin/insmod mmuhack.o");
	mmufd = open("/dev/mmuhack", O_RDWR);
	if(mmufd < 0) return 0;

	close(mmufd);
	return 1;
}

/* Unload MMU Hack kernel module after closing all memory devices*/
int mmuunhack(void)
{
	system("/sbin/rmmod mmuhack");
	return 1;
}


/*********************************************************************/
/* GP2X USB Joystick Handling -GnoStiC                               */
/*********************************************************************/
void gp2x_usbjoy_init (void) {
	/* Open available joysticks -GnoStiC */
	int i;
	num_of_joys = 0;
	for (i=0; i<4; i++) {
		joys[i] = joy_open (i+1);
		joy = joys[i];
		if (joy_buttons (joy)>0) { num_of_joys++; }
	}

	printf ("\n\nFound %d Joystick(s)\n",num_of_joys);
	for (i=0; i < num_of_joys; i++) {
		joy = joys[i];
		printf ("+-Joystick %d: %s\n",i+1,joy_name(joy));
	}
}

void gp2x_usbjoy_update (void) {
	/* Update Joystick Event Cache */
	int q, foo;
	for (q=0; q < num_of_joys; q++) {
		foo = joy_update (joys[q]);
	}
}

int gp2x_usbjoy_check (int joyno) {
	/* Check Joystick */
	int q, joyExKey = 0;
	joy = joys[joyno];

	if (joy != NULL) {
		if (joy_getaxe(JOYUP, joy))    { joyExKey |= GP2X_UP; }
		if (joy_getaxe(JOYDOWN, joy))  { joyExKey |= GP2X_DOWN; }
		if (joy_getaxe(JOYLEFT, joy))  { joyExKey |= GP2X_LEFT; }
		if (joy_getaxe(JOYRIGHT, joy)) { joyExKey |= GP2X_RIGHT; }

		/* loop through joy buttons to check if they are pushed */
		for (q=0; q<joy_buttons (joy); q++) {
			if (joy_getbutton (q, joy)) {
				if (q == 0)  { joyExKey |= GP2X_Y; }
				if (q == 1)  { joyExKey |= GP2X_B; }
				if (q == 2)  { joyExKey |= GP2X_X; }
				if (q == 3)  { joyExKey |= GP2X_A; }

				if (q == 4)  { joyExKey |= GP2X_L; }
				if (q == 5)  { joyExKey |= GP2X_R; }
				if (q == 6)  { joyExKey |= GP2X_L; } /* left shoulder button 2 */
				if (q == 7)  { joyExKey |= GP2X_R; } /* right shoulder button 2 */
				if (q == 8)  { joyExKey |= GP2X_SELECT; }
				if (q == 9)  { joyExKey |= GP2X_START; }

				if (q == 10) { joyExKey |= GP2X_PUSH; }
				if (q == 11) { joyExKey |= GP2X_PUSH; }
			}
		}
		return joyExKey;
	} else {
		joyExKey = 0;
		return joyExKey;
	}
}
