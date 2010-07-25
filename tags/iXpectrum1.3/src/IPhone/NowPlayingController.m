/* 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
   Copyright (c) 2010 Seleuco. Based in ZodTTD code.
   
*/

//http://code.google.com/p/metasyntactic/source/browse/trunk/MetasyntacticShared/Classes/ImageUtilities.m?r=4217

#import "helpers.h"
#import "NowPlayingController.h"
#import "HelpController.h"
#import "OptionsController.h"
#import "DonateController.h"
#import "DownloadController.h"
#import <pthread.h>

#include "shared.h"

#define IPHONE_MENU_DISABLED            0
#define IPHONE_MENU_MAIN                1

#define IPHONE_MENU_HELP                5
#define IPHONE_MENU_OPTIONS             6
#define IPHONE_MENU_DONATE              9
#define IPHONE_MENU_DOWNLOAD           11

#define IPHONE_MENU_QUIT                7
#define IPHONE_MENU_MAIN_LOAD           8

    
#define MyCGRectContainsPoint(rect, point)						\
	(((point.x >= rect.origin.x) &&								\
		(point.y >= rect.origin.y) &&							\
		(point.x <= rect.origin.x + rect.size.width) &&			\
		(point.y <= rect.origin.y + rect.size.height)) ? 1 : 0)  

#define RADIANS( degrees ) ( degrees * M_PI / 180 )

unsigned short* screenbuffer = NULL;

static unsigned short img_buffer[320 * 240 * 4];

int iphone_menu = IPHONE_MENU_DISABLED;

int iphone_controller_opacity = 50;
int iphone_keyboard_opacity = 50;
int iphone_is_landscape = 0;
int iphone_smooth_land = 0;
int iphone_smooth_port = 0;
int iphone_keep_aspect_ratio = 0;

extern int isIpad;
int safe_render_path = 1;
int enable_dview = 0;

int crop_border_land = 0;
int crop_border_port = 0;
int crop_state=0;

int tv_filter_land = 0;
int tv_filter_port = 0;

int scanline_filter_land = 0;
int scanline_filter_port = 0;

int controller = 1;
int hide_keyboard = 0;


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

   if(sharedInstance==nil) return;
    
   [sharedInstance performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone:NO];  
}

void* app_Thread_Start(void* args)
{
	__emulation_run = 1;
	iphone_main(0, NULL);
	__emulation_run = 0;
	return NULL;
}


@implementation ScreenLayer

+ (id) defaultActionForKey:(NSString *)key
{
    return nil;
}

- (id)init {
//printf("Crean layer %ld\n",self);
	if (self = [super init])
	{		    
	   bitmapContext = nil;
	   _screenSurface = nil;
	   
	    if(safe_render_path)
	    {
	       	      
	       screenbuffer = img_buffer;
	       	        
	       CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();    
           bitmapContext = CGBitmapContextCreate(
             screenbuffer,
             320,
             240,
             /*8*/5, // bitsPerComponent
             /*4*320*/2*320, // bytesPerRow
             colorSpace,
             kCGImageAlphaNoneSkipFirst  | kCGBitmapByteOrder16Little/*kCGImageAlphaNoneSkipLast */);

            CFRelease(colorSpace);
            
        }
        else
        {	   
		    CFMutableDictionaryRef dict;
			int w = rEmulatorFrame.size.width;
			int h = rEmulatorFrame.size.height;
			
	    	int pitch = w * 2, allocSize = 2 * w * h;
	    	char *pixelFormat = "565L";
			 
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
			
			_screenSurface = CoreSurfaceBufferCreate(dict);
			
			screenbuffer = CoreSurfaceBufferGetBaseAddress(_screenSurface);		 		 		
		 }
		        		
        //rotateTransform = CGAffineTransformMakeRotation(RADIANS(90));
        rotateTransform = CGAffineTransformIdentity;
        self.affineTransform = rotateTransform;
                       		
		if(iphone_smooth_land && iphone_is_landscape || iphone_smooth_port && !iphone_is_landscape)
		{
		   [self setMagnificationFilter:kCAFilterLinear];
  	       [self setMinificationFilter:kCAFilterLinear];
		}
		else
		{

  	       [self setMagnificationFilter:kCAFilterNearest];
		   [self setMinificationFilter:kCAFilterNearest];
  	    } 
  	    
  	    if (0) {
		    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
            [[NSNotificationCenter defaultCenter] addObserver:self 
                                                     selector:@selector(orientationChanged:) 
                                                         name:@"UIDeviceOrientationDidChangeNotification" 
                                                       object:nil];
		}
		
	}
	return self;
}
	
- (void) orientationChanged:(NSNotification *)notification
{
    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
    if (orientation == UIDeviceOrientationLandscapeLeft) {
        rotateTransform = CGAffineTransformMakeRotation(RADIANS(90));
    } else if (orientation == UIDeviceOrientationLandscapeRight) {
        rotateTransform = CGAffineTransformMakeRotation(RADIANS(270));
    }
    
}	

- (void)display {

   if((!iphone_is_landscape && crop_border_port) || (iphone_is_landscape && crop_border_land))//hay que hacer crop
   {
       //TODO: mejor 1.15 para iPhone si borde, resto 1.20
       if(emulating && crop_state==0)
       {
           float factor = isIpad ? 1.20f : 1.15f;
           CGRect r = sharedInstance.frame;
           
           int oldw = r.size.width;          
           r.size.width = r.size.width * factor;
           r.origin.x  = r.origin.x  - ((r.size.width - oldw)/2);
           
           int oldh = r.size.height; 
		   r.size.height = r.size.height * factor;
           r.origin.y  = r.origin.y  - ((r.size.height - oldh)/2);
                      
           sharedInstance.frame = r;
           crop_state=1;
    
       }
       else if(!emulating && crop_state==1)
       {
           /*
           float factor = isIpad ? 1.20f : 1.15f;
           CGRect r = sharedInstance.frame;
           
           int oldw = r.size.width;          
           r.size.width = r.size.width / factor;
           r.origin.x  = r.origin.x  + ((oldw - r.size.width)/2);
           
           int oldh = r.size.height; 
		   r.size.height = r.size.height / factor;
           r.origin.y  = r.origin.y  + ((oldh - r.size.height)/2);
           */
           CGRect r;
           if(!iphone_is_landscape) //Portrait
           {
			   if(!iphone_keep_aspect_ratio)
			     r = rPortraitNoKeepAspectViewFrame;
			   else
			     r = rPortraitViewFrame;  
		   }  		
		   else
		   {			   			
		      if(!iphone_keep_aspect_ratio)
			    r = rLandscapeNoKeepAspectViewFrame;			    
		      else
		        r = rLandscapeImageViewFrame;
		   }
           
           self.frame = r;           
           crop_state=0;
       }   
   }
        
    if(safe_render_path)
    {
   
        CGImageRef cgImage = CGBitmapContextCreateImage(bitmapContext);
        
        self.contents = (id)cgImage;
    
        CFRelease(cgImage);
        
    }
    else
    {
	    CoreSurfaceBufferLock(_screenSurface, 3);
	    
	    self.contents = nil;
	    
	    //self.affineTransform = CGAffineTransformIdentity;
	     self.affineTransform = rotateTransform;
	    self.contents = (id)_screenSurface;
	    		
	    CoreSurfaceBufferUnlock(_screenSurface);
    }   
    
}

- (void)dealloc {
        
                    
    if(bitmapContext!=nil)
    {

       CFRelease(bitmapContext);
       bitmapContext=nil;
    }   
    
    if(_screenSurface!=nil)
    {
      //CoreSurfaceBufferLock(_screenSurface, 3);
      CFRelease(_screenSurface);
      //CoreSurfaceBufferUnlock(_screenSurface);
       _screenSurface = nil;
       self.contents = nil;
    }
    
    [super dealloc];
}
@end

@implementation ScreenView


