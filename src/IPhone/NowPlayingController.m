/* 
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
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
   
   Copyright (c) 2010 Seleuco. Based in ZodTTD code.
   
*/
#import "helpers.h"
#import "NowPlayingController.h"
#import "HelpController.h"
#import "OptionsController.h"
#import <pthread.h>

#define IPHONE_MENU_DISABLED            0
#define IPHONE_MENU_MAIN                1

#define IPHONE_MENU_HELP                5
#define IPHONE_MENU_OPTIONS             6

#define IPHONE_MENU_QUIT                7
#define IPHONE_MENU_MAIN_LOAD           8

    
#define MyCGRectContainsPoint(rect, point)						\
	(((point.x >= rect.origin.x) &&								\
		(point.y >= rect.origin.y) &&							\
		(point.x <= rect.origin.x + rect.size.width) &&			\
		(point.y <= rect.origin.y + rect.size.height)) ? 1 : 0)  


unsigned short* screenbuffer;

int iphone_menu = IPHONE_MENU_DISABLED;

int iphone_controller_opacity = 50;
int iphone_is_landscape = 0;
int iphone_smooth = 0;
int iphone_keep_aspect_ratio = 0;


enum  { GP2X_UP=0x1,       GP2X_LEFT=0x4,       GP2X_DOWN=0x10,  GP2X_RIGHT=0x40,
	    GP2X_START=1<<8,   GP2X_SELECT=1<<9,    GP2X_L=1<<10,    GP2X_R=1<<11,
	    GP2X_A=1<<12,      GP2X_B=1<<13,        GP2X_X=1<<14,    GP2X_Y=1<<15,
        GP2X_VOL_UP=1<<23, GP2X_VOL_DOWN=1<<22, GP2X_PUSH=1<<27 };


extern float __audioVolume;
extern unsigned short BaseAddress[240*160];
extern unsigned long gp2x_pad_status;
extern int __emulation_run;
extern int __emulation_paused;

extern pthread_t main_tid;

static unsigned long newtouches[10];
static unsigned long oldtouches[10];

extern unsigned short* videobuffer;


extern int iphone_main (int argc, char **argv);



//SHARED y GLOBALES!
pthread_t	main_tid;
int			__emulation_run = 0;
int        __emulation_paused = 0;

static ScreenView *sharedInstance = nil;


void iphone_UpdateScreen()
{
  //return;
  //usleep(100);
  //sched_yield();

  [sharedInstance performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone:NO];  
}


void* app_Thread_Start(void* args)
{
	__emulation_run = 1;
	iphone_main(0, NULL);
	__emulation_run = 0;
	return NULL;
}