+ (Class) layerClass
{
    return [ScreenLayer class];
}


- (id)initWithFrame:(CGRect)frame {
	if ((self = [super initWithFrame:frame])!=nil) {
		
		//[NSThread setThreadPriority:0.0];
		//MODO ALTERNATIVO DE HACER EL UPDATE
		//[NSThread detachNewThreadSelector:@selector(updateScreen) toTarget:self withObject:nil];
				
	}
    
    self.opaque = YES;
    self.clearsContextBeforeDrawing = NO;
    self.multipleTouchEnabled = NO;
	self.userInteractionEnabled = NO;
        
    sharedInstance = self;
    	
	return self;
}


- (void)dealloc
{
	//screenbuffer == NULL;
	//[ screenLayer release ];
	
	//sharedInstance = nil;
	
	[ super dealloc ];
}

- (void)drawRect:(CGRect)rect
{
    //printf("Draw rect\n");
    // UIView uses the existence of -drawRect: to determine if should allow its CALayer
    // to be invalidated, which would then lead to the layer creating a backing store and
    // -drawLayer:inContext: being called.
    // By implementing an empty -drawRect: method, we allow UIKit to continue to implement
    // this logic, while doing our real drawing work inside of -drawLayer:inContext:
    
   // if(1)return;
      
   /*
    CGImageRef cgImage = CGBitmapContextCreateImage(bitmapContext);
        
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGRect area;
    
        if(!iphone_is_landscape) //Portrait
		{

			area= rPortraitSurfaceviewFrame;			
		  
		}
		else
		{	
			   			
		   if(!iphone_keep_aspect_ratio)
			  area= rLandscapeNokeepAspectSurfaceviewFrame;
		   else
		   {
		      area.origin.x = 0;
		      area.origin.y = 0; 
		      area.size.width = rLandscapeKeepAspectSurfaceviewFrame.size.width;//keep aspect
		      area.size.height = rLandscapeKeepAspectSurfaceviewFrame.size.height;//keep aspect
		   }
				
		}
    */
    //CGContextTranslateCTM(context, 0, area.size.height);
	
	//CGContextScaleCTM(context, 1.0, -1.0);
	
	//CGContextSetAllowsAntialiasing(context,false);
	
    //CGContextSetShouldAntialias(context, false);
    
    //CGContextSetInterpolationQuality(context,kCGInterpolationNone);
	
    //CGContextDrawImage(context, area, cgImage);
    
    //self.layer.contents = (id)cgImage;
    
    
    //UIImage *newUIImage = [UIImage imageWithCGImage:cgImage];
    //[newUIImage drawInRect:rect];
    //[newUIImage release];
    
    
	/*    
	CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR("image.png"), kCFURLPOSIXPathStyle, false);
	
	CFStringRef type = kUTTypePNG; // or kUTTypeBMP if you like
	CGImageDestinationRef dest = CGImageDestinationCreateWithURL(url, type, 1, 0);
	
	CGImageDestinationAddImage(dest, cgImage, 0);
	*/
	
	//CFRelease(cgImage);
	//CFRelease(bitmapContext);
	//CGImageDestinationFinalize(dest);
	//free(rgba);
    
}

- (void)dummy
{
	
}

- (void)updateScreen
{
   //[NSThread setThreadPriority:1.0];

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
	    else if(buttonIndex == 2)    
	    {
           iphone_menu = IPHONE_MENU_DOWNLOAD;
	    }	    
	    else if(buttonIndex == 3)    
	    {
           iphone_menu = IPHONE_MENU_DONATE;
	    }
	    else	    
	    {
           iphone_menu = IPHONE_MENU_DISABLED;
	    }
	  }
	  
	  //myPool = [[NSAutoreleasePool alloc] init];
	     
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
	  
	  if(iphone_menu == IPHONE_MENU_DONATE)
	  {
    
           DonateController *addController =[DonateController alloc];
                                   
           [self presentModalViewController:addController animated:YES];

           [addController release];
	  }
	  
	  if(iphone_menu == IPHONE_MENU_DOWNLOAD)
	  	  
	  {                          
           DownloadController *downloadController =[DownloadController alloc];
                                                        
           [self presentModalViewController:downloadController animated:YES];

           [downloadController release];
	  }

	}
}

-(void)done:(id)sender {
	
    [self dismissModalViewControllerAnimated:YES];

	Options *op = [[Options alloc] init];
		   
    if(iphone_smooth_port != [op smoothedPort] 
        || iphone_smooth_land != [op smoothedLand] 
        || safe_render_path != [op safeRenderPath]
        || iphone_keep_aspect_ratio != [op keepAspectRatio]
        || crop_border_land != [op cropBorderLand]
        || crop_border_port != [op cropBorderPort]
        || tv_filter_land != [op tvFilterLand]
        || tv_filter_port != [op tvFilterPort]
        || scanline_filter_land != [op scanlineFilterLand]
        || scanline_filter_port != [op scanlineFilterPort]        
        )
    {
        iphone_keep_aspect_ratio = [op keepAspectRatio];
        iphone_smooth_land = [op smoothedLand];
        iphone_smooth_port = [op smoothedPort];
        safe_render_path = [op safeRenderPath];
        
        crop_border_land = [op cropBorderLand];
        crop_border_port = [op cropBorderPort];
        
        tv_filter_land = [op tvFilterLand];
        tv_filter_port = [op tvFilterPort];
        
        scanline_filter_land = [op scanlineFilterLand];
        scanline_filter_port = [op scanlineFilterPort];
             
       [screenView removeFromSuperview];
       [screenView release];
       [imageView removeFromSuperview];
       [imageView release];
       
       if(imageBorder!=nil)
       {
         [imageBorder removeFromSuperview];
         [imageBorder release];
         imageBorder= nil;
       }
  
       [self buildPortrait];  
    }
    
    
    [op release];
    
    iphone_menu = IPHONE_MENU_DISABLED;
    
    //[myPool release];
}

- (void)runMenu
{
  
  if(__emulation_paused)return;
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
   usleep(100000);
  __emulation_paused = 0;
  [pool release];
}

- (void)runMainMenu
{

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	UIActionSheet *alert = [[UIActionSheet alloc] initWithTitle:
	   @"Choose an option from the menu. Press cancel to go back." delegate:self cancelButtonTitle:nil destructiveButtonTitle:nil 
	   otherButtonTitles:@"Help",@"Options",@"Download",@"Donate",@"Cancel", nil];
	/*   
	if(isIpad)
	  //[alert showInView:imageView];
	  [alert showInView:self.view];
	else
	*/
	  [alert showInView:self.view];
	
	[alert release];
	
	[pool release];
    
}


- (void)startEmu:(char*)path {
	int i = 0;
	 
    iphone_menu = IPHONE_MENU_DISABLED;
		
	//self.view.frame = [[UIScreen mainScreen] bounds];//rMainViewFrame;
		
	Options *op = [[Options alloc] init];
	    
    
    iphone_keep_aspect_ratio = [op keepAspectRatio];
    iphone_smooth_land = [op smoothedLand];
    iphone_smooth_port = [op smoothedPort];
    safe_render_path = [op safeRenderPath];
        
    crop_border_land = [op cropBorderLand];
    crop_border_port = [op cropBorderPort];
        
    tv_filter_land = [op tvFilterLand];
    tv_filter_port = [op tvFilterPort];
        
    scanline_filter_land = [op scanlineFilterLand];
    scanline_filter_port = [op scanlineFilterPort];
        
    [op release];
    		
    [self buildPortrait];
				
    pthread_create(&main_tid, NULL, app_Thread_Start, NULL);
		
	struct sched_param    param;
 
    //param.sched_priority = 63;
    param.sched_priority = 46;  
    //param.sched_priority = 100;
   
        
    if(pthread_setschedparam(main_tid, /*SCHED_RR*/ SCHED_OTHER, &param) != 0)
    {
             fprintf(stderr, "Error setting pthread priority\n");
    }
	//iPAD test
	//[NSThread detachNewThreadSelector:@selector(updateScreen2) toTarget:self withObject:nil];
	

}