@implementation ScreenView
- (id)initWithFrame:(CGRect)frame {
	if ((self = [super initWithFrame:frame])!=nil) {
		//NSLog(@"Inicializando initWithFrame\n");
		CFMutableDictionaryRef dict;
		int w = 320; //rect.size.width;
		int h = 240; //rect.size.height;
		
    	int pitch = w * 2, allocSize = 2 * w * h;
    	char *pixelFormat = "565L";
		
  		self.opaque = YES;
  		self.clearsContextBeforeDrawing = NO;
  		self.userInteractionEnabled = NO;
  		self.multipleTouchEnabled = NO;
  		self.contentMode = UIViewContentModeTopLeft;

 
		dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
										 &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		CFDictionarySetValue(dict, kCoreSurfaceBufferGlobal, kCFBooleanTrue);
		CFDictionarySetValue(dict, kCoreSurfaceBufferMemoryRegion,
							 @"IOSurfaceMemoryRegion");
		CFDictionarySetValue(dict, kCoreSurfaceBufferPitch,
							 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &pitch));
		CFDictionarySetValue(dict, kCoreSurfaceBufferWidth,
							 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &w));
		CFDictionarySetValue(dict, kCoreSurfaceBufferHeight,
							 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &h));
		CFDictionarySetValue(dict, kCoreSurfaceBufferPixelFormat,
							 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, pixelFormat));
		CFDictionarySetValue(dict, kCoreSurfaceBufferAllocSize,
							 CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &allocSize));
		
		CoreSurfaceBufferRef _screenSurface = CoreSurfaceBufferCreate(dict);
		CoreSurfaceBufferLock(_screenSurface, 3);
		  
		screenLayer = [[CALayer layer] retain];

		if(!iphone_is_landscape) //Portrait
		{
			screenLayer.frame = CGRectMake(0.0f, 0.0f, 319.0f, 239.0f);
		   [ screenLayer setOpaque: YES ];
		}
		else
		{				
		   if(!iphone_keep_aspect_ratio)
			  screenLayer.frame = CGRectMake(0.0f, 0.0f, 480.0f, 320.0f);
		   else
		      screenLayer.frame = CGRectMake(28.0f, 1.0f, 424.0f, 318.0f);//keep aspect
				
		   [ screenLayer setOpaque:YES ];
		}
		
		if(iphone_smooth==0)
		{
		   [screenLayer setMinificationFilter:kCAFilterNearest];
		   [screenLayer setMagnificationFilter:kCAFilterNearest];
		}
		else
		{
		   [screenLayer setMagnificationFilter:kCAFilterLinear];
  	       [screenLayer setMinificationFilter:kCAFilterLinear];
  	    }  
		
		screenLayer.contents = (id)_screenSurface;
		[[self layer] addSublayer:screenLayer];
		
    	CoreSurfaceBufferUnlock(_screenSurface);

		screenbuffer = CoreSurfaceBufferGetBaseAddress(_screenSurface);
		//[NSThread setThreadPriority:0.0];
		//MODO ALTERNATIVO DE HACER EL UPDATE
		//[NSThread detachNewThreadSelector:@selector(updateScreen) toTarget:self withObject:nil];
				
	}
    
    sharedInstance = self;
    	
	return self;
}

- (void)dealloc
{
	screenbuffer == NULL;
	[ screenLayer release ];
	[ super dealloc ];
}

- (void)drawRect:(CGRect)rect
{

}

- (void)dummy
{
	
}

- (void)updateScreen
{
   [NSThread setThreadPriority:1.0];

   NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
   while(__emulation_run)
   {
	  [self performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone:NO];
      //usleep(16666);//60hz
      usleep(20000);//50hz
      //usleep(25000);//40hz
      //usleep(33333);//30hz
      sched_yield();
   }
   [pool release];  
}

@end

@implementation NowPlayingController

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
	// the user clicked one of the OK/Cancel buttons
	if(__emulation_run)
	{
	  if(iphone_menu == IPHONE_MENU_MAIN)
	  {
        if(buttonIndex == 0)
	    {
           iphone_menu = IPHONE_MENU_HELP;
	    }
	    else if(buttonIndex == 1)
	    {
           iphone_menu = IPHONE_MENU_OPTIONS;
	    }
	    else	    
	    {
           iphone_menu = IPHONE_MENU_DISABLED;
	    }
	  }
	  
	  if(iphone_menu == IPHONE_MENU_HELP)
	  {

           HelpController *addController =[HelpController alloc];
                                   
           [self presentModalViewController:addController animated:YES];

           [addController release];
	  }
	  
	  if(iphone_menu == IPHONE_MENU_OPTIONS)
	  {
            
           OptionsController *addController =[OptionsController alloc];
                                   
           [self presentModalViewController:addController animated:YES];

           [addController release];
	  }

	}
}

-(void)done:(id)sender {
	
    [self dismissModalViewControllerAnimated:YES];

	Options *op = [[Options alloc] init];
	   
    iphone_keep_aspect_ratio = [op keepAspectRatio];
    
    if(iphone_smooth != [op smoothed] )
    {
        iphone_smooth = [op smoothed];
             
       [screenView removeFromSuperview];
       [screenView release];
       [controllerImageView removeFromSuperview];
       [controllerImageView release];
  
       [self buildPortrait];  
    }
    
    [op release];
    
    iphone_menu = IPHONE_MENU_DISABLED;
}

- (void)runMenu
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
  __emulation_paused = 1;
  iphone_menu = IPHONE_MENU_MAIN_LOAD;
  while(iphone_menu != IPHONE_MENU_DISABLED)
  {
    if(iphone_menu == IPHONE_MENU_MAIN_LOAD)
    {
      iphone_menu = IPHONE_MENU_MAIN;
      [NSThread detachNewThreadSelector:@selector(runMainMenu) toTarget:self withObject:nil];
	}
	else
	{
      usleep(1000000);
    }
  }
  __emulation_paused = 0;
  [pool release];
}