- (void)updateScreen2
{
   //[NSThread setThreadPriority:1.0];
   //printf("PING\n");
   NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
   while(__emulation_run)
   {
	  //printf("updaing\n");
	  //[self performSelectorOnMainThread:@selector(setNeedsDisplay) withObject:nil waitUntilDone:NO];
	  [screenView setNeedsDisplay];
	  [self.view setNeedsDisplay];
	  [self.view addSubview:nil];
	
	  //[UIImageView setNeedsDisplay];
      //usleep(16666);//60hz
      usleep(20000);//50hz
      //usleep(25000);//40hz
      //usleep(33333);//30hz
      //sched_yield();
   }
   [pool release];  
}

- (void)loadView {

	struct CGRect rect = [[UIScreen mainScreen] bounds];
	rect.origin.x = rect.origin.y = 0.0f;
	UIView *view= [[UIView alloc] initWithFrame:rect];
	self.view = view;
	[view release];
    self.view.backgroundColor = [UIColor blackColor];	
}

-(void)viewDidLoad{	

   [ self getConf];

	//[self.view addSubview:self.imageView];
 	
	//[ self getControllerCoords:0 ];
	
	//self.navigationItem.hidesBackButton = YES;
	
	
    self.view.opaque = YES;
	self.view.clearsContextBeforeDrawing = NO; //Performance?
	self.view.userInteractionEnabled = YES;
	
	self.view.multipleTouchEnabled = YES;
	self.view.exclusiveTouch = YES;
	
    //self.view.multipleTouchEnabled = NO; investigar porque se queda
	//self.view.contentMode = UIViewContentModeTopLeft;
	
	//[[self.view layer] setMagnificationFilter:kCAFilterNearest];
	//[[self.view layer] setMinificationFilter:kCAFilterNearest];
	
	//kito
	[NSThread setThreadPriority:1.0];
	
	loop = nil;
	dview = nil;
	touchTimer = nil;
	
}

- (void)drawRect:(CGRect)rect
{
            
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	//return (interfaceOrientation == UIInterfaceOrientationPortrait);
	return YES;
}

-(void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {
   
   NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
   
  __emulation_paused = 1;
  usleep(100000);//ensure some frames displayed

        
  [screenView removeFromSuperview];
  [screenView release];
  /*
  if(!hide_keyboard)
  {
  */
     hide_keyboard = 0;
     [imageView removeFromSuperview];
     [imageView release];
  /*   
  }
  else hide_keyboard = 0;
  */
  if(imageBorder!=nil)
  {
     [imageBorder removeFromSuperview];
     [imageBorder release];
     imageBorder= nil;
  }

	if((self.interfaceOrientation ==  UIDeviceOrientationLandscapeLeft) || (self.interfaceOrientation == UIDeviceOrientationLandscapeRight)){
	    [self buildLandscape];	        
	
	} else	if((self.interfaceOrientation == UIDeviceOrientationPortrait) || (self.interfaceOrientation == UIDeviceOrientationPortraitUpsideDown)){	
        [self buildPortrait];
	}
	
	__emulation_paused = 0;
	
	[pool release];
}

- (void)buildPortraitImageView {
  /*
   [UIView beginAnimations:@"foo2" context:nil];
   [UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
   [UIView setAnimationDuration:0.50];
   */
   
   
   if(controller)
   {
      if(isIpad)
        imageView = [ [ UIImageView alloc ] initWithImage:[UIImage imageNamed:[NSString stringWithFormat:@"controller_portrait_iPad.png"]]];
      else
        imageView = [ [ UIImageView alloc ] initWithImage:[UIImage imageNamed:[NSString stringWithFormat:@"controller_portrait_iPhone.png"]]];
   }   
   else
   {
      if(isIpad)
        imageView = [ [ UIImageView alloc ] initWithImage:[UIImage imageNamed:[NSString stringWithFormat:@"keyboard_portrait_iPad.png"]]];
      else
        imageView = [ [ UIImageView alloc ] initWithImage:[UIImage imageNamed:[NSString stringWithFormat:@"keyboard_portrait_iPhone.png"]]];
   }   
   
   imageView.frame = rPortraitImageViewFrame; // Set the frame in which the UIImage should be drawn in.
   
   imageView.userInteractionEnabled = NO;
   imageView.multipleTouchEnabled = NO;
   imageView.clearsContextBeforeDrawing = NO;
   //[imageView setOpaque:YES];
    if(controller)
       [imageView setAlpha:((float)iphone_controller_opacity / 100.0f)];
    else
       [imageView setAlpha:((float)iphone_keyboard_opacity / 100.0f)];
   [self.view addSubview: imageView]; // Draw the image in self.view.
   //[UIView commitAnimations];
   
  /////////////////
  if(enable_dview)
  {
	  if(dview!=nil)
	  {
	    [dview removeFromSuperview];
	    [dview release];
	  }  	 
	
	  dview = [[DView alloc] initWithFrame:self.view.bounds];
	  
	  [self.view addSubview:dview];   
	
	  [self filldrectsController];
	  [self filldrectsKeyboard];
	  
	  [dview setNeedsDisplay];
  }
  ////////////////
}

- (void)buildPortrait {

   iphone_is_landscape = 0;
   crop_state = 0;
   [ self getControllerCoords:0 ];
   [ self getKeyboardCoords:0 ];
   __emulation_run = 1;
   
   //screenView = [ [ScreenView alloc] initWithFrame: [[UIScreen mainScreen] bounds] ];
   
   if(!isIpad)
   {
	   if(safe_render_path)//HACK
	   {
	       rPortraitViewFrame = CGRectMake(0,0,320,240);
	   }
	   else
	   {
	       rPortraitViewFrame = CGRectMake(0,0,319,240);
	   }   
   }
   
   if(!iphone_keep_aspect_ratio && isIpad)
		screenView = [ [ScreenView alloc] initWithFrame: rPortraitNoKeepAspectViewFrame];	  
   else
        screenView = [ [ScreenView alloc] initWithFrame: rPortraitViewFrame];
               
   [self.view addSubview: screenView];
   
   [self buildPortraitImageView];
   
   
   if(safe_render_path || scanline_filter_port || tv_filter_port)
   {
       //prueba para tiling!
/*
       imageBorder = [ [ UIImageView alloc ] initWithImage:[UIImage imageNamed:[NSString stringWithFormat:@"borde.png"]]];
       imageBorder.frame = rPortraitBorderFrame;
       [self.view addSubview: imageBorder];
*/       
             
       UIImage *image1 = [UIImage imageNamed:[NSString stringWithFormat:@"borde.png"]];
                                                                                                                                                 
       UIGraphicsBeginImageContext(rPortraitBorderFrame.size);  
       
       //[image1 drawInRect: rPortraitBorderFrame];
       
       CGContextRef uiContext = UIGraphicsGetCurrentContext();
             
       CGContextTranslateCTM(uiContext, 0, rPortraitBorderFrame.size.height);
	
       CGContextScaleCTM(uiContext, 1.0, -1.0);

       if(scanline_filter_port)
       {       
            
          UIImage *image2 = [UIImage imageNamed:[NSString stringWithFormat: @"scanline-1.png"]];
                        
          CGImageRef tile = CGImageRetain(image2.CGImage);
                   
          CGContextSetAlpha(uiContext,((float)22 / 100.0f));   
              
          CGContextDrawTiledImage(uiContext, CGRectMake(0, 0, image2.size.width, image2.size.height), tile);
       
          CGImageRelease(tile);       
       }

       if(tv_filter_port)
       {              
          
          UIImage *image3 = [UIImage imageNamed:[NSString stringWithFormat: @"crt-1.png"]];              
          
          CGImageRef tile = CGImageRetain(image3.CGImage);
              
          CGContextSetAlpha(uiContext,((float)19 / 100.0f));     
          
          CGContextDrawTiledImage(uiContext, CGRectMake(0, 0, image3.size.width, image3.size.height), tile);
       
          CGImageRelease(tile);       
       }
       
       CGImageRef img = CGImageRetain(image1.CGImage);
       
       CGContextSetAlpha(uiContext,((float)100 / 100.0f));  
   
       CGContextDrawImage(uiContext,rPortraitBorderFrame , img);
   
       CGImageRelease(img);  
              
       UIImage *finishedImage = UIGraphicsGetImageFromCurrentImageContext();
                                                            
       UIGraphicsEndImageContext();
       
       imageBorder = [ [ UIImageView alloc ] initWithImage: finishedImage];
         
       imageBorder.frame = rPortraitBorderFrame;
              
       [self.view addSubview: imageBorder];            
   }    
   
}

- (void)buildLandscapeImageView{
/*
   [UIView beginAnimations:@"foo2" context:nil];
   [UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
   [UIView setAnimationDuration:0.50];
*/

   UIImage *image1;

   if(controller)
   {
      if(isIpad)
        image1 = [UIImage imageNamed:[NSString stringWithFormat:@"controller_landscape_iPad.png"]];
      else  
        image1 =  [UIImage imageNamed:[NSString stringWithFormat:@"controller_landscape_iPhone.png"]];
   }   			
   else
   {
      if(isIpad)
         image1 = [UIImage imageNamed:[NSString stringWithFormat:@"keyboard_landscape_iPad.png"]];
      else
         image1 = [UIImage imageNamed:[NSString stringWithFormat:@"keyboard_landscape_iPhone.png"]];
   }   
   
   if(scanline_filter_land || tv_filter_land || hide_keyboard)
   {                                                                                                                                              
	   UIGraphicsBeginImageContext(rLandscapeImageViewFrame.size);
	
	   CGContextRef uiContext = UIGraphicsGetCurrentContext();  
	   
	   CGContextTranslateCTM(uiContext, 0, rLandscapeImageViewFrame.size.height);
		
	   CGContextScaleCTM(uiContext, 1.0, -1.0);
	   
	   if(scanline_filter_land)
	   {       
	       
	      UIImage *image2;
	      
	      if(isIpad)
	        image2 =  [UIImage imageNamed:[NSString stringWithFormat: @"scanline-2.png"]];
	      else
	        image2 =  [UIImage imageNamed:[NSString stringWithFormat: @"scanline-1.png"]];
	                        
	      CGImageRef tile = CGImageRetain(image2.CGImage);
	      
	      if(isIpad)             
	         CGContextSetAlpha(uiContext,((float)10 / 100.0f));
	      else
	         CGContextSetAlpha(uiContext,((float)22 / 100.0f));
	              
	      CGContextDrawTiledImage(uiContext, CGRectMake(0, 0, image2.size.width, image2.size.height), tile);
	       
	      CGImageRelease(tile);       
	    }
	
	    if(tv_filter_land)
	    {              
	       UIImage *image3 = [UIImage imageNamed:[NSString stringWithFormat: @"crt-1.png"]];              
	          
	       CGImageRef tile = CGImageRetain(image3.CGImage);
	              
	       CGContextSetAlpha(uiContext,((float)20 / 100.0f));     
	          
	       CGContextDrawTiledImage(uiContext, CGRectMake(0, 0, image3.size.width, image3.size.height), tile);
	       
	       CGImageRelease(tile);       
	    }
	       
	   if(!hide_keyboard)
	   {
	      
		    if(controller)
		      CGContextSetAlpha(uiContext,((float)iphone_controller_opacity / 100.0f));
		    else
		      CGContextSetAlpha(uiContext,((float)iphone_keyboard_opacity / 100.0f));
		       
		   //[image1 drawInRect: rLandscapeImageViewFrame];
		   	   
		   CGImageRef img = CGImageRetain(image1.CGImage);
		   
		   CGContextDrawImage(uiContext,rLandscapeImageViewFrame , img);
		   
		   CGImageRelease(img);
	   }  
	       
	    UIImage *finishedImage = UIGraphicsGetImageFromCurrentImageContext();
	                  
	    UIGraphicsEndImageContext();
	    
	    imageView = [ [ UIImageView alloc ] initWithImage: finishedImage];
    }
    else //speedy
    {
       imageView = [ [ UIImageView alloc ] initWithImage: image1];
      
       if(controller)
          [imageView setAlpha:((float)iphone_controller_opacity / 100.0f)];
       else
          [imageView setAlpha:((float)iphone_keyboard_opacity / 100.0f)];    
    }
     
    imageView.frame = rLandscapeImageViewFrame; // Set the frame in which the UIImage should be drawn in.
      
    imageView.userInteractionEnabled = NO;
    imageView.multipleTouchEnabled = NO;
    imageView.clearsContextBeforeDrawing = NO;
   
    //[imageView setOpaque:YES];

    //TEST CODE
    //if(!iphone_is_landscape)//remove this line
  
     [self.view addSubview: imageView]; // Draw the image in self.view.
     //[UIView commitAnimations];
  
  //////////////////
  if(enable_dview)
  {
	  if(dview!=nil)
	  {
        [dview removeFromSuperview];
        [dview release];
      }	 	  
	  
	  dview = [[DView alloc] initWithFrame:self.view.bounds];
		 	  
	  [self filldrectsController];
	  [self filldrectsKeyboard];
	  
	  [self.view addSubview:dview];   
	  [dview setNeedsDisplay];
	  
	 
  }
  /////////////////	
}

- (void)buildLandscape{
	
   iphone_is_landscape = 1;
   crop_state = 0;
   [ self getControllerCoords:1 ];
   [ self getKeyboardCoords:1 ];
   __emulation_run = 1;
   
   //screenView = [ [ScreenView alloc] initWithFrame: [[UIScreen mainScreen] bounds] ];
      
   if(!iphone_keep_aspect_ratio && !isIpad)
		screenView = [ [ScreenView alloc] initWithFrame: rLandscapeNoKeepAspectViewFrame];	  
   else
        screenView = [ [ScreenView alloc] initWithFrame: rLandscapeViewFrame];
         
   //screenView = [ [ScreenView alloc] initWithFrame: rlandscapeViewFrame];			
    			
   [self.view addSubview: screenView];
   
   [self buildLandscapeImageView];
	
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{	 

  if(controller)
  {
      //test code 
      //if(iphone_is_landscape)
         //[self touchesController2:touches withEvent:event];else
      
      if(isIpad)
        [self touchesControllerv1:touches withEvent:event];
      else
        [self touchesControllerv2:touches withEvent:event];
  }
  else
  {
      [self touchesKeyboard:touches withEvent:event];
  }  
  
}

- (void)showKey:(CGRect)rect{

  if(iphone_is_landscape)return;
     
  if(loop==nil)
  {
     loop = [[MagnifierView alloc] initWithFrame:self.view.bounds];
	 loop.viewref = self.view;
  }
  if(touchTimer!=nil)
  {
     [touchTimer invalidate];
     touchTimer = nil;
  }
  [self.view addSubview:loop];
  
  CGPoint p;
  p.x = rect.origin.x + (rect.size.width / 2);
  p.y = rect.origin.y + (rect.size.height / 2);
	
  loop.touchPoint = p;
 
  [loop setNeedsDisplay];	

}

- (void)hideKey {
  if(loop!=nil)
  {
	 touchTimer = [NSTimer scheduledTimerWithTimeInterval:0.10
												 target:self
												 selector:@selector( handleAction: )
											     userInfo:nil
												 repeats:NO];    
   }
}

- (void) handleAction:(id)timerObj {
     [loop removeFromSuperview];
     touchTimer = nil;
}

#define SJ_PI 3.14159265359f
#define SJ_PI_X_2 6.28318530718f
#define SJ_RAD2DEG 180.0f/SJ_PI
#define SJ_DEG2RAD SJ_PI/180.0f
int lastX = 0;
int lastY = 0;
int k = 0;
- (void)touchesController2:(NSSet *)touches withEvent:(UIEvent *)event
{	

    if(!k)
    {
       k = 1;
       lastLocation1.x = 71;//48;
	   lastLocation1.y = 250;//280;
    }

    int i =0;
    //printf("entrando: %d\n",gp2x_pad_status);
    
    //printf("llega evento\n");
    CGRect r1 = CGRectMake(0,0,240,320);
    CGRect r2 = CGRectMake(240,0,240,320);

    //CGRect r1 = CGRectMake(0,240,160,240);
    //CGRect r2 = CGRectMake(160,240,160,240);    
    
    int x = 1;
    int y = 1;
    
   // gp2x_pad_status = 0;      
  
	NSSet *allTouches = [event allTouches];
	int touchcount = [allTouches count];
		for (i = 0; i < touchcount; i++) 
	{
		UITouch *touch = [[allTouches allObjects] objectAtIndex:i];
	    struct CGPoint point;
		point = [touch locationInView:self.view];
		if(touch.phase == UITouchPhaseCancelled)
		{
		   //printf("cancelado fuera de recta!\n");
		}
		
		
		if (MyCGRectContainsPoint(r2, point)) {
		   if(touch.phase != UITouchPhaseEnded)
		   {
		        //printf("1/fired\n");
		        //printf("%d\n",gp2x_pad_status);
		       gp2x_pad_status |= GP2X_B;
		        //printf("%d\n",gp2x_pad_status);
		       //printf("2/fired\n");
		   }    
		   else
		   {
		        //printf("1/ended fired\n");
		       //printf("%d\n",gp2x_pad_status);
		       gp2x_pad_status &= ~GP2X_B;
		       //printf("%d\n",gp2x_pad_status);
		       //printf("2/ended fired\n");
		   }    
		}
		else
		if (MyCGRectContainsPoint(r1, point)) {
            /*		
		    if(touch.phase == UITouchPhaseBegan)
		    {
		       //printf("beagan\n");
		       lastLocation1 = point;
		       //gp2x_pad_status |= lastX;
		       //gp2x_pad_status |= lastY;
		    }  
		    
		    else */ if(touch.phase == UITouchPhaseMoved || touch.phase == UITouchPhaseBegan)
		    {

		       int rad = 1;   		       
               //printf("moved\n");
               CGFloat xDisplacement = point.x - lastLocation1.x;
               CGFloat yDisplacement = point.y - lastLocation1.y;
               
               CGFloat xDisplacementAbs = fabs(xDisplacement);
               CGFloat yDisplacementAbs = fabs(yDisplacement);
               
               
               float angle = atan2f(xDisplacement, yDisplacement); // in radians
               angle = angle * SJ_RAD2DEG;
               
               
               
               float r =  sqrt((xDisplacement* xDisplacement)+ (yDisplacement * yDisplacement));
               
               /*
               if(r > rad)
               {
               printf("\n pto %f,%f\n",xDisplacement,yDisplacement);
               printf("angulo %f\n",angle);
               printf("radio %f\n",r);
               }
               */
               
               
               
               int b1 = r > rad;
               int b2 = r > rad;
               //printf("moved %f %f\n",xDisplacement,yDisplacement);
               /*
               if(xDisplacementAbs > yDisplacementAbs &&  yDisplacementAbs < 1)
               {
                  b1 = 1; b2=0;
                  //gp2x_pad_status &= ~GP2X_DOWN;
		          //gp2x_pad_status &= ~GP2X_UP;

               }
               else if(xDisplacementAbs < yDisplacementAbs && xDisplacementAbs < 1)
               {
                  b1 = 0; b2=1;
                  //gp2x_pad_status &= ~GP2X_LEFT;
		          //gp2x_pad_status &= ~GP2X_RIGHT;
               }                   
               else
               {
                   b1=1;b2=1;
               }           
               */
               
               if(b1)
               {
	               if(angle < -30 && angle > -150)
	               //if(angle < -15 && angle > -165)
	               {
	                   //printf("izq\n");
	                   gp2x_pad_status |= GP2X_LEFT;
	                   gp2x_pad_status &= ~GP2X_RIGHT;
	                   lastX = GP2X_LEFT;
	                   //if(xDisplacementAbs<-2)
	                       lastLocation1 = point;
	                   x=1;
	               }    
	               else  if(angle > 30  && angle < 150)
	               //else  if(angle > 15  && angle < 165)
	               {
	                   //printf("der\n");
	                   gp2x_pad_status |= GP2X_RIGHT;
	                   gp2x_pad_status &= ~GP2X_LEFT;
	                   lastX = GP2X_RIGHT;
	                   //if(xDisplacementAbs>2)
	                       lastLocation1 = point;
	                   x=1;
	               }
	               else
	               {
	                 //printf("NOderizq\n");
	                 //lastX = 0;
	                 x = 0;
	                 //gp2x_pad_status &= ~GP2X_LEFT;
		             //gp2x_pad_status &= ~GP2X_RIGHT;
	               }  
               }
               
               if(b2)
               {   
                   if(angle < -120 || angle > 120)
	               //if(angle < -135 || angle > 135)
	               {
	                   //printf("up\n");
	                   gp2x_pad_status |= GP2X_UP;
	                   gp2x_pad_status &= ~GP2X_DOWN;
	                   lastY = GP2X_UP;
	                   //if(yDisplacementAbs<-16)
	                       lastLocation1 = point;
	                   y=1;
	                  
	               }
	               else  if(angle > -60 && angle < 60)    
	               //else  if(angle > -75 && angle < 75)
	               {
	                   //printf("down\n");
	                   gp2x_pad_status |= GP2X_DOWN;
	                   gp2x_pad_status &= ~GP2X_UP;
	                   lastY = GP2X_DOWN;
	                   //if(yDisplacementAbs>16)
	                      lastLocation1 = point;
	                   y=1;
	               }
                   else
                   { 
                      //printf("NO updown\n");
                      y = 0;
                      //lastY = 0;
                      //gp2x_pad_status &= ~GP2X_DOWN;
		              //gp2x_pad_status &= ~GP2X_UP;
                   }                      
               }
               
               if(x && !y)
               {
                  //printf("NO Y!\n");

		          gp2x_pad_status &= ~GP2X_DOWN;
		          gp2x_pad_status &= ~GP2X_UP;
               }
               if(!x && y)
               {
                  //printf("NO X!\n");
                  gp2x_pad_status &= ~GP2X_LEFT;
		          gp2x_pad_status &= ~GP2X_RIGHT;
               }
		    }	
		    else if(touch.phase == UITouchPhaseStationary)
		    {
		       //printf("stationary\n");
		       //gp2x_pad_status &= lastX;
		       //gp2x_pad_status &= lastY;
		    }
		    else if(touch.phase == UITouchPhaseEnded)
		    {
		        //printf("ended\n");
		        gp2x_pad_status &= ~GP2X_LEFT;
		        gp2x_pad_status &= ~GP2X_RIGHT;
		        gp2x_pad_status &= ~GP2X_DOWN;
		        gp2x_pad_status &= ~GP2X_UP;
		        
		        lastLocation1.x = 71;
		        lastLocation1.y = 250;
		        
		        //printf("ended control\n");
		    }
		    //else	printf("otro\n");

		      
		}				
	}	
	/*
	if(x==1 && y==0)
	{
	   gp2x_pad_status &= ~GP2X_UP;
	   gp2x_pad_status &= ~GP2X_DOWN;
	}
	
	if(y==1 && x==0)
	{
	    gp2x_pad_status &= ~GP2X_LEFT;
		gp2x_pad_status &= ~GP2X_RIGHT;	
	}
	*/
	
	//printf("saliendo: %d\n",gp2x_pad_status);
}

/*

int lastX = 0;
int lastY = 0;
- (void)touchesController2:(NSSet *)touches withEvent:(UIEvent *)event
{	

    int i =0;
    
    //printf("llega evento\n");
    CGRect r1 = CGRectMake(0,256,320,64);      
    //CGRect r1 = CGRectMake(0,416,320,64);
     CGRect r2 = CGRectMake(416,0,64,320);  
  
	NSSet *allTouches = [event allTouches];
	int touchcount = [allTouches count];
		for (i = 0; i < touchcount; i++) 
	{
		UITouch *touch = [[allTouches allObjects] objectAtIndex:i];
	    struct CGPoint point;
		point = [touch locationInView:self.view];
		if(touch.phase == UITouchPhaseCancelled)
		{
		   //printf("cancelado fuera de recta!\n");
		}
		
		if (MyCGRectContainsPoint(r1, point)) {
		
		    if(touch.phase == UITouchPhaseBegan)
		    {
		       //printf("beagan\n");
		       lastLocation1 = point;
		       //gp2x_pad_status = lastX;
		    }   
		    else if(touch.phase == UITouchPhaseMoved)
		    {
		       
               //printf("moved\n");
               CGFloat xDisplacement = point.x - lastLocation1.x;
               
               if(xDisplacement<-2)
               {
                   //printf("izq\n");
                   gp2x_pad_status |= GP2X_LEFT;
                   gp2x_pad_status &= ~GP2X_RIGHT;
                   lastX = GP2X_LEFT;
                   lastLocation1 = point;
               }    
               else  if(xDisplacement>2)
               {
                   //printf("der\n");
                   gp2x_pad_status |= GP2X_RIGHT;
                   gp2x_pad_status &= ~GP2X_LEFT;
                   lastX = GP2X_RIGHT;
                   lastLocation1 = point;
               }    
        
		    }	
		    else if(touch.phase == UITouchPhaseStationary)
		    {
		       //printf("tationary\n");
		    }
		    else if(touch.phase == UITouchPhaseEnded)
		    {
		        //printf("cancelled\n");
		        gp2x_pad_status &= ~GP2X_LEFT;
		        gp2x_pad_status &= ~GP2X_RIGHT;
		    }
		    //else	printf("otro\n");
		      
		}		
		
	    if (MyCGRectContainsPoint(r2, point)) {
		
		    if(touch.phase == UITouchPhaseBegan)
		    {
		       //printf("beagan\n");
		       lastLocation2 = point;
		       //gp2x_pad_status = lastY;
		    }   
		    else if(touch.phase == UITouchPhaseMoved)
		    {
		       
               //printf("moved\n");
               CGFloat yDisplacement = point.y - lastLocation2.y;
               
               if(yDisplacement<-2)
               {
                   //printf("izq\n");
                   gp2x_pad_status |= GP2X_UP;
                   gp2x_pad_status &= ~GP2X_DOWN;
                   lastY = GP2X_UP;
                   lastLocation2 = point;
               }    
               else  if(yDisplacement>2)
               {
                   //printf("der\n");
                   gp2x_pad_status |= GP2X_DOWN;
                   gp2x_pad_status &= ~GP2X_UP;
                   lastY = GP2X_DOWN;
                   lastLocation2 = point;
               }    
        
		    }	
		    else if(touch.phase == UITouchPhaseEnded)
		    {
		        //printf("cancelled\n");
		        gp2x_pad_status &= ~GP2X_DOWN;
		        gp2x_pad_status &= ~GP2X_UP;
		    }
		    //else	printf("otro\n");
		      
		}
		
	}	
}
*/
- (void)touchesKeyboard:(NSSet *)touches withEvent:(UIEvent *)event {	
	int i;
	//Get all the touches.
	NSSet *allTouches = [event allTouches];
	int touchcount = [allTouches count];
	int isHide;
	
	numKeys = 0;    
    isShiftKey  =  0;
    isSymbolKey =  0;
    isHide = 0;
 
   if(!__emulation_run)
   {
    return;
   }
  
	for (i = 0; i < touchcount; i++) 
	{
		UITouch *touch = [[allTouches allObjects] objectAtIndex:i];
		
		if(touch == nil)
		{
			return;
		}
		
		if( touch.phase == UITouchPhaseBegan		||
			touch.phase == UITouchPhaseMoved		||
			touch.phase == UITouchPhaseStationary	)
		{
			struct CGPoint point;
			point = [touch locationInView:self.view];
			
			if(hide_keyboard)
			{
			    hide_keyboard = 0;
			   [imageView removeFromSuperview];
               [imageView release]; 
			   if(iphone_is_landscape)
                 [self buildLandscapeImageView]; 
               else
                 [self buildPortraitImageView];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_SHIFT, point)) {
			    isShiftKey = 1;
			    if(touchcount > 1)continue;
                [self showKey:rSPECKEY_SHIFT];
			}	
						
			else if (MyCGRectContainsPoint(rSPECKEY_SHIFT2, point)) {
			    isShiftKey = 2;
			    if(touchcount > 1)continue;
                [self showKey:rSPECKEY_SHIFT2];                
			}
			else if (MyCGRectContainsPoint(rSPECKEY_0, point)) {
                keyboardKeys[numKeys++] = SPECKEY_0; [self showKey:rSPECKEY_0];                          
			}
			else if (MyCGRectContainsPoint(rSPECKEY_1, point)) {
                keyboardKeys[numKeys++] = SPECKEY_1; [self showKey:rSPECKEY_1];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_2, point)) {
                keyboardKeys[numKeys++] = SPECKEY_2; [self showKey:rSPECKEY_2];      
			}
			else if (MyCGRectContainsPoint(rSPECKEY_3, point)) {
                keyboardKeys[numKeys++] = SPECKEY_3;[self showKey:rSPECKEY_3];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_4, point)) {
                keyboardKeys[numKeys++] = SPECKEY_4;[self showKey:rSPECKEY_4];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_5, point)) {
                keyboardKeys[numKeys++] = SPECKEY_5;[self showKey:rSPECKEY_5];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_6, point)) {
                keyboardKeys[numKeys++] = SPECKEY_6;[self showKey:rSPECKEY_6];
			}			
			else if (MyCGRectContainsPoint(rSPECKEY_7, point)) {
                keyboardKeys[numKeys++] = SPECKEY_7;[self showKey:rSPECKEY_7];
			} 
			else if (MyCGRectContainsPoint(rSPECKEY_8, point)) {
                keyboardKeys[numKeys++] = SPECKEY_8;[self showKey:rSPECKEY_8];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_9, point)) {
                keyboardKeys[numKeys++] = SPECKEY_9;[self showKey:rSPECKEY_9];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_0, point)) {
                keyboardKeys[numKeys++] = SPECKEY_0;[self showKey:rSPECKEY_0];
			}
			
			else if (MyCGRectContainsPoint(rSPECKEY_Q, point)) {
                keyboardKeys[numKeys++] = SPECKEY_Q;[self showKey:rSPECKEY_Q];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_W, point)) {
                keyboardKeys[numKeys++] = SPECKEY_W;[self showKey:rSPECKEY_W];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_E, point)) {
                keyboardKeys[numKeys++] = SPECKEY_E;[self showKey:rSPECKEY_E];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_R, point)) {
                keyboardKeys[numKeys++] = SPECKEY_R;[self showKey:rSPECKEY_R];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_T, point)) {
                keyboardKeys[numKeys++] = SPECKEY_T;[self showKey:rSPECKEY_T];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_Y, point)) {
                keyboardKeys[numKeys++] = SPECKEY_Y;[self showKey:rSPECKEY_Y];
			}			
			else if (MyCGRectContainsPoint(rSPECKEY_U, point)) {
                keyboardKeys[numKeys++] = SPECKEY_U;[self showKey:rSPECKEY_U];
			} 
			else if (MyCGRectContainsPoint(rSPECKEY_I, point)) {
                keyboardKeys[numKeys++] = SPECKEY_I;[self showKey:rSPECKEY_I];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_O, point)) {
                keyboardKeys[numKeys++] = SPECKEY_O;[self showKey:rSPECKEY_O];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_P, point)) {
                keyboardKeys[numKeys++] = SPECKEY_P;[self showKey:rSPECKEY_P];
			}						

			else if (MyCGRectContainsPoint(rSPECKEY_A, point)) {
                keyboardKeys[numKeys++] = SPECKEY_A;[self showKey:rSPECKEY_A];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_S, point)) {
                keyboardKeys[numKeys++] = SPECKEY_S;[self showKey:rSPECKEY_S];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_D, point)) {
                keyboardKeys[numKeys++] = SPECKEY_D;[self showKey:rSPECKEY_D];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_F, point)) {
                keyboardKeys[numKeys++] = SPECKEY_F;[self showKey:rSPECKEY_F];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_G, point)) {
                keyboardKeys[numKeys++] = SPECKEY_G;[self showKey:rSPECKEY_G];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_H, point)) {
                keyboardKeys[numKeys++] = SPECKEY_H;[self showKey:rSPECKEY_H];
			}			
			else if (MyCGRectContainsPoint(rSPECKEY_J, point)) {
                keyboardKeys[numKeys++] = SPECKEY_J;[self showKey:rSPECKEY_J];
			} 
			else if (MyCGRectContainsPoint(rSPECKEY_K, point)) {
                keyboardKeys[numKeys++] = SPECKEY_K;[self showKey:rSPECKEY_K];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_L, point)) {
                keyboardKeys[numKeys++] = SPECKEY_L;[self showKey:rSPECKEY_L];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_ENTER, point)) {
                keyboardKeys[numKeys++] = SPECKEY_ENTER;[self showKey:rSPECKEY_ENTER];
			}						


			else if (MyCGRectContainsPoint(rSPECKEY_Z, point)) {
                keyboardKeys[numKeys++] = SPECKEY_Z;[self showKey:rSPECKEY_Z];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_X, point)) {
                keyboardKeys[numKeys++] = SPECKEY_X;[self showKey:rSPECKEY_X];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_C, point)) {
                keyboardKeys[numKeys++] = SPECKEY_C;[self showKey:rSPECKEY_C];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_V, point)) {
                keyboardKeys[numKeys++] = SPECKEY_V;[self showKey:rSPECKEY_V];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_B, point)) {
                keyboardKeys[numKeys++] = SPECKEY_B;[self showKey:rSPECKEY_B];
			}			
			else if (MyCGRectContainsPoint(rSPECKEY_N, point)) {
                keyboardKeys[numKeys++] = SPECKEY_N;[self showKey:rSPECKEY_N];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_M, point)) {
                keyboardKeys[numKeys++] = SPECKEY_M;[self showKey:rSPECKEY_M];
			} 
			else if (MyCGRectContainsPoint(rSPECKEY_SYMB, point)) {
                isSymbolKey = 1;
                if(touchcount > 1)continue;
                [self showKey:rSPECKEY_SYMB];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_SYMB2, point)) {
                isSymbolKey = 2;
                if(touchcount > 1)continue;
                [self showKey:rSPECKEY_SYMB2];
			}			
			else if (MyCGRectContainsPoint(rSPECKEY_SPACE, point)) {
                keyboardKeys[numKeys++] = SPECKEY_SPACE;[self showKey:rSPECKEY_SPACE];
			}
			else if (MyCGRectContainsPoint(rSPECKEY_ENTER, point)) {
                keyboardKeys[numKeys++] = SPECKEY_ENTER;[self showKey:rSPECKEY_ENTER];
			}	
			else if (MyCGRectContainsPoint(rShowController, point)) {
		       [self hideKey];
               controller = 1;
               ext_keyboard = 0;
               numKeys = 0;    
               isShiftKey  =  0;
               isSymbolKey =  0;
               [imageView removeFromSuperview];
               [imageView release];
               if(iphone_is_landscape)
                 [self buildLandscapeImageView]; 
               else
                 [self buildPortraitImageView];
                                  
               break;
			}
		    else if (MyCGRectContainsPoint(rHideKeyboard, point) || MyCGRectContainsPoint(rHideKeyboard2, point)) {
		       [self hideKey];
               hide_keyboard = 1;
               /*
               [imageView removeFromSuperview];
               [imageView release];
               */
               [imageView removeFromSuperview];
               [imageView release];
               if(iphone_is_landscape)
                 [self buildLandscapeImageView]; 
               else
                 [self buildPortraitImageView];
			}				
		}
		else
		{
			[self hideKey];
			isHide = 1;
		}								
	}
	
	if(isHide && isShiftKey==1)
	  [self showKey:rSPECKEY_SHIFT];
	if(isHide && isShiftKey==2)
	  [self showKey:rSPECKEY_SHIFT2];	  
	if(isHide && isSymbolKey==1)
      [self showKey:rSPECKEY_SYMB];
	if(isHide && isSymbolKey==2)
      [self showKey:rSPECKEY_SYMB2];      	  
}