- (void)runMainMenu
{

    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	UIActionSheet *alert = [[UIActionSheet alloc] initWithTitle:
	   @"Choose an option from the menu. Press cancel to go back." delegate:self cancelButtonTitle:nil destructiveButtonTitle:nil 
	   otherButtonTitles:@"Help",@"Options",@"Cancel", nil];
	[alert showInView:screenView];
	[alert release];
    [pool release];
}

- (void)startEmu:(char*)path {
	int i = 0;
	 
    iphone_menu = IPHONE_MENU_DISABLED;
		
	self.view.frame = CGRectMake(0.0f, 0.0f, 320.0f, 480.0f);
	Options *op = [[Options alloc] init];
	    
    iphone_smooth = [op smoothed];
    iphone_keep_aspect_ratio = [op keepAspectRatio];
    
    [op release];
		
    [self buildPortrait];
		
    pthread_create(&main_tid, NULL, app_Thread_Start, NULL);
		
	struct sched_param    param;
    //param.sched_priority = 63;
    param.sched_priority = 46;
    //param.sched_priority = 100;
        
    if(pthread_setschedparam(main_tid, SCHED_RR, &param) != 0)
    {
             fprintf(stderr, "Error setting pthread priority\n");
    }

}

- (void)loadView {

	struct CGRect rect = [[UIScreen mainScreen] bounds];
	rect.origin.x = rect.origin.y = 0.0f;
	self.view = [[UIView alloc] initWithFrame:rect];
    self.view.backgroundColor = [UIColor blackColor];
	
}

-(void)viewDidLoad{	

	//[self.view addSubview:self.imageView];
 	
	//[ self getControllerCoords:0 ];
	
	//self.navigationItem.hidesBackButton = YES;
	
    //self.view.opaque = YES;
	self.view.clearsContextBeforeDrawing = NO; //Performance?
	//self.view.userInteractionEnabled = YES;
	
	self.view.multipleTouchEnabled = YES;
	//self.view.exclusiveTouch = YES;
	
    //self.view.multipleTouchEnabled = NO; investigar porque se queda
	self.view.contentMode = UIViewContentModeTopLeft;
	
	//[[self.view layer] setMagnificationFilter:kCAFilterNearest];
	//[[self.view layer] setMinificationFilter:kCAFilterNearest];
	
	[NSThread setThreadPriority:1.0];
	
}

- (void)drawRect:(CGRect)rect
{
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	//return (interfaceOrientation == UIInterfaceOrientationPortrait);
	return YES;
}