- (void)touchesControllerv2:(NSSet *)touches withEvent:(UIEvent *)event
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
        //printf("Tocuche %d %d\n",i,[allTouches count]);
	    UITouch *touch = [[allTouches allObjects] objectAtIndex:i];
    
		if( touch != nil && 
		    ( touch.phase == UITouchPhaseBegan ||
			    touch.phase == UITouchPhaseMoved ||
  			  touch.phase == UITouchPhaseStationary) )
		{
			//printf("Miro tocuches\n");
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
			else if (MyCGRectContainsPoint(rShowKeyboard, point)) {
			   if(!emulating) continue;
               controller = 0;
               ext_keyboard = 1;
               gp2x_pad_status = 0;
               int ii=0;
               for (ii = 0; ii < 10; ii++) 
               {
                   touchstate[ii] = 0;
                   oldtouches[ii] = 0;
               }
               [imageView removeFromSuperview];
               [imageView release];
               if(iphone_is_landscape)
                 [self buildLandscapeImageView]; 
               else
                 [self buildPortraitImageView];
               break;
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

- (void)touchesControllerv1:(NSSet *)touches withEvent:(UIEvent *)event {	
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
			else if (MyCGRectContainsPoint(rShowKeyboard, point)) {
               controller = 0;
               [imageView removeFromSuperview];
               [imageView release];
               if(iphone_is_landscape)
                 [self buildLandscapeImageView]; 
               else
                 [self buildPortraitImageView];
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


- (void)getControllerCoords:(int)orientation {
    char string[256];
    FILE *fp;
	
	if(!orientation)
	{
		if(isIpad)
		  fp = fopen([[NSString stringWithFormat:@"%scontroller_portrait_iPad.txt", get_resource_path("/")] UTF8String], "r");
		else
		  fp = fopen([[NSString stringWithFormat:@"%scontroller_portrait_iPhone.txt", get_resource_path("/")] UTF8String], "r");
    }
	else
	{
		if(isIpad)
		   fp = fopen([[NSString stringWithFormat:@"%scontroller_landscape_iPad.txt", get_resource_path("/")] UTF8String], "r");
		else
		   fp = fopen([[NSString stringWithFormat:@"%scontroller_landscape_iPhone.txt", get_resource_path("/")] UTF8String], "r");
	}
	
	if (fp) 
	{

		int i = 0;
        while(fgets(string, 256, fp) != NULL && i < 25) 
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
    		case 23:   rShowKeyboard  = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
            case 24:   iphone_controller_opacity= coords[0]; break;
			}
      i++;
    }
    fclose(fp);
  }
}


- (void)getKeyboardCoords:(int)orientation {
    char string[256];
    FILE *fp;
	
	if(!orientation)
	{
		if(isIpad)
		  fp = fopen([[NSString stringWithFormat:@"%skeyboard_portrait_iPad.txt", get_resource_path("/")] UTF8String], "r");
		else
		  fp = fopen([[NSString stringWithFormat:@"%skeyboard_portrait_iPhone.txt", get_resource_path("/")] UTF8String], "r");
    }
	else
	{
		if(isIpad)
		   fp = fopen([[NSString stringWithFormat:@"%skeyboard_landscape_iPad.txt", get_resource_path("/")] UTF8String], "r");
		else
		   fp = fopen([[NSString stringWithFormat:@"%skeyboard_landscape_iPhone.txt", get_resource_path("/")] UTF8String], "r");
	}
	
	if (fp) 
	{

		int i = 0;
        while(fgets(string, 256, fp) != NULL && i < 46) 
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
    		case 0:    rSPECKEY_1   	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 1:    rSPECKEY_2   	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 2:    rSPECKEY_3    = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 3:    rSPECKEY_4  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 4:    rSPECKEY_5  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 5:    rSPECKEY_6     	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 6:    rSPECKEY_7    	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 7:    rSPECKEY_8  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 8:    rSPECKEY_9 = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 9:    rSPECKEY_0  = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		
    		case 10:   rSPECKEY_Q   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 11:   rSPECKEY_W   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 12:   rSPECKEY_E   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 13:   rSPECKEY_R   	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 14:   rSPECKEY_T   	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 15:   rSPECKEY_Y    	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 16:   rSPECKEY_U  		= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 17:   rSPECKEY_I  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 18:   rSPECKEY_O     	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 19:   rSPECKEY_P     	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		
    		case 20:   rSPECKEY_A  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 21:   rSPECKEY_S   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 22:   rSPECKEY_D   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 23:   rSPECKEY_F  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 24:   rSPECKEY_G   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 25:   rSPECKEY_H   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 26:   rSPECKEY_J  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 27:   rSPECKEY_K   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 28:   rSPECKEY_L   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
            case 29:   rSPECKEY_ENTER   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;    		

    		case 30:   rSPECKEY_SHIFT  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 31:   rSPECKEY_Z   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 32:   rSPECKEY_X   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 33:   rSPECKEY_C  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 34:   rSPECKEY_V   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 35:   rSPECKEY_B   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 36:   rSPECKEY_N  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 37:   rSPECKEY_M   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 38:   rSPECKEY_SYMB   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
            case 39:   rSPECKEY_SPACE   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
            case 40:   rShowController = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
            
            case 41:   rHideKeyboard = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
            case 42:   rHideKeyboard2 = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		
    		case 43:   rSPECKEY_SHIFT2  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] );break;
    		case 44:   rSPECKEY_SYMB2   = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		            
            case 45:   iphone_keyboard_opacity= coords[0];break;
			}
      i++;
    }
    fclose(fp);
  }
}