-(void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {

  __emulation_paused = 1;
  usleep(100000);//ensure some frames displayed

  [screenView removeFromSuperview];
  [screenView release];
  [controllerImageView removeFromSuperview];
  [controllerImageView release];

	if((self.interfaceOrientation ==  UIDeviceOrientationLandscapeLeft) || (self.interfaceOrientation == UIDeviceOrientationLandscapeRight)){
	    [self buildLandscape];	        
	
	} else	if((self.interfaceOrientation == UIDeviceOrientationPortrait) || (self.interfaceOrientation == UIDeviceOrientationPortraitUpsideDown)){	
        [self buildPortrait];
	}
	
	__emulation_paused = 0;
}

- (void)buildPortrait {

   iphone_is_landscape = 0;
   [ self getControllerCoords:0 ];
   __emulation_run = 1;
   screenView = [ [ScreenView alloc] initWithFrame: CGRectMake(0, 0, 320, 480)];
   [self.view addSubview: screenView];
   controllerImageView = [ [ UIImageView alloc ] initWithImage:[UIImage imageNamed:[NSString stringWithFormat:@"controller_hs%d.png", 0/*[SOApp.optionsView getCurrentSkin]*/]]];
   controllerImageView.frame = CGRectMake(0.0f, 240.0f, 320.0f, 240.0f); // Set the frame in which the UIImage should be drawn in.
   controllerImageView.userInteractionEnabled = NO;
   controllerImageView.multipleTouchEnabled = NO;
   controllerImageView.clearsContextBeforeDrawing = NO;
   [controllerImageView setOpaque:YES];
   [controllerImageView setAlpha:((float)iphone_controller_opacity / 100.0f)];
   [self.view addSubview: controllerImageView]; // Draw the image in self.view.
}

- (void)buildLandscape{
	
   iphone_is_landscape = 1;
   [ self getControllerCoords:1 ];
   __emulation_run = 1;
    screenView = [ [ScreenView alloc] initWithFrame: CGRectMake(0, 0, 480, 320)];			
   [self.view addSubview: screenView];
   controllerImageView = [ [ UIImageView alloc ] initWithImage:[UIImage imageNamed:[NSString stringWithFormat:@"controller_fs%d.png", 0]]];			
   controllerImageView.frame = CGRectMake(0.0f, 0.0f, 480.0f, 320.0f); // Set the frame in which the UIImage should be drawn in.
   controllerImageView.userInteractionEnabled = NO;
   controllerImageView.multipleTouchEnabled = NO;
   controllerImageView.clearsContextBeforeDrawing = NO;
  [controllerImageView setOpaque:YES];
  [controllerImageView setAlpha:((float)iphone_controller_opacity / 100.0f)];
  [self.view addSubview: controllerImageView]; // Draw the image in self.view.		
}

#if 1
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{	    
  int touchstate[10];
	//Get all the touches.
  int i;
  NSSet *allTouches = [event allTouches];
  int touchcount = [allTouches count];
	
  if(!__emulation_run)
  {
    return;
  }
  
	
  for (i = 0; i < 10; i++) 
  {
    touchstate[i] = 0;
    oldtouches[i] = newtouches[i];
    //printf("Entrada i=%d -> new %d old %d, ",i,newtouches[i],oldtouches[i]);
  }
  
  //if(touchcount>1) printf("num touches %d\n",touchcount);

  
  for (i = 0; i < touchcount ; i++) 
  {
	    UITouch *touch = [[allTouches allObjects] objectAtIndex:i];
    
		if( touch != nil && 
		    ( touch.phase == UITouchPhaseBegan ||
			    touch.phase == UITouchPhaseMoved ||
  			  touch.phase == UITouchPhaseStationary) )
		{
			struct CGPoint point;
			point = [touch locationInView:self.view];
			
            touchstate[i] = 1;

	        if (MyCGRectContainsPoint(Left, point)) 
			{
				gp2x_pad_status |= GP2X_LEFT;
                newtouches[i] = GP2X_LEFT;
                //printf("l ");
			}
			else if (MyCGRectContainsPoint(Right, point)) 
			{
				gp2x_pad_status |= GP2X_RIGHT;
				newtouches[i] = GP2X_RIGHT;
				//printf("r ");
			}
			else if (MyCGRectContainsPoint(Up, point)) 
			{
				gp2x_pad_status |= GP2X_UP;
				newtouches[i] = GP2X_UP;
				//printf("u ");
			}
			else if (MyCGRectContainsPoint(Down, point))
			{
				gp2x_pad_status |= GP2X_DOWN;
				newtouches[i] = GP2X_DOWN;
				//printf("d ");
			}
			else if (MyCGRectContainsPoint(ButtonLeft, point)) 
			{
				gp2x_pad_status |= GP2X_A;
				newtouches[i] = GP2X_A;
				//printf("a ");
			}
			else if (MyCGRectContainsPoint(ButtonRight, point)) 
			{
				gp2x_pad_status |= GP2X_B;
				newtouches[i] = GP2X_B;
				//printf("b ");
			}
			else if (MyCGRectContainsPoint(ButtonUp, point)) 
			{
				gp2x_pad_status |= GP2X_Y;
				newtouches[i] = GP2X_Y;
			}
			else if (MyCGRectContainsPoint(ButtonDown, point)) 
			{
				gp2x_pad_status |= GP2X_X;
				newtouches[i] = GP2X_X;
			}
			else if (MyCGRectContainsPoint(ButtonUpLeft, point)) 
			{
				gp2x_pad_status |= GP2X_A | GP2X_Y;
				newtouches[i] = GP2X_A | GP2X_Y;
			}
			else if (MyCGRectContainsPoint(ButtonDownLeft, point)) 
			{
				if(iphone_is_landscape)
				{
					gp2x_pad_status |= GP2X_B;
	                newtouches[i] = GP2X_B;
				}
				else
				{
					gp2x_pad_status |= GP2X_X | GP2X_A;
					newtouches[i] = GP2X_X | GP2X_A;
				}
			}
			else if (MyCGRectContainsPoint(ButtonUpRight, point)) 
			{
				gp2x_pad_status |= GP2X_B | GP2X_Y;
				newtouches[i] = GP2X_B | GP2X_Y;
			}
			else if (MyCGRectContainsPoint(ButtonDownRight, point)) 
			{
				gp2x_pad_status |= GP2X_X | GP2X_B;
				newtouches[i] = GP2X_X | GP2X_B;
			}
			else if (MyCGRectContainsPoint(UpLeft, point)) 
			{
				gp2x_pad_status |= GP2X_UP | GP2X_LEFT;
				newtouches[i] = GP2X_UP | GP2X_LEFT;
			} 
			else if (MyCGRectContainsPoint(DownLeft, point)) 
			{
				gp2x_pad_status |= GP2X_DOWN | GP2X_LEFT;
				newtouches[i] = GP2X_DOWN | GP2X_LEFT;
			}
			else if (MyCGRectContainsPoint(UpRight, point)) 
			{
				gp2x_pad_status |= GP2X_UP | GP2X_RIGHT;
				newtouches[i] = GP2X_UP | GP2X_RIGHT;
			}
			else if (MyCGRectContainsPoint(DownRight, point)) 
			{
				gp2x_pad_status |= GP2X_DOWN | GP2X_RIGHT;
				newtouches[i] = GP2X_DOWN | GP2X_RIGHT;
			}			
			else if (MyCGRectContainsPoint(LPad, point)) 
			{
				gp2x_pad_status |= GP2X_L;
				newtouches[i] = GP2X_L;
			}
			else if (MyCGRectContainsPoint(RPad, point)) 
			{
				gp2x_pad_status |= GP2X_R;
				newtouches[i] = GP2X_R;
			}			
			else if (MyCGRectContainsPoint(LPad2, point)) 
			{
				gp2x_pad_status |= GP2X_VOL_DOWN;
				newtouches[i] = GP2X_VOL_DOWN;
			}
			else if (MyCGRectContainsPoint(RPad2, point)) 
			{
				gp2x_pad_status |= GP2X_VOL_UP;
				newtouches[i] = GP2X_VOL_UP;
			}
			else if (MyCGRectContainsPoint(Select, point)) 
			{
				gp2x_pad_status |= GP2X_SELECT;
				newtouches[i] = GP2X_SELECT;
			}			
			else if (MyCGRectContainsPoint(Start, point)) 
			{
				gp2x_pad_status |= GP2X_START;
				newtouches[i] = GP2X_START;
			}			
			else if (MyCGRectContainsPoint(Menu, point)) 
			{
				if(touch.phase == UITouchPhaseBegan || touch.phase == UITouchPhaseStationary)
				{
					if(__emulation_run)
					{
                        [NSThread detachNewThreadSelector:@selector(runMenu) toTarget:self withObject:nil];
					}

				}
			}
			else
			{
			    //printf(" fuera\n\n\n");
			}
			
            if(oldtouches[i] != newtouches[i])
            {
                gp2x_pad_status &= ~(oldtouches[i]);
                //printf(" ( %d old %d != new %d) PAD=%d ",i,oldtouches[i],newtouches[i],gp2x_pad_status);
                
            }
            else
            {
                //printf(" ( %d old %d == new %d) PAD=%d ",i,oldtouches[i],newtouches[i],gp2x_pad_status);
            }
		}	
	} 

//Tiene un fallo cuando no llegan en orden.
//Si se pulsa  en [0] tenemos r y luego llegan 2 touches, el primero no coincide con ningun rectangulo, pero mantiene la
//pulsacion anterior por newstate, el segundo es el r. en la siguiente iteracion tenemos 2 r y al haber solo un touch, se limpia en la salida
//el estado anterior del touch, quedandonos sin r.

    for (i = 0; i < 10; i++) //CRAP CODE downt works me & ZodTTD , i need to look later...
    {
       //printf("Salida i=%d -> state %d new %d old %d. ",i,touchstate[i],newtouches[i],oldtouches[i]);
       if(touchstate[i] == 0 /*&& newtouches[i]!=0*/)
      {
         int really = 1;
         
         int j;
         for(j=0;j<10 && really;j++)
         {
            if(j==i)continue;
               //really=newtouches[j]!=newtouches[i];
               really=(newtouches[j] & newtouches[i]) == 0;
         }
         
         if(really)
             gp2x_pad_status &= ~(newtouches[i]);
         
         newtouches[i] = 0;
         oldtouches[i] = 0;
         //printf(" ( %d PAD=%d ) ",i,gp2x_pad_status);
      }
    }
}


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesBegan:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesBegan:touches withEvent:event];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesBegan:touches withEvent:event];
}