- (void)getConf{
    char string[256];
    FILE *fp;
	
	if(isIpad)
	   fp = fopen([[NSString stringWithFormat:@"%sconfig_iPad.txt", get_resource_path("/")] UTF8String], "r");
	else
	   fp = fopen([[NSString stringWithFormat:@"%sconfig_iPhone.txt", get_resource_path("/")] UTF8String], "r");
	   	
	if (fp) 
	{

		int i = 0;
        while(fgets(string, 256, fp) != NULL && i < 10) 
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
    		case 0:    rEmulatorFrame   	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 1:    rPortraitViewFrame     	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 2:    rPortraitNoKeepAspectViewFrame = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;    		
    		case 3:    rPortraitImageViewFrame     	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 4:    rPortraitBorderFrame     	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;    		    		
    		case 5:    rLandscapeViewFrame = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;    		    		
    		case 6:    rLandscapeNoKeepAspectViewFrame = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;
    		case 7:    rLandscapeImageViewFrame  	= CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;    		
            case 8:    rLoopImageMask = CGRectMake( coords[0], coords[1], coords[2], coords[3] ); break;    	
            case 9:    enable_dview = coords[0]; break;
			}
      i++;
    }
    fclose(fp);
    
  }
}

- (void)didReceiveMemoryWarning {
	//[super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
	// Release anything that's not essential, such as cached data
}