#else

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	int i;
	//Get all the touches.
	NSSet *allTouches = [event allTouches];
	int touchcount = [allTouches count];
	
	gp2x_pad_status = 0;
			
	for (i = 0; i < touchcount; i++) 
	{
		UITouch *touch = [[allTouches allObjects] objectAtIndex:i];
		
		if(touch == nil)
		{
			return;
		}
		
		/*if(touch.phase == UITouchPhaseBegan)
		{
			NSLog([NSString stringWithFormat:@"%s", test_print_buffer]);
		}*/
		if( touch.phase == UITouchPhaseBegan		||
			touch.phase == UITouchPhaseMoved		||
			touch.phase == UITouchPhaseStationary	)
		{
			struct CGPoint point;
			point = [touch locationInView:self.view];
			
			if (MyCGRectContainsPoint(ButtonUp, point)) {
				gp2x_pad_status |= GP2X_Y;
				//NSLog(@"GP2X_Y");
			}
			else if (MyCGRectContainsPoint(ButtonDown, point)) {
				gp2x_pad_status |= GP2X_X;
				//NSLog(@"GP2X_X");
			}
			else if (MyCGRectContainsPoint(ButtonLeft, point)) {
				gp2x_pad_status |= GP2X_A;
				//NSLog(@"GP2X_A");
			}
			else if (MyCGRectContainsPoint(ButtonRight, point)) {
				gp2x_pad_status |= GP2X_B;
				//NSLog(@"GP2X_B");
			}
			else if (MyCGRectContainsPoint(ButtonUpLeft, point)) {
				gp2x_pad_status |= GP2X_Y | GP2X_A;
				//NSLog(@"GP2X_Y | GP2X_A");
			}
			else if (MyCGRectContainsPoint(ButtonDownLeft, point)) {

				if(iphone_is_landscape)
				{
					gp2x_pad_status |= GP2X_B;
				}
				else
				{
					gp2x_pad_status |= GP2X_X | GP2X_A;
				}			
				//NSLog(@"GP2X_X | GP2X_A");
			}
			else if (MyCGRectContainsPoint(ButtonUpRight, point)) {
				gp2x_pad_status |= GP2X_Y | GP2X_B;
				//NSLog(@"GP2X_Y | GP2X_B");
			}			
			else if (MyCGRectContainsPoint(ButtonDownRight, point)) {
				gp2x_pad_status |= GP2X_X | GP2X_B;
				//NSLog(@"GP2X_X | GP2X_B");
			} 
			else if (MyCGRectContainsPoint(Select, point)) {
			    //NSLog(@"GP2X_SELECT");
				gp2x_pad_status |= GP2X_SELECT;
			}
			else if (MyCGRectContainsPoint(Start, point)) {
				//NSLog(@"GP2X_START");
				gp2x_pad_status |= GP2X_START;
			}
			else if (MyCGRectContainsPoint(Up, point)) {
				//NSLog(@"GP2X_UP");
				gp2x_pad_status |= GP2X_UP;
			}			
			else if (MyCGRectContainsPoint(Down, point)) {
				//NSLog(@"GP2X_DOWN");
				gp2x_pad_status |= GP2X_DOWN;
			}			
			else if (MyCGRectContainsPoint(Left, point)) {
				//NSLog(@"GP2X_LEFT");
				gp2x_pad_status |= GP2X_LEFT;
			}			
			else if (MyCGRectContainsPoint(Right, point)) {
				//NSLog(@"GP2X_RIGHT");
				gp2x_pad_status |= GP2X_RIGHT;
			}			
			else if (MyCGRectContainsPoint(UpLeft, point)) {
				//NSLog(@"GP2X_UP | GP2X_LEFT");
				gp2x_pad_status |= GP2X_UP | GP2X_LEFT;
			}			
			else if (MyCGRectContainsPoint(UpRight, point)) {
				//NSLog(@"GP2X_UP | GP2X_RIGHT");
				gp2x_pad_status |= GP2X_UP | GP2X_RIGHT;
			}			
			else if (MyCGRectContainsPoint(DownLeft, point)) {
				//NSLog(@"GP2X_DOWN | GP2X_LEFT");
				gp2x_pad_status |= GP2X_DOWN | GP2X_LEFT;
			}			
			else if (MyCGRectContainsPoint(DownRight, point)) {
				//NSLog(@"GP2X_DOWN | GP2X_RIGHT");
				gp2x_pad_status |= GP2X_DOWN | GP2X_RIGHT;
			}
			else if (MyCGRectContainsPoint(LPad, point)) {
				//NSLog(@"GP2X_L");
				gp2x_pad_status |= GP2X_L;
			}
			else if (MyCGRectContainsPoint(RPad, point)) {
				//NSLog(@"GP2X_R");
				gp2x_pad_status |= GP2X_R;
			}			
			else if (MyCGRectContainsPoint(LPad2, point)) {
				//NSLog(@"GP2X_VOL_DOWN");
				gp2x_pad_status |= GP2X_VOL_DOWN;
			}
			else if (MyCGRectContainsPoint(RPad2, point)) {
				//NSLog(@"GP2X_VOL_UP");
				gp2x_pad_status |= GP2X_VOL_UP;
			}			
			else if (MyCGRectContainsPoint(Menu, point)) {
				if(touch.phase == UITouchPhaseBegan || touch.phase == UITouchPhaseStationary)
				{
					if(__emulation_run)
					{
                        [NSThread detachNewThreadSelector:@selector(runMenu) toTarget:self withObject:nil];
					}
					else
					{
  					//[SOApp.delegate switchToBrowse];
  					//[tabBar didMoveToWindow];
					}
				}
			}
		}
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesBegan:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesBegan:touches withEvent:event];

}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesBegan:touches withEvent:event];

}
#endif


- (void)getControllerCoords:(int)orientation {
    char string[256];
    FILE *fp;
	
	if(!orientation)
	{
		fp = fopen([[NSString stringWithFormat:@"%scontroller_hs%d.txt", get_resource_path("/"), 0] UTF8String], "r");
    }
	else
	{
		fp = fopen([[NSString stringWithFormat:@"%scontroller_fs%d.txt", get_resource_path("/"), 0] UTF8String], "r");
	}
	
	if (fp) 
	{

		int i = 0;
        while(fgets(string, 256, fp) != NULL && i < 24) 
       {
			char* result = strtok(string, ",");
			int coords[4];
			int i2 = 1;
			while( result != NULL && i2 < 5 )
			{
				coords[i2 - 1] = atoi(result);
				result = strtok(NULL, ",");
				i2++;
			}
			
			switch(i)
			{
    		case 0:    DownLeft   	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 1:    Down   	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 2:    DownRight    = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 3:    Left  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 4:    Right  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 5:    UpLeft     	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 6:    Up     	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 7:    UpRight  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 8:    Select = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 9:    Start  = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 10:   LPad   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 11:   RPad   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 12:   Menu   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 13:   ButtonDownLeft   	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 14:   ButtonDown   	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 15:   ButtonDownRight    	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 16:   ButtonLeft  		= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 17:   ButtonRight  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 18:   ButtonUpLeft     	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 19:   ButtonUp     	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 20:   ButtonUpRight  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 21:   LPad2   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 22:   RPad2   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
        case 23:   iphone_controller_opacity = coords[0]; break;
			}
      i++;
    }
    fclose(fp);
  }
}

- (void)didReceiveMemoryWarning {
	[super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
	// Release anything that's not essential, such as cached data
}


- (void)dealloc {
	
	[super dealloc];
}


@end