- (void)dealloc {
    if(screenView!=nil)
       [screenView release];
       
    if(imageView!=nil)
      [imageView release];
      
    if(imageBorder!=nil)
     [imageBorder release];
 
    if(loop!=nil)
	   [loop release];
    if(dview!=nil)
	   [dview release];	   
	[super dealloc];
}

- (void)filldrectsController {

    		drects[0]=DownLeft;
    		drects[1]=Down;
    		drects[2]=DownRight;
    		drects[3]=Left;
    		drects[4]=Right;
    		drects[5]=UpLeft;
    		drects[6]=Up;
    		drects[7]=UpRight;
    		drects[8]=Select;
    		drects[9]=Start;
    		drects[10]=LPad;
    		drects[11]=RPad;
    		drects[12]=Menu;
    		drects[13]=ButtonDownLeft;
    		drects[14]=ButtonDown;
    		drects[15]=ButtonDownRight;
    		drects[16]=ButtonLeft;
    		drects[17]=ButtonRight;
    		drects[18]=ButtonUpLeft;
    		drects[19]=ButtonUp;
    		drects[20]=ButtonUpRight;
    		drects[21]=LPad2;
    		drects[22]=RPad2;
    		drects[23]=rShowKeyboard;
            ndrects = 24;        
}



- (void)filldrectsKeyboard{

    		drects2[0]=    rSPECKEY_1;
    		drects2[1]=    rSPECKEY_2;
    		drects2[2]=    rSPECKEY_3;
    		drects2[3]=   rSPECKEY_4;
    		drects2[4]=    rSPECKEY_5;
    		drects2[5]=    rSPECKEY_6;
    		drects2[6]=    rSPECKEY_7;
    		drects2[7]=    rSPECKEY_8;
    		drects2[8]=    rSPECKEY_9;
    		drects2[9]=    rSPECKEY_0;
    		
    		drects2[10]=   rSPECKEY_Q;
    		drects2[11]=   rSPECKEY_W;
    		drects2[12]=  rSPECKEY_E;
    		drects2[13]=  rSPECKEY_R;
    		drects2[14]=   rSPECKEY_T;
    		drects2[15]=   rSPECKEY_Y;
    		drects2[16]=  rSPECKEY_U;
    		drects2[17]=   rSPECKEY_I;
    		drects2[18]=   rSPECKEY_O;
    		drects2[19]=   rSPECKEY_P;
    		
    		drects2[20]=   rSPECKEY_A;
    		drects2[21]=   rSPECKEY_S;
    		drects2[22]=  rSPECKEY_D;
    		drects2[23]=   rSPECKEY_F;
    		drects2[24]=   rSPECKEY_G;
    		drects2[25]=   rSPECKEY_H;
    		drects2[26]=   rSPECKEY_J;
    		drects2[27]=   rSPECKEY_K;
    		drects2[28]=  rSPECKEY_L;
            drects2[29]=   rSPECKEY_ENTER;    		

    		drects2[30]=  rSPECKEY_SHIFT;
    		drects2[31]=  rSPECKEY_Z;
    		drects2[32]=  rSPECKEY_X;
    		drects2[33]=  rSPECKEY_C;
    		drects2[34]=   rSPECKEY_V;
    		drects2[35]=   rSPECKEY_B;
    		drects2[36]=   rSPECKEY_N;
    		drects2[37]=  rSPECKEY_M;
    		drects2[38]=  rSPECKEY_SYMB;
            drects2[39]=   rSPECKEY_SPACE;
            drects2[40]=   rShowController;
            
            drects2[41]=  rHideKeyboard;
            drects2[42]=   rHideKeyboard2;
    		
    		drects2[43]=   rSPECKEY_SHIFT2;
    		drects2[44]=   rSPECKEY_SYMB2;
    		
            ndrects2 = 45;        
}

@end
